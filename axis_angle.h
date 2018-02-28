//#####################################################################
// Class axis_angle
//#####################################################################
#ifndef __axis_angle__
#define __axis_angle__
#include "vec.h"

struct axis_angle
{
    vec3 w; // represents theta * e where theta is in radians
    axis_angle(const vec3& w_input=vec3()):w(w_input) {}

    void from_angle_and_axis(float angle,const vec3& axis);
    void to_angle_and_axis(float& angle,vec3& axis) const;
};

#endif 
