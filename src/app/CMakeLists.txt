aux_source_directory(. DIR_LIB_SRCS)

include_directories(${CMAKE_SOURCE_DIR}/fileHelper)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Add source to this project's executable.
add_executable (SourceStatistics ${DIR_LIB_SRCS})

target_link_libraries(SourceStatistics "fileHelper")