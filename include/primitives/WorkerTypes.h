#ifndef PRIMITIVES_WORKERTYPES_H
#define PRIMITIVES_WORKERTYPES_H

#include <string>
#include <utility>
#include <functional>
#include "exceptions.h"


namespace primitives {
    enum class OperationStatus {
        //Operation done succesfully.
        Success,
        //Exit requested, stop immediatelly.
        Exit,
    };

    class InputItem
    {
    public:
        InputItem(
                const size_t string_id,
                const std::string& input_string):StringId(string_id),
                                                 String(input_string)
        {
        }
        static InputItem make_invalid()
        {
           return InputItem(-1,{});
        }
        InputItem():StringId(),String(){}
        const size_t StringId;
        const std::string String;
    };

    class ParsedItem
    {
    public:
        ParsedItem(int string_id, size_t start_position): StringId(string_id), StartPosition(start_position){}
        const int StringId;
        const size_t StartPosition;

        bool operator<( const ParsedItem& other ) const
        {
            return (StringId) < (other.StringId);
        }

        bool is_invalid() const
        {
            return StartPosition == std::string::npos;
        }

        static ParsedItem make_invalid()
        {
            return ParsedItem(-1,std::string::npos);
        }

    };

    template<typename U, typename T>
    class ItemOperationWithStatus
    {
    public:
        ItemOperationWithStatus(const U& status,const T item):Status(status),Item(item){}
        const U Status;
        const T Item;
    };
    using GetItemToParseResult = ItemOperationWithStatus<OperationStatus, InputItem>;
    using GetItemToSaveResult = ItemOperationWithStatus<OperationStatus, ParsedItem>;
}
#endif //PRIMITIVES_WORKERTYPES_H
