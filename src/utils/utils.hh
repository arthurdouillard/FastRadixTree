#pragma once

#include <fstream>

#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../trie/trie.hh"

Trie *create_trie(std::string path);
void *mmap_file(char* path);
void print_trie(Trie* t);
void print_child(Trie* t, int indent_level);
void indent_print(int indent_level, std::string value);
void pretty_print(std::vector<Word> vect);
std::vector<Word> search_close_words(void* begin, std::string word, int distance);
std::vector<Word> exact_search(void* begin, std::string word);
