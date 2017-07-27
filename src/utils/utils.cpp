#include <iostream>
#include "utils.hh"
#include "../word/word.hh"

Trie *create_trie(std::string path) {
    std::ifstream dict(path);

    auto* root = new Trie(0, "");
    std::string word;
    int frequency;

    int i = 0;
    while(dict >> word >> frequency)
    {
        if (i++ > 10)
            break;
        root->add_word_compressed(word, frequency);
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

// FIXME -- For debug
void indent_print(int indent_level, std::string value) {
    while (indent_level > 0) {
        std::cout << "--";
        indent_level--;
    }
    std::cout << value << '\n';
}

void print_child(Trie node, int indent_level)
{
    indent_print(indent_level, node.value);

    for (size_t i = 0; i < node.children->size(); i++) {
        auto curr_child = node.children->at(i);
        print_child(curr_child, ++indent_level);
    }
}

void print_trie(Trie* t)
{
    print_child(*t, 0);
}

void pretty_print(std::vector<Word> vect) {
    if (vect.size() == 0)
        return;

    std::cout << "[";
    for (size_t i = 0; i < vect.size(); i++) {
        auto curr_word = vect.at(i);
        std::cout << "{\"word:\"" << "\"" << curr_word.get_content()<< "\","
                  << "\"freq:\"" << curr_word.get_frequency() << ","
                  << "\"distance:\"" << curr_word.get_distance() << "}";
        if (i != vect.size() -1)
            std::cout << ',';
    }
    std::cout << "]\n";
}

// Offset is at the beginning of the struct
void* get_child(void* offset) {
    char* ptr = (char*)offset;
    // Jump at the end of the frequency
    // Go to the end of the string
    // Jump to the end of the brother's offset
    ptr += sizeof(uint32_t);
    while (*ptr != '\0') {
        ptr++;
    }
    ptr += sizeof(unsigned long);
    return ptr;
}

// Offset is at the beginning of the struct
size_t get_child_num(void* offset) {
    char* ptr = (char*)offset;
    ptr += sizeof(uint32_t);
    while (*ptr != '\0') {
        ptr++;
    }
    return (size_t)ptr;
}

std::vector<Word>
search_close_words(void* begin, std::string word, int distance)
{
    if (distance == 0)
        return exact_search(begin, word);
}

std::vector<Word>
exact_search(void* begin, std::string word)
{
    bool found;
    size_t initial_length = word.length();
    std::string curr_word("");
    void* ptr = begin;
    std::cout << get_child_num(ptr);

    /*while(true) {
        found = false;
        if (curr_word.length() < initial_length) {
            for (size_t i = 0; i < node.children->size(); i++) {
                auto curr_child = node.children->at(i);
                int prefix = get_common_prefix(curr_child.value, word);

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
        if (!found) {
            Word result(curr_word, node.frequency, 0);
            std::vector<Word> vect;
            vect.push_back(Word(result));
            return vect;
        }
    }*/
}