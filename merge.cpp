#include "merge.h"

struct MergeRun {
    off_t offset;
    off_t end;
    std::deque<uint64_t> numbers;

    MergeRun() {
        offset = 0;
        end = 0;
    };
};

Merge::Merge(size_t bufferSz, size_t memoryLimit) :
        _bufferSz(bufferSz), _memoryLimit(memoryLimit) {};

size_t Merge::mergeStep(std::ifstream &in, std::ofstream &out, size_t curChunkSz, bool sortInput) const {
    size_t nRuns = _memoryLimit / _bufferSz - 2;
    size_t chunkIdx = 0;

    while (!in.eof()) {
        std::vector<MergeRun> runs(nRuns);
        for (size_t idxOffset = chunkIdx; chunkIdx - idxOffset != nRuns; ++chunkIdx) {
            runs[chunkIdx - idxOffset].offset = static_cast<ssize_t>(chunkIdx * curChunkSz);
            runs[chunkIdx - idxOffset].end = static_cast<ssize_t>((chunkIdx + 1) * curChunkSz - 1);
            _fill_run(runs[chunkIdx - idxOffset], in, sortInput);
        }

        std::priority_queue<
                std::pair<int_t, MergeRun *>,
                std::vector<std::pair<int_t, MergeRun *>>,
                std::greater<std::pair<int_t, MergeRun *>>
        > minTree;

        for (auto &run : runs) {
            if (!run.numbers.empty()) {
                minTree.push(std::make_pair(run.numbers.front(), &run));
                run.numbers.pop_front();
            }
        }

        std::vector<int_t> buf;
        while (!minTree.empty()) {
            buf.push_back(minTree.top().first);

            if (buf.size() * sizeof(int_t) >= _bufferSz) {
                write_binary(out, buf);
                buf.clear();
            }

            MergeRun *run = minTree.top().second;
            minTree.pop();

            if (run->numbers.empty()) {
                _fill_run(*run, in, sortInput);
            }

            if (!run->numbers.empty()) {
                minTree.push(std::make_pair(run->numbers.front(), run));
                run->numbers.pop_front();
            }
        }

        if (!buf.empty()) {
            out.write(reinterpret_cast<const char *>(buf.data()), sizeof(int_t) * buf.size());
            buf.clear();
        }
    }

    return curChunkSz * nRuns;
}

void Merge::_fill_run(MergeRun &run, std::ifstream &in, bool sort) const {
    std::vector<int_t> buf(_bufferSz / sizeof(int_t));

    if (run.offset > run.end) {
        return;
    }

    read_binary(in, buf, _bufferSz, run.offset);
    if (sort) {
        std::sort(buf.begin(), buf.end());
    }

    while (!buf.empty()) {
        run.numbers.push_front(buf.back());
        buf.pop_back();
    }

    run.offset += _bufferSz;
}
