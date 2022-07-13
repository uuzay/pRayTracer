# pRayTracer
A fairly fast simple ray tracer built from scratch in C++. Currently it features the following:
  - Sphere objects with the following customizable properties:
    - Color
    - Transparency
    - Reflectivity
    - Material (only diffuse and glass objects as of now)
  - A configuration class to adjust:
    - Screen size
    - FOV
    - Maximum ray depth
  - Single light source (which can be easily extended) with customizable:
    - Origin
    - Brightness
  - Schlick's approximation for fast and accurate transmittance/reflectance
  - Saving files in .ppm format (version P3)
