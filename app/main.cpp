#include <iostream>
#include <cassert> 
#include "raytracer.hpp"

#define MAX_RAY_DEPTH 3

using namespace pRayTracer;

int main(int argc, char const *argv[]) 
{  
    Config& cfg = Config::getInstance();
    // cfg.setScreenSize(800, 600);
    // cfg.setRayDepth(3);
    // cfg.setEyeOrigin({400, 300, -5});

    std::vector<Object*> objects;

    auto s1 = Sphere({0, -10008, -40}, 10000, {170.0/255.0, 170.0/255.0, 170.0/255.0}, false, 0.7, 0.5, Material::Glass);
    auto s2 = Sphere({0,      0, -30},     4, {255.0/255.0, 255.0/255.0, 255.0/255.0}, false, 1.0, 0.5, Material::Glass);
    auto s3 = Sphere({-10,    -2, -50},     4.5, {120.0/255.0, 120.0/255.0, 220.0/255.0}, true,  0.8, 0.5, Material::Diffuse);
    auto s4 = Sphere({10,     -2, -50},     4.5, {255.0/255.0, 0.0/255.0,   0.0/255.0},   false, 1.0, 0.2, Material::Glass);

    cfg.setLightSource(LightSource({0, 20, -10}, 1.0));

    objects.push_back(&s1);
    objects.push_back(&s2);
    objects.push_back(&s3);
    objects.push_back(&s4);

    //Todo: initialize objects

    simulate(objects);



    return 0; 
} 
