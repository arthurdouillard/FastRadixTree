#pragma once

#include <stdio.h>


/** 
 *   @brief Returns the prefix length between two words   
 *  
 *   @param  w1 is a string containing a word
 *   @param  w2 is a string containing a word
 *   @return the prefix length
 */  
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


/** 
 *   @brief Adds a word in the Trie by splitting nodes if necessary 
 *  
 *   @param  word is the word to insert
 *   @param  frequency is the word's frequency
 *   @return void 
 */  
inline void
Trie::add_word_compressed(std::string word, uint32_t frequency)
{
    bool final_node = true;
    auto node = this;
    size_t prefix;

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
            else // Split the node
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
            if (!word.size()) // Updating an existing node.
                node->frequency = frequency;
            else // Creating a new final node.
            {
                auto child = std::make_shared<Trie>(frequency, word);
                node->children->push_back(child);
            }
            return;
        }
    }
}


/** 
 *   @brief Serializes the trie in the file indicated by path
 *  
 *   @param  path the path of the file
 *   @return void 
 */  
inline void
Trie::save_trie(std::string path)
{
    std::ofstream stream(path, std::ios::binary);
    auto offset = std::make_shared<unsigned long>(0);

    this->walk(stream, offset);
}

/** 
 *   @brief Walks the trie and serializes every node.
 *   The node is serialized then it's first child
 *   Finally, for each child, it's brother's offset is written
 *  
 *   @param  offset the current writting offset 
 *   @param  stream the file's stream 
 *   @return void 
 */  
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

/** 
 *   @brief Writes the node content into the stream 
 *  
 *   @param  stream the file's stream 
 *   @return number of bytes written 
 */  
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

/** 
 *   @brief Write the address of the node's brother in the node's structure 
 *  
 *   @param  stream the file's stream 
 *   @param  offset the node's offset 
 *   @param  next_offset the node's brother's offset 
 *   @return void 
 */  
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