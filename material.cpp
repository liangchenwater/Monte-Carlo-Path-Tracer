#include "material.h"
#include "glCanvas.h"
#ifdef SPECULAR_FIX
extern int SPECULAR_FIX_WHICH_PASS;
#endif

//only used in whitted-style
Vec3f CookMaterial::Shade(Vec3f wi, Hit& hit, Vec3f wo, Vec3f color,bool cosineWeigthted) const
{
    Vec3f normal=hit.getNormal();
    if(wo.Dot3(normal)<0) normal=-1.0*normal;
	  wi.Normalize();
	 normal.Normalize();
    wo.Normalize();
	 Vec3f h=wi+wo;
	 h.Normalize();
    float frac=normal.Dot3(wi);
	 if(frac<0) return Vec3f(0,0,0);
	 Vec3f res=1.0/M_PI*albedo;
    float cosineHN=normal.Dot3(h);
    float cosineIS=cosineHN*cosineHN*2-1;
/*	#if SPECULAR_FIX
       res+=(float)pow(cosineIS,exponent)*(exponent+2)/2.0/M_PI*specularColor;
    #else*/
        res+=(float)pow(cosineIS,exponent)*(exponent+2)/2.0/M_PI*ks;
    /*  #endif */
     res*=frac;
	 res.Set(res.x()*color.x(),res.y()*color.y(),res.z()*color.z());
    if(map_Kd!=NULL){
        Vec2f text=hit.getText();
        int width=map_Kd->Width(),height=map_Kd->Height();
        float x=text.x(),y=text.y();
        x=x-(int)x;y=y-(int)y;
        if(x<0) x+=1.0;
        if(y<0) y+=1.0;
        Vec3f origColor=map_Kd->GetPixel((int)(width*x),(int)(height*y));
        res+=normal.Dot3(wo)*origColor;
    }
	return res;
}

Vec3f CookMaterial::sampleKd(Hit& hit){
    Vec3f res(0,0,0);
    if(map_Kd==NULL) return res;
    Vec2f text=hit.getText();
    int width=map_Kd->Width(),height=map_Kd->Height();
    float x=text.x(),y=text.y();
    x=x-(int)x;y=y-(int)y;
    if(x<0) x+=1.0;
    if(y<0) y+=1.0;
    if(y>1.0-1e-6) y-=1e-6;
    if(x>1.0-1e-6) x-=1e-6;
    Vec3f origColor=map_Kd->GetPixel((int)(width*x),(int)(height*y));
    return origColor;
}


void CookMaterial::glSetMaterial(void) const {

  GLfloat one[4] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat zero[4] = { 0.0, 0.0, 0.0, 0.0 };
  GLfloat specular[4] = {
    getReflectance().r(),
      getReflectance().g(),
      getReflectance().b(),
    1.0};
  GLfloat diffuse[4] = {
      getAlbedo().r(),
      getAlbedo().g(),
      getAlbedo().b(),
    1.0};
  // NOTE: GL uses the Blinn Torrance version of Phong...      
  float glexponent = exponent;
  if (glexponent < 0) glexponent = 0;
  if (glexponent > 128) glexponent = 128;
  
#if !SPECULAR_FIX 
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);
  
#else

  // OPTIONAL: 3 pass rendering to fix the specular highlight 
  // artifact for small specular exponents (wide specular lobe)

  if (SPECULAR_FIX_WHICH_PASS == 0) {
    // First pass, draw only the specular highlights
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);
    
  } else if (SPECULAR_FIX_WHICH_PASS == 1) {
    // Second pass, compute normal dot light 
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
  } else {
    // Third pass, add ambient & diffuse terms
    assert (SPECULAR_FIX_WHICH_PASS == 2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
  }

#endif
}
