#ifndef PRIMITIVES_PARSERWORKER_H
#define PRIMITIVES_PARSERWORKER_H
#include <string>
#include <utility>
#include <functional>
#include <regex>

#include "WorkerTypes.h"
namespace primitives {
    class ParserWorker {
    public:
        //reads value from producer (raw string from file)
        //parse it and dump to consumer.
        static void Run(
                 const std::string& pattern,
                 std::function<GetItemToParseResult()> producer,
                 std::function<OperationStatus (const ParsedItem)> consumer) {

            do {
                auto value = producer();
                if (value.Status == OperationStatus::Exit)
                    break;

                auto consumeStatus = consumer(
                        ParsedItem(value.Item.StringId, Find(pattern,value.Item.String)));


                if (consumeStatus == OperationStatus::Exit)
                    break;

            }while(true);
        }

        static size_t Find(const std::string& pattern,const std::string& value)
        {
            auto plen =pattern.length();
            auto vlen =value.length();

            if (plen>vlen)
                return std::string::npos;

            for(int k =0;k!=vlen-plen+1;++k)
            {
                auto success = true;
                for(int c = k; c!=k+plen; ++c)
                {
                    if (pattern[c-k] == '?')
                        continue;
                    if (pattern[c-k] != value[c]) {
                        success = false;
                        break;
                    }
                }
                if (success)
                    return k;
            }

            return std::string::npos;
        }
    };

}


#endif //PRIMITIVES_PARSERWORKER_H
