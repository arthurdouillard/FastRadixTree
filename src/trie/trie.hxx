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
        for (int i = 0; i < node->children.size(); i++)
        {
            has_inserted = false;

            prefix = get_common_prefix(node->children[i]->value, word);
            if (prefix == 0)
                continue;

            node->children[i]->value = node->children[i]->value.substr(prefix);
            auto* new_child = new Trie(0, word.substr(0, prefix),
                                       std::vector<Trie::Trie*>{node->children});
            node->children[i] = new_child;

            node = new_child;
            word = word.substr(prefix);
            has_inserted = true;
            break;
        }

        if (!has_inserted)
        {
            auto* child = new Trie(frequency, word, std::vector<Trie::Trie*>());
            node->children.push_back(child);
            break;
        }
    }
    /*
    std::string current_val;
    size_t prefix;

    for (int i = 0; i < this->children.size(); i++)
    {
        current_val = this->children[i]->value;
        prefix = get_common_prefix(current_val, words);
        if (prefix == 0) // No prefix in common
            continue;

        // Insertion of an intermediary child node called 'new_child'.
        this->children[i]->value = current_val.substr(prefix);
        auto* new_child = new Trie(0, current_val.substr(0, prefix),
                                  std::vector<Trie::Trie*>());
        new_child->children.push_back(this->children[i]);
        this->children[i] = new_child;
        
        this->children[i]->add_word(words.substr(prefix), frequency);
    }

    auto* child = new Trie(frequency, current_val.substr(0, prefix),
                          std::vector<Trie::Trie*>());
    this->children.push_back(child);
    */
}

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
}