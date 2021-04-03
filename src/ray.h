#ifndef _RAY_H
#define _RAY_H

#include <iostream>
#define eps 1e-6
using namespace std;

#include "vectors.h"
#include <cmath>
// ====================================================================
// ====================================================================

// Ray class mostly copied from Peter Shirley and Keith Morley

class Ray {

public:

  // CONSTRUCTOR & DESTRUCTOR
  Ray () {}
  Ray (const Vec3f &orig, const Vec3f &dir) {
    origin = orig; 
    direction = dir; 
    }
  Ray (const Ray& r) {*this=r;}
  Ray (const Vec3f &orig, const Vec3f &dir,int tp) {
    origin = orig; 
    direction = dir; 
    type=tp;
 }
 Ray (Vec3f& p0,Vec3f& norm, float theta,float phi)
    {
        origin=p0;
        float cosinePhi=(float)cos(phi),sinePhi=(float)sin(phi),cosineTheta=(float)cos(theta),sineTheta=(float)sin(theta);
        Vec3f Z(0,0,1);
        Vec3f baseX;
        Vec3f baseY;
        norm.Normalize();
        Vec3f::Cross3(baseX,norm,Z);
        if(baseX.Length()<1e-4) direction.Set(sinePhi*cosineTheta,sinePhi*sineTheta,cosinePhi);
        else{
            Vec3f::Cross3(baseY,norm,baseX);
            baseX.Normalize();
            baseY.Normalize();
        direction=sinePhi*cosineTheta*baseX+sinePhi*sineTheta*baseY+cosinePhi*norm;
            direction.Normalize();
        }
    }
  // ACCESSORS
  const Vec3f& getOrigin() const { return origin; }
  const Vec3f& getDirection() const { return direction; }
  Vec3f pointAtParameter(float t) const {
    return origin+direction*t; }
  int getType() const{ return type;}
    void setOrigin(const Vec3f& v){
        origin=v;
    }
private:

  // REPRESENTATION
  Vec3f origin;
  Vec3f direction;
  int type=0;
};

inline ostream &operator<<(ostream &os, const Ray &r) {
  os << "Ray <o:" <<r.getOrigin()<<", d:"<<r.getDirection()<<">";
  return os;
}

// ====================================================================
// ====================================================================

#endif
