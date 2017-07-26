#include <iostream>
#include "utils.hh"


Trie *create_trie(std::string path) {
    std::ifstream dict(path);

    auto* root = new Trie(0, "");


    std::string word;
    int frequency;
  //  int i = 0;
    while(dict >> word >> frequency)
    {
       // std::cout << i++ << std::endl;
        root->add_word_compressed(word, frequency);
    }

    return root;
}