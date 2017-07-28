#include <iostream>
#include <algorithm>

#include "utils.hh"
#include "../word/word.hh"

#define LONG_SIZE sizeof(unsigned long)

Trie *create_trie(std::string path) {
    std::ifstream dict(path);

    auto* root = new Trie(0, "");
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
    char* ch_a = a.content.c_str();
    char* ch_b = b.content.c_str();

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
    if (a.distance < b.distance)
        return true;
    else if (a.distance == b.distance)
    {
        if (a.frequency > b.frequency)
            return true;
        else if (a.frequency == b.frequency)
            return lexicoOrder(a, b);
    }

    return b;
}

void pretty_print(std::vector<Word> vect)
{
    if (vect.size() == 0)
        return;

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

    auto words = std::make_shared<std::vector<Word>>();
    
    // Deletion:
   // compute_distance();
}

std::vector<Word>
exact_search(void* begin, std::string word)
{
    bool found;
    size_t initial_length = word.length();
    std::string curr_word("");
    void* node = begin;
    void* curr_child = nullptr;

    while(true) {
        found = false;
        if (curr_word.length() < initial_length) {
            std::cout << "Curr node: " << get_value(node)
                      << " num childs: " << get_children_count(node) << std::endl;

            for (size_t i = 0; i < get_children_count(node); i++) {
                curr_child = get_child_at(begin, i, node);
                std::string child_value = get_value(curr_child);
                std::cout << "- child value: <" << child_value << ">\n";
                int prefix = get_common_prefix(child_value, word);

                // There's a common prefix
                if (prefix != 0) {
                    std::cout << "Stepping in\n"; 
                    node = curr_child;
                    curr_word += word.substr(0, prefix);
                    word = word.substr(prefix);
                    found = true;
                    break;
                }
            }
        }

        // No child matches, return the result
        if (!found) {
            std::cout << " Return node: " << curr_word  << '\n';
            Word result(curr_word, get_frequency(node), 0);
            std::vector<Word> vect;
            vect.push_back(Word(result));
            return vect;
        }
    }
}