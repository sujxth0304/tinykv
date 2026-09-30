add_library(tinykv STATIC
    tinykv.c
)
target_include_directories(tinykv PUBLIC ${CMAKE_SOURCE_DIR}/include)
tinykv_set_warnings(tinykv)

add_executable(tinykv-server main.c)
target_link_libraries(tinykv-server PRIVATE tinykv)
tinykv_set_warnings(tinykv-server)
