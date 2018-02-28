#ifndef QUATERNION_H
#define QUATERNION_H

#include <iostream>
#include "vec.h"
#include "mat.h"

class quaternion;
float dot(const quaternion& p, const quaternion& q);

struct quaternion
{
    float s;
    vec3 v;

    quaternion(): s(1) {}

    quaternion(float angle, const vec3& u): s(angle), v(u) {}

    // angle is in degrees
    void from_angle_and_axis(float angle,const vec3& axis);
    void to_angle_and_axis(float& angle,vec3& axis) const;
    mat3 rotation_matrix3() const;
    mat4 rotation_matrix4() const;
    vec3 rotate(const vec3& x) const;

    quaternion& operator += (const quaternion& q)
    {s += q.s; v += q.v; return *this;}

    quaternion& operator -= (const quaternion& q)
    {s -= q.s; v -= q.v; return *this;}

    quaternion& operator *= (const quaternion& q)
    {return *this = *this * q;}

    // Assumes normalized
    quaternion& operator /= (const quaternion& q)
    {return *this *= q.inv();}

    quaternion& operator *= (const float& c)
    {s *= c; v *= c; return *this;}

    quaternion& operator /= (const float& c)
    {s /= c; v /= c; return *this;}

    quaternion operator + () const
    {return *this;}

    quaternion operator - () const
    {return quaternion(-s, -v);}

    quaternion operator + (const quaternion& q) const
    {return quaternion(s + q.s, v + q.v);}

    quaternion operator - (const quaternion& q) const
    {return quaternion(s - q.s, v - q.v);}

    quaternion operator * (const quaternion& q) const
    {return quaternion(s * q.s - dot(v, q.v), v * q.s + s * q.v + cross(v, q.v));}

    // Assumes normalized
    quaternion operator / (const quaternion& q) const
    {return *this * q.inv();}

    quaternion operator * (const float& c) const
    {return quaternion(s * c, v * c);}

    quaternion operator / (const float& c) const
    {return quaternion(s / c, v / c);}

    // Assumes normalized
    quaternion inv() const
    {return quaternion(s, -v);}

    float magnitude() const
    {return sqrt(dot(*this, *this));}

    quaternion normalized() const
    {return *this / magnitude();}

};

inline float dot(const quaternion& p, const quaternion& q)
{return p.s * q.s + dot(p.v, q.v);}

quaternion pow(const quaternion& q, float r);

// interpolate from p to q (u is in the range 0 to 1)
quaternion slerp(float u, const quaternion& p, const quaternion& q);

inline std::ostream& operator<<(std::ostream& ostr, const quaternion& q)
{return ostr << q.s << " " << q.v;}

#endif
