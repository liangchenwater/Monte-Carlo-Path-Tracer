#ifndef SPHERE_H
#define SPHERE_H
#include "object3d.h"
#include <cmath>
 #include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
 #include <GLUT/glut.h>
 #include <vector>
 using namespace std;
 
class Sphere:public Object3D{
	public:
		Sphere(Vec3f& _center,float _radius,Material* m):center(_center),radius(_radius),Object3D(m){}
		virtual bool intersect(const Ray&, Hit&, float);
		virtual bool intersectShadowRay(const Ray&,float);
		virtual void intersectTShadowRay(const Ray&, Vec3f&,vector<Hit>&,float);
		float getR() const{return radius;}
		Vec3f getC() const{return center;}
	virtual void paint(void);
    virtual void getMinMax(Vec3f& Min, Vec3f& Max){
        Max.Set(center.x()+radius,center.y()+radius,center.z()+radius);
        Min.Set(center.x()-radius,center.y()-radius,center.z()-radius);
    }
virtual int getIdx()
{
return idx;
}
    virtual Vec3f getCenter(){
        return center;
    }
	private:
		Vec3f center;
		float radius;
};
#endif
