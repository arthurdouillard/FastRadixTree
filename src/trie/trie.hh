#pragma once

#include <map>
#include <vector>

class Trie
{
public:
    int frequency;
    std::string value 
    std::vector<Trie::Trie*> children;

    Trie(int frequency, std::string value, std::vector<Trie::Trie*> children,
        : frequency(frequency)
        , children(children)
        , value(value)
    {}

    void add_word(std::string, int);
};

# include "trie.hxx"

