project(kminidlna_restserver)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(RESTServer_SRCS serverrequest.cpp restserver.cpp main.cpp)
qt4_automoc(${RESTServer_SRCS})
add_executable(kminidlna_restserver ${RESTServer_SRCS})
target_link_libraries(kminidlna_restserver ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY})
