#include <iostream>

#include "utils/utils.hh"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./TextMiningApp /path/to/compiled/dict.bin\n";
        exit(134);
    }

    std::string approx;
    std::string word;
    int distance;

    auto mmap_addr = mmap_file(argv[1]);
    while (std::cin >> approx >> distance >> word) {
       auto words = search_close_words(mmap_addr, word, distance);
       pretty_print(words);
    }

    return 0;
}
