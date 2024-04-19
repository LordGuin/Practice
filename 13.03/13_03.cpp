#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <string>
#include <omp.h>
#include <cctype>
#include <vector>
#include <algorithm>

std::vector<std::string> file_to_words(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string line;
    std::vector<std::string> words;
    while (std::getline(file, line)) {

        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string temp;
        while (ss >> temp) {
            temp.erase(std::remove_if(temp.begin(), temp.end(), ::ispunct), temp.end());
            if (!temp.empty()) {
                tokens.push_back(temp);
            }
        }

        words.insert(words.end(), tokens.begin(), tokens.end());
    }
    return words;
}

int main() {
    setlocale(LC_ALL, "rus");
    std::string filepath = "input.txt";
    std::vector<std::string> words = file_to_words(filepath);

    std::unordered_map<std::string, int> frequency;
#pragma omp parallel for shared(frequency)
    for (size_t i = 0; i < words.size(); i++) {
        std::string word = words[i];
#pragma omp critical
        {
            frequency[word]++;
        }
    }

    std::vector<std::pair<std::string, int>> sorted_frequency(frequency.begin(), frequency.end());
    std::sort(sorted_frequency.begin(), sorted_frequency.end(),
        [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return a.second > b.second;
        });

    for (size_t i = 0; i < 10 && i < sorted_frequency.size(); i++) {
        std::cout << sorted_frequency[i].first << " - " << sorted_frequency[i].second << std::endl;
    }

    return 0;
}
