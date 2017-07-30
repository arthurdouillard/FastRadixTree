#pragma once

inline int Word::get_frequency(){
    return this->frequency;
}

inline int Word::get_distance(){
    return this->distance;
}

inline std::string Word::get_content(){
    return this->content;
}

inline void Word::set_distance(int value){
    this->distance = value;
}