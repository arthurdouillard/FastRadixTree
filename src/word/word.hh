#pragma once

#include <string>

class Word
{

public:

/** 
 *   @brief Word constructor 
 *
 *   @param content the word's content
 *   @param frequency the word's frequency
 *   @param distance the distance at which the word was found
 *   @return The word 
 */  
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