#pragma once

#include <fstream>

#include "../trie/trie.hh"

Trie *create_trie(std::string path);
void save_trie(Trie* t, std::string path);
Trie load_trie(std::string path);
void print_trie(Trie* t);
void print_child(Trie* t, int indent_level);
void indent_print(int indent_level, std::string value);
