#ifndef PRIMITIVES_RUNNER_H
#define PRIMITIVES_RUNNER_H
#include <string>
#include <mutex>
#include <thread>
#include <vector>

#include "ParserWorker.h"
#include "Dispatcher.h"
#include "ProgramOptions.h"
#include "FileWorkers.h"

namespace primitives {
    class Runner {
    public:
        Runner(ifstream& input,
               ofstream& output,
               const ProgramOptions& options):
                        _input(input),
                        _output(output),
                        _options(options),
                        _dispatcher(
                                options.MaximumFoundItemsContainerSize,
                                options.MaximumInputItemsContainerSize)
        {
            ValidateOptions(_options);
            _threads.reserve(_options.MaximumParseThreads + 2);
        }

        void Run(std::string&& pattern)
        {
            //parse threads. they read input items from deque, converts them to parsed items and
            //places them to set (as it is sorted container).

           for(int k = 0; k!=_options.MaximumParseThreads;k++)
               _threads.emplace_back(ParserWorker::Run,
                                     pattern,
                                     std::bind(&Dispatcher::GetItemToParse, &_dispatcher),
                                     std::bind(&Dispatcher::SaveParsedItem, &_dispatcher,
                                               std::placeholders::_1));

           //add two additional threads, one for read things from file
           //and second to write parsed items to disk.
           _threads.emplace_back(FileReaderWorker::Run,std::ref(reinterpret_cast<istream &>(_input)),
                                std::bind(&Dispatcher::ReadInputItemFromDisk, &_dispatcher,
                                          std::placeholders::_1),
                                 std::bind(&Dispatcher::SignalEndOfReading, &_dispatcher,
                                           std::placeholders::_1)         );

           _threads.emplace_back(FileWriterWorker::Run,std::ref(reinterpret_cast<ostream &>(_output)),
                                 std::bind(&Dispatcher::GetParsedItem, &_dispatcher));

            for(auto& thread : _threads)
                thread.join();
        }

        void Stop()
        {
            _dispatcher.Stop();
        }

    private:
        void ValidateOptions(const ProgramOptions& options) const
        {
            if (options.MaximumParseThreads < MINIMUM_THREADS)
                throw new std::invalid_argument(
                        std::format("Threads count of {} exceed maximum of {} threads",
                                    options.MaximumParseThreads,
                                    MAXIMUM_THREADS));

            if (options.MaximumParseThreads > MAXIMUM_THREADS)
                throw new std::invalid_argument(
                        std::format("Threads count of {} exceed maximum of {} threads",
                                    options.MaximumParseThreads,
                                    MAXIMUM_THREADS));
        }
    private:
        const size_t MINIMUM_THREADS = 1;
        const size_t MAXIMUM_THREADS = 100;
        ifstream& _input;
        ofstream& _output;
        Dispatcher _dispatcher;
        std::vector<std::thread> _threads;
        const ProgramOptions _options;
    };
}


#endif //PRIMITIVES_RUNNER_H
