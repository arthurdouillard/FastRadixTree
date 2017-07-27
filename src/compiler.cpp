#include <iostream>

#include "utils/utils.hh"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./TextMiningCompiler /path/to/word/freq.txt /path/to/output/dict.bin\n";
        exit(134);
    }

    std::string path(argv[1]);
    std::string bin_path(argv[2]);
    auto* root = create_trie(path);
    root->save_trie(bin_path);
    return 0;
}
