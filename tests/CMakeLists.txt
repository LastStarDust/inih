add_executable(INIReaderTest INIReaderTest.cpp)

target_link_libraries(INIReaderTest inih inihcpp)

add_executable(unittest unittest.c)

target_link_libraries(unittest inih)