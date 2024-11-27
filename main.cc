#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <windows.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "stb_image_write.h"

struct Pixel {
  unsigned char r, g, b;
};

struct PPMImage {
  int width;
  int height;
  int max_color_value;
  std::vector<Pixel> pixels;
};

std::ifstream& operator>>(std::ifstream& ifs, Pixel& pixel) {
  std::string num;
  ifs >> num;
  pixel.r = std::atoi(num.c_str());
  ifs >> num;
  pixel.g = std::atoi(num.c_str());
  ifs >> num;
  pixel.b = std::atoi(num.c_str());
  return ifs;
}

PPMImage ReadPPM(const std::filesystem::path& filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file.");
  }

  std::string format;
  file >> format;
  if (format != "P3") {
    throw std::runtime_error("Unsupported PPM format.");
  }

  int width, height, max_color_value;
  file >> width >> height >> max_color_value;

  if (max_color_value != 255) {
    throw std::runtime_error("Unsupported max color value");
  }

  PPMImage image;
  image.width = width;
  image.height = height;
  image.max_color_value = max_color_value;
  image.pixels.reserve(width * height);

  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      Pixel pixel;
      file >> pixel;
      image.pixels.push_back(pixel);
    }
  }

  return image;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <path_to_file>" << std::endl;
    return 1;
  }

  try {
    std::filesystem::path filename = argv[1];
    PPMImage image = ReadPPM(filename);
    std::cout << "File Name: " << std::filesystem::absolute(filename).string()
              << '\n';
    std::cout << "Width: " << image.width << "\n";
    std::cout << "Height: " << image.height << "\n";
    std::cout << "Max Color Value: " << image.max_color_value << "\n";
    stbi_write_png("result.png", image.width, image.height, 3,
                   image.pixels.data(), image.width * 3);

    // Convert std::string to LPCSTR and open file by ShellExecute
    LPCSTR lpFile = "result.png";
    HINSTANCE result =
        ShellExecute(NULL, "open", lpFile, NULL, NULL, SW_SHOWNORMAL);

    // If result is less than or equal to 32, there was an error
    if ((int)result <= 32) {
      std::cerr << "Error opening file. Error code: " << (int)result
                << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
  return 0;
}
