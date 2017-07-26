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
Trie::add_word(std::string words, uint32_t frequency)
{   
    std::string current_val;
    size_t prefix;
    bool has_inserted = false;

    for (int i = 0; i < this->children.size(); i++)
    {
        has_inserted = false;

        current_val = this->children[i]->value;
        prefix = get_common_prefix(current_val, words);
        if (prefix == 0) // No prefix in common
            continue;

        // Insertion of an intermediary child node called 'new_child'.
        this->children[i]->value = current_val.substr(prefix);
        auto new_child = new Trie(0, current_val.substr(0, prefix),
                                  std::vector<Trie::Trie*>());
        new_child->children.push_back(this->children[i]);
        this->children[i] = new_child;
        
        this->children[i]->add_word(words.substr(prefix), frequency);
    }

    auto child = new Trie(frequency, current_val.substr(0, prefix),
                          std::vector<Trie::Trie*>());
    this->children.push_back(child);

    /*
    auto letter = letters.substr(0, 1);
    if (!this->children.count(letter)) 
    {
        // Letter not present among children.
        auto* trie = new Trie(0, std::map<std::string, Trie::Trie*>(), letter, 0);
        this->children.insert(std::make_pair(letter, trie));
    }

    if (letters.size() == 1)
    {
        this->children[letter]->frequency = frequency;
        this->children[letter]->is_word = true;
    }
    else
    {
        this->children[letter]->add_word(letters.substr(1, letters.size()-1), frequency);
    }
    */
}