#include <iostream>
#include "utils.hh"


Trie *create_trie(std::string path) {
    std::ifstream dict(path);

    auto* root = new Trie(0, "", std::vector<Trie::Trie*>());

    std::string word;
    int frequency;
    while(dict >> word >> frequency)
        root->add_word_compressed(word, frequency);

    return root;
}