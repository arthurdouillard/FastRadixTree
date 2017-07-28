#pragma once

#include <stdio.h>

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
    auto node = *this;
    bool final_node = true;
    size_t prefix;

    while (true) {
        for (size_t i = 0; i < node.children->size(); i++)
        {
            final_node = true;
            auto curr_child = node.children->at(i);

            prefix = get_common_prefix(curr_child.value, word);
            if (prefix == 0)
                continue;

            if (prefix == node.children->at(i).value.length())
                node = node.children->at(i);
            else
            {
                curr_child.value = curr_child.value.substr(prefix);
                auto new_child = Trie(0, word.substr(0, prefix));
                new_child.children->push_back(curr_child);
                node.children->at(i) = new_child;
                node = new_child;
            }
            
            final_node = false;
            word = word.substr(prefix);
            break;
        }

        if (final_node || !node.children->size())
        {
            auto child = Trie(frequency, word);
            node.children->push_back(child);
            break;
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
Trie::walk(std::ofstream& stream, std::shared_ptr<unsigned long>& offset)
{
    auto len_written = this->write_trie(stream);
    std::cout << "Writing node with val: " << this->value 
              << " at offset: " << *offset
              << " len written: " << len_written << '\n';
    this->offset = *offset;
    *offset += len_written;

    for (int i = 0; i < this->children->size(); i++)
        this->children->at(i).walk(stream, offset);

    // Now all children have got an offset.
    for (int i = 0; i < this->children->size(); i++)
    {
        auto cur_offset = this->children->at(i).offset;
        auto bro_offset = (i == this->children->size()-1) ? 0 : this->children->at(i+1).offset;

        this->children->at(i).write_offset(stream, cur_offset, bro_offset);
    }
}

// freq is uint32_t
// value has variable length
// child number is size_t
// offsets are unsigned longs
inline size_t
Trie::write_trie(std::ofstream& stream) {
    size_t total_size = 0;
    auto value_char = this->value.c_str();
    unsigned long default_brother_loc = 0;
    uint32_t child_size = this->children->size();
    total_size += sizeof(uint32_t) * 2 + this->value.size()  
                                          + sizeof(default_brother_loc);
    stream.write(reinterpret_cast<const char *>(&this->frequency),
                 sizeof(this->frequency));
    stream.write(reinterpret_cast<const char*>(&child_size)
                , sizeof(uint32_t));
    stream.write(value_char, this->value.size());
    stream.write(reinterpret_cast<const char *>(&default_brother_loc),
                 sizeof(default_brother_loc));
    return total_size;
}

inline void 
Trie::write_offset(std::ofstream& stream, unsigned long offset,
                   unsigned long next_offset)
{
    long base_offset = stream.tellp();
    std::cout << "curr offset: "  << offset
              << " bro offset: "  << next_offset
              << " base offset: " << base_offset << '\n';
    long write_offset = offset
                        + sizeof(uint32_t) * 2
                        + this->value.size();

    stream.seekp(write_offset);
    std::cout << "write offset: " << write_offset << '\n';
    stream.write(reinterpret_cast<const char*>(&next_offset),
                 sizeof(next_offset));
    stream.seekp(base_offset);

}

/*int
Trie::dist_search(Trie* node, std::string word, std::string curr_word, int curr_dist, int max_dist) {
    if (curr_dist > max_dist) {
        return curr_dist;
    }

    int res = 10, mdist = -1, sub = -1, trans = -1, del = -1;

    if (node->frequency == 0) {
        res = word.length();
    }

    if (curr_dist + 1 <= max_dist && word.length() > 0) {
        dist_search()
    }
}*/