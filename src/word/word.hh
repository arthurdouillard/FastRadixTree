#pragma once

#include <string>

class Word
{

public:
    Word(std::string content, int frequency, int distance)
        : content(content)
        , frequency(frequency)
        , distance(distance)
    {}

    int get_distance();
    int get_frequency();
    std::string get_content();
    void set_distance(int);

private:
    std::string content;
    int frequency;
    int distance;
};

# include "word.hxx"