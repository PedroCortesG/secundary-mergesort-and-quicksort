cmake_minimum_required(VERSION 3.10)
project(ExternalSorts)

set(CMAKE_CXX_STANDARD 17)

# Agregar flags de compilación
add_compile_options(-Wall -Wextra -O2)

# Archivos fuente comunes
set(SOURCE_FILES
        utils.cpp
        randomBin.cpp
)

# Ejecutable principal para correr los experimentos
add_executable(run_all
        main.cpp
        mergesort.cpp
        quicksort.cpp
        ${SOURCE_FILES}
)

# Ejecutable solo para MergeSort externo
add_executable(mergesort_exec
        mergesort.cpp
        ${SOURCE_FILES}
)

# Ejecutable solo para QuickSort externo
add_executable(quicksort_exec
        quicksort.cpp
        ${SOURCE_FILES}
)