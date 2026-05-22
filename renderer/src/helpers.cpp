#include "renderer/helpers.h"

std::vector<std::string> split(std::string s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}

float clamp(float v, float lo, float hi){
    return std::max(std::min(v, hi), lo);
}

int clamp(int v, int lo, int hi){
    return std::max(std::min(v, hi), lo);
}