#include <iostream>
#include "utils.hh"


Trie *create_trie(std::string path) {
    std::ifstream dict(path);

    auto* root = new Trie(0, "");
    std::string word;
    int frequency;
    while(dict >> word >> frequency)
        root->add_word_compressed(word, frequency);

    return root;
}

Trie* load_trie(std::string path) {
    Trie t;
    std::ifstream ifs(path);
    {
        boost::archive::text_iarchive ia(ifs);
        ia >> t;
    }
    return t;
}

void save_trie(Trie* t, std::string path) {
    std::ofstream ofs(path);
    {
        boost::archive::text_oarchive oa(ofs); 
        oa << *t;
    }
}

// FIXME -- For debug
void indent_print(int indent_level, std::string value) {
    while (indent_level > 0) {
        std::cout << "--";
        indent_level--;
    }
    std::cout << value << '\n';
}

void print_child(Trie* t, int indent_level) {
    auto node = t;
    indent_print(indent_level, t->value);

    for (size_t i = 0; i < node->children->size(); i++) {
        auto curr_child = node->children->at(i);
        print_child(curr_child.get(), indent_level++);
    }
}

void print_trie(Trie* t) {
    print_child(t, 0);
}