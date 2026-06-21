#include "renderer/helpers.h"

// String Processing

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

// JSON Processing

std::any ParsePrimitive(std::string text, int &start){
    std::any value;

    if (text[start] == '\"'){ // string
        int end = ++start;
        while(text[end] != '\"'){
            end++;
        }
        value = std::any(text.substr(start, end-start));

        start = end+1;
    }
    else{ // float
        int end = start;
        while(text[end] != ',' && text[end] != '}' && text[end] != ']'){
            end++;
        }
        value = std::any(std::stof(text.substr(start, end-start)));

        start = end;
    }

    return value;
}

std::any ParseJSONValue(std::string text, int &start){
    std::any value;
    if (text[start] == '{'){ // Element
        value = std::any(ParseJSONElement(text, start));
    }
    else if (text[start] == '['){ // Array
        value = std::any(ParseJSONArray(text, start));
    }
    else{
        value = ParsePrimitive(text, start);
    }

    return value;
}

std::pair<std::string, std::any> ParseJSONKeyValue(std::string text, int &start){
    assert(text[start] == '\"');
    start++;

    int end = start;
    while (text[end] != '\"'){
        end++;
    }
    
    std::string key = text.substr(start, end-start);

    assert(text[++end] == ':');

    start = ++end;

    std::any value = ParseJSONValue(text, start);

    return std::pair<std::string, std::any>(key, value);
}

std::map<std::string, std::any> ParseJSONElement(std::string text, int &start){
    assert(text[start] == '{');
    start++;

    std::map<std::string, std::any> result = std::map<std::string, std::any>();

    while(text[start] != '}'){
        std::pair<std::string, std::any> next_pair = ParseJSONKeyValue(text, start);
        result[next_pair.first] = next_pair.second;

        if (text[start] == ',') start++;
    }
    start++;

    return result;
}

std::vector<std::any> ParseJSONArray(std::string text, int &start){
    assert(text[start] == '[');
    start++;

    std::vector<std::any> result = std::vector<std::any>();

    while(text[start] != ']'){
        result.push_back(ParseJSONValue(text, start));

        if (text[start] == ',') start++;
    }
    start++;

    return result;
}

std::string ReadJSONFile(const std::string &filename){
    std::string result;

    std::string file_end = ".json";
    if (filename.length() < file_end.length() || filename.compare(filename.length() - file_end.length(), file_end.length(), file_end)){
        printf("Scene: Error, invalid file type (got: %s)\n", filename.c_str());
        return result;
    }

    std::string line;
    std::ifstream file(filename);

    if (!file.is_open()){
        printf("Scene: Error, failed to open file, filename: %s\n", filename.c_str());
        return result;
    }

    while(getline(file, line)){
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
        result.append(line);
    }
    file.close();

    return result;
}

std::map<std::string, std::any> ParseJSONFile(const std::string &filename){
    std::string file = ReadJSONFile(filename);
    int start = 0;
    return ParseJSONElement(file, start);
}

// Math

float clamp(float v, float lo, float hi){
    return std::max(std::min(v, hi), lo);
}

int clamp(int v, int lo, int hi){
    return std::max(std::min(v, hi), lo);
}