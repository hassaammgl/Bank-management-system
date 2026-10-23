cmake_minimum_required(VERSION 3.20)
project(BankApp VERSION 0.1.0 LANGUAGES C CXX)

find_package(SQLite3 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED IMPORTED_TARGET libsodium)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

add_executable(BankApp ${SOURCES}
    includes/core/dashboard.hpp
    includes/core/session.hpp)

target_include_directories(BankApp PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/includes"
)

if(TARGET SQLite3::SQLite3)
    target_link_libraries(BankApp PRIVATE SQLite3::SQLite3)
else()
    target_link_libraries(BankApp PRIVATE SQLite::SQLite3)
endif()

target_link_libraries(BankApp PRIVATE PkgConfig::SODIUM)

target_compile_options(BankApp PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
        -Wnull-dereference
    >
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /Zc:__cplusplus /utf-8>
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>
)

target_compile_definitions(BankApp PRIVATE
    BANKAPP_DB_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data/bank.db"
    $<$<CONFIG:Debug>:_GLIBCXX_ASSERTIONS>
)
