# example

file(GLOB SOURCES "*.cpp")

foreach(source ${SOURCES})
    get_filename_component(target ${source} NAME_WE)
    add_executable(${target} ${source})
    target_link_libraries(${target} reliable_udp)
    install(TARGETS ${target} DESTINATION ${CMAKE_SOURCE_DIR}/bin)
endforeach()

install(TARGETS reliable_udp DESTINATION ${CMAKE_SOURCE_DIR}/bin)