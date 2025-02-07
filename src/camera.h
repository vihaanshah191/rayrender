#ifndef CAMERAH
#define CAMERAH

#include "ray.h"
#include "rng.h"
#include "Rcpp.h"
#include "onbh.h"
#include "animatedtransform.h"
#include "bounds.h"

class camera {
  public:
    camera(point3f lookfrom, point3f lookat, vec3f vup, Float vfov, Float aspect, Float aperture, Float focus_dist,
           Float t0, Float t1);
    ray get_ray(Float s, Float t, point3f u3, Float u1);
    
    point3f origin;
    point3f lower_left_corner;
    vec3f horizontal;
    vec3f vertical;
    vec3f u, v, w;
    Float time0, time1;
    Float lens_radius;
};

class ortho_camera {
public:
  ortho_camera(point3f lookfrom, point3f lookat, vec3f vup, 
               Float cam_width, Float cam_height, 
               Float t0, Float t1);
  ray get_ray(Float s, Float t, Float u);
  
  point3f origin;
  point3f lower_left_corner;
  vec3f horizontal;
  vec3f vertical;
  vec3f u, v, w;
  Float time0, time1;
};


class environment_camera {
  public:
    environment_camera(point3f lookfrom, point3f lookat, vec3f vup, 
                       Float t0, Float t1);
    ray get_ray(Float s, Float t, Float u1);
    
    point3f origin;
    vec3f u, v, w;
    Float nx, ny;
    Float time0, time1;
    onb uvw;
};

struct CameraSample {
  point2f pFilm;
  point2f pLens;
  Float time;
  CameraSample(point2f pFilm_, point2f pLens_, Float time_) : 
    pFilm(pFilm_), pLens(pLens_), time(time_) {};
  CameraSample(vec2f pFilm_, vec2f pLens_, Float time_) : 
    pFilm(point2f(pFilm_.x(),pFilm_.y())), pLens(point2f(pLens_.x(),pLens_.y())), time(time_) {};
};

class RealisticCamera  {
public:
  // RealisticCamera Public Methods
  RealisticCamera(const AnimatedTransform &CameraToWorld, Float shutterOpen,
                  Float shutterClose, Float apertureDiameter, Float cam_width, Float cam_height,
                  Float focusDistance, bool simpleWeighting,
                  std::vector<Float> &lensData,
                  Float film_size, Float camera_scale
                  );
  Float GenerateRay(const CameraSample &sample, ray* ray2) const;
  
private:
  // RealisticCamera Private Declarations
  struct LensElementInterface {
    Float curvatureRadius;
    Float thickness;
    Float eta;
    Float apertureRadius;
  };
  
  // RealisticCamera Private Data
  std::vector<LensElementInterface> elementInterfaces;
  std::vector<Bounds2f> exitPupilBounds;
  
  // RealisticCamera Private Methods
  Float LensRearZ() const { 
    return elementInterfaces.back().thickness; 
  }
  Float LensFrontZ() const {
    Float zSum = 0;
    for (const LensElementInterface &element : elementInterfaces)
      zSum += element.thickness;
    return zSum;
  }
  Float RearElementRadius() const {
    return elementInterfaces.back().apertureRadius;
  }
  
  bool TraceLensesFromFilm(const ray &r, ray *rOut) const;
  static bool IntersectSphericalElement(Float radius, Float zCenter,
                                        const ray &ray, Float *t,
                                        normal3f *n);
  bool TraceLensesFromScene(const ray &rCamera, ray* rOut) const;
  static void ComputeCardinalPoints(const ray &rIn, const ray &rOut, Float *p,
                                    Float *f);
  void ComputeThickLensApproximation(Float pz[2], Float f[2]) const;
  Float FocusThickLens(Float focusDistance);
  Bounds2f BoundExitPupil(Float pFilmX0, Float pFilmX1) const;
  point3f SampleExitPupil(const point2f &pFilm, const point2f &lensSample,
                          Float *sampleBoundsArea) const;
  Float FocusBinarySearch(Float focusDistance);
  Float FocusDistance(Float filmDistance);
  Bounds2f GetPhysicalExtent() const;
  AnimatedTransform CameraToWorld;
  Float shutterOpen;
  Float shutterClose;
  const bool simpleWeighting;
  Float cam_width;
  Float cam_height;
  Float diag;
  Float min_aperture;
  bool init;
};


  
#endif
