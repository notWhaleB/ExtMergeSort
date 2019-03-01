#ifndef HSE_EM_MERGE_H
#define HSE_EM_MERGE_H

#include <fstream>
#include <vector>
#include <queue>
#include <deque>
#include <algorithm>

#include "constants.h"
#include "io.h"

struct MergeRun;

class Merge {
public:
    Merge(size_t bufferSz, size_t memoryLimit);
    size_t mergeStep(std::ifstream &in, std::ofstream &out, size_t curChunkSz, bool sortInput) const;

private:
    size_t _bufferSz;
    size_t _memoryLimit;

    void _fill_run(MergeRun &run, std::ifstream &in, bool sort) const;
};

#endif //HSE_EM_MERGE_H
