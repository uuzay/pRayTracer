file(GLOB SRC ./app/*.cpp ./app/*.c ./app/*.hpp ./app/*.h ./main.cpp) 
add_executable(simple_raytracer ${SRC})   
target_link_libraries(simple_raytracer lib) 
