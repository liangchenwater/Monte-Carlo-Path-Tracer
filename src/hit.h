#ifndef _HIT_H
#define _HIT_H
#define inf  (0xff<<23)
#include "vectors.h"
#include "ray.h"

class Material;

// ====================================================================
// ====================================================================

class Hit {
  
public:

  // CONSTRUCTOR & DESTRUCTOR
  Hit() { material = NULL; }
  Hit(float _t, Material *m, Vec3f n) { 
    t = _t; material = m; normal=n; }
  Hit(const Hit &h) { 
    t = h.t; 
    material = h.material; 
    normal=h.normal;
    intersectionPoint = h.intersectionPoint; }
  ~Hit() {}
    // ACCESSORS
  bool operator <(const Hit& h)const{return t<h.t;}
  float getT() const { return t; }
  Material* getMaterial() const { return material; }
  Vec3f getNormal() const {return normal;}
  Vec3f getIntersectionPoint() const { return intersectionPoint; }
    Vec2f getText() const {return text;}
  // MODIFIER
  void set(float _t, Material *m, Vec3f n,Vec2f _text, const Ray &ray) {
      t = _t; material = m; normal=n; text=_text;
    intersectionPoint = ray.pointAtParameter(t); }
    void set(float _t, Material *m, Vec3f n,const Ray &ray) {
        t = _t; material = m; normal=n;
      intersectionPoint = ray.pointAtParameter(t); }
    void set(float _t, const Ray& ray)
    {
        t = _t;intersectionPoint = ray.pointAtParameter(t);
    }
    bool isLight(){
        return light;
    }
    void setLight()
    {
        light=true;
    }
private: 

  // REPRESENTATION
  float t;
  Material *material;
  Vec3f normal;
  Vec3f intersectionPoint;
  Vec2f text;
  bool light=false;
};

inline ostream &operator<<(ostream &os, const Hit &h) {
  os << "Hit " << h.getT() <<","<<h.getNormal()<<">";
  return os;
}
// ====================================================================
// ====================================================================

#endif
