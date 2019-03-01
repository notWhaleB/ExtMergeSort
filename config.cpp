#include "config.h"

ArgsParseStatus szParse(const std::string &strVal, size_t &value) {
    const static std::vector<std::pair<std::string, size_t>> SizeUnits = {
            {"KB", KB}, {"KiB", KB}, {"MB", MB}, {"MiB", MiB}, {"GB", GB}, {"GiB", GiB}
    };

    for (auto &&unit : SizeUnits) {
        size_t pos = strVal.find(unit.first);
        try {
            if (pos != std::string::npos) {
                value = static_cast<size_t>(std::stold(strVal.substr(0, pos)) * unit.second);
                return ArgsParseStatus::OK;
            }
        } catch (std::exception &e) {
            return ArgsParseStatus::ArgumentIncorrect;
        }
    }

    value = std::stoul(strVal);

    return ArgsParseStatus::OK;
}

ArgsParseStatus config(const std::vector<std::string> &args, std::string &filenameIn,
                       std::string &filenameOut, size_t &memoryLimit, size_t &blockSize, bool &textMode) {

    if (args.size() < 4) {
        return ArgsParseStatus::ArgsNumberIncorrect;
    }

    filenameIn = args[0];
    filenameOut = args[1];

    // Expecting --ml argument.
    ArgsParseStatus status = ArgsParseStatus::ArgsNumberIncorrect;

    for (size_t i = 2; i != args.size(); ++i) {
        std::string arg = args[i];
        if (arg == "--ml") {
            if (i == args.size()) {
                return ArgsParseStatus::ArgumentIncorrect;
            }

            status = szParse(args[++i], memoryLimit);
            if (status != ArgsParseStatus::OK) {
                return status;
            }

            if (memoryLimit < 128 * MB) {
                return ArgsParseStatus::MemoryLimitTooSmall;
            }
        } else if (arg == "--bs") {
            if (i == args.size()) {
                return ArgsParseStatus::ArgumentIncorrect;
            }

            status = szParse(args[++i], blockSize);
            blockSize &= ~0x111;
            if (status != ArgsParseStatus::OK) {
                return status;
            }
        } else if (arg == "--text") {
            textMode = true;
        } else {
            return ArgsParseStatus::ArgumentIncorrect;
        }
    }

    if (status == ArgsParseStatus::ArgsNumberIncorrect) {
        return status;
    }

    if (memoryLimit / blockSize < 4) {
        return ArgsParseStatus::BufferSizeTooSmall;
    }

    return ArgsParseStatus::OK;
}
