#include <iostream>
#include <sys/stat.h>

#include "utils/utils.hh"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./TextMiningApp /path/to/compiled/dict.bin\n";
        abort();
    }

    struct stat fs;
    std::string approx;
    std::string word;
    int distance;

    int stat_res = stat(argv[1], &fs);
    if (stat_res == -1)
    {
        std::cerr << "File not found: " << argv[1] << '\n';
        exit(255);
    }

    auto mmap_addr = mmap_file(argv[1]);
    while (std::cin >> approx >> distance >> word) {
       auto words = search_close_words(mmap_addr, word, distance);
       pretty_print(words);
       words.clear();
    }
    return 0;
}
