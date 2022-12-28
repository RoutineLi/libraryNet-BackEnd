cmake_minimum_required(VERSION 3.0)
project(Book-System)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -fno-strict-aliasing -export-dynamic -o3 -pipe -g -D_GNU_SOURCE -D_REENTRANT -m64 -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-dev -lssl -lcrypto")

set(PROJECT_SOURCE_DIR /home/vrvuser/Book-System/)

find_package(Boost REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(yaml-cpp REQUIRED)
if(yaml-cpp_FOUND)
    include_directories(${YAML_CPP_INCLUDE_DIRS})
endif()

find_package(jwt-cpp REQUIRED)
if(jwt-cpp_FOUND)
    include_directories(${JWT_CPP_INCLUDE_DIRS})
endif()

include_directories(.)
include_directories(/usr/local/include)
link_directories(/usr/lib)
link_directories(/usr/local/lib)

set(LIBS
        ssl
        crypto
        hiredis
    )


find_library(HIREDIS hiredis)
find_library(PTHREAD pthread)
find_library(OPENSSL ssl)
find_library(CRYPTO crypto)

set(LIB_SRC
    module-src/book_module.cpp
    module-src/book_servlet.cpp
)
    

add_library(heart SHARED module-src/heart_module.cpp)

add_library(book SHARED ${LIB_SRC})
target_link_libraries(book ${LIBS})



SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/module)

