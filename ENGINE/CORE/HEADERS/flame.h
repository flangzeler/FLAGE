#pragma once
#ifndef FLAME_MATH_H
#define FLAME_MATH_H

#include <immintrin.h>
#include <cmath>
#include <cstdint>

#if defined(__AVX2__) || defined(__AVX__)
  #define FLAME_SIMD_AVX
#elif defined(__SSE2__)
  #define FLAME_SIMD_SSE
#endif

#ifndef FLAME_INLINE
  #if defined(_MSC_VER)
	#define FLAME_INLINE __forceinline
  #else
	#define FLAME_INLINE inline __attribute__((always_inline))
  #endif
#endif

#ifndef FLAME_ALIGN
  #define FLAME_ALIGN(x) alignas(x)
#endif

namespace flame  
{

constexpr float kPi = 3.14159265358979323846f;
constexpr float kEpsilon = 1e-6f;

FLAME_INLINE float radians(float deg) { return deg * (kPi / 180.0f); }
FLAME_INLINE float degrees(float rad) { return rad * (180.0f / kPi); }

using Simd128 = __m128;

struct FLAME_ALIGN(16) Vec4
{
  union { struct { float x,y,z,w; }; Simd128 simd; };

  FLAME_INLINE Vec4(float X=0,float Y=0,float Z=0,float W=0){ simd=_mm_setr_ps(X,Y,Z,W);} 
  FLAME_INLINE Vec4(Simd128 v){ simd=v; }

  FLAME_INLINE Vec4 operator+(const Vec4& r) const { return _mm_add_ps(simd,r.simd); }
  FLAME_INLINE Vec4 operator-(const Vec4& r) const { return _mm_sub_ps(simd,r.simd); }
  FLAME_INLINE Vec4 operator*(float s) const { return _mm_mul_ps(simd,_mm_set1_ps(s)); }
  FLAME_INLINE Vec4 operator/(float s) const { return _mm_div_ps(simd,_mm_set1_ps(s)); }

  FLAME_INLINE Vec4& operator+=(const Vec4& r){ simd=_mm_add_ps(simd,r.simd); return *this; }
  FLAME_INLINE Vec4& operator-=(const Vec4& r){ simd=_mm_sub_ps(simd,r.simd); return *this; }
};

using Vec3 = Vec4;
using Vec2 = Vec4;

FLAME_INLINE float dot(const Vec4& a,const Vec4& b){ return _mm_cvtss_f32(_mm_dp_ps(a.simd,b.simd,0xFF)); }
FLAME_INLINE float length(const Vec4& v){ return std::sqrt(dot(v,v)); }

FLAME_INLINE Vec4 normalize(const Vec4& v)
{
  Simd128 dp=_mm_dp_ps(v.simd,v.simd,0xFF);
  Simd128 inv=_mm_rsqrt_ps(dp);
  return _mm_mul_ps(v.simd,inv);
}

FLAME_INLINE Vec3 cross(const Vec3& a,const Vec3& b)
{
  Simd128 t0=_mm_shuffle_ps(a.simd,a.simd,_MM_SHUFFLE(3,0,2,1));
  Simd128 t1=_mm_shuffle_ps(b.simd,b.simd,_MM_SHUFFLE(3,1,0,2));
  Simd128 t2=_mm_shuffle_ps(a.simd,a.simd,_MM_SHUFFLE(3,1,0,2));
  Simd128 t3=_mm_shuffle_ps(b.simd,b.simd,_MM_SHUFFLE(3,0,2,1));
  return _mm_sub_ps(_mm_mul_ps(t0,t1),_mm_mul_ps(t2,t3));
}

struct FLAME_ALIGN(16) Mat4 
{
  Simd128 rows[4];

  static Mat4 identity(){
	Mat4 m;
	m.rows[0]=_mm_setr_ps(1,0,0,0);
	m.rows[1]=_mm_setr_ps(0,1,0,0);
	m.rows[2]=_mm_setr_ps(0,0,1,0);
	m.rows[3]=_mm_setr_ps(0,0,0,1);
	return m;
  }
};

FLAME_INLINE Mat4 mul(const Mat4& a,const Mat4& b)
{
  Mat4 o;
  for(int i=0;i<4;i++){
	Simd128 e0=_mm_shuffle_ps(a.rows[i],a.rows[i],0x00);
	Simd128 e1=_mm_shuffle_ps(a.rows[i],a.rows[i],0x55);
	Simd128 e2=_mm_shuffle_ps(a.rows[i],a.rows[i],0xAA);
	Simd128 e3=_mm_shuffle_ps(a.rows[i],a.rows[i],0xFF);
	o.rows[i]=_mm_add_ps(_mm_add_ps(_mm_mul_ps(e0,b.rows[0]),_mm_mul_ps(e1,b.rows[1])),_mm_add_ps(_mm_mul_ps(e2,b.rows[2]),_mm_mul_ps(e3,b.rows[3])));
  }
  return o;
}

struct FLAME_ALIGN(16) Quat
{
  float x,y,z,w;
};

FLAME_INLINE Quat quat_mul(const Quat& a,const Quat& b)
{
  return {
	a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
	a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
	a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
	a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z
  };
}

struct Ray { Vec3 origin, direction; };
struct AABB { Vec3 min, max; };
struct Plane { Vec3 normal; float d; };

FLAME_INLINE bool intersect_ray_aabb(const Ray& r,const AABB& b)
{
  float tmin = (b.min.x - r.origin.x) / r.direction.x;
  float tmax = (b.max.x - r.origin.x) / r.direction.x;
  if(tmin > tmax) std::swap(tmin,tmax);

  float tymin = (b.min.y - r.origin.y) / r.direction.y;
  float tymax = (b.max.y - r.origin.y) / r.direction.y;
  if(tymin > tymax) std::swap(tymin,tymax);

  return !(tmin > tymax || tymin > tmax);
}

FLAME_INLINE Mat4 look_at(const Vec3& eye,const Vec3& target,const Vec3& up)
{
  Vec3 z = normalize(eye - target);
  Vec3 x = normalize(cross(up,z));
  Vec3 y = cross(z,x);

  Mat4 m;
  m.rows[0]=_mm_setr_ps(x.x,x.y,x.z,-dot(x,eye));
  m.rows[1]=_mm_setr_ps(y.x,y.y,y.z,-dot(y,eye));
  m.rows[2]=_mm_setr_ps(z.x,z.y,z.z,-dot(z,eye));
  m.rows[3]=_mm_setr_ps(0,0,0,1);
  return m;
}

}   

#endif  
