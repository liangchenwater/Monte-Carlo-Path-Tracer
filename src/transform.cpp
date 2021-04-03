#include "transform.h"

bool Transform::intersect(const Ray& r,Hit& h, float tmin)
{
	Matrix inv,Tinv;
	mx.Inverse(inv);
	inv.Transpose(Tinv);
	Vec3f orig(r.getOrigin()),dir(r.getDirection());
	inv.Transform(orig);
	inv.TransformDirection(dir);
	Ray r1(orig,dir);
	if(!o->intersect(r1,h,tmin)) return false;
	Vec3f normal=h.getNormal();
	Tinv.TransformDirection(normal);
	normal.Normalize();
	h.set(h.getT(),h.getMaterial(),normal,r);
	return true;
	}
	
void Transform::paint()
{
  glPushMatrix();
  GLfloat *glMatrix = mx.glGet();
  glMultMatrixf(glMatrix);
  delete[] glMatrix;
  o->paint();
  glPopMatrix();
}

bool Transform::intersectShadowRay(const Ray& r,float tmin)
{
	Matrix inv,Tinv;
	mx.Inverse(inv);
	inv.Transpose(Tinv);
	Vec3f orig(r.getOrigin()),dir(r.getDirection());
	inv.Transform(orig);
	inv.TransformDirection(dir);
	Ray r1(orig,dir);
	if(!o->intersectShadowRay(r1,tmin)) return false;
	return true;
}

void Transform::intersectTShadowRay(const Ray& r, Vec3f& term, vector<Hit>& v,float tmin)
{
	Matrix inv,Tinv;
	mx.Inverse(inv);
	inv.Transpose(Tinv);
	Vec3f orig(r.getOrigin()),dir(r.getDirection());
	inv.Transform(orig);
	inv.TransformDirection(dir);
	Ray r1(orig,dir);
	o->intersectTShadowRay(r1,term,v,tmin);
}