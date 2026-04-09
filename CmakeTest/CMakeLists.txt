# Минимально допустимая версия Cmake
cmake_minimum_required(VERSION 3.20)

# Имя проекта и языки которые используются в проекте
project(CmakeTest LANGUAGES C)

# выходной файл и входные файлы 
# Можно создать нескуолько выходных одновременно 
add_executable(result 
        main.c
        EXCLUDE_FROM_ALL
        asdf.c
)
add_executable(ap ap.c)

