#ifndef __raytracer_H__
#define __raytracer_H__

#include <utility>
#include <cmath>
#include <vector>
#include <cstdint>
#include <iostream>

#define P_RAY_TRACER_DEFAULT_WIDTH 800
#define P_RAY_TRACER_DEFAULT_HEIGHT 600
#define P_RAY_TRACER_DEFAULT_RAY_DEPTH 5
#define P_RAY_TRACER_DEFAULT_FOV 30

#define P_RAY_TRACER_DEFAULT_ORIGIN_X (P_RAY_TRACER_DEFAULT_WIDTH / 2)
#define P_RAY_TRACER_DEFAULT_ORIGIN_Y (P_RAY_TRACER_DEFAULT_HEIGHT / 2)
#define P_RAY_TRACER_DEFAULT_ORIGIN_Z -100

#define P_RAY_TRACER_DEFAULT_LIGHT_ORIGIN {500, 300, -50}
#define P_RAY_TRACER_DEFAULT_LIGHT_BRIGHTNESS 1.0

namespace pRayTracer
{

enum class Material : size_t
{
    Diffuse = 0,
    Glass = 1,
    nMaterial = 2,
};

struct Vec3
{
    float x,y,z;

    constexpr Vec3() : x(float{}), y(float{}), z(float{}) {}
    constexpr Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    constexpr Vec3 operator-(const Vec3& other)  const { return Vec3(x-other.x, y-other.y, z-other.z); }
    constexpr Vec3 operator+(const Vec3& other)  const { return Vec3(x+other.x, y+other.y, z+other.z); }
    constexpr Vec3 operator*(const Vec3& other)  const { return Vec3(x*other.x, y*other.y, z*other.z); }
    constexpr Vec3 operator*(const float& scale) const { return Vec3(x*scale, y*scale, z*scale); }
    constexpr Vec3 operator/(const float& scale) const { return Vec3(x/scale, y/scale, z/scale); }

    constexpr void operator+=(const Vec3& other)  { x+=other.x; y+=other.y; z+=other.z; }

    //Todo: check overflow
    constexpr float dot      (const Vec3& other) const { return x*other.x + y*other.y + z*other.z; }
    constexpr float distance (const Vec3& other) const { return (*this-other).norm(); }
    constexpr float norm()                       const { return std::sqrt(x*x + y*y + z*z); }
    constexpr Vec3 normalize()                   const { return (*this)/(*this).norm(); }

};

using Point = Vec3;
using Color = Vec3;

std::ostream& operator<<(std::ostream& os, Color& color);

struct Ray
{
    Point origin;
    Vec3  direction;
};

struct Object
{
public:
    bool  is_diffuse;
    float transparency, reflectivity;
    Color color;
    Material material;

    virtual bool intersect(const Ray& ray, float& t1, float& t2) const = 0;
    virtual Vec3 getNormal(Point& p_intersect) const = 0;

};

struct Sphere : public Object
{
public:
    float radius;
    float radius_square;
    Point center;

    Sphere(Point center_, float radius_, Color color_,
           bool is_diffuse_, float transparency_, float reflectivity_,
           Material material_) : center(center_), radius(radius_), radius_square(radius_*radius_)
        { 
            color = color_;
            is_diffuse = is_diffuse_;
            transparency = transparency_;
            reflectivity = reflectivity_;
            material = material_;
        }

    bool intersect(const Ray& ray, float& t1, float& t2) const override;
    Vec3 getNormal(Point& p_intersect) const override;
};

struct LightSource 
{
    LightSource(Point origin_, float brightness_) : origin(origin_), brightness(brightness_) {}

    Point origin;
    float brightness;
};

void simulate(std::vector<Object*>& objects);

struct Config
{
public:
    static Config& getInstance()
    {
        static Config cfg{};
        return cfg;
    }

    Config(const Config&) = delete;
    Config(Config&&) = delete;

    Config& operator=(const Config&) = delete;
    Config& operator=(Config&&) = delete;

    void setRayDepth    (const size_t& d)   { max_ray_depth = d; }
    void setEyeOrigin   (const Point& p)    { eye_origin = p; }
    void setFOV         (const float& fov_) { fov = fov_; }
    void setLightSource (const LightSource& ls) { light_source = ls; }
    void setScreenSize  (const uint32_t& w, const uint32_t& h) { height = h; width = w;}

private:
    Config() : height(P_RAY_TRACER_DEFAULT_HEIGHT),
               width(P_RAY_TRACER_DEFAULT_WIDTH),
               max_ray_depth(P_RAY_TRACER_DEFAULT_RAY_DEPTH),
               fov(P_RAY_TRACER_DEFAULT_FOV),
               light_source(LightSource(P_RAY_TRACER_DEFAULT_LIGHT_ORIGIN, P_RAY_TRACER_DEFAULT_LIGHT_BRIGHTNESS)),
               eye_origin(
                    Point
                    (
                        P_RAY_TRACER_DEFAULT_ORIGIN_X,
                        P_RAY_TRACER_DEFAULT_ORIGIN_Y,
                        P_RAY_TRACER_DEFAULT_ORIGIN_Z
                    )
               ){}

public:
    uint32_t    height, width;
    float       fov;
    size_t      max_ray_depth;
    Point       eye_origin;
    LightSource light_source;
};

}

#endif // __raytracer_H__