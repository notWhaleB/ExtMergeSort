#ifndef HSE_EM_IO_H
#define HSE_EM_IO_H

#include <fstream>
#include <vector>

#include "constants.h"

size_t read_binary(std::ifstream &in, std::vector<int_t> &numbers, size_t bufSize);

size_t read_binary(std::ifstream &in, std::vector<int_t> &numbers, size_t bufSize, ssize_t offset);

size_t read_lines(std::ifstream &in, std::vector<int_t > &numbers, size_t bufSize);

size_t write_binary(std::ofstream &out, const std::vector<int_t> &buf);

size_t write_lines(std::ofstream &out, const std::vector<int_t> &buf);

#endif //HSE_EM_IO_H