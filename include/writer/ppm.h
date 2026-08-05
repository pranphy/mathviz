#pragma once

#include <string>
#include <vector>

void write_ppm(const std::string& filename, int width, int height, const std::vector<unsigned char>& buffer);
