#include <stdio.h>
#include <fstream>
#include <iostream>
#include "primitives/Runner.h"
#include "primitives/TimeLogger.h"
std::pair<ifstream,ofstream> open_files(const std::string& input_name,
                     const std::string& output_name)
{
    ifstream input_file(input_name);
    if (!input_file.is_open())
        throw std::ifstream::failure(std::format("Could not open file {}",input_name));

    ofstream output_file(output_name);
    if (!output_file.is_open())
        throw std::ifstream::failure(std::format("Could not open file {}",output_name));
    return std::make_pair(std::move(input_file),std::move(output_file));
}
int main(int argc,char* argv[]) {

    try {
        auto files = open_files("data_long.txt", "out.txt");
       /*
        auto options = primitives::ProgramOptions{5,
                                             5,
                                             5};
        */
        auto options = primitives::ProgramOptions{10,
                                                  1000,
                                                  1000};
        primitives::Runner runner(files.first, files.second,options );
        auto time_logger = primitives::TimeLogger(options);
        runner.Run("???D?");
    } catch (const std::exception &e) {
        std::cout << e.what();
    }

}

