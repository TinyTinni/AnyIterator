cmake_minimum_required(VERSION 3.0)
project(AnyIterator_tests CXX)

set(SRCS
 "basic.cpp"
 "main.cpp")

include_directories("../")


find_package(Catch2 QUIET)
if (NOT TARGET Catch2::Catch)
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
    
    add_library(Catch INTERFACE)
    set(INCLUDE_POSTFIX "catch2/")
    if (MSVC_VERSION LESS 1900)
        set(INCLUDE_POSTFIX "")
    endif()
    target_include_directories(Catch INTERFACE "${source_dir}/single_include/${INCLUDE_POSTFIX}")
    add_dependencies(Catch catch)
    add_library(Catch2::Catch ALIAS Catch)
endif()

add_executable(tests "../any_iterator.hpp;../Readme.md" ${SRCS})
target_link_libraries(tests PRIVATE Catch2::Catch)

if (MSVC)
    target_compile_definitions(tests PUBLIC CATCH_CONFIG_WINDOWS_CRTDBG)
endif()

include(CTest)
enable_testing(true)  # Enables unit-testing.

add_test(NAME any_iterator_tests COMMAND tests)

find_package(benchmark QUIET)
if (benchmark_FOUND)
    add_executable(any_iter_benchmark "../any_iterator.hpp;any_iterator_virtual.hpp" "benchmark.cpp" "Readme.md")
    target_link_libraries(any_iter_benchmark PUBLIC benchmark::benchmark)
	include_external_msproject(benchmark_plotter "${CMAKE_CURRENT_SOURCE_DIR}/benchmark_plotter.pyproj" TYPE 888888a0-9f3d-457c-b088-3a5042f75d52 GUID 4d9723ef-7dfd-4dd0-9c08-5c530557b25e)
endif()

if (MSVC)
    target_compile_definitions(tests PUBLIC CATCH_CONFIG_WINDOWS_CRTDBG)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT tests)
endif()