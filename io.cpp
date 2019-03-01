#include "io.h"

size_t read_binary(std::ifstream &in, std::vector<int_t> &numbers, size_t bufSize, ssize_t offset) {
    size_t readSz = (bufSize / sizeof(int_t)) * sizeof(int_t);

    numbers.resize(bufSize / sizeof(int_t));

    in.clear();
    in.seekg(offset, in.beg);
    in.read(reinterpret_cast<char *>(numbers.data()), readSz);
    if (in.eof()) {
        numbers.resize(static_cast<size_t>(in.gcount() / sizeof(int_t)));
    }

    return static_cast<size_t>(in.gcount());
}

size_t read_binary(std::ifstream &in, std::vector<int_t> &numbers, size_t bufSize) {
    return read_binary(in, numbers, bufSize, 0);
}

size_t read_lines(std::ifstream &in, std::vector<int_t > &numbers, size_t bufSize) {
    size_t nLines = bufSize / sizeof(int_t);
    size_t count = 0;

    for (; count != nLines && !in.eof(); ++count) {
        std::string str;
        std::getline(in, str);
        if (str.empty()) {
            continue;
        }
        numbers.push_back(std::stoull(str));
    }

    return numbers.size();
}

size_t write_binary(std::ofstream &out, const std::vector<int_t> &buf) {
    out.write(reinterpret_cast<const char *>(buf.data()), sizeof(int_t) * buf.size());

    return buf.size();
}

size_t write_lines(std::ofstream &out, const std::vector<int_t> &buf) {
    for (auto &&num : buf) {
        std::string line = std::to_string(num) + "\n";
        out.write(line.c_str(), line.length());
    }

    return buf.size();
}
