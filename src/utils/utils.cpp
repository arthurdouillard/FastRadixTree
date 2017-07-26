#include <iostream>
#include "utils.hh"


Trie *create_trie(std::string path) {
    std::ifstream dict(path);

    auto* root = new Trie(false, std::map<std::string, Trie::Trie*>(), "", 0);

    std::string word;
    int frequency;
    while(dict >> word >> frequency)
    {
        std::cout << word << " " << frequency << std::endl;
        root->add_word(word, frequency);
    }

    return root;
}