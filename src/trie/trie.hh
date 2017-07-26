#pragma once

#include <vector>
#include <string>
#include <fstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


class Trie
{
public:
    uint32_t frequency;
    std::string value;
    std::vector<std::shared_ptr<Trie>> *children;

    Trie() {}
    Trie(uint32_t frequency, std::string value)
        : frequency(frequency)
        , value(value)
    {
        children = new std::vector<std::shared_ptr<Trie>>();
    }

    void add_word_compressed(std::string, uint32_t);
    void save_trie(Trie* t, std::string path);
    Trie* load_trie(std::string path);


private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
            ar & frequency;
            ar & value;
            for (size_t i = 0; i < children->size(); i++) {
                auto curr_node = children->at(i);
                ar & *curr_node;
            }
    }
};

# include "trie.hxx"

