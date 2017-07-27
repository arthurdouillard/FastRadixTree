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
    bool final_node = true;
    size_t prefix;

    while (true) {
        for (size_t i = 0; i < node->children->size(); i++)
        {
            final_node = true;
            auto curr_child = node->children->at(i);

            prefix = get_common_prefix(curr_child->value, word);
            if (prefix == 0)
                continue;

            if (prefix == node->children->at(i)->value.length())
                node = node->children->at(i).get();
            else
            {
                curr_child->value = curr_child->value.substr(prefix);
                auto new_child = std::make_shared<Trie>(0, word.substr(0, prefix));
                new_child->children->push_back(curr_child);
                node->children->at(i) = new_child;
                node = new_child.get();
            }
            
            final_node = false;
            word = word.substr(prefix);
            break;
        }

        if (final_node || !node->children->size())
        {
            auto child = std::make_shared<Trie>(frequency, word);
            node->children->push_back(child);
            break;
        }
    }
}