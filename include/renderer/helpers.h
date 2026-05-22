#ifndef HELPERS_H
#define HELPERS_H

#include <vector>
#include <iostream>

std::vector<std::string> split(std::string s, const std::string& delimiter);
float clamp(float v, float lo, float hi);
int clamp(int v, int lo, int hi);

#endif