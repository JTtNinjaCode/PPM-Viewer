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

std::ifstream &operator>>(std::ifstream &ifs, Pixel &pixel) {
  std::string num;
  ifs >> num;
  pixel.r = std::atoi(num.c_str());
  ifs >> num;
  pixel.g = std::atoi(num.c_str());
  ifs >> num;
  pixel.b = std::atoi(num.c_str());
  return ifs;
}

PPMImage ReadPPM(const std::filesystem::path &filename) {
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

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <path_to_file> <output_file>" << std::endl;
    return 1;
  }

  try {
    std::filesystem::path input_filename = argv[1];
    std::filesystem::path output_filename = argv[2];

    PPMImage image = ReadPPM(input_filename);
    std::cout << "File Name: " << std::filesystem::absolute(input_filename).string() << '\n';
    std::cout << "Width: " << image.width << "\n";
    std::cout << "Height: " << image.height << "\n";
    std::cout << "Max Color Value: " << image.max_color_value << "\n";

    // Save the image to the specified output file
    if (output_filename.extension() == ".png") {
      stbi_write_png(output_filename.string().c_str(), image.width, image.height, 3, image.pixels.data(), image.width * 3);
    } else {
      std::cerr << "Unsupported output file format. Only PNG is supported." << std::endl;
      return 1;
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
  return 0;
}
