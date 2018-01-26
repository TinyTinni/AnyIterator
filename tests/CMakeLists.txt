cmake_minimum_required(VERSION 3.0)
project(AnyIterator_tests CXX)

set(SRCS
 "basic.cpp"
 "main.cpp")

add_executable(tests "../any_iterator.hpp" ${SRCS})
if (MSVC)
    target_compile_definitions(tests PUBLIC CATCH_CONFIG_WINDOWS_CRTDBG)
endif()

find_path(
    CATCH_INCLUDE_DIR 
    NAMES catch.hpp
    DOC "Catch include dir"
)
if (NOT CATCH_INCLUDE_DIR)
    include(ExternalProject)
    find_package(Git REQUIRED)
    
    set(BRANCH "master")
	if (MSVC_VERSION LESS 1900)
		set(BRANCH "Catch1.x") #catch2 needs C++11
    endif()
    
    ExternalProject_Add(
        catch
        PREFIX ${CMAKE_BINARY_DIR}/catch
        GIT_REPOSITORY https://github.com/philsquared/Catch.git
        GIT_TAG ${BRANCH}
        TIMEOUT 10
        UPDATE_COMMAND ${GIT_EXECUTABLE} pull
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
    )
    ExternalProject_Get_Property(catch source_dir)
    set(CATCH_INCLUDE_DIR ${source_dir}/single_include CACHE INTERNAL "Path to include folder for Catch.")
    add_dependencies(tests catch)
endif()

include_directories(${CATCH_INCLUDE_DIR} "../")

include(CTest)
enable_testing(true)  # Enables unit-testing.

add_test(NAME any_iterator_tests COMMAND tests)

if (MSVC)
    target_compile_definitions(tests PUBLIC CATCH_CONFIG_WINDOWS_CRTDBG)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT tests)
endif()