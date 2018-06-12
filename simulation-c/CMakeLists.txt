cmake_minimum_required(VERSION 3.6)
project(simulation-c)

# include the header files
include_directories(include)

# find sources in src/
file(GLOB SOURCES "src/*.c")

# add pthreads
find_package(Threads)
set(PROJECT_LINK_LIBS libigraph.so)
link_directories(~/espaces/travail/compiled/lib/)
include_directories(~/espaces/travail/compiled/include/)


add_executable(simulation-c ${SOURCES})

target_link_libraries(simulation-c ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(simulation-c ${IGRAPH_LIBRARIES})
