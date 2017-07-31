#include <iostream>
#include <algorithm>

#include "utils.hh"
#include "../word/word.hh"

#define LONG_SIZE sizeof(unsigned long)

std::unique_ptr<Trie>
create_trie(std::string path)
{
    std::ifstream dict(path);

    std::unique_ptr<Trie> root(new Trie(0, ""));
    std::string word;
    int frequency;
    while (dict >> word >> frequency)
        root->add_word_compressed(word, frequency);

    return root;
}

void *mmap_file(char *path)
{
    int fd;
    struct stat stat;
    void *trie_addr;

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        std::cerr << "Invalid input dic." << std::endl;
        exit(1);
    }

    if (fstat(fd, &stat) != 0)
    {
        std::cerr << "Fstat failed." << std::endl;
        exit(1);
    }

    if ((trie_addr = mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
    {
        std::cerr << "Mmap failed." << std::endl;
        exit(1);
    }

    return trie_addr;
}

// Lexicographic order comparison
// Returns true if a is smaller than b
bool lexicoOrder(const std::shared_ptr<Word> a, const std::shared_ptr<Word> b)
{
    auto ch_a = a->get_content().c_str();
    auto ch_b = b->get_content().c_str();

    while (*ch_a != '\0' && *ch_b != '\0')
    {
        if (*ch_a != *ch_b)
            return *ch_a < *ch_b;
        ch_a++;
        ch_b++;
    }

    return a->get_content().length() < b->get_content().length();
}

bool compare_words(const std::shared_ptr<Word> a, const std::shared_ptr<Word> b)
{
    if (a->get_distance() < b->get_distance())
        return true;
    else if (a->get_distance() == b->get_distance())
    {
        if (a->get_frequency() > b->get_frequency())
            return true;
        else if (a->get_frequency() == b->get_frequency())
            return lexicoOrder(a, b);
    }

    return false;
}

void pretty_print(std::vector<std::shared_ptr<Word>> vect)
{
    std::stable_sort(vect.begin(), vect.end(), compare_words);

    std::cout << "[";
    for (size_t i = 0; i < vect.size(); i++)
    {
        auto curr_word = vect.at(i);
        std::cout << "{\"word\":"
                  << "\"" << curr_word->get_content() << "\","
                  << "\"freq\":" << curr_word->get_frequency() << ","
                  << "\"distance\":" << curr_word->get_distance() << "}";
        if (i != vect.size() - 1)
            std::cout << ',';
    }
    std::cout << "]\n";
}

void *get_struct_end(void *offset)
{
    char *ptr = (char *)offset;
    ptr += sizeof(uint32_t) * 2;
    while (*ptr != '\0')
    {
        ptr++;
    }
    ptr++;
    ptr += LONG_SIZE;
    return ptr;
}

// Returns the node value pointed by offset
std::string get_value(void *offset)
{
    char *ptr = (char *)offset;
    char *end_ptr = ptr;
    ptr += sizeof(uint32_t) * 2;
    int char_len = 0;
    end_ptr++;
    while (*end_ptr != '\0')
    {
        end_ptr++;
        char_len++;
    }
    char result[char_len];
    std::memcpy(result, ptr, sizeof(result));
    return std::string(ptr);
}

void *get_brother(void *begin, void *offset)
{
    char *end = (char *)get_struct_end(offset);
    unsigned long *bro_offset = (unsigned long *)(end - LONG_SIZE);
    return (char *)begin + *bro_offset;
}

// Returns the i-th child of the node pointed by offset
void *get_child_at(void *begin, int index, void *offset)
{
    char *ptr = (char *)get_struct_end(offset);
    while (index > 0)
    {
        ptr = (char *)get_brother(begin, ptr);
        index--;
    }
    return ptr;
}

uint32_t get_children_count(void *offset)
{
    char *ptr = (char *)offset;
    ptr += sizeof(uint32_t);
    uint32_t *child_ptr = (uint32_t *)ptr;
    return *child_ptr;
}

uint32_t get_frequency(void *offset)
{
    uint32_t *ptr = (uint32_t *)offset;
    return *ptr;
}

std::vector<std::shared_ptr<Word>>
search_close_words(void *begin, std::string word, int distance)
{

    if (distance == 0)
        return exact_search(begin, word);
    else
    {
        Word_Struct ws;
        ws.word_list = new std::vector<std::shared_ptr<Word>>();
        ws.word_map = new std::map<std::string, std::shared_ptr<Word>>();
        std::string curr_word("");

        if (1 <= distance)
            dist_search(begin, begin, word.substr(1), 1, distance,
                        curr_word, &ws, word[0], 0, "Del");

        for (size_t i = 0; i < get_children_count(begin); i++)
        {
            void *child = get_child_at(begin, i, begin);
            int mdist = -1;

            if (get_value(child)[0] == word[0])
                mdist = 0;
            else
                mdist = 1;

            // Substitution
            dist_search(begin, child, word.substr(1), mdist, distance,
                        curr_word, &ws, word[0], 0, "Sub");
            // Insertion
            dist_search(begin, child, word, 1, distance, curr_word, &ws, '\0', 0, "Ins");
        }
        delete (ws.word_map);
        return *ws.word_list;
    }
}

std::vector<std::shared_ptr<Word>>
exact_search(void *begin, std::string word)
{
    bool found;
    size_t initial_length = word.length();
    std::vector<std::shared_ptr<Word>> vect{};
    std::string curr_word;
    void *node = begin;
    void *curr_child = nullptr;

    while (true)
    {
        found = false;

        if (curr_word.length() < initial_length)
        {
            for (size_t i = 0; i < get_children_count(node); i++)
            {
                curr_child = get_child_at(begin, i, node);
                std::string child_value = get_value(curr_child);
                size_t prefix = get_common_prefix(child_value, word);

                // There's a common prefix
                if (prefix > 0 && prefix == child_value.length())
                {
                    node = curr_child;
                    curr_word += child_value;
                    word = word.substr(prefix);
                    found = true;
                    break;
                }
            }
        }

        // No child matches, return the result
        if (!found && curr_word.length() == initial_length && get_frequency(node))
        {
            auto freq = get_frequency(node);
            vect.push_back(std::make_shared<Word>(curr_word, freq, 0));
            return vect;
        }
        else if (!found)
            return vect;
    }
}

int array_min(std::vector<int> vect)
{
    int min = 100;
    for (size_t i = 0; i < vect.size(); i++)
    {
        if (vect[i] < min)
            min = vect[i];
    }
    return min;
}

void insert_word(std::string new_val, uint32_t frequency, Word_Struct *ws,
                 int curr_distance)
{
    if (new_val[0] == 0)
        new_val = new_val.substr(1);

    Word new_word(new_val, frequency, curr_distance);
    auto old_value = ws->word_map->find(new_val);
    auto has_updated = false;

    // Update distance
    if (old_value != ws->word_map->end() && old_value->second->get_distance() > curr_distance)
    {
        old_value->second->set_distance(curr_distance);
        for (auto it = ws->word_list->begin(); it != ws->word_list->end(); ++it)
        {
            if ((*it)->get_content() == old_value->second->get_content())
            {
                (*it)->set_distance(curr_distance);
                has_updated = true;
                break;
            }
        }
    }

    if (old_value == ws->word_map->end() && !has_updated)
    {
        std::pair<std::string, std::shared_ptr<Word>> p;
        auto word_ptr = std::make_shared<Word>(new_word);
        p.first = new_val;
        p.second = word_ptr;
        ws->word_map->insert(p);
        ws->word_list->push_back(word_ptr);
    }
}

int manage_compressed_node(void *begin, void *node, std::string word, int curr_distance,
                           int max_distance, std::string curr_word, Word_Struct *ws,
                           char deleted_char, int offset, std::string node_val,
                           std::string step)
{
    int next_offset = offset + 1;
    auto char_val = node_val[1];
    int res = 10, mdist, subs, insert, transpo, del;
    mdist = subs = insert = transpo = del = res;

    if (word.length() > 0 && char_val == word[0])
        mdist = 0;
    else
        mdist = 1;

    del = deletion(begin, node, word, curr_distance + 1, max_distance, curr_word,
                   ws, word[0], offset, "del");

    subs = substitution(begin, node, word, curr_distance + mdist,
                        max_distance, curr_word + node_val[0],
                        ws, word[0], next_offset, "Sub");

    insert = insertion(begin, node, word, curr_distance + 1,
                       max_distance, curr_word + node_val[0],
                       ws, '\0', next_offset, mdist, "Ins");

    transpo = transposition(begin, node, word, curr_distance,
                            max_distance, curr_word, ws,
                            deleted_char, next_offset, node_val, step);

    return array_min(std::vector<int>{res, del, subs, insert, transpo});
}

int dist_search(void *begin, void *node, std::string word, int curr_distance,
                int max_distance, std::string curr_word, Word_Struct *ws,
                char deleted_char, int offset, std::string step)
{

    if (curr_distance > max_distance)
        return curr_distance;

    int res = 10, mdist, subs, insert, transpo, del;
    mdist = subs = insert = transpo = del = res;

    // The offset is used to read of the compressed nodes' strings
    int next_offset = 0;
    std::string node_val = get_value(node).substr(offset);

    // If the node is compressed, remove one char and call again
    if (node_val.length() > 1)
        res = manage_compressed_node(begin, node, word, curr_distance,
                                     max_distance, curr_word, ws, deleted_char,
                                     offset, node_val, step);
    else
    {
        if (get_frequency(node) != 0)
            res = word.length();

        del = deletion(begin, node, word, curr_distance + 1, max_distance, curr_word,
                       ws, word[0], offset, "del");
        res = std::min(res, del);

        for (size_t i = 0; i < get_children_count(node); i++)
        {
            void *child = get_child_at(begin, i, node);

            if (word.length() > 0 && get_value(child)[0] == word[0])
                mdist = 0;
            else
                mdist = 1;

            subs = substitution(begin, child, word, curr_distance + mdist,
                                max_distance, curr_word + node_val[0],
                                ws, word[0], next_offset, "Sub");

            insert = insertion(begin, child, word, curr_distance + 1,
                               max_distance, curr_word + node_val[0],
                               ws, '\0', next_offset, mdist, "Ins");

            transpo = transposition(begin, child, word, curr_distance,
                                    max_distance, curr_word, ws,
                                    deleted_char, next_offset, node_val, step);

            res = std::min(res, std::min(std::min(del, subs), insert));
        }
    }

    // Insert if we're on a word and we have consumed all the
    // node's chars
    if (word.length() == 0 && get_frequency(node) != 0 &&
        node_val.length() == 1 && res <= max_distance)
        insert_word(curr_word + node_val[0], get_frequency(node),
                    ws, curr_distance);

    return res;
}

int substitution(void *begin, void *child, std::string word, int curr_distance,
                 int max_distance, std::string curr_word, Word_Struct *ws,
                 char deleted_char, int offset, std::string step)
{
    if (word.length() > 0 && curr_distance <= max_distance)
    {
        return dist_search(begin, child, word.substr(1), curr_distance,
                           max_distance, curr_word,
                           ws, deleted_char, offset, step);
    }
    else
        return 10;
}

int insertion(void *begin, void *child, std::string word, int curr_distance,
              int max_distance, std::string curr_word, Word_Struct *ws,
              char deleted_char, int offset, int mdist, std::string step)
{
    if (curr_distance <= max_distance &&
        (word.length() == 0 || (word.length() > 0 && mdist == 1)))
    {
        return dist_search(begin, child, word, curr_distance,
                           max_distance, curr_word,
                           ws, deleted_char, offset, step);
    }
    else
        return 10;
}

int deletion(void *begin, void *node, std::string word, int curr_distance,
             int max_distance, std::string curr_word, Word_Struct *ws,
             char deleted_char, int offset, std::string step)
{
    if (curr_distance <= max_distance && word.length() > 0)
    {
        return dist_search(begin, node, word.substr(1), curr_distance,
                           max_distance, curr_word, ws, deleted_char,
                           offset, step);
    }
    else
        return 10;
}

bool can_transpose(char child_char, char deleted_char, char node_char,
                   std::string word)
{
    return (word.length() > 0 && deleted_char == child_char && node_char == word[0] && node_char != child_char);
}

int transposition(void *begin, void *child, std::string word, int curr_distance,
                  int max_distance, std::string curr_word, Word_Struct *ws,
                  char deleted_char, int offset, std::string node_val,
                  std::string step)
{
    if (can_transpose(get_value(child)[offset], deleted_char, node_val[0], word))
    {
        if (step == "Trans")
            curr_distance++;
        return dist_search(begin, child, word.substr(1), curr_distance,
                           max_distance, curr_word + node_val[0],
                           ws, word[0], offset, "Trans");
    }

    return 10;
}