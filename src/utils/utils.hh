#pragma once

#include <fstream>

#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>

#include "../trie/trie.hh"

struct Word_Struct
{
    std::vector<std::shared_ptr<Word>> *word_list;
    std::map<std::string, std::shared_ptr<Word>> *word_map;
};

std::unique_ptr<Trie> create_trie(std::string path);
void *mmap_file(char* path);
void print_trie(Trie* t);
void print_child(Trie* t, int indent_level);
void indent_print(int indent_level, std::string value);
void pretty_print(std::vector<std::shared_ptr<Word>>);
std::vector<std::shared_ptr<Word>> search_close_words(void* begin, std::string word, int distance);
std::vector<std::shared_ptr<Word>> exact_search(void* begin, std::string word);
int dist_search(void*, void*, std::string, int, int, std::string,
                Word_Struct*, char, int, std::string);

int substitution(void*, void*, std::string, int, int, std::string,
                 Word_Struct*, char, int, std::string);

int deletion(void*, void*, std::string, int, int, std::string,
             Word_Struct*, char, int, std::string);

int transposition(void*, void*, std::string, int, int, std::string,
                  Word_Struct*, char, int, std::string, std::string);

int insertion(void*, void*, std::string, int, int, std::string,
              Word_Struct*, char, int, int, std::string);
