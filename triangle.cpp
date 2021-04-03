#include "triangle.h"

#define eps 1e-6
extern float det3x3(float a1,float a2,float a3,
	     float b1,float b2,float b3,
	     float c1,float c2,float c3);

bool Triangle::intersect(const Ray& r,Hit& h,float tmin)
{
		Vec3f c1=a-b,c2=a-c,c3=a-r.getOrigin(),Rd=r.getDirection();
		float judge=det3x3(Rd.x(),c1.x(),c2.x(),Rd.y(),c1.y(),c2.y(),Rd.z(),c1.z(),c2.z());
		if(judge<eps&&judge>-eps) return 0;
		float t=det3x3(c3.x(),c1.x(),c2.x(),c3.y(),c1.y(),c2.y(),c3.z(),c1.z(),c2.z())/judge;
		float beta=det3x3(Rd.x(),c3.x(),c2.x(),Rd.y(),c3.y(),c2.y(),Rd.z(),c3.z(),c2.z())/judge;
		float gamma=det3x3(Rd.x(),c1.x(),c3.x(),Rd.y(),c1.y(),c3.y(),Rd.z(),c1.z(),c3.z())/judge;
		float alpha=1-beta-gamma;
		if(t<tmin) return 0;
		if(beta>1||beta<0||gamma>1||gamma<0||alpha>1||alpha<0) return 0;
    Vec3f normal=alpha*na+beta*nb+gamma*nc;
    Vec2f text=alpha*ta+beta*tb+gamma*tc;
    normal.Normalize();
    if(t<h.getT()) {
        h.set(t,m,normal,text,r);
        Vec3f emit=m->getEmittance();
        if(emit.Length()>0)
            h.setLight();
    }
		return 1;
}

void Triangle::paint(void)
{
		Vec3f c1=a-b,c2=a-c;
		Vec3f normal(0.0f,0.0f,0.0f);
		Vec3f::Cross3(normal,c1,c2);
		normal.Normalize();
		m->glSetMaterial();
	glBegin(GL_TRIANGLES);
   glNormal3f(normal.x(),normal.y(),normal.z());
   glVertex3f(a.x(), a.y(), a.z());
   glVertex3f(b.x(), b.y(), b.z());
   glVertex3f(c.x(), c.y(),c.z());
   glEnd();
}

bool Triangle::intersectShadowRay(const Ray& r,float tHitLight)
{
    Vec3f Le=m->getEmittance();
    if(Le.Length()>0)
        return 0;
	Vec3f c1=a-b,c2=a-c,c3=a-r.getOrigin(),Rd=r.getDirection();
	Vec3f normal(0.0f,0.0f,0.0f);
	Vec3f::Cross3(normal,c1,c2);
	normal.Normalize();
	float judge=det3x3(Rd.x(),c1.x(),c2.x(),Rd.y(),c1.y(),c2.y(),Rd.z(),c1.z(),c2.z());
	if(judge<eps&&judge>-eps) return 0;
	float t=det3x3(c3.x(),c1.x(),c2.x(),c3.y(),c1.y(),c2.y(),c3.z(),c1.z(),c2.z())/judge;
	float beta=det3x3(Rd.x(),c3.x(),c2.x(),Rd.y(),c3.y(),c2.y(),Rd.z(),c3.z(),c2.z())/judge;
	float gamma=det3x3(Rd.x(),c1.x(),c3.x(),Rd.y(),c1.y(),c3.y(),Rd.z(),c1.z(),c3.z())/judge;
	float alpha=1-beta-gamma;
    if(t<0) return 0;
	if(abs(t-tHitLight)<1e-2||abs(t)<1e-2) return 0;
	if(beta>1||beta<0||gamma>1||gamma<0||alpha>1||alpha<0) return 0;
   // cout<<"t is "<<t<<"tHItLIght is"<<tHitLight<<endl;
	return 1;
}

void Triangle::intersectTShadowRay(const Ray& r, Vec3f& term,vector<Hit>& v,float tmin)
{
	Vec3f c1=a-b,c2=a-c,c3=a-r.getOrigin(),Rd=r.getDirection();
	Vec3f normal(0.0f,0.0f,0.0f);
	Vec3f::Cross3(normal,c1,c2);
	normal.Normalize();
	float judge=det3x3(Rd.x(),c1.x(),c2.x(),Rd.y(),c1.y(),c2.y(),Rd.z(),c1.z(),c2.z());
	if(judge<eps&&judge>-eps) return;
	float t=det3x3(c3.x(),c1.x(),c2.x(),c3.y(),c1.y(),c2.y(),c3.z(),c1.z(),c2.z())/judge;
	float beta=det3x3(Rd.x(),c3.x(),c2.x(),Rd.y(),c3.y(),c2.y(),Rd.z(),c3.z(),c2.z())/judge;
	float gamma=det3x3(Rd.x(),c1.x(),c3.x(),Rd.y(),c1.y(),c3.y(),Rd.z(),c1.z(),c3.z())/judge;
	float alpha=1-beta-gamma;
	if(t<tmin) return;
	if(beta>1||beta<0||gamma>1||gamma<0||alpha>1||alpha<0) return;
	Vec3f color=m->getTransparentColor();
	term.Set(term.x()*color.x(),term.y()*color.y(),term.z()*color.z());
	Hit h;
	h.set(t,m,normal,r);
	v.push_back(h);
}
