# @author Batuhan TOPALOGLU

if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
	message(FATAL_ERROR "
		FATAL: In-source builds are not allowed.
		You should create a separate directory for build files.
		")
endif()

if(WIN32)
        set(CMAKE_USE_WIN32_THREADS_INIT ON)
else()
	set(THREADS_PREFER_PTHREAD_FLAG ON)


endif()

cmake_minimum_required(VERSION 3.9.1)

project(MYTIMER)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

include_directories ("${PROJECT_SOURCE_DIR}/includes")
link_directories("${PROJECT_SOURCE_DIR}/sources")

add_executable(MyTimer.exe test.cpp sources/MyTimer.cpp)

target_link_libraries(MyTimer.exe pthread)


