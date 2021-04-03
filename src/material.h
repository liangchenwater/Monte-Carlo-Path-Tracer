#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"
#include "ray.h"
#include "hit.h"
#include "image.h"
#include <cmath>
 #include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
 #include <GLUT/glut.h>
class Material {

public:

  // CONSTRUCTORS & DESTRUCTOR
  Material(const Vec3f &d_color):albedo(d_color){}
  virtual ~Material(){};
  virtual Vec3f Shade(Vec3f wi, Hit& hit, Vec3f wo, Vec3f color,bool cosineWeigthted) const=0;
  // ACCESSORS
    Vec3f getAlbedo() const { return albedo; }
  virtual void glSetMaterial(void) const=0;
  virtual Vec3f getReflectance() const=0;
  virtual Vec3f getReflectiveColor() const=0;
  virtual Vec3f getTransparentColor() const=0;
    virtual Vec3f getEmittance() const=0;
  virtual float getExponent()=0;
  virtual float getIndexOfRefraction()=0;
    virtual float getAlpha()=0;
    virtual float getMentalness()=0;
   virtual Vec3f sampleKd(Hit&)=0;
protected:
  // REPRESENTATION
  Vec3f albedo;
};

class CookMaterial:public Material{
    public:
        CookMaterial(const Vec3f & _diffuseColor, const Vec3f & _specularColor, float _exponent):Material(_diffuseColor),ks(_specularColor),exponent(_exponent){}
    
         CookMaterial(const Vec3f &d_color):Material(d_color){}
    
         CookMaterial(const Vec3f& _diffuseColor, const Vec3f& _specularColor, float _exponent, const Vec3f& _reflectiveColor, const Vec3f& _transparentColor,  float _indexOfRefraction):Material(_diffuseColor),ks(_specularColor),exponent(_exponent),reflectiveColor(_reflectiveColor),transparentColor(_transparentColor),indexOfRefraction(_indexOfRefraction){}
    
    CookMaterial(const Vec3f& d, const Vec3f& s, const Vec3f& e, float exp, float _alpha,
                 float _mentalness,
                 Image* map_Kd):Material(d),ks(s),emittance(e),exponent(exp),map_Kd(map_Kd)
    ,alpha(_alpha),mentalness(_mentalness){}
    
    Vec3f Shade(Vec3f wi, Hit& hit, Vec3f wo, Vec3f color,bool cosineWeigthted) const;
       virtual void glSetMaterial(void) const;
    
    Vec3f getReflectance()const{return ks;}
    float getAlpha() {return alpha;}
    float getMentalness() {return mentalness;}
    
        Vec3f getReflectiveColor() const{return reflectiveColor;}
         Vec3f getTransparentColor() const{return transparentColor;}
    Vec3f getEmittance() const {return emittance;}
         float getExponent (){return exponent;}
        float getIndexOfRefraction(){return indexOfRefraction;}
    Vec3f sampleKd(Hit&);
    
    private:
        //Vec3f albedo;
        Vec3f emittance;
        Vec3f ks;
        float exponent;
    float alpha;
    float mentalness;
        Vec3f transparentColor;
    Vec3f reflectiveColor;
        float indexOfRefraction=1;
        Image* map_Kd=NULL;
};
#endif
