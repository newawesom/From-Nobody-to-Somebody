aux_source_directory(. SRC_LIST)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} ../include)

add_executable(task1 ${SRC_LIST})

target_link_libraries(task1 ${OpenCV_LIBS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)