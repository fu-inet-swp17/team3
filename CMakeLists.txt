project(bgp-project)
cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# find BGPStream library
find_package(BGPStream)

# add both tutorial projects
add_executable(bgpstream-tutorial tutorials/bgpstream-tutorial.c)
add_executable(bgpstream-pfx-mon tutorials/bgpstream-tutorial.c)

target_link_libraries(bgpstream-tutorial ${LIBBGPSTREAM_LIBRARIES})
target_link_libraries(bgpstream-pfx-mon ${LIBBGPSTREAM_LIBRARIES})

install(TARGETS bgpstream-tutorial bgpstream-pfx-mon DESTINATION bin)
