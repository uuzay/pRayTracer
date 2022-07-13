cmake_minimum_required(VERSION 3.12.0) 
project(simple_raytracer VERSION 0.0.1)  
 
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF) 
 
add_subdirectory(lib) 
add_subdirectory(app) 
