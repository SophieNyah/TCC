cmake_minimum_required()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall")

project(UserProject)

link_directories(${CMAKE_SOURCE_DIR}/src/lib_msa)

#add_subdirectory(src/lib_msa)
add_executable(Main src/main.cpp src/AstSymbols.cpp src/Tree.cpp src/Tree.h)
target_link_libraries(Main msa.a)
