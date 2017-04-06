cmake_minimum_required(VERSION 2.8.8)
project(AnyIterator_tests CXX)

include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
    catch
    PREFIX ${CMAKE_BINARY_DIR}/catch
    GIT_REPOSITORY https://github.com/philsquared/Catch.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
   )
ExternalProject_Get_Property(catch source_dir)
set(CATCH_INCLUDE_DIR ${source_dir}/single_include CACHE INTERNAL "Path to include folder for Catch.")

include_directories(${CATCH_INCLUDE_DIR} "../")
enable_testing(true)  # Enables unit-testing.

set(SRCS
 "basic.cpp"
 "main.cpp")

add_executable(tests "../AnyIterator.hpp" ${SRCS})
add_dependencies(tests catch)