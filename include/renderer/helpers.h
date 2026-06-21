#ifndef HELPERS_H
#define HELPERS_H

#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <any>
#include <algorithm>
#include <assert.h>

std::vector<std::string> split(std::string s, const std::string& delimiter);
std::any ParsePrimitive(std::string text, int &start);
std::any ParseJSONValue(std::string text, int &start);
std::pair<std::string, std::any> ParseJSONKeyValue(std::string text, int &start);
std::map<std::string, std::any> ParseJSONElement(std::string text, int &start);
std::vector<std::any> ParseJSONArray(std::string text, int &start);
std::string ReadJSONFile(const std::string &filename);
std::map<std::string, std::any> ParseJSONFile(const std::string &filename);
float clamp(float v, float lo, float hi);
int clamp(int v, int lo, int hi);

#endif