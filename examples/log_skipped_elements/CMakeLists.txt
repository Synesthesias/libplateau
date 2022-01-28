project(log_skipped_elements)

add_executable(log_skipped_elements "log_skipped_elements.cpp")

target_link_libraries(log_skipped_elements citygml)

include_directories("${LIBCITYGML_INCLUDE}")
