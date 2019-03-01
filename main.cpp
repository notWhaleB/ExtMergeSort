#include "main.h"

inline std::string filename_by_step(size_t step) {
    return "./step-" + std::to_string(step);
}

inline size_t file_size(const std::string &filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    if (rc == 0) {
        return static_cast<size_t>(stat_buf.st_size);
    }

    return 0;
}

int main(int argc, char **argv) {
    std::string filenameIn, filenameOut;
    size_t memoryLimit = 0, bufferSize = 4 * MiB;
    bool textMode = false;

    std::vector<std::string> args;
    for (size_t i = 1; i != argc; ++i) {
        args.emplace_back(std::string(argv[i]));
    }

    std::string usageInfo = "Usage: <inputPath> <outputPath> --ml <memoryLimit> [--text] [--bs <bufferSize>]";

    ArgsParseStatus status = config(args, filenameIn, filenameOut, memoryLimit, bufferSize, textMode);
    switch (status) {
        case ArgsParseStatus::ArgsNumberIncorrect: {
            std::cerr << "Incorrect number of arguments." << std::endl;
            std::cerr << usageInfo << std::endl;
            return ArgsParseStatus::ArgsNumberIncorrect;
        }
        case ArgsParseStatus::ArgumentIncorrect: {
            std::cerr << "Incorrect usage of an argument." << std::endl;
            std::cerr << usageInfo << std::endl;
            return ArgsParseStatus::ArgsNumberIncorrect;
        }
        case ArgsParseStatus::MemoryLimitTooSmall: {
            std::cerr << "Memory limit is too small, minimum is 128MB." << std::endl;
            return ArgsParseStatus::MemoryLimitTooSmall;
        }
        case ArgsParseStatus::BufferSizeTooSmall: {
            std::cerr << "Buffer size is too small, buffer should be at least 4 times less than memory limit." << std::endl;
            return ArgsParseStatus::MemoryLimitTooSmall;
        }
        case ArgsParseStatus::OK: break;
    }

    Merge mrg(bufferSize, memoryLimit);

    std::ifstream tempIn;
    std::ofstream tempOut;

    std::vector<int_t> buf;

    size_t iterN = 0;
    size_t totalSz = 0;

    if (textMode) {
        std::ifstream in(filenameIn);

        tempOut.open(filename_by_step(iterN));

        while (!in.eof()) {
            read_lines(in, buf, bufferSize);

            tempOut.write(reinterpret_cast<const char *>(buf.data()), sizeof(int_t) * buf.size());
            totalSz += buf.size() * sizeof(int_t);

            buf.clear();
        }

        tempOut.close();
        tempOut.clear();

        in.close();
    } else {
        totalSz = file_size(filenameIn);
    }

    for (size_t chunkSz = bufferSize; iterN == 0 || chunkSz < totalSz; ++iterN) {
        if (iterN == 0 && !textMode) {
            tempIn.open(filenameIn, std::ios::in | std::ios::binary);
        } else {
            tempIn.open(filename_by_step(iterN), std::ios::in | std::ios::binary);
        }

        tempOut.open(filename_by_step(iterN + 1), std::ios::out | std::ios::binary);

        chunkSz = mrg.mergeStep(tempIn, tempOut, chunkSz, iterN == 0);

        tempOut.close();
        tempOut.clear();

        tempIn.close();
        tempIn.clear();

        if (iterN != 0 || textMode) {
            remove((filename_by_step(iterN)).c_str());
        }
    }

    if (textMode) {
        tempIn.open(filename_by_step(iterN), std::ios::in | std::ios::binary);
        std::ofstream out(filenameOut);

        size_t binaryReadOffset = 0;
        while (!tempIn.eof()) {
            binaryReadOffset += read_binary(tempIn, buf, bufferSize, binaryReadOffset);
            write_lines(out, buf);
            buf.clear();
        }

        out.close();
        tempIn.close();

        remove((filename_by_step(iterN)).c_str());
    } else {
        rename(filename_by_step(iterN).c_str(), filenameOut.c_str());
    }

    return 0;
}