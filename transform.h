#ifndef _TRANSFORM_H
#define _TRANSFORM_H
#include "object3d.h"
#include "ray.h"
#include "hit.h" 
#include "material.h"
#include "matrix.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
 #include<vector>
 using namespace std;
 class Transform:public Object3D{
	public:
		Transform(Matrix& _m,Object3D* _o):mx(_m),o(_o){}
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
		Object3D* o;
		Matrix mx;
};
#endif
