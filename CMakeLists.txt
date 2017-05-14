project(bgp-project)
cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Compiler flags (pedantic C++17)
set(CMAKE_CXX_FLAGS "-Wall -Wpedantic -Wextra -std=c++1z")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# find boost library components
add_definitions(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.60 COMPONENTS log program_options REQUIRED)

# find BGPStream library
find_package(BGPStream)

file(GLOB HEADER_FILES include/*.hpp)

# add main executable
add_executable(bgp src/main.cpp)

target_include_directories(bgp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(bgp ${LIBBGPSTREAM_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)

# add both tutorial projects
add_executable(bgpstream-tutorial tutorials/bgpstream-tutorial.c)
add_executable(bgpstream-pfx-mon tutorials/bgpstream-tutorial.c)

target_link_libraries(bgpstream-tutorial ${LIBBGPSTREAM_LIBRARIES})
target_link_libraries(bgpstream-pfx-mon ${LIBBGPSTREAM_LIBRARIES})

install(TARGETS bgpstream-tutorial bgpstream-pfx-mon DESTINATION bin)
