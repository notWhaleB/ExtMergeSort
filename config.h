#ifndef HSE_EM_CONFIG_H
#define HSE_EM_CONFIG_H

#include <vector>
#include <string>

#include "constants.h"

enum ArgsParseStatus { OK, ArgsNumberIncorrect, ArgumentIncorrect, MemoryLimitTooSmall, BufferSizeTooSmall };

ArgsParseStatus config(const std::vector<std::string> &args, std::string &filenameIn,
                       std::string &filenameOut, size_t &memoryLimit, size_t &blockSize, bool &textMode);

#endif //HSE_EM_CONFIG_H
