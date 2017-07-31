#include <iostream>
#include <sys/stat.h>

#include "utils/utils.hh"

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
