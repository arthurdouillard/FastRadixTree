#pragma once

#include "trie.hh"


inline size_t
get_common_prefix(std::string& w1, std::string& w2)
{
    auto min_len = w1.size() > w2.size() ? w2.size() : w1.size();

    for (size_t i = 0; i < min_len; i++)
    {
        if (w1[i] != w2[i])
            return i;
    }

    return min_len;
}

inline void
Trie::add_word_compressed(std::string word, uint32_t frequency)
{   
    auto node = this;
    bool has_inserted = false;
    size_t prefix;

    while (true) {
        for (int i = 0; i < node->children->size(); i++)
        {
            has_inserted = false;
            auto* curr_child = node->children->at(i);

            prefix = get_common_prefix(curr_child->value, word);
            if (prefix == 0)
                continue;

            curr_child->value = curr_child->value.substr(prefix);
            auto* new_child = new Trie(0, word.substr(0, prefix));
            new_child->children->push_back(curr_child);
            node->children->at(i) = new_child;

            node = new_child;
            word = word.substr(prefix);
            has_inserted = true;
            break;
        }

        if (!has_inserted)
        {
            auto* child = new Trie(frequency, word);
            node->children->push_back(child);
            break;
        }
    }
}

/*
inline void
Trie::add_word(std::string word, uint32_t frequency)
{
    auto letter = word.substr(0, 1);

    int i = -1;
    for (int j = 0; j < this->children.size(); j++)
    {
        if (this->children[j]->value == letter)
        {
            i = j;
            break;
        }
    }

    if (i == -1) 
    {
        // Letter not present among children.
        auto* trie = new Trie(0, letter, std::vector<Trie::Trie*>());
        this->children.push_back(trie);
        i = this->children.size() - 1;
    }

    if (word.size() == 1)
        this->children[i]->frequency = frequency;
    else
        this->children[i]->add_word(word.substr(1, word.size()-1), frequency);
}*/