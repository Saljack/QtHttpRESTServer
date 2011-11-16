project(RESTServer)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(RESTServer_SRCS restconnection.cpp RESTServer.cpp main.cpp)
qt4_automoc(${RESTServer_SRCS})
add_executable(RESTServer ${RESTServer_SRCS})
target_link_libraries(RESTServer ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY})
