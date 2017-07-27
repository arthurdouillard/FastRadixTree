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
    this->offset = *offset;
    *offset += len_written;

    for (int i = 0; i < this->children->size(); i++)
    {
        this->children->at(i).walk(stream, offset);

        if (i >= 1 && i != this->children->size()-1)
        {
            unsigned long left_offset = this->children->at(i-1).offset;
            unsigned long right_offset = this->children->at(i).offset;

            this->children->at(i).write_offset(stream, left_offset, right_offset);
        }
        else if (i == this->children->size()-1)
            this->children->at(i).write_offset(stream,
                                               this->children->at(i).offset,
                                               0);
    }
}

inline std::vector<Word>
Trie::search_close_words(std::string word, int distance) {
    if (distance == 0)
        return this->exact_search(word);
}

inline std::vector<Word>
Trie::exact_search(std::string word) {
    bool found;
    size_t initial_length = word.length();
    auto node = *this;
    std::string curr_word("");

    while(true) {
        found = false;
        if (curr_word.length() < initial_length) {
            for (size_t i = 0; i < node.children->size(); i++) {
                auto curr_child = node.children->at(i);
                int prefix = get_common_prefix(curr_child.value, word);

                // There's a common prefix
                if (prefix != 0) {
                    node = curr_child;
                    curr_word += word.substr(0, prefix);
                    word = word.substr(prefix);
                    found = true;
                    break;
                }
            }
        }

        // No child matches, return the result
        if (!found) {
            Word result(curr_word, node.frequency, 0);
            std::vector<Word> vect;
            vect.push_back(Word(result));
            return vect;
        }

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
    char size_buffer[sizeof(size_t)];
    unsigned long default_brother_loc = 0;
    size_t child_size = this->children->size();
    total_size += sizeof(this->frequency) + sizeof(value_char) + sizeof(size_t) 
                                          + sizeof(default_brother_loc);

    stream.write(reinterpret_cast<const char *>(&this->frequency),
                 sizeof(this->frequency));
    stream.write(value_char, sizeof(value_char));
    stream.write(reinterpret_cast<const char*>(&child_size)
                , sizeof(size_t));
    stream.write(reinterpret_cast<const char *>(&default_brother_loc),
                 sizeof(default_brother_loc));
    return total_size;
}

inline void 
Trie::write_offset(std::ofstream& stream, unsigned long offset,
                   unsigned long next_offset)
{
    long base_offset = stream.tellp();
    long write_offset = offset + sizeof(this->frequency)
                               + sizeof(this->value.c_str())
                               + sizeof(size_t);
    stream.seekp(write_offset);
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