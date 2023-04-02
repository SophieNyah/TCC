cmake_minimum_required()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall")

project(UserProject)

link_directories(${CMAKE_SOURCE_DIR}/src/lib/msa)
link_directories(${CMAKE_SOURCE_DIR}/src/lib/yamg)

#add_subdirectory(src/lib)
add_executable(Main src/main.cpp src/AstSymbols.cpp src/Tree.cpp src/Tree.h output/mips.cpp)
target_link_libraries(Main msa)
target_link_libraries(Main yamg)


include(CTest)
add_test(NAME TestFatorial  COMMAND sh -c "$<TARGET_FILE:Main> fatorial < ../entradas/fatorial.c")
add_test(NAME TestFibonacci COMMAND sh -c "$<TARGET_FILE:Main> fibonacci < ../entradas/fibonacci.c")
add_test(NAME TestMultiplicaVetor COMMAND sh -c "$<TARGET_FILE:Main> multiplica_vetor < ../entradas/multiplica_vetor.c")
add_test(NAME TestInverteVetor COMMAND sh -c "$<TARGET_FILE:Main> inverte_vetor < ../entradas/inverte_vetor.c")
