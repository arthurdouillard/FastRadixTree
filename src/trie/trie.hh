#pragma once

#include <vector>
#include <string>

class Trie
{
public:
    uint32_t frequency;
    std::string value;
    std::vector<Trie::Trie*> children;

    Trie(uint32_t frequency, std::string value, std::vector<Trie::Trie*> children)
        : frequency(frequency)
        , children(children)
        , value(value)
    {}

    void add_word(std::string, uint32_t);
};

# include "trie.hxx"

