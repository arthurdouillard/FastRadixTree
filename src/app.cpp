#include <iostream>

#include "utils/utils.hh"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./TextMiningApp /path/to/compiled/dict.bin\n";
        exit(134);
    }
    std::string test("Hello");
    std::cout << sizeof(uint8_t) << '\n';
    std::string path(argv[1]);
    std::string approx;
    int distance;
    std::string word;

    /*while (std::cin >> approx >> distance >> word) {
       auto words = root.search_close_words(word, distance);
       pretty_print(words);
    }*/
    return 0;
}
