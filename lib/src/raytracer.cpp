#include "raytracer.hpp"

#include <vector>
#include <limits>
#include <array>
#include <fstream>

#define BIAS 0.0001f
#define M_PI 3.14159265358979323846
#define X_5 x*x*x*x*x
#define PIXEL(x, y, w) (y*w + x)
#define GroundColor Color{220.0/255.0, 220.0/255.0, 220.0/255.0}
#define SkyColor Color{130.0/255.0, 190.0/255.0, 255.0/255.0}

using namespace pRayTracer;

using Screen = std::vector<Color>;

namespace 
{

// Refractive index lookup tables
constexpr const std::array<float, size_t(Material::nMaterial)> refr_LUT = {1, 1.52};
constexpr const std::array<float, size_t(Material::nMaterial)> invRefr_LUT = {1, 0.65789473684};

Screen generateScreen(uint32_t height, uint32_t width)
{
    return Screen{height*width};
}

Color trace(const Ray& ray, const std::vector<Object*>& objects, const size_t& depth)
{
    const Object* min_object = nullptr;
    Config& cfg = Config::getInstance();
    Point min_pHit;
    Vec3 min_vNormal;

    float min_dist = std::numeric_limits<float>::max();
    for(const auto& object : objects)
    {
        float t1, t2;
        if(object->intersect(ray, t1, t2))
        {
            if(t1 < 0) t1 = t2;
            if(t1 < min_dist)
            {
                min_object = object;
                min_dist = t1;
            }
        }
    }

    if(min_object == nullptr)
    {
        return SkyColor;
        // if(ray.direction.y > 0)
        //     return SkyColor;
        // else
        //     return GroundColor;
    }
    

    min_pHit = ray.origin + ray.direction*min_dist;
    min_vNormal = min_object->getNormal(min_pHit);
    Color surface_color{};

    bool inside{};
    if(ray.direction.dot(min_vNormal) > 0) 
    {
        min_vNormal = min_vNormal*-1;
        inside = true;
    }
    // else { std::cout << "f"; }

    if(min_object->is_diffuse || depth >= cfg.max_ray_depth)
    {
        Ray shadow_ray = 
        {
            .origin = min_pHit + min_vNormal*BIAS, 
            .direction = (cfg.light_source.origin-min_pHit).normalize() 
        };

        float perceptible = 1.0;
        for(const auto& object : objects)
        {
            float t1, t2;
            if(object->intersect(shadow_ray, t1, t2))
            {
                perceptible = 0.0;
                break; 
            }
        }

        surface_color += min_object->color * 
            (
                cfg.light_source.brightness * 
                perceptible *

                //the opposite side of the object should have a smaller product
                std::max(float{}, min_vNormal.dot(shadow_ray.direction)) 
            );
    }
    else
    {
        Color reflection_color{};
        Color refraction_color{};

        float schlick_reflection = 1;

        if(min_object->reflectivity > 0.0)
        {
            //dot result will be negative so we have to subtract
            Ray reflection_ray = {
                .origin = min_pHit + min_vNormal*BIAS,
                .direction = (ray.direction - min_vNormal*2*ray.direction.dot(min_vNormal)).normalize()
            };
            reflection_color = trace(reflection_ray, objects, depth+1);
        }

        if(min_object->transparency > 0.0)
        {
            // i => ray, t => transmitted
            // n1/n2 or n2/n1 => depending on inside
            float n = inside ? refr_LUT[size_t(min_object->material)] : invRefr_LUT[size_t(min_object->material)];
            float cosi = ray.direction.dot(min_vNormal) * -1;
            float sint_square = n*n*(1.0-cosi*cosi);

            // angle is smaller than critical angle => no total internal reflection
            if(sint_square <= 1.0)
            {
                Ray refraction_ray = {
                    .origin = {min_pHit - min_vNormal*BIAS}, // if outside push the ray inside and vice versa
                    .direction = (ray.direction*n + min_vNormal*(n*cosi - std::sqrt(1.0-sint_square))).normalize()
                };

                //schlick method for tranmission and reflection rates
                float n1 = inside ? refr_LUT[size_t(min_object->material)] : 1.0;
                float n2 = inside ? 1.0 : refr_LUT[size_t(min_object->material)];
                
                float cosx = cosi;
                float r0 = (n1-n2)/(n1+n2);
                r0 *= r0;
                if(n1 > n2)
                {
                    cosx = std::sqrt(1.0-sint_square);
                }

                float x = 1.0-cosx;
                schlick_reflection = r0 + (1.0-r0)*X_5;

                refraction_color = trace(refraction_ray, objects, depth+1);
            }
        }

        // the resulting color will be a combination of reflected and refracted colors filtered on the object's surface
        surface_color = min_object->color*(reflection_color*(schlick_reflection*min_object->reflectivity) + 
        refraction_color*((1.0-schlick_reflection)*min_object->transparency));
    }

    return surface_color;
}

}

namespace pRayTracer
{
    void saveScreen(Screen& screen)
    {
        Config& cfg = Config::getInstance();
        std::ofstream ofs("./output.ppm", std::ios::out | std::ios::binary); 
        ofs << "P3"       << "\n" 
            << cfg.width  << "\n"
            << cfg.height << "\n"
            << "255"      << "\n"; 
        for (size_t i = 0; i < cfg.width * cfg.height; ++i) { 
            ofs << (int)(255.0 * (std::min(float(1), screen[i].x))) << " "
                << (int)(255.0 * (std::min(float(1), screen[i].y))) << " "
                << (int)(255.0 * (std::min(float(1), screen[i].z))) << "\n"; 
        }
        ofs.close();
    }

    void simulate(std::vector<Object*>& objects)
    {
        Config& cfg = Config::getInstance();
        auto screen = generateScreen(cfg.height, cfg.width);

        float inv_width = 1.0 / float(cfg.width);
        float inv_height = 1.0 / float(cfg.height);
        float aspect_ratio = cfg.width/float(cfg.height);
        float angle = std::tan(M_PI * 0.5 * cfg.fov/180.0);

        std::ofstream debug_file("./debug.txt"); 

        for (size_t y = 0; y < cfg.height; y++)
        {
            for (size_t x = 0; x < cfg.width; x++)
            {
                Ray ray = {
                    .origin = Point{},
                    .direction = Vec3
                        {
                            (2 * ((x+0.5f) * inv_width) - 1) * angle * aspect_ratio,
                            (1 - 2 * ((y+0.5f) * inv_height)) * angle,
                            -1
                        }.normalize()
                }; 
                screen[PIXEL(x,y,cfg.width)] = trace(ray, objects, 0);

                debug_file << screen[PIXEL(x,y,cfg.width)] << ", ";
            }            
            debug_file << '\n';
        }

        //Todo: save screen to file
        saveScreen(screen);
    }

    std::ostream& operator<<(std::ostream& os, Color& color)
    {
        os << "(" << (int)(color.x*255) << "," << (int)(color.y*255) << "," << (int)(color.z*255) << ")";
        return os;
    }
}

bool Sphere::intersect(const Ray& ray, float& t1, float& t2) const 
{
    const Sphere& sphere = *this;
    Vec3 hypot = sphere.center-ray.origin;
    float t = (hypot).dot(ray.direction);
    if(t < 0) return false;
    float y_square = hypot.dot(hypot) - t*t;
    if(y_square <= sphere.radius_square)
    {
        float x = std::sqrt(sphere.radius_square - y_square);
        t1 = t-x;
        t2 = t+x;
        return true;
    }
    else
    {
        return false;
    }
}

Vec3 Sphere::getNormal(Point& p_intersect) const 
{
    return (p_intersect-center).normalize();
}