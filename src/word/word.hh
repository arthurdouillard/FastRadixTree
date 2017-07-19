#pragma once

#include <string>

class Word
{
    std::string content;
    int frequency;
    int distance;

    Word(std::string content, int frequency, int distance)
        : content(content)
        , frequency(frequency)
        , distance(distance)
    {}
};

# include "word.hxx"