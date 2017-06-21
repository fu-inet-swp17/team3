project(bgp-project)
cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Compiler flags (pedantic C++17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -fdiagnostics-color=always -std=c++1z")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# find boost library components
add_definitions(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.58 COMPONENTS log program_options REQUIRED)

# find RTR library
find_package(RTRLib REQUIRED)

# find BGPStream library
find_package(BGPStream REQUIRED)

# add bgpreadercpp executable
add_executable(bgpreadercpp src/reader.cpp)
target_include_directories(bgpreadercpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(bgpreadercpp ${LIBBGPSTREAM_LIBRARIES} ${RTRLIB_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)

# add fuzzbgp executable
add_executable(fuzzbgp src/fuzzbgp.cpp src/prefix_store.cpp src/stream_controller.cpp)
target_include_directories(fuzzbgp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(fuzzbgp ${LIBBGPSTREAM_LIBRARIES} ${RTRLIB_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)

# add both bgpstream tutorial projects & rtrclient but exclude from 'make all'
add_executable(bgpstream-tutorial misc/tutorials/bgpstream-tutorial.c)
add_executable(bgpstream-pfx-mon misc/tutorials/bgpstream-tutorial.c)
add_executable(rtrclient misc/tutorials/rtrclient.c)

target_link_libraries(bgpstream-tutorial ${LIBBGPSTREAM_LIBRARIES})
target_link_libraries(bgpstream-pfx-mon ${LIBBGPSTREAM_LIBRARIES})
target_link_libraries(rtrclient ${RTRLIB_LIBRARIES})

set_target_properties(bgpstream-tutorial bgpstream-pfx-mon rtrclient
  PROPERTIES EXCLUDE_FROM_ALL 1)

install(TARGETS fuzzbgp bgpreadercpp DESTINATION bin)
