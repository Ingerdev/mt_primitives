#ifndef PRIMITIVES_DISPATCHER_H
#define PRIMITIVES_DISPATCHER_H

#include <set>
#include <thread>
#include <deque>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>

#include "exceptions.h"

#include "WorkerTypes.h"
#include "ParserWorker.h"

namespace primitives {
    class ParsedItemsArea
    {
    public:
        ParsedItemsArea(size_t max_found_items_container_size,
                        std::atomic_bool& exit_signal):
                _max_items_count(max_found_items_container_size),
                _max_line_id(-1),
                _next_to_remove_id(0),
                _exit_requested(exit_signal)
        {
            ValidateParameters(max_found_items_container_size);
        }
        OperationStatus Add(const ParsedItem& item)
        {
            do {
                if (_exit_requested)
                    return OperationStatus::Exit;

                const std::lock_guard<std::mutex> lock(_mutex);
                //we cannot add more items than container's capacity.
                //and as ordered output required, we should
                //process items from lowest ids to highest.
                //its why items with highest ids must wait.
                if (too_high_id(item.StringId))
                    continue;

                _parsed_items.insert(item);
                return OperationStatus::Success;

            }while(true);
        }

        bool too_high_id(size_t id)
        {
            return _max_items_count + _next_to_remove_id < id;
        }
        GetItemToSaveResult Get()
        {
            do {
                if (_exit_requested || is_file_fully_saved())
                    return GetItemToSaveResult(OperationStatus::Exit,
                                               ParsedItem::make_invalid());
                {
                    const std::lock_guard<std::mutex> lock(_mutex);
                    if (_parsed_items.empty())
                        continue;

                    auto item = *_parsed_items.begin();
                    //important thing: bunch of thread writes to our sorted container.
                    //we may be here when higher ids already written in container
                    //and required lower ids still in processing.
                    //we must wait lowest id as we must follow order in
                    //result file.
                    if (item.StringId!=_next_to_remove_id)
                        continue;
                    _parsed_items.erase(_parsed_items.begin());
                    _next_to_remove_id++;

                    return GetItemToSaveResult(OperationStatus::Success, item);
                }

            }while(true);
        }

        void SetupLastLineId(int last_line_id)
        {
            _max_line_id = last_line_id;
        }

        bool is_file_fully_saved()
        {
            return _next_to_remove_id == _max_line_id;
        };
    private:
        void ValidateParameters(size_t max_container_size) const
        {
            if (max_container_size < MINIMUM_CONTAINER_SIZE)
                throw new std::invalid_argument(
                        std::format("Container size of {} is less than required {} elements.",
                                    max_container_size,
                                    MINIMUM_CONTAINER_SIZE));

            if (max_container_size > MAXIMUM_CONTAINER_SIZE)
                throw new std::invalid_argument(
                        std::format("Container size of  {} exceed maximum of {} elements.",
                                    max_container_size,
                                    MAXIMUM_CONTAINER_SIZE));
        }


        const size_t MINIMUM_CONTAINER_SIZE = 1;
        const size_t MAXIMUM_CONTAINER_SIZE = 100000;
        std::set<ParsedItem> _parsed_items;
        size_t _max_items_count;
        std::mutex _mutex;
        //upper limit id that defines which items we can add container
        //on this moment and which are not yet.
        std::atomic<size_t> _next_to_remove_id;
        std::atomic<int> _max_line_id;
        std::atomic<bool>& _exit_requested;
    };

    class InputItemsArea{
    public:
        InputItemsArea(size_t max_input_items_container_size,
                       std::atomic<bool>& exit_signal):
                            _max_items_count(max_input_items_container_size),
                            _exit_requested(exit_signal),
                            _file_is_fully_red(false)
        {
            ValidateParameters(max_input_items_container_size);
        }

        //add input item to deque
        OperationStatus Add(const InputItem& item)
        {
            do {
                if (_exit_requested)
                    return OperationStatus::Exit;
                {
                    const std::lock_guard<std::mutex> lock(_mutex);
                    if (_input_items.size() == _max_items_count)
                        continue;

                    _input_items.push_back(item);
                    break;
                }
            }while(true);

            return OperationStatus::Success;
        }
        void NotifyReadCompleted()
        {
            const std::lock_guard<std::mutex> lock(_mutex);
            _file_is_fully_red = true;
        }
        //retrieve input item from deque for further parsing.
        GetItemToParseResult Get()
        {
            do {
                if (_exit_requested)
                    return GetItemToParseResult(OperationStatus::Exit,
                                                InputItem::make_invalid());
                {
                    const std::lock_guard<std::mutex> lock(_mutex);
                    if (_input_items.empty()) {
                        if (_file_is_fully_red)
                            return GetItemToParseResult(OperationStatus::Exit,
                                                        InputItem::make_invalid());
                        continue;
                    }

                    auto item = std::move(_input_items.front());
                    _input_items.pop_front();
                    return GetItemToParseResult(OperationStatus::Success, item);
                }

            }while(true);
        }
    private:

        void ValidateParameters(size_t max_container_size) const
        {
            if (max_container_size < MINIMUM_CONTAINER_SIZE)
                throw new std::invalid_argument(
                        std::format("Container size of {} is less than required {} elements.",
                                    max_container_size,
                                    MINIMUM_CONTAINER_SIZE));

            if (max_container_size > MAXIMUM_CONTAINER_SIZE)
                throw new std::invalid_argument(
                        std::format("Container size of  {} exceed maximum of {} elements.",
                                    max_container_size,
                                    MAXIMUM_CONTAINER_SIZE));
        }


        const size_t MINIMUM_CONTAINER_SIZE = 1;
        const size_t MAXIMUM_CONTAINER_SIZE = 100000;
        std::deque<InputItem> _input_items;
        size_t _max_items_count;
        std::mutex _mutex;
        std::atomic_bool& _exit_requested;
        std::atomic_bool _file_is_fully_red;
    };

    class Dispatcher{
    public:
        Dispatcher(size_t max_found_items_container_size,
                   size_t max_input_items_container_size):
                _input_area(max_input_items_container_size, _exit_requested),
                _found_area(max_found_items_container_size, _exit_requested)
        {
        }
        void Stop()
        {
            _exit_requested = true;
        }

        OperationStatus ReadInputItemFromDisk(const InputItem& item)
        {
            return _input_area.Add(item);
        }
        GetItemToParseResult GetItemToParse()
        {
            return _input_area.Get();
        }

        OperationStatus SaveParsedItem(const ParsedItem item)
        {
            return _found_area.Add(item);
        }
        GetItemToSaveResult GetParsedItem()
        {
            return _found_area.Get();
        }

        void SignalEndOfReading(size_t last_line_id)
        {
            _input_area.NotifyReadCompleted();
            _found_area.SetupLastLineId(last_line_id);

        }

    private:
        InputItemsArea _input_area;
        ParsedItemsArea _found_area;
        std::atomic_bool _exit_requested;
    };

} // primitives

#endif //PRIMITIVES_DISPATCHER_H
