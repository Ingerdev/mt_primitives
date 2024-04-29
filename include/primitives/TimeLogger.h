#ifndef PRIMITIVES_TIMELOGGER_H
#define PRIMITIVES_TIMELOGGER_H

#include <iostream>
#include <chrono>
#include "ProgramOptions.h"

namespace primitives {
    class TimeLogger {
    public:
        TimeLogger(const ProgramOptions &options) : _options(options) {

        }

        ~TimeLogger() {
            auto duration = std::chrono::steady_clock::now() - _start;

            std::cout << "Parser threads: " << _options.MaximumParseThreads << std::endl;
            std::cout << "Read container size: " << _options.MaximumInputItemsContainerSize << std::endl;
            std::cout << "Write container size: " << _options.MaximumFoundItemsContainerSize << std::endl;
            std::cout << "Running time: " <<std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
            << " ms." << std::endl;
        }

    private:
        const ProgramOptions &_options;
        const decltype(std::chrono::steady_clock::now()) _start = std::chrono::steady_clock::now();
    };
}
#endif //PRIMITIVES_TIMELOGGER_H
