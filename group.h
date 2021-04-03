#ifndef _GROUP_H
#define _GROUP_H
#include "object3d.h"
 #include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
 #include <GLUT/glut.h>
 #include <vector>
#include <random>
#include "triangle.h"
 using namespace std;
bool intersectFacet(const Ray& ray, Vec3f& minPoint, Vec3f& maxPoint,Hit& hit);
extern std::default_random_engine generator;
extern std::uniform_real_distribution<double> distribution;
extern int resolution;
extern int SAH;

struct cmpx{
    bool operator() (Object3D* a, Object3D* b)
{
    return a->getCenter().x() < b->getCenter().x();
}
};

struct cmpy{
 bool operator()( Object3D* a, Object3D* b)
 {
     return a->getCenter().y() < b->getCenter().y();
 }
};

struct cmpz{
 bool operator()( Object3D* a, Object3D* b)
 {
     return a->getCenter().z() < b->getCenter().z();
 }
};

class SpatialTree{
public:
    SpatialTree(vector<Object3D*>& shapeBuffer,Vec3f Min, Vec3f Max){
        minPoint=Min;
        maxPoint=Max;
        if(shapeBuffer.size()<=resolution) objects=shapeBuffer;
        else{
            Vec3f rightMin(inf,inf,inf),rightMax(-inf,-inf,-inf),leftMin(inf,inf,inf),leftMax(-inf,-inf,-inf);
            float centerMinX=inf,centerMinY=inf,centerMinZ=inf;
            float centerMaxX=-inf,centerMaxY=-inf,centerMaxZ=-inf;
            for(unsigned int i=0;i<shapeBuffer.size();i++){
                Vec3f center=shapeBuffer[i]->getCenter();
                if(center.x()<centerMinX) centerMinX=center.x();
                if(center.x()>centerMaxX) centerMaxX=center.x();
                if(center.y()<centerMinY) centerMinY=center.y();
                if(center.y()>centerMaxY) centerMaxY=center.y();
                if(center.z()<centerMinZ) centerMinZ=center.z();
                if(center.z()>centerMaxZ) centerMaxZ=center.z();
            }
            float lenX=centerMaxX-centerMinX,lenY=centerMaxY-centerMinY,lenZ=centerMaxZ-centerMinZ;
            float len=max(lenX,max(lenY,lenZ));
        if(abs(len-lenX)<1e-6){
        sort(shapeBuffer.begin(),shapeBuffer.end(),cmpx());
        }
        else if(abs(len-lenY)<1e-6){
         sort(shapeBuffer.begin(),shapeBuffer.end(),cmpy());
        }
        else {
            sort(shapeBuffer.begin(),shapeBuffer.end(),cmpz());
        }
        vector<Object3D*> leftShapeBuffer;
        vector<Object3D*> rightShapeBuffer;
            unsigned int optimal=shapeBuffer.size()/2;
            //search cost
            if(optimal<=SAH){
           float minCost=inf;
            for(int i=1;i<shapeBuffer.size();i++){
                leftMin.Set(inf,inf,inf);
                leftMax.Set(-inf,-inf,-inf);
                rightMin=leftMin;
                rightMax=leftMax;
                float pa=-1,pb=-1;
                for(int j=0;j<i;j++){
                    shapeBuffer[j]->getMinMax(Min,Max);
                    if(Min.x()<leftMin.x()) leftMin.Set(Min.x(),leftMin.y(),leftMin.z());
                    if(Max.x()>leftMax.x()) leftMax.Set(Max.x(),leftMax.y(),leftMax.z());
                    if(Min.y()<leftMin.y()) leftMin.Set(leftMin.x(),Min.y(),leftMin.z());
                    if(Max.y()>leftMax.y()) leftMax.Set(leftMax.x(),Max.y(),leftMax.z());
                    if(Min.z()<leftMin.z()) leftMin.Set(leftMin.x(),leftMin.y(),Min.z());
                    if(Max.z()>leftMax.z()) leftMax.Set(leftMax.x(),leftMax.y(),Max.z());
                }
                for(int j=i;j<shapeBuffer.size();j++){
                    shapeBuffer[j]->getMinMax(Min,Max);
                    if(Min.x()<rightMin.x()) rightMin.Set(Min.x(),rightMin.y(),rightMin.z());
                    if(Max.x()>rightMax.x()) rightMax.Set(Max.x(),rightMax.y(),rightMax.z());
                    if(Min.y()<rightMin.y()) rightMin.Set(rightMin.x(),Min.y(),rightMin.z());
                    if(Max.y()>rightMax.y()) rightMax.Set(rightMax.x(),Max.y(),rightMax.z());
                    if(Min.z()<rightMin.z()) rightMin.Set(rightMin.x(),rightMin.y(),Min.z());
                    if(Max.z()>rightMax.z()) rightMax.Set(rightMax.x(),rightMax.y(),Max.z());
                }
                if(abs(lenX-len)<1e-6){
                    pa=(leftMax.y()-leftMin.y())*(leftMax.z()-leftMin.z());
                    pb=(rightMax.y()-rightMin.y())*(rightMax.z()-rightMin.z());
                }
                if(abs(lenY-len)<1e-6){
                       pa=(leftMax.x()-leftMin.x())*(leftMax.z()-leftMin.z());
                       pb=(rightMax.x()-rightMin.x())*(rightMax.z()-rightMin.z());
                   }
                   if(abs(lenZ-len)<1e-6){
                       pa=(leftMax.y()-leftMin.y())*(leftMax.x()-leftMin.x());
                       pb=(rightMax.y()-rightMin.y())*(rightMax.x()-rightMin.x());
                   }
                float cost=pa*i+pb*(shapeBuffer.size()-1-i);
                if(cost<minCost){
                    optimal=i;
                    minCost=cost;
                }
            }
            leftMin.Set(inf,inf,inf);
            leftMax.Set(-inf,-inf,-inf);
            rightMin=leftMin;
            rightMax=leftMax;
            }
            //cout<<1.0*optimal/shapeBuffer.size()<<endl;
            for(unsigned int i=0;i<shapeBuffer.size();i++){
                shapeBuffer[i]->getMinMax(Min,Max);
                if(i<optimal){
                    leftShapeBuffer.push_back(shapeBuffer[i]);
                    if(Min.x()<leftMin.x()) leftMin.Set(Min.x(),leftMin.y(),leftMin.z());
                    if(Max.x()>leftMax.x()) leftMax.Set(Max.x(),leftMax.y(),leftMax.z());
                    if(Min.y()<leftMin.y()) leftMin.Set(leftMin.x(),Min.y(),leftMin.z());
                    if(Max.y()>leftMax.y()) leftMax.Set(leftMax.x(),Max.y(),leftMax.z());
                    if(Min.z()<leftMin.z()) leftMin.Set(leftMin.x(),leftMin.y(),Min.z());
                    if(Max.z()>leftMax.z()) leftMax.Set(leftMax.x(),leftMax.y(),Max.z());
                }
                else{
                    rightShapeBuffer.push_back(shapeBuffer[i]);
                    if(Min.x()<rightMin.x()) rightMin.Set(Min.x(),rightMin.y(),rightMin.z());
                    if(Max.x()>rightMax.x()) rightMax.Set(Max.x(),rightMax.y(),rightMax.z());
                    if(Min.y()<rightMin.y()) rightMin.Set(rightMin.x(),Min.y(),rightMin.z());
                    if(Max.y()>rightMax.y()) rightMax.Set(rightMax.x(),Max.y(),rightMax.z());
                    if(Min.z()<rightMin.z()) rightMin.Set(rightMin.x(),rightMin.y(),Min.z());
                    if(Max.z()>rightMax.z()) rightMax.Set(rightMax.x(),rightMax.y(),Max.z());
                    
                }
            }
        left=new SpatialTree(leftShapeBuffer,leftMin,leftMax);
        right=new SpatialTree(rightShapeBuffer,rightMin,rightMax);
        }
    }
    bool intersect(const Ray& ray,Hit& hit, float tmin){
        bool intersected=false;
        Hit hitt(inf,NULL,-1.0*ray.getDirection());
        if(intersectFacet(ray, minPoint, maxPoint,hitt)){
        if(!objects.empty())
        for(unsigned int i=0;i<objects.size();i++) intersected |= objects[i]->intersect(ray,hit,tmin);
        if(left!=NULL) intersected |= left->intersect(ray,hit,tmin);
        if(right!=NULL) intersected |= right->intersect(ray,hit,tmin);
        }
            return intersected;
    }
    bool intersectShadowRay(const Ray& ray,float tHitLight){
        bool intersected=false;
        Hit hit(inf,NULL,-1.0*ray.getDirection());
        if(intersectFacet(ray, minPoint, maxPoint,hit)){
        for(unsigned int i=0;i<objects.size();i++)
            if(objects[i]->intersectShadowRay(ray,tHitLight)) return 1;
        if(left!=NULL) intersected |= left->intersectShadowRay(ray,tHitLight);
        if(right!=NULL) intersected |= right->intersectShadowRay(ray,tHitLight);
        }
            return intersected;
    }
private:
    vector<Object3D*> objects;
    SpatialTree* left=NULL;
    SpatialTree* right=NULL;
    Vec3f minPoint;
    Vec3f maxPoint;
};
class Group:public Object3D{
		public:
			Group(int _num);
			void addObject(int index, Object3D* obj);
			virtual bool intersect(const Ray&, Hit&,float);
			virtual bool intersectShadowRay(const Ray&, float);
			virtual void intersectTShadowRay(const Ray&, Vec3f&,vector<Hit>&,float);
			virtual void paint(void);
            void computeBoundingBox(int);
    bool initializeMarch(Ray& ray);
            virtual void getMinMax(Vec3f& Min, Vec3f& Max)
    {
        Min=minPoint;
        Max=maxPoint;
    }
    virtual int getIdx()
    {
        return idx;
    }
    bool cameraIn(const Vec3f& cameraPos);
    void createTree(){
        Vec3f Min,Max;
        for(int i=0;i<num;i++){
            arr[i]->getMinMax(Min,Max);
            if(Min.x()<minPoint.x()) minPoint.Set(Min.x(),minPoint.y(),minPoint.z());
            if(Max.x()>maxPoint.x()) maxPoint.Set(Max.x(),maxPoint.y(),maxPoint.z());
            if(Min.y()<minPoint.y()) minPoint.Set(minPoint.x(),Min.y(),minPoint.z());
            if(Max.y()>maxPoint.y()) maxPoint.Set(maxPoint.x(),Max.y(),maxPoint.z());
            if(Min.z()<minPoint.z()) minPoint.Set(minPoint.x(),minPoint.y(),Min.z());
            if(Max.z()>maxPoint.z()) maxPoint.Set(maxPoint.x(),maxPoint.y(),Max.z());
        }
        float Epsilon=min(maxPoint.z()-minPoint.z(),min(maxPoint.x()-minPoint.x(),maxPoint.y()-minPoint.y()));
        Vec3f epsilon(0.01*Epsilon,0.01*Epsilon,0.01*Epsilon);
        maxPoint=maxPoint+epsilon;
        minPoint=minPoint-epsilon;
        vector<Object3D*> shapeBuffer;
        for(int i=0;i<num;i++) shapeBuffer.push_back(arr[i]);
        tree=new SpatialTree(shapeBuffer,minPoint,maxPoint);
    }
    virtual Vec3f getCenter(){
        Vec3f center=0.5*(minPoint+maxPoint);
        return center;
    }
			~Group();
		private:
			int num;
			Object3D** arr;
            vector<int> ***shapeList;
            bool* intersected;
            float stepX;
            float stepY;
            float stepZ;
            int resolution;
            Vec3f maxPoint;
            Vec3f minPoint;
            SpatialTree* tree;
            bool within(const Vec3f& p, int idx, int idy, int idz);
};


#endif

