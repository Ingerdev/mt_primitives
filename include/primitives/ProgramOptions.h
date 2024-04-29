#ifndef PRIMITIVES_PROGRAMOPTIONS_H
#define PRIMITIVES_PROGRAMOPTIONS_H
using namespace std;
namespace primitives {
    struct ProgramOptions {
        ProgramOptions(size_t maximum_parse_threads,
                       size_t max_found_items_container_size,
                       size_t max_input_items_container_size)
                            : MaximumParseThreads(maximum_parse_threads),
                              MaximumFoundItemsContainerSize(max_found_items_container_size),
                              MaximumInputItemsContainerSize(max_input_items_container_size)

        {

        }

        const size_t MaximumParseThreads;
        const size_t MaximumFoundItemsContainerSize;
        const size_t MaximumInputItemsContainerSize;
    };
}
#endif //PRIMITIVES_PROGRAMOPTIONS_H
