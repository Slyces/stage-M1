cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 11)
project(simulation-cpp)

set(CMAKE_CXX_FLAGS "-g")

# include the header files
include_directories(include)

# find sources in src/
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.hpp")

# add pthreads
find_package(Threads)
set(PROJECT_LINK_LIBS libigraph.so)
link_directories(~/espaces/travail/compiled/lib/)
include_directories(~/espaces/travail/compiled/include/)


add_executable(simulation-cpp ${SOURCES} ${HEADERS})

target_link_libraries(simulation-cpp ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(simulation-cpp ${IGRAPH_LIBRARIES})

# tests
set(TEST_SOURCES ${SOURCES})
list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/simulation.cpp)
set(TEST_HEADERS ${HEADERS})
list(REMOVE_ITEM TEST_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/simulation.hpp)

add_executable(simulation-tests
        tests/catch.hpp tests/main-tests.cpp
        ${TEST_SOURCES} ${TEST_HEADERS})
target_link_libraries(simulation-tests ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(simulation-tests ${IGRAPH_LIBRARIES})
