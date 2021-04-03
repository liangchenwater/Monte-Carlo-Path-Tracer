#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "vectors.h"
#include "object3d.h"
#include "triangle.h"
#include "sphere.h"
#include <random>

// ====================================================================
// ====================================================================
extern std::default_random_engine generator;
extern std::uniform_real_distribution<double> distribution;
class Light {

public:

  // CONSTRUCTOR & DESTRUCTOR
  Light() {}
  virtual ~Light() {}

  // VIRTUAL METHODS
  virtual void getIllumination (const Vec3f &p, Vec3f &dir, Vec3f &col, 
				float &distanceToLight) const = 0;
  virtual void glInit(int id) = 0;
  virtual void samplingLight(Vec3f&)=0;
  virtual float getArea()=0;
    virtual  Vec3f getEmit(float u,float v)=0;
  int getType(){
        return lightType;
    }
protected:
    int lightType;
};

// ====================================================================
// ====================================================================

class DirectionalLight : public Light {

public:

  // CONSTRUCTOR & DESTRUCTOR
  DirectionalLight(const Vec3f &d, const Vec3f &c) {
    direction = d; direction.Normalize();
      color = c; lightType=0;}
  ~DirectionalLight() {}

  // VIRTUAL METHODS
  void getIllumination (const Vec3f &p, Vec3f &dir, Vec3f &col, 
			float &distanceToLight) const {
    // the direction to the light is the opposite of the
    // direction of the directional light source
    dir = direction * (-1.0f);
    col = color;
    // the distance is INFINITY
    distanceToLight = INFINITY; 
  }

  void glInit(int id);
    //null
    void samplingLight(Vec3f& pos) {}
    float getArea(){return 0.0;}
    Vec3f getEmit(float u,float v){
        return Vec3f(1,1,1);
    }
private:

  DirectionalLight(); //don't use

  // REPRESENTATION
  Vec3f direction;
  Vec3f color;

};

// ====================================================================
// ====================================================================

class PointLight : public Light {

public:

  // CONSTRUCTOR & DESTRUCTOR
  PointLight(const Vec3f &p, const Vec3f &c, float a1, float a2, float a3) {
    position = p; 
    color = c; 
    attenuation_1 = a1;
    attenuation_2 = a2;
      attenuation_3 = a3; lightType=1;}
  ~PointLight() {}

  // VIRTUAL METHODS
  void getIllumination (const Vec3f &p, Vec3f &dir, Vec3f &col, float &distanceToLight) const {
    dir = position - p;
    // grab the length before the direction is normalized
    distanceToLight = dir.Length(); 
    dir.Normalize(); 
    float attenuation = 1 / (attenuation_1 + 
			     attenuation_2*distanceToLight + 
			     attenuation_3*distanceToLight*distanceToLight);
    if (attenuation < 0) attenuation = 0;
    col = color * attenuation;
  }

  void glInit(int id);
    //null
    void samplingLight(Vec3f& pos) {}
    float getArea(){return 0.0;}
    Vec3f getEmit(float u,float v){
        return Vec3f(1,1,1);
    }
private:

  PointLight(); // don't use

  // REPRESENTATION
  Vec3f position;
  Vec3f color;

  // attenuation of the light
  float attenuation_1;
  float attenuation_2;
  float attenuation_3;

};

class AreaLight:public Light{
public:
    AreaLight(Triangle* shape, Vec3f color):shape(shape),color(color){lightType=2;}
    void samplingLight(Vec3f& pos){
        Vec3f a=shape->a,b=shape->b,c=shape->c;
        float r1=(float)distribution(generator),r2=(float)distribution(generator);
        float coe_a=1-sqrt(r1),coe_b=sqrt(r1)*r2,coe_c=1-coe_a-coe_b;
        pos=coe_a*a+coe_b*b+coe_c*c;
    }
    float getArea(){
        Vec3f a=shape->a,b=shape->b,c=shape->c;
        Vec3f base1=a-b,base2=a-c;
        Vec3f area;
        Vec3f::Cross3(area,base1,base2);
        return 0.5*area.Length();
    }
    void getIllumination(const Vec3f& objPos, Vec3f& lightPos, Vec3f& col, float& dist) const
    {
        Vec3f dir=lightPos-objPos;
        dist=dir.Length();
        float coe=abs(dir.Dot3(shape->norm));
        coe=coe/(dist*dist);
        col=color*coe;
    }
    void glInit(int id);
    Vec3f getEmit(float u,float v){
        return Vec3f(1,1,1);
    }
private:
    Triangle* shape;
    Vec3f color;
};

class EnvLight:public Light{
public:
    EnvLight(Image* _img, Sphere* sphere):img(_img),shape(sphere){
        lightType=3;
        int width=img->Width(),height=img->Height();
        pdfu=new float[width];
        cdfu=new float[width+1];
        pdfv=new float* [width];
        cdfv=new float* [width];
        for(int i=0;i<width;i++){
            pdfv[i]=new float [height];
            cdfv[i]=new float [height+1];
        }
        float* sum=new float[width+1];
        for(int i=0;i<width;i++){
            sum[i]=0;
            for(int j=0;j<height;j++) sum[i]+=img->GetPixel(i,j).Length();
            for(int j=0;j<height;j++){
                pdfv[i][j]=img->GetPixel(i,j).Length()/sum[i];
            }
            cdfv[i][0]=0;cdfv[i][height]=1;
            for(int j=1;j<height;j++) cdfv[i][j]=cdfv[i][j-1]+pdfv[i][j];
        }
        total=0;
        for(int i=0;i<width;i++) total+=sum[i];
        for(int i=0;i<width;i++){
            pdfu[i]=sum[i]/total;
        }
        cdfu[0]=0;cdfu[width]=1;
        for(int i=1;i<width;i++) cdfu[i]=cdfu[i-1]+pdfu[i];
    }
    void samplingLight(Vec3f& container)
    {
        float u,v,pdf;
        samplePic(u,v,pdf);
        //use Vec3f to store theta, phi, and pdf
        container.Set(u,v,pdf*img->Width()*img->Height()/2/M_PI/M_PI);
    }
    Vec3f getEmit(float u,float v)
    {
        return img->GetPixel(u*img->Width(),v*img->Height());
    }
    void getIllumination (const Vec3f & pos, Vec3f &dir, Vec3f &col,
              float &distanceToLight) const
    {
        /*if(distanceToLight==-1.0){
            float theta=col.x();
            float phi=col.y();
            col=img->GetPixel(0.5/M_PI*theta*img->Width(),1.0/M_PI*phi*img->Height());
            return;
        }*/
        /*Ray ray(pos,dir);
        Hit hit(inf,NULL,-1.0*dir);
        shape->intersect(ray,hit,1e-6);
         */
        Vec3f intersectPoint=dir;//hit.getIntersectionPoint();dir;
        intersectPoint.Normalize();
        float cosineTheta=intersectPoint.z();
        if(cosineTheta>1.0) cosineTheta=1.0;
        float theta=acos(cosineTheta);
        float sinTheta=sqrt(1-intersectPoint.z()*intersectPoint.z());
        float cosPhi=intersectPoint.x()/sinTheta,sinPhi=intersectPoint.y()/sinTheta;
        if(cosPhi>1.0) cosPhi=1.0;
        float phi=acos(cosPhi);
        if(sinPhi<0) phi=2.0*M_PI-phi;
        float u=0;
        if(phi<M_PI) u=0.5+phi/2.0/M_PI;
        else u=-0.5+phi/2.0/M_PI;
        int w = (int)(u*img->Width()),h=(int)((1.0-theta/M_PI)*img->Height());
        if(w>=img->Width()) w=img->Width()-1;
        if(h>=img->Height()) h=img->Height()-1;
        if(w<0) w=0;
        if(h<0) h=0;
        col=img->GetPixel(w,h);
    }
    void glInit(int id);//{}
    float getArea(){return 0.0;}
private:
    Image* img;
    int bisearchU()
    {
        float r=(float)distribution(generator);
        int width=img->Width();
        int res=width/2,left=0,right=width;
        while(left+1<right){
            if(r<cdfu[res]){
                right=res;
                res=(left+res)/2;
            }
            else{
                left=res;
                res=(right+res)/2;
            }
        }
        return res;
    }
    int bisearchV(int u)
    {
        float r=(float)distribution(generator);
        int height=img->Height();
        int res=height/2,left=0,right=height;
        while(left+1<right){
            if(r<cdfv[u][res]){
                right=res;
                res=(left+res)/2;
            }
            else{
                left=res;
                res=(right+res)/2;
            }
        }
        return res;
    }
    void samplePic(float& u, float& v,float& pdf)
    {
        int U=bisearchU(),V=bisearchV(U);
        u=1.0*U/img->Width();
        v=1.0*V/img->Height();
        pdf=pdfu[U]*pdfv[U][V];
    }
    float* pdfu;
    float* cdfu;
    float** pdfv;
    float** cdfv;
    Sphere* shape;
    float total;
};
// ====================================================================
// ====================================================================


#endif
