#include <iostream>
#include <sys/stat.h>

#include "utils/utils.hh"

/** 
 *   @brief Starts the compiler . Reads the word from the file passed
 *   as an argument, creates the trie and serializes it.
 *   
 *   @return 0 if no exceptions occured. Aborts if the command is not valid 
 */  
int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./TextMiningCompiler /path/to/word/freq.txt /path/to/output/dict.bin\n";
        abort();
    }

    struct stat fs;
    std::string path(argv[1]);
    std::string bin_path(argv[2]);

    int stat_res = stat(path.c_str(), &fs);
    if (stat_res == -1)
    {
        std::cerr << "File not found: " << path << '\n';
        exit(255);
    }

    auto root = create_trie(path);
    root->save_trie(bin_path);
    return 0;
}
