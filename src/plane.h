#ifndef _PLANE_H
#define _PLANE_H

#include "object3d.h"
#include "material.h"
#include "hit.h"
#include "ray.h"
 #include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
 #include <GLUT/glut.h>
 #include<vector>
 using namespace std;
 
class Plane:public Object3D{
	public:
		Plane(Vec3f& _normal,float _d, Material * _m):d(_d),Object3D(_m),normal(_normal){}
    Plane(){}
		virtual bool intersect(const Ray&,Hit&,float);
		virtual bool intersectShadowRay(const Ray&,float);
		virtual void intersectTShadowRay(const Ray&, Vec3f&,vector<Hit>&,float);
		virtual void paint(void);
    virtual void getMinMax(Vec3f& Min, Vec3f& Max){}
    virtual int getIdx()
{
return idx;
}
    virtual Vec3f getCenter(){
        Vec3f a;
        return a;
    }
private:
		Vec3f normal;
		float d;
};

#endif
