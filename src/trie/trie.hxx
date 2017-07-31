#pragma once

#include <stdio.h>

inline size_t
get_common_prefix(std::string &w1, std::string &w2)
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
Trie::add_word_compressed(std::string word, uint32_t frequency, int i)
{
    bool final_node = true;
    auto node = this;
    bool flag = false;
    size_t prefix;

    if(false)
        flag = true;

    if (flag)
        std::cout << "\n--- " << word << " | " << frequency << " ---\n";

    while (true)
    {
        for (size_t i = 0; i < node->children->size(); i++)
        {
            final_node = true;
            auto curr_child = node->children->at(i);

            prefix = get_common_prefix(curr_child->value, word);
            if (prefix == 0)
                continue; // No common prefix, going to the next brother.

            // A prefix is existing but not whole. Going to its children.
            if (prefix == curr_child->value.length())
                node = curr_child.get();
            else
            {
                // Need for splitting.
                if (flag)
                    std::cout << "Before split: " << curr_child->value << '\n'; 
                curr_child->value = curr_child->value.substr(prefix);
                auto new_child = std::make_shared<Trie>(0, word.substr(0, prefix));

                if (flag)
                {
                    std::cout << "Split:\n\t<" << new_child->value << "> " << new_child->frequency
                              << "\n\t-- <" << curr_child->value << "> " << curr_child->frequency << std::endl;
                }

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
            if (!word.size()) // Updating an existing node.
            {
                if (flag)
                    std::cout  << "upd <" << node->value << "> " << frequency << std::endl;          
                node->frequency = frequency;
            }
            else // Creating a new final node.
            {
                if (flag)
                    std::cout << "new <" << word << "> " << frequency << std::endl;
                auto child = std::make_shared<Trie>(frequency, word);
                node->children->push_back(child);
            }

            return;
        }
    }
}

inline void
Trie::save_trie(std::string path)
{
    std::ofstream stream(path, std::ios::binary);
    auto offset = std::make_shared<unsigned long>(0);

    this->walk(stream, offset);
}

inline void
Trie::walk(std::ofstream &stream, std::shared_ptr<unsigned long> &offset)
{
    auto len_written = this->write_trie(stream);
    this->offset = *offset;
    *offset += len_written;
   
    for (size_t i = 0; i < this->children->size(); i++)
        this->children->at(i)->walk(stream, offset);

    // Now all children have got an offset.
    for (size_t i = 0; i < this->children->size(); i++)
    {
        auto cur_offset = this->children->at(i)->offset;
        auto bro_offset = (i == this->children->size() - 1) ? 0 : this->children->at(i + 1)->offset;
        this->children->at(i)->write_offset(stream, cur_offset, bro_offset);
    }
}

// freq is uint32_t
// child number is uint32_t
// value has variable length
// offsets are unsigned longs
inline size_t
Trie::write_trie(std::ofstream &stream)
{
    size_t total_size = 0;
    auto value_char = this->value.c_str();
    unsigned long default_brother_loc = 0;
    uint32_t child_size = this->children->size();
    total_size += sizeof(uint32_t) * 2 + this->value.size() + 1 + sizeof(default_brother_loc);
    stream.write(reinterpret_cast<const char *>(&this->frequency),
                 sizeof(uint32_t));
    stream.write(reinterpret_cast<const char *>(&child_size), sizeof(uint32_t));
    stream.write(value_char, this->value.size() + 1);
    stream.write(reinterpret_cast<const char *>(&default_brother_loc),
                 sizeof(default_brother_loc));
    stream.flush();
    return total_size;
}

inline void
Trie::write_offset(std::ofstream &stream, unsigned long offset,
                   unsigned long next_offset)
{
    long base_offset = stream.tellp();
    long write_offset = offset + sizeof(uint32_t) * 2 + this->value.size() + 1;
    stream.seekp(write_offset);
    stream.write(reinterpret_cast<const char *>(&next_offset),
                 sizeof(next_offset));
    stream.flush();
    stream.seekp(base_offset);
}