#ifndef PRIMITIVES_FILEWORKERS_H
#define PRIMITIVES_FILEWORKERS_H
#include <istream>
#include <format>
#include "WorkerTypes.h"

namespace primitives {
    class FileWriterWorker {
        enum class WriteStatus
        {
            Success,
            UnspecifiedProblem
        };
    public:
        static void Run(std::ostream & output_file,
                        std::function<GetItemToSaveResult()> producer ) {

            do
            {
                auto result = producer();
                if (result.Status == OperationStatus::Exit)
                    break;
                if (result.Item.is_invalid())
                    continue;

                auto status = write_line( output_file,result.Item);

                if (status == WriteStatus::UnspecifiedProblem)
                    throw std::runtime_error("Cannot write to file.");

              continue;
            }while(true);
        }

    private:
        static WriteStatus write_line(std::ostream & output_file,const ParsedItem& item)
        {
            output_file << std::format("{} {}",
                                       make_human_readable_line_number(item.StringId),
                                       item.StartPosition) << std::endl;
            return make_status_from_ostream(output_file);
        }
        static WriteStatus make_status_from_ostream(std::ostream & output_file)
        {
            if (output_file)
                return WriteStatus::Success;
            return WriteStatus::UnspecifiedProblem;
        }
        static int make_human_readable_line_number(int line_id)
        {
            return line_id + 1;
        }
    };

    class FileReaderWorker {
        enum class ReadStatus
        {
            Success,
            Eof,
            UnspecifiedProblem
        };
    public:
        static void Run(std::istream & input_file,
                        std::function<OperationStatus(const InputItem&)> consumer,
                        std::function<void(size_t)> signal_end_of_reading ) {
            int line_number = 0;
            do
            {
                auto read_result = read_line(input_file);
                //here we may instead use something that passed as parameter
                //and notify host that we cant read file anymore.
                //however, no such mechanism was implemented.
                if (read_result.Status == ReadStatus::UnspecifiedProblem)
                    throw std::runtime_error("Cannot read from file.");

                switch(read_result.Status)
                {
                    case ReadStatus::UnspecifiedProblem:
                        throw std::runtime_error("Cannot read from file.");
                    case ReadStatus::Eof:
                        signal_end_of_reading(line_number - 1);
                        return;
                    case ReadStatus::Success:
                        break;
                    default:
                        throw ::NotImplementedException();
                }
                auto write_status = consumer(InputItem(line_number,read_result.Item));
                if (write_status ==OperationStatus::Exit)
                    break;
                ++line_number;
            }while(true);
        }
    private:
        using ReadLineResultWithStatus = ItemOperationWithStatus<ReadStatus,std::string>;
        static ReadLineResultWithStatus read_line(std::istream & input_file)
        {
            std::string string;
            std::getline(input_file,string);
            return ReadLineResultWithStatus(make_status_from_istream(input_file),string);
        }
        static ReadStatus make_status_from_istream(std::istream & input_file)
        {
            if (input_file)
                return ReadStatus::Success;
            if (input_file.eof())
                return ReadStatus::Eof;
            return ReadStatus::UnspecifiedProblem;
        }
    };
}

#endif //PRIMITIVES_FILEWORKERS_H
