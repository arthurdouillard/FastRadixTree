#include <iostream>
#include <algorithm>

#include "utils.hh"
#include "../word/word.hh"

#define LONG_SIZE sizeof(unsigned long)

Trie *create_trie(std::string path) {
    std::ifstream dict(path);

    auto* root = new Trie(1337, "");
    std::string word;
    int frequency;
    //int i = 0;
    while(dict >> word >> frequency)
    {
        //if (i > 13)
        //    break;
        //std::cout << word << " " << frequency << '\n';
        root->add_word_compressed(word, frequency);
        //i++;
    }

    return root;
}

void *mmap_file(char* path)
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
bool lexicoOrder(Word a, Word b)
{
    auto ch_a = a.get_content().c_str();
    auto ch_b = b.get_content().c_str();

    while (*ch_a != '\0' && *ch_b != '\0') {
        if (*ch_a != *ch_b)
            return *ch_a < *ch_b;
        ch_a++;
        ch_b++;
    }

    return true;
}

bool compare_words(Word a, Word b)
{
    if (a.get_distance() < b.get_distance())
        return true;
    else if (a.get_distance() == b.get_distance())
    {
        if (a.get_frequency() > b.get_frequency())
            return true;
        else if (a.get_frequency() == b.get_frequency())
            return lexicoOrder(a, b);
    }

    return false;
}

void pretty_print(std::vector<Word> vect)
{
    std::stable_sort(vect.begin(), vect.end(), compare_words);

    std::cout << "[";
    for (size_t i = 0; i < vect.size(); i++) {
        auto curr_word = vect.at(i);
        std::cout << "{\"word\":" << "\"" << curr_word.get_content()<< "\","
                  << "\"freq\":" << curr_word.get_frequency() << ","
                  << "\"distance\":" << curr_word.get_distance() << "}";
        if (i != vect.size() -1)
            std::cout << ',';
    }
    std::cout << "]\n";
}

void* get_struct_end(void* offset)
{
    char* ptr = (char*)offset;
    // Jump at the end of the frequency
    // Jump children count
    // Go to the end of the string
    // Jump to the end of the brother's offset
    ptr += sizeof(uint32_t) * 2;
    while (*ptr != '\0') {
        ptr++;
    }
    ptr++;
    ptr += LONG_SIZE;
    return ptr;
}

// Returns the node value pointed by offset
std::string get_value(void* offset)
{
    char* ptr = (char*)offset;
    char* end_ptr = ptr;
    ptr += sizeof(uint32_t) * 2;
    int char_len = 0;
    while(*end_ptr != '\0') 
    {
        end_ptr++;
        char_len++;
    }
    char result [char_len];
    std::memcpy(result, ptr, sizeof(result));
    return std::string(ptr);
}

void* get_brother(void* begin, void* offset)
{
    char* end = (char*)get_struct_end(offset);
    unsigned long* bro_offset = (unsigned long*)(end - LONG_SIZE);
    return (char*)begin + *bro_offset;
}

unsigned long get_bro_offset(void* offset) {
    char* ptr = (char*)get_struct_end(offset);
    char* bro_offset = ptr - LONG_SIZE;
    unsigned long* res = (unsigned long*)bro_offset;
    return *res;
}

// Returns the i-th child of the node pointed by offset
void* get_child_at(void* begin, int index, void* offset) {
    char* ptr = (char*)get_struct_end(offset);
    while(index > 0)
    {
        ptr = (char*)get_brother(begin, ptr);
        index--;
    }
    return ptr;
}

uint32_t get_children_count(void* offset) {
    char* ptr = (char*)offset;
    ptr += sizeof(uint32_t);
    uint32_t* child_ptr = (uint32_t*)ptr;
    return *child_ptr;
}

uint32_t get_frequency(void* offset)
{
    uint32_t* ptr = (uint32_t*)offset;
    return *ptr;
} 

std::vector<Word>
search_close_words(void* begin, std::string word, int distance)
{
    if (distance == 0)
        return exact_search(begin, word);
    else
    {
        auto word_list = new std::vector<Word>();

        if (1 <= distance)
            dist_search(begin, begin, word.substr(1), 1, distance,
                        "", word_list, word[0]);

        for (size_t i = 0; i < get_children_count(begin); i++)
        {
            void* child = get_child_at(begin, i, begin);
            int mdist = -1;

            if (get_value(child)[0] == word[0])
                mdist = 0;
            else
                mdist = 1;

            // Substitution
            dist_search(begin, child, word.substr(1), mdist, distance,
                         "", word_list, word[0]);
            // Insertion
            dist_search(begin, child, word, 1, distance, "", word_list, '\0');
        }

        return *word_list;
    }
}

std::vector<Word>
exact_search(void* begin, std::string word)
{
    bool found;
    size_t initial_length = word.length();
    std::vector<Word> vect{} ;
    std::string curr_word("");
    void* node = begin;
    void* curr_child = nullptr;

    while(true) {
        found = false;
        if (curr_word.length() < initial_length) {

            for (size_t i = 0; i < get_children_count(node); i++) {
                curr_child = get_child_at(begin, i, node);
                std::string child_value = get_value(curr_child);
                int prefix = get_common_prefix(child_value, word);

                // There's a common prefix
                if (prefix != 0) {
                    node = curr_child;
                    curr_word += word.substr(0, prefix);
                    word = word.substr(prefix);
                    found = true;
                    break;
                }
            }
        }

        // No child matches, return the result
        if (!found && curr_word.length() == initial_length) {
            Word result(curr_word, get_frequency(node), 0);
            vect.push_back(Word(result));
            return vect;
        }
        else if (!found) {
            return vect;
        }
        
    }
}

int array_min(std::vector<int> vect) {
    int min = 100;

    for (size_t i = 0; i < vect.size(); i++)
    {
        if (vect[i] < min)
            min = vect[i];
    }
    return min;
}

int
dist_search(void* begin, void* node, std::string word, int curr_distance,
            int max_distance, std::string curr_word, std::vector<Word>* res_list,
            char deleted_char) {
    
    if (curr_distance > max_distance)
        return curr_distance;
    

    int res = 10, mdist, subs, insert, transpo, del;
    mdist = subs = insert = transpo = del = -1;
    bool recall = false;

    // If the node is compressed, remove one char and call
    // again on it
    if (get_value(node).length() > 1)
        recall = true;

    if (get_frequency(node) != 0)
        res = word.length();

    // Deletion
    if (curr_distance + 1 <= max_distance && word.length() > 0)
    {
        del = dist_search(begin, node, word.substr(1), ++curr_distance,
                          max_distance, curr_word, res_list, word[0]);
    }

    for (size_t i = 0; i < get_children_count(node); i++) {
        void* child = get_child_at(begin, i, node);

        if (word.length() > 0 && get_value(child)[0] == word[0])
            mdist = 0;
        else
            mdist = 1;

        // Substitution
        if (word.length() > 0 && curr_distance + mdist < max_distance) {
            curr_word += get_value(child)[0];

            if (recall)
                child = node;

            subs = dist_search(begin, child, word.substr(1), ++curr_distance,
                               max_distance, curr_word, res_list, word[0]);
        }

        // Insertion
        if (word.length() == 0 || (word.length() > 0 && mdist == 1))
        {
            curr_word += get_value(child)[0];

            if (recall)
                child = node;
            insert = dist_search(begin, child, word, ++curr_distance,
                                 max_distance, curr_word, res_list, '\0');
        }

        // Transposition
        /*if (word.length() > 0 && curr_word.length() > 0 && mdist == 0
            && deleted_char == get_value(child)[0]
            && get_value(node)[0] != get_value(child)[0])
        {
            if (recall)
                child = node;
            else
                word = word.substr(1);
            transpo = dist_search(begin, child, )
        }*/

        std::vector<int> int_vect{del, subs, insert}; 
        res = array_min(int_vect); 
    }

    if (word.length() == 0 && get_frequency(node) != 0 && res <= max_distance)
    {
        Word new_word(curr_word, get_frequency(node), res);
        res_list->push_back(new_word);
    }

    return res;
}