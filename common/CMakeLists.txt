set(SOURCES tools.c include/tools.h)

set(LIBRARY_NAME Common)

add_library(${LIBRARY_NAME} ${SOURCES})
target_include_directories(${LIBRARY_NAME} PUBLIC "./")
