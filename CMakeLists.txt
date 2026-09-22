cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER g++)

project(CalculoNumericoUnidade1 CXX)

# padrão C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define o executável principal
add_executable(calculo_numerico main.cpp)

add_executable(comparar_metodos benchmarks/compare.cpp)
