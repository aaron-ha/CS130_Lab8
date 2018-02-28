#include "quaternion.h"
#include "vec.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

using namespace std;

float sqr(float t)
{return t*t;}

quaternion slerp(float u, const quaternion& q0, const quaternion& q1)
{
    //TODO: implement me!
	float dot = dot(q0,q1);

	if(dot < 0.0f){
		q1 = -q1;
		dot = -dot;
	}
	
	return pow((q1*q0.inv()),u)*q0;
}

void quaternion::from_angle_and_axis(float angle,const vec3& axis)
{
    //TODO: comute the quaternion parameters (s,v) from the input rotation angle (in degrees) and axis
	angle = angle * M_PI / 180;
    s = cos(angle/2); 
    v = sin(angle/2) * axis.normalized();
}

void quaternion::to_angle_and_axis(float& angle,vec3& axis) const
{
    //TODO: compute the angle (in degrees) and the axis of the rotation, from the quaternion parameters (s,v)
	angle = atan((v.magnitude()/s)* 180 / M_PI)*2;
    axis = (v/v.magnitude()).normalized();
}

quaternion pow(const quaternion& q, float r)
{
    vec3 axis;
    float angle;
    q.to_angle_and_axis(angle,axis);
    quaternion p;
    angle*=r;
    p.from_angle_and_axis(angle,axis);
    return p;
}

vec3 quaternion::rotate(const vec3& x) const
{
    return ((*this)*quaternion(0,x)*this->inv()).v;
}

mat3 quaternion::rotation_matrix3() const
{float vx2=sqr(v.x[0]),vy2=sqr(v.x[1]),vz2=sqr(v.x[2]),vxvy=v.x[0]*v.x[1],vxvz=v.x[0]*v.x[2],vyvz=v.x[1]*v.x[2],svx=s*v.x[0],svy=s*v.x[1],svz=s*v.x[2];
    mat3 m;
    m(0,0)=1-2*(vy2+vz2);
    m(1,0)=2*(vxvy+svz);
    m(2,0)=2*(vxvz-svy);
    m(0,1)=2*(vxvy-svz);
    m(1,1)=1-2*(vx2+vz2);
    m(2,1)=2*(vyvz+svx);
    m(0,2)=2*(vxvz+svy);
    m(1,2)=2*(vyvz-svx);
    m(2,2)=1-2*(vx2+vy2);
    return m;}



mat4 quaternion::rotation_matrix4() const
{float vx2=sqr(v.x[0]),vy2=sqr(v.x[1]),vz2=sqr(v.x[2]),vxvy=v.x[0]*v.x[1],vxvz=v.x[0]*v.x[2],vyvz=v.x[1]*v.x[2],svx=s*v.x[0],svy=s*v.x[1],svz=s*v.x[2];
    mat4 m;
    m(0,0)=1-2*(vy2+vz2);
    m(1,0)=2*(vxvy+svz);
    m(2,0)=2*(vxvz-svy);
    m(0,1)=2*(vxvy-svz);
    m(1,1)=1-2*(vx2+vz2);
    m(2,1)=2*(vyvz+svx);
    m(0,2)=2*(vxvz+svy);
    m(1,2)=2*(vyvz-svx);
    m(2,2)=1-2*(vx2+vy2);
    m(3,3)=1;
    return m;}
