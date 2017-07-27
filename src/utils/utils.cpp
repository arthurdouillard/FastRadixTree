#include <iostream>
#include "utils.hh"
#include "../word/word.hh"

Trie *create_trie(std::string path) {
    std::ifstream dict(path);

    auto* root = new Trie(0, "");
    std::string word;
    int frequency;

    while(dict >> word >> frequency)
        root->add_word_compressed(word, frequency);

    return root;
}

// FIXME -- For debug
void indent_print(int indent_level, std::string value) {
    while (indent_level > 0) {
        std::cout << "--";
        indent_level--;
    }
    std::cout << value << '\n';
}

void print_child(Trie node, int indent_level) {
    indent_print(indent_level, node.value);

    for (size_t i = 0; i < node.children->size(); i++) {
        auto curr_child = node.children->at(i);
        print_child(curr_child, ++indent_level);
    }
}

void print_trie(Trie* t) {
    print_child(*t, 0);
}

void pretty_print(std::vector<std::shared_ptr<Word>> vect) {
    if (vect.size() == 0)
        return;

    std::cout << "[";
    for (size_t i = 0; i < vect.size(); i++) {
        auto curr_word = vect.at(i);
        std::cout << "{\"word:\"" << "\"" << curr_word->get_content()<< "\","
                  << "\"freq:\"" << curr_word->get_frequency() << ","
                  << "\"distance:\"" << curr_word->get_distance() << "}";
        if (i != vect.size() -1)
            std::cout << ',';
    }
    std::cout << "]\n";
}