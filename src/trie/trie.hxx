#pragma once

inline size_t
get_common_prefix(std::string& w1, std::string& w2)
{
    auto min_len = w1.size() > w2.size() ? w2.size() : w1.size();

    for (size_t i = 0; i < min_len; i++)
    {
        if (w1[i] != w2[i])
            return i;
    }

    return min_len;
}

inline void
Trie::add_word_compressed(std::string word, uint32_t frequency)
{   
    auto node = this;
    bool final_node = true;
    size_t prefix;

    while (true) {
        for (size_t i = 0; i < node->children->size(); i++)
        {
            final_node = true;
            auto curr_child = node->children->at(i);

            prefix = get_common_prefix(curr_child->value, word);
            if (prefix == 0)
                continue;

            if (prefix == node->children->at(i)->value.length())
                node = node->children->at(i).get();
            else
            {
                curr_child->value = curr_child->value.substr(prefix);
                auto new_child = std::make_shared<Trie>(0, word.substr(0, prefix));
                new_child->children->push_back(curr_child);
                node->children->at(i) = new_child;
                node = new_child.get();
            }
            
            final_node = false;
            word = word.substr(prefix);
            break;
        }

        if (final_node || !node->children->size())
        {
            auto child = std::make_shared<Trie>(frequency, word);
            node->children->push_back(child);
            break;
        }
    }
}

inline std::vector<std::shared_ptr<Word>>
Trie::search_close_words(std::string word, int distance) {
    if (distance == 0)
        return this->exact_search(word);
}

inline std::vector<std::shared_ptr<Word>>
Trie::exact_search(std::string word) {
    bool found;
    auto node = this;
    std::string curr_word("");

    while(true) {
        std::cout << "Curr node: " << node->value
                  << " child size: " << node->children->size()
                  << " curr word: " << curr_word;
        found = false;
        for (size_t i = 0; i < node->children->size(); i++) {
            auto curr_child = node->children->at(i);
            std::cout << "child value: " << curr_child->value << '\n';
            int prefix = get_common_prefix(curr_child->value, word);

            // There's a common prefix
            if (prefix != 0) {
                node = curr_child.get();
                curr_word += word.substr(0, prefix);
                word = word.substr(prefix);
                std::cout << "Step on child\n";
                found = true;
                break;
            }
        }

        // No child matches, return the result
        if (!found) {
            std::cout << "Return result: " << curr_word << '\n';
            Word result(curr_word, node->frequency, 0);
            std::vector<std::shared_ptr<Word>> vect;
            vect.push_back(std::make_shared<Word>(result));
            return vect;
        }

    }
}

/*int
Trie::dist_search(Trie* node, std::string word, std::string curr_word, int curr_dist, int max_dist) {
    if (curr_dist > max_dist) {
        return curr_dist;
    }

    int res = 10, mdist = -1, sub = -1, trans = -1, del = -1;

    if (node->frequency == 0) {
        res = word.length();
    }

    if (curr_dist + 1 <= max_dist && word.length() > 0) {
        dist_search()
    }
}*/