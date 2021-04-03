#include "sphere.h"
#define PI 3.1415926
#define eps 1e-6
extern int phi_step,theta_step;
extern bool gouraud;
bool Sphere::intersect(const Ray& r, Hit & h, float tmin)
{
	Vec3f dir(r.getDirection()),orig(r.getOrigin()),fake_orig=orig-center;
	//int type=r.getType();
	//dir.Normalize();
	if((fake_orig.Length()-radius>-tmin&&fake_orig.Length()-radius<tmin)) return false;
	float a=dir.Dot3(dir),b=2*dir.Dot3(fake_orig),c=fake_orig.Dot3(fake_orig)-radius*radius;
	float d2=b*b-4*a*c;
	if(d2<0) return false;
	float t=(-b-(float)sqrt(d2))/(2.0f*a);
if(t<0)  t=(-b+(float)sqrt(d2))/(2.0f*a);
	if(t<tmin) return false;
	if(t<h.getT()){
		Vec3f n=orig+t*dir-center;
		n.Normalize();
		h.set(t,m,n,r);
		}
	return true;
}

inline static void glGetV(Vec3f& v1,Vec3f& v2, Vec3f& v3,Vec3f& v4,float Phi,float Theta,float phi_delta,float theta_delta,const Sphere* sphere)
{
	Vec3f dir1(sin(Phi)*cos(Theta),cos(Phi),sin(Phi)*sin(Theta)),
			 dir4(sin(Phi+phi_delta)*cos(Theta),cos(Phi+phi_delta),sin(Phi+phi_delta)*sin(Theta)),
			 dir2(sin(Phi)*cos(Theta+theta_delta),cos(Phi),sin(Phi)*sin(Theta+theta_delta)),
			 dir3(sin(Phi+phi_delta)*cos(Theta+theta_delta),cos(Phi+phi_delta),sin(Phi+phi_delta)*sin(Theta+theta_delta));
	v1=sphere->getC()+sphere->getR()*dir1;
	v2=sphere->getC()+sphere->getR()*dir2;
	v3=sphere->getC()+sphere->getR()*dir3;
	v4=sphere->getC()+sphere->getR()*dir4;
}

void Sphere::paint()
{
	float phi_delta=1.0f*PI/phi_step,theta_delta=2.0f*PI/theta_step;
	m->glSetMaterial();
		if(!gouraud){
		for(float Phi=0;Phi<PI;Phi+=phi_delta)
		for(float Theta=0;Theta<2*PI;Theta+=theta_delta){
		Vec3f v1,v2,v3,v4;
		glGetV(v1,v2,v3,v4,Phi,Theta,phi_delta,theta_delta,this);
		Vec3f b1=v1-v2,b2=v1-v3,n;
		Vec3f::Cross3(n,b1,b2);
		glBegin(GL_TRIANGLES);
		glNormal3f(n.x(),n.y(),n.z());
		 glVertex3f(v1.x(), v1.y(), v1.z());
         glVertex3f(v2.x(), v2.y(), v2.z());
         glVertex3f(v3.x(), v3.y(), v3.z());
		glVertex3f(v3.x(), v3.y(), v3.z());
         glVertex3f(v4.x(), v4.y(), v4.z());
         glVertex3f(v1.x(), v1.y(), v1.z());
         glEnd();
		}
	}
	else {
			for(float Phi=0;Phi<PI;Phi+=phi_delta)
		for(float Theta=0;Theta<2*PI;Theta+=theta_delta){
						Vec3f v1,v2,v3,v4;
	glGetV(v1,v2,v3,v4,Phi,Theta,phi_delta,theta_delta,this);
	glBegin(GL_TRIANGLES);
		glNormal3f(v1.x()-center.x(),v1.y()-center.y(),v1.z()-center.z());
		 glVertex3f(v1.x(), v1.y(), v1.z());
		 glNormal3f(v2.x()-center.x(),v2.y()-center.y(),v2.z()-center.z());
         glVertex3f(v2.x(), v2.y(), v2.z());
         glNormal3f(v3.x()-center.x(),v3.y()-center.y(),v3.z()-center.z());
         glVertex3f(v3.x(), v3.y(), v3.z());
          glVertex3f(v3.x(), v3.y(), v3.z());
         glNormal3f(v4.x()-center.x(),v4.y()-center.y(),v4.z()-center.z());
         glVertex3f(v4.x(), v4.y(), v4.z());
         glNormal3f(v1.x()-center.x(),v1.y()-center.y(),v1.z()-center.z());
		 glVertex3f(v1.x(), v1.y(), v1.z());
		 glEnd();
		}
	}
	}
	
bool Sphere::intersectShadowRay(const Ray& r, float epsilon)
{
    epsilon=0.0001;
	Vec3f dir(r.getDirection()),orig(r.getOrigin()),fake_orig=orig-center;
	//dir.Normalize();
	//if(fake_orig.Length()-radius>-epsilon&&fake_orig.Length()-radius<epsilon) return false;
	float a=dir.Dot3(dir),b=2*dir.Dot3(fake_orig),c=fake_orig.Dot3(fake_orig)-radius*radius;
	float d2=b*b-4*a*c;
	if(d2<0) return false;
	float t=(-b-(float)sqrt(d2))/(2.0f*a);
	if(/*-b-(float)sqrt(d2)<-eps*/t<-eps)   t=(-b+(float)sqrt(d2))/(2.0f*a);
	if(t<epsilon) return false;
	return true;
}

void Sphere::intersectTShadowRay(const Ray& r, Vec3f& term, vector<Hit>& v,float epsilon)
{
	Vec3f dir(r.getDirection()),orig(r.getOrigin()),fake_orig=orig-center;
	//dir.Normalize();
	if(fake_orig.Length()-radius>-epsilon&&fake_orig.Length()-radius<epsilon) return;
	float a=dir.Dot3(dir),b=2*dir.Dot3(fake_orig),c=fake_orig.Dot3(fake_orig)-radius*radius;
	float d2=b*b-4*a*c;
	if(d2<0) return;
	float t=(-b-(float)sqrt(d2))/(2.0f*a);
	if(t<-eps)   t=(-b+(float)sqrt(d2))/(2.0f*a);
	if(t<epsilon) return;
	Vec3f color=m->getTransparentColor();
	term.Set(term.x()*color.x(),term.y()*color.y(),term.z()*color.z());
	Vec3f n=orig+t*dir-center;
		n.Normalize();
	Hit h;
	h.set(t,m,n,r);
	v.push_back(h);
	}
