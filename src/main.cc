// �~�N: SSE > AVX > AVX2 > AVX512
// bit: 128 > 256 > 256 > 512

// SSE
// �]�A SSE�BSSE2�BSSE3�BSSSE3�BSSE4.1 �M SSE4.2
// ��@���B�z 4 �� 32 ��B�I�Ʃ� 2 �� 64 ��B�I��
// ��B��l�Ҧ��Aa = a + b

// AVX
// ��@���B�z 8 �� 32 ��B�I�Ʃ� 4 �� 64 ��B�I��
// ����s���T�ާ@�Ʈ榡 c = a + b

// AVX2
// �X�i�F AVX�A�W�[�F�� 256 ���ƾާ@�����
// �ޤJ�F��h����ƫ��O�A�p�v��ާ@�B���k�M�޿�ާ@
// ��� 512 ��H�s���A��@���B�z 16 �� 32 ��B�I�Ʃ� 8 �� 64 ��B�I��

// Register: XMM > YMM > ZMM
// instruction: SSE > AVX/AVX2 > AVX512
// bits: 128 > 256 > 512
// ���~�AYMM �� lower bits �P XMM �@��

// #include <iostream>
//
// int main() {
//   float* bigArray1 = new float[16384];
//   float* bigArray2 = new float[16384];
//   float* bigArray3 = new float[16384];
//
//   for (size_t i = 0; i < 16384; i++) {
//     bigArray1[i] = 20.0f;
//     bigArray2[i] = 50.0f;
//     bigArray3[i] = 0.0f;
//   }
//
//   for (size_t i = 0; i < 16384; i++) {
//     bigArray3[i] = bigArray1[i] + bigArray2[i];
//   }
//
//   delete[] bigArray1;
//   delete[] bigArray2;
//   delete[] bigArray3;
//   return 0;
// }
//

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
