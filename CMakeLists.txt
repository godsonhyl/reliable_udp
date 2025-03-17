cmake_minimum_required(VERSION 3.18.0)

project(reliable_udp)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    add_definitions(-DNOMINMAX)
    add_definitions(-DJD_DLL_EXPORT)
elseif (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -pthread -fPIC")
endif ()


include_directories(${CMAKE_SOURCE_DIR}/include
                    ${CMAKE_SOURCE_DIR}/include/aeron
                    ${CMAKE_SOURCE_DIR}/src
)

link_directories(${CMAKE_SOURCE_DIR}/libs/win64
                 ${CMAKE_SOURCE_DIR}/libs/linux
)

aux_source_directory(${CMAKE_SOURCE_DIR}/src UDPSRC)

add_library(${PROJECT_NAME} SHARED ${UDPSRC})

target_link_libraries(${PROJECT_NAME} aeron_client)

add_subdirectory(example)

#install
set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/bin)
set(OUTLIB_DIR ${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}    DESTINATION ${OUTLIB_DIR}/lib)
install(FILES ${CMAKE_SOURCE_DIR}/src/reliable_udp.h    DESTINATION ${OUTLIB_DIR}/include)
