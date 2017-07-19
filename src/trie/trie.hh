#pragma once

#include <map>

class Trie
{
    bool is_word;
    std::map<std::string, Trie::Trie*> children;
    std::string characters;
    int frequency;

    Trie(bool is_word, std::map<std::string, Trie::Trie*> children,
         std::string characters, int frequency)
        : is_word(is_word)
        , children(children)
        , characters(characters)
        , frequency(frequency)
    {}

    void add_word(std::string letters, int frequency);
};

# include "trie.hxx"

