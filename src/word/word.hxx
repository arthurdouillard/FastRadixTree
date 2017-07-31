#pragma once

/** 
 *   @brief Returns the word's frequency 
 *  
 *   @return the frequency 
 */  
inline int Word::get_frequency(){
    return this->frequency;
}

/** 
 *   @brief Returns the word's distance 
 *  
 *   @return the distance 
 */  
inline int Word::get_distance(){
    return this->distance;
}

/** 
 *   @brief Returns the word's content
 *  
 *   @return the content 
 */  
inline std::string Word::get_content(){
    return this->content;
}

/** 
 *   @brief Sets the node distance
 *  
 *   @return void 
 */  
inline void Word::set_distance(int value){
    this->distance = value;
}