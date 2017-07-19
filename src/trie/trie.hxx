#pragma once

#include "trie.hh"

void
Trie::add_word(std::string letters, int frequency)
{
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
        this->children[letter]->add_word(letters.substr(1, letters.size()-1), frequency);
}


