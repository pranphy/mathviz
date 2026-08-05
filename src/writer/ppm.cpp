#include <fstream>
#include <print>
#include <format>

#include "writer/ppm.h"

void write_ppm(const std::string& filename, int width, int height, const std::vector<unsigned char>& buffer) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::println(stderr, "Error: Failed to open file for writing: {}", filename);
        return;
    }

    // 1. Write the PPM header using standard formatting
    file << std::format("P6\n{} {}\n255\n", width, height);

    // 2. Write pixel data, flipping vertically from bottom-left to top-left
    int stride = width * 3;
    for (int y = height - 1; y >= 0; --y) {
        file.write(reinterpret_cast<const char*>(&buffer[y * stride]), stride);
    }

    file.close();
    std::println("Successfully saved PPM image to {}", filename);
}
