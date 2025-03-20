cmake_minimum_required(VERSION 3.21)
project(umba_tokenizer CXX)


if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


set(MODULE_ROOT "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB_RECURSE sources "${MODULE_ROOT}/*.cpp")
# list(FILTER sources EXCLUDE REGEX ".*CMakeCXXCompilerId\\.cpp$")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Sources" FILES ${sources})

file(GLOB_RECURSE headers "${MODULE_ROOT}/*.h")
# list(FILTER headers EXCLUDE REGEX ".*zz_.*\\.h$")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Headers" FILES ${headers})


add_library(${PROJECT_NAME} ${sources} ${headers})
add_library(umba::tokenizer ALIAS ${PROJECT_NAME})

# Это не нужно, у нас каталог с библиотеками и так входит в пути поиска, и CMake вроде добавит модуль в инклюды при add_subdirectory
# target_include_directories(${PROJECT_NAME} PRIVATE ${MODULE_ROOT}/..)

# Это нужно, если у нас модуль является подмодулем другого и разделяет с ним иерархию файлов.
# Так, у нас есть модуль umba.
# Также, у нас есть модуль umba_tokenizer.
# umba_tokenizer зависит от umba, и также является частью umba-модуля, но распространяется отдельно.
# Но для пользователя не должно быть разницы, отдельно ли поставляется umba_tokenizer или вместе с umba.
# Он должен подключаться и использоваться, как часть модуля umba:
# #include "umba/simple_formatter.h"
# Файловая система модуля umba и подмодуля umba::tokenizer должна выглядеть так:
# libs                                   - каталог, в котором лежат все модули/библиотеки
#   umba                                 - каталог модуля umba
#   umba_tokenizer                       - каталог модуля umba::tokenizer
#     inc                                - каталог, который должен входить в пути поиска include
#       umba                             - каталог umba модулей
#         tokenizer                      - каталог umba модуля tokenizer
#           tokenizer/tokenizer.h.h     - include "umba/simple_formatter.h"
target_include_directories(${PROJECT_NAME} PUBLIC  ${MODULE_ROOT}/inc)

target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32_LEAN_AND_MEAN)
