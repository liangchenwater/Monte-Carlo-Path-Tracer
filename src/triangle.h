#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "object3d.h"
#include "material.h"
#include "hit.h"
#include "ray.h"
#include "matrix.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
 #include<vector>
 using namespace std;
 
class Triangle:public Object3D{
		public:
			Triangle(Vec3f& _a,Vec3f& _b,Vec3f& _c, Material *m):a(_a),b(_b),c(_c),Object3D(m){
                Vec3f c1=a-b,c2=a-c;
                Vec3f::Cross3(norm,c1,c2);
                norm.Normalize();
                na=norm;
                nb=norm;
                nc=norm;
            }
            Triangle(Vec3f& _a,Vec3f& _b,Vec3f& _c, Vec3f& _na,Vec3f& _nb,Vec3f& _nc,
                     Vec2f& _ta,Vec2f& _tb,Vec2f& _tc, Material *m):a(_a),b(_b),c(_c),na(_na),nb(_nb),nc(_nc),ta(_ta),tb(_tb),tc(_tc),Object3D(m){
                Vec3f c1=a-b,c2=a-c;
                Vec3f::Cross3(norm,c1,c2);
                norm.Normalize();
            }
    Triangle(Vec3f& _a,Vec3f& _b,Vec3f& _c, Vec3f& _na,Vec3f& _nb,Vec3f& _nc,
             Vec2f& _ta,Vec2f& _tb,Vec2f& _tc, Material *m, int _idx):a(_a),b(_b),c(_c),na(_na),nb(_nb),nc(_nc),ta(_ta),tb(_tb),tc(_tc),Object3D(m){
        Vec3f c1=a-b,c2=a-c;
        Vec3f::Cross3(norm,c1,c2);
        norm.Normalize();
        idx=_idx;
    }
			virtual bool intersect(const Ray&,Hit&,float);
			virtual bool intersectShadowRay(const Ray&,float);
			virtual void intersectTShadowRay(const Ray&, Vec3f&,vector<Hit>&,float);
			virtual void paint(void) ;
    virtual void getMinMax(Vec3f& Min, Vec3f& Max)
{
    float minx=min(a.x(),min(b.x(),c.x())),
    miny=min(a.y(),min(b.y(),c.y())),
    minz=min(a.z(),min(b.z(),c.z())),
    maxx=max(a.x(),max(b.x(),c.x())),
    maxy=max(a.y(),max(b.y(),c.y())),
    maxz=max(a.z(),max(b.z(),c.z()));
    Min.Set(minx,miny,minz);
    Max.Set(maxx,maxy,maxz);
}
    virtual int getIdx(){
        return idx;
    }
    virtual Vec3f getCenter(){
        Vec3f center= 1.0/3*(a+b+c);
        return center;
    }
		private:
			Vec3f a;
			Vec3f b;
			Vec3f c;
            Vec3f na;
            Vec3f nb;
            Vec3f nc;
            Vec3f norm;
            Vec2f ta;
            Vec2f tb;
            Vec2f tc;
    friend class AreaLight;
};
#endif
