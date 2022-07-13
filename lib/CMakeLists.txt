file(GLOB_RECURSE SRC ./src/*.cpp ./src/*.c ./include/*.hpp ./include/*.h) 
add_library(lib STATIC ${SRC})  
target_include_directories(lib PUBLIC ./include) 
