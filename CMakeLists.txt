# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
#mute modern cmake compatibility issue
cmake_policy(SET CMP0074 NEW)

project (Primitives VERSION 0.1 LANGUAGES CXX)
add_executable (${PROJECT_NAME}  ${PROJECT_SOURCE_DIR}/src/main.cpp
        src/Runner.cpp
        include/primitives/Runner.h
        include/primitives/ParserWorker.h
        include/primitives/Dispatcher.h
        include/primitives/ProgramOptions.h
        include/primitives/FileWorkers.h
        include/primitives/WorkerTypes.h
        include/primitives/exceptions.h
        include/primitives/TimeLogger.h
)

#set environment variable which will be used as destination for copy dlls in subprojects
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeIncludes ${CMAKE_MODULE_PATH})

#20 - for std::format
target_compile_features (${PROJECT_NAME} PUBLIC cxx_std_20)

find_package(cxxopts CONFIG)

# set where to find additional cmake modules if any
# comment it out if not required
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Add source to this project's executable.
target_sources(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src/main.cpp)

#target_link_libraries(${PROJECT_NAME} cxxopts::cxxopts)




