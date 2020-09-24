# 指示在哪里找头文件
include_directories(${CMAKE_SOURCE_DIR}/fileHelper)
include_directories(${CMAKE_SOURCE_DIR}/thread)

# 指定在哪里找链接库
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_executable (FileHelperTest  catch_cache.cpp "FileHelperTest.cpp")

# 指定FileHelperTest需要链接哪些库
target_link_libraries(FileHelperTest "fileHelper")
