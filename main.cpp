//
//  main.cpp
//  PT_Graphics2020
//
//  Created by Liang Chen on 2021/3/26.
//

#include "image.h"
#include "scene_parser.h"
#include "hit.h"
#include "glCanvas.h"
#include "matrix.h"
#include "camera.h"
#include "light.h"
#include "material.h"
#include "object3d.h"
#include "group.h"
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "transform.h"
#include "raytracer.h"
#include <iostream>
#include <random>
#include "rayTree.h"
using namespace std;

#define inf  (0xff<<23)
#define eps 1e-6
char *input_file = NULL;
int width = 100;
int height = 100;
char *output_file = NULL;
float depth_min = 0;
float depth_max = 1;
char *depth_file = NULL;
char* normal_file=NULL;
bool shade_back=false;
bool gouraud=false;
bool gui=false;
int phi_step=5;
int theta_step=10;
bool shadows=false;
int bounces=0;
float weight=0;
bool transparent_shadows;
bool semi;
float epsilon=1e-4;
float semi_coe=1.0f;
//arguments for Monte Carlo Ray Tracing
float RR_cutdown=0.5;
float Gamma=1.0;
bool MonteCarlo=false;
int spp=2;
int resolution=1000;
bool cook_torrance=0;
bool how_to_intersect=0;
int SAH=2000;

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0,1.0);

SceneParser* scene;
RayTracer* rt;
inline void process_parameters(int argc, char** argv, char** env);
void WhittedRenderer(void);
void MCRenderer(void);
void tracerDebugger(float,float);

int main(int argc, char** argv, char** env)
{
    process_parameters(argc,argv,env);
    scene=new SceneParser(input_file);
    rt=new RayTracer(scene,bounces,weight,shadows,shade_back,transparent_shadows,semi,epsilon,semi_coe);
    Group* group=scene->getGroup();
    Camera* camera=scene->getCamera();
    Vec3f cameraPos=camera->getPos();
    if(!how_to_intersect) group->computeBoundingBox(resolution);
    else{
    group->createTree();
        cout<<"BVH is built successfully!"<<endl;
    }
    if(gui){
        glutInit(&argc,argv);
        glutInitWindowSize(400,400);
        glutInitWindowPosition(100,100);
        glutCreateWindow("OpenGL Viewer");
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
        GLCanvas* canvas=new GLCanvas();
        if(!MonteCarlo) canvas->initialize(scene,WhittedRenderer,tracerDebugger);
        else canvas->initialize(scene,MCRenderer,tracerDebugger);
    }
   else if(!MonteCarlo) WhittedRenderer();
   else MCRenderer();
   return 0;
}

void WhittedRenderer(void)
{
    Image pic(width,height);
    Image depth_pic(width,height);
    Image normal_pic(width,height);
    Camera* c=scene->getCamera();
    Material* bg=new CookMaterial(scene->getBackgroundColor());
    int size=min(width,height);
    for(int x=0;x<width;x++)
    for(int y=0;y<height;y++){
        Vec2f pos;
        if(size==height)
        pos=Vec2f((x-0.5*(width-size))*1.0/size,y*1.0/size );
        else pos=Vec2f(x*1.0/size,(y-0.5*(height-size))*1.0/size);
            Ray r=c->generateRay(pos);
            Hit h(inf,bg,-1.0*r.getDirection());
            rt->clearStack(1);
           pic.SetPixel(x,y,rt->traceRay(r,c->getTMin(),bounces,1,h));
           float d=(depth_max-h.getT())/(depth_max-depth_min);
           if(d<0) d=0;
           if(d>1) d=1;
          if(depth_file) depth_pic.SetPixel(x,y,Vec3f(d,d,d));
          if(normal_file) {
              Vec3f n=h.getNormal();
              n.Set(fabs(n.x()),fabs(n.y()),fabs(n.z()));
              normal_pic.SetPixel(x,y,n);
          }
      }
      
  pic.SaveTGA(output_file);
  if(depth_file) depth_pic.SaveTGA(depth_file);
  if(normal_file) normal_pic.SaveTGA(normal_file);
  if(rt) delete rt;
  rt=NULL;
  if(bg) delete bg;
    bg=NULL;
}

//for monte carlo renderer
Vec3f gammaCorrection(Vec3f radiance)
{
    radiance.Set(pow(radiance.r(),Gamma),pow(radiance.g(),Gamma),pow(radiance.b(),Gamma));
    return radiance;
}

void MCRenderer(void)
{
    float weight=1.0/spp;
    Image pic(width,height);
    Camera* camera=scene->getCamera();
    Group* group=scene->getGroup();
    Vec3f cameraPos=camera->getPos();
    bool cameraIn=group->cameraIn(cameraPos);
    Vec3f **data=new Vec3f* [width];
    int size=min(width,height);
    for(int x=0;x<width;x++) data[x]=new Vec3f [height];
    if(!cameraIn){
    for(int i=0;i<spp;i++){
    for(int x=0;x<width;x++)
        for(int y=0;y<height;y++){
            Vec2f pos;
            if(size==height)
            pos=Vec2f((x-0.5*(width-size))*1.0/size,y*1.0/size );
            else pos=Vec2f(x*1.0/size,(y-0.5*(height-size))*1.0/size);
            Ray ray=camera->generateRay(pos);
            Hit hit(inf,NULL,-1.0*ray.getDirection());
            if(!how_to_intersect){
            if(group->initializeMarch(ray)) data[x][y]+=rt->traceRay(ray,camera->getTMin(),RR_cutdown,hit,cook_torrance);
            
            else{
                int lightCount=scene->getNumLights();
                for(int i=0;i<lightCount;i++){
                    Light* light=scene->getLight(i);
                    int lightType=light->getType();
                    if(lightType==3){
                        float dist=0;
                        Vec3f origin=ray.getOrigin();
                        Vec3f direction=ray.getDirection();
                        light->getIllumination(origin,direction,data[x][y],dist);
                    }
                    }
            }
            }
            else data[x][y]+=rt->traceRay(ray,camera->getTMin(),RR_cutdown,hit,cook_torrance);
        }
        cout<<"The "<<i+1<<"th sample has been rendered successfully!"<<endl;
    }
    }
    else
        for(int i=0;i<spp;i++){
        for(int x=0;x<width;x++)
            for(int y=0;y<height;y++){
                Vec2f pos(x*1.0/width,y*1.0/height );
                Ray ray=camera->generateRay(pos);
                Hit hit(inf,NULL,-1.0*ray.getDirection());
                 data[x][y]+=rt->traceRay(ray,camera->getTMin(),RR_cutdown,hit,cook_torrance);
            }
            cout<<"The "<<i+1<<"th sample has been rendered successfully!"<<endl;
        }
    
    for(int x=0;x<width;x++)
    for(int y=0;y<height;y++) pic.SetPixel(x,y,gammaCorrection(weight*data[x][y]));
    pic.SaveTGA(output_file);
    if(rt) delete rt;
    rt=NULL;
}

void tracerDebugger(float x,float y)
{
    Camera* c=scene->getCamera();
    Vec2f pos(x,y);
    Ray r=c->generateRay(pos);
    Material* bg=new CookMaterial(scene->getBackgroundColor());
    Hit h(inf,bg,-1.0*r.getDirection());
    rt->clearStack(1);
    rt->traceRay(r,c->getTMin(),bounces,1,h,0);
    RayTree::paint();
    RayTree::Print();
}

inline void process_parameters(int argc, char** argv, char** env)
{
    for (int i = 1; i < argc; i++) {
  if (!strcmp(argv[i],"-input")) {
    i++; assert (i < argc);
    input_file = argv[i];
  } else if (!strcmp(argv[i],"-size")) {
    i++; assert (i < argc);
    width = atoi(argv[i]);
    i++; assert (i < argc);
    height = atoi(argv[i]);
  } else if (!strcmp(argv[i],"-output")) {
    i++; assert (i < argc);
    output_file = argv[i];
  } else if (!strcmp(argv[i],"-depth")) {
    i++; assert (i < argc);
    depth_min = atof(argv[i]);
    i++; assert (i < argc);
    depth_max = atof(argv[i]);
    i++; assert (i < argc);
    depth_file = argv[i];
  } else if(!strcmp(argv[i],"-normals")){
      i++; assert(i<argc);
      normal_file=argv[i];
  } else if(!strcmp(argv[i],"-shade_back")){
    assert(i<argc);
      shade_back=true;
}  else if (!strcmp(argv[i],"-gui")){
      assert(i<argc);
      gui=true;
} else if(!strcmp(argv[i],"-tessellation")) {
        i++;
          assert(i<argc);
          theta_step=atoi(argv[i]);
          i++;
          assert(i<argc);
          phi_step=atoi(argv[i]);
}else if(!strcmp(argv[i],"-gouraud")){
          assert(i<argc);
          gouraud=true;
}else if(!strcmp(argv[i],"-shadows")){
          assert(i<argc);
          shadows=true;
}else if(!strcmp(argv[i],"-bounces")){
        i++;
        assert(i<argc);
        bounces=atoi(argv[i]);
}else if(!strcmp(argv[i],"-weight")){
        i++;
        assert(i<argc);
        weight=atof(argv[i]);
}else if(!strcmp(argv[i],"-transparent_shadows")){
        assert(i<argc);
        transparent_shadows=1;
}else if(!strcmp(argv[i],"-semi")){
        i++;
        assert(i<argc);
        semi=1;
        semi_coe=atof(argv[i]);
}else if(!strcmp(argv[i],"-epsilon")){
        i++;
        assert(i<argc);
        epsilon=atof(argv[i]);
}else if(!strcmp(argv[i],"-MonteCarlo")){
    MonteCarlo=true;
}else if(!strcmp(argv[i],"-RR_cutdown")){
    i++;
    assert(i<argc);
    RR_cutdown=atof(argv[i]);
}else if(!strcmp(argv[i],"-spp")){
    i++;
    assert(i<argc);
    spp=atoi(argv[i]);
}else if(!strcmp(argv[i],"-Gamma")){
    i++;
    assert(i<argc);
    Gamma=atof(argv[i]);
    Gamma=1.0/Gamma;
}else if(!strcmp(argv[i],"-resolution")){
    i++;
    assert(i<argc);
    resolution=atoi(argv[i]);
}else if(!strcmp(argv[i],"-cook_torrance")){
    cook_torrance=1;
}else if(!strcmp(argv[i],"-SpatialTree")){
    how_to_intersect=1;
}
else if(!strcmp(argv[i],"-SAH")){
    i++;
    assert(i<argc);
    SAH=atoi(argv[i]);
}
else{
    printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
    assert(0);
  }
}
}



