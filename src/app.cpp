#include <iostream>

#include "utils/utils.hh"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./TextMiningApp /path/to/compiled/dict.bin\n";
        exit(134);
    }
    std::string path(argv[1]);
    auto root = load_trie(path);


    return 0;
}
