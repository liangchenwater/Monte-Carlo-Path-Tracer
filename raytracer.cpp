#include "raytracer.h"
#define eps 1e-6
#include <vector>
#include <random>
using namespace std;
extern std::default_random_engine generator;
extern std::uniform_real_distribution<double> distribution;
inline float gainTravelDist(const vector<Hit>& v,const Ray& r)
{
	if(v.size()<=1) return 1;
	Vec3f dir=r.getDirection();
	stack<float> temp;
	float d=v[v.size()-1].getT()-v[0].getT(),t_skip=0;
	for(int i=0;i<v.size();i++){
		if(temp.empty()&&i>0){ t_skip+=v[i].getT()-v[i-1].getT();  /*cout<<i<<endl;*/}
		Material* m=v[i].getMaterial();
		float idx=m->getIndexOfRefraction();
		if(!temp.empty()&&temp.top()==idx) temp.pop();
		else temp.push(idx);
	}
	d-=t_skip;
	if(d<=eps) return 1;
	return d;
}

Vec3f RayTracer::traceRay(Ray& ray, float tmin, int bounces, float weight,Hit& hit)
{	
 	bool shadeback=this->shadeback;
	Group* g=scene->getGroup();
	g->intersect(ray,hit,tmin);
	Vec3f nowPoint=hit.getIntersectionPoint(),dir_R=-1.0f*ray.getDirection(),n=hit.getNormal();
	dir_R.Normalize();n.Normalize();
	assert(!refractionStack.empty());
	float judge=n.Dot3(dir_R);
	int num_lights=scene->getNumLights();
    Material* bg=new CookMaterial(scene->getBackgroundColor());
	
	Vec3f light_col,light_dir,res_col(0.0f,0.0f,0.0f);
	float dist_to_light=0;
	
	Vec3f reflectiveColor=hit.getMaterial()->getReflectiveColor();
	Vec3f transparentColor=hit.getMaterial()->getTransparentColor();
	Vec3f obj_col=hit.getMaterial()->getAlbedo();
	Vec3f ambient_col=scene->getAmbientLight();
	if(transparentColor.Length()>eps) shadeback=true;
		
	if(judge<-eps&&shadeback){
		 hit.set(hit.getT(),hit.getMaterial(),-1.0f*n,ray);
		 judge*=-1.0f;
		 n*=-1.0f;
	}
	
	
	if(judge<-eps)  return res_col;
	if(hit.getT()==inf) return obj_col;
	
	ambient_col.Set(obj_col.x()*ambient_col.x(),obj_col.y()*ambient_col.y(),obj_col.z()*ambient_col.z());
	res_col+=ambient_col;
	if(!shadows) for(int i=0;i<num_lights;i++){
		scene->getLight(i)->getIllumination(nowPoint,light_dir,light_col,dist_to_light);
		res_col+=hit.getMaterial()->Shade(light_dir,hit,-1.0*ray.getDirection(),light_col,0);
		}
	else for(int i=0;i<num_lights;i++){
		scene->getLight(i)->getIllumination(nowPoint,light_dir,light_col,dist_to_light);
		light_dir.Normalize();
		Ray r(nowPoint,light_dir,1);
	   if(!transparent_shadow){
           if(!g->intersectShadowRay(r,sqrt(epsilon))) res_col+=hit.getMaterial()->Shade(light_dir,hit,-1.0*ray.getDirection(),light_col,0);
      	}
      else{
        Vec3f shadowTerm(1,1,1);
      	vector<Hit> v;
      	g->intersectTShadowRay(r,shadowTerm,v,sqrt(epsilon));
      	shadowTerm.Set((float)sqrt(shadowTerm.x()),(float)sqrt(shadowTerm.y()),(float)sqrt(shadowTerm.z()));
      	if(semi){
      	sort(v.begin(),v.end());
       	if(shadowTerm.Length()>eps) shadowTerm*=(semi_coe/(float)pow(gainTravelDist(v,r),0.15));
       	}
          Vec3f render_col=hit.getMaterial()->Shade(light_dir,hit,-1.0*ray.getDirection(),light_col,0);
      	render_col.Set(render_col.x()*shadowTerm.x(),render_col.y()*shadowTerm.y(),render_col.z()*shadowTerm.z());
      	res_col+=render_col;
      }
	}
	
	if(reflectiveColor.Length()>eps&&bounces>0&&weight>cutoff_weight){
		Vec3f out=2*n.Dot3(dir_R)*n-dir_R;
		out.Normalize();
		Ray r(nowPoint,out,2);
		Hit reflectHit(inf,bg,-1.0f*out);
		stack<float> tmp=refractionStack;
		Vec3f reflectiveTerm=traceRay(r,epsilon,bounces-1,weight*reflectiveColor.Length(),reflectHit);
		refractionStack=tmp;
		reflectiveTerm.Set(reflectiveTerm.x()*reflectiveColor.x(),reflectiveTerm.y()*reflectiveColor.y(),reflectiveTerm.z()*reflectiveColor.z());
		res_col+=reflectiveTerm;
	}
	
	if(transparentColor.Length()>eps&&bounces>0&&weight>cutoff_weight)
	{
		bool trans_flag=0;//in
		float index_obj=hit.getMaterial()->getIndexOfRefraction(),relativeIndex=refractionStack.top()/index_obj;
		if(index_obj==bg->getIndexOfRefraction()) index_obj*=1.0001;
		if(index_obj==refractionStack.top()) {
			trans_flag=1;//out
			refractionStack.pop();
			relativeIndex=index_obj/refractionStack.top();
			refractionStack.push(index_obj);
			}
		float delta=1-relativeIndex*relativeIndex*(1-judge*judge);
		if(delta>epsilon){
			Vec3f out=(relativeIndex*judge-(float)sqrt(delta))*n-relativeIndex*dir_R;
			out.Normalize();
			Ray r(nowPoint,out,3);
			Hit refractionHit(inf,bg,-1.0f*out);
			if(trans_flag==0) refractionStack.push(index_obj);
			else {
				assert(!refractionStack.empty());
				refractionStack.pop();
			}
			Vec3f refractiveTerm=traceRay(r,epsilon,bounces-1,weight*transparentColor.Length(),refractionHit);
			refractiveTerm.Set(refractiveTerm.x()*transparentColor.x(),refractiveTerm.y()*transparentColor.y(),refractiveTerm.z()*transparentColor.z());
			res_col+=refractiveTerm;
		}
	}
	if(bg) delete bg;
	bg=NULL;	
	return res_col;
}

//for MCRenderer
void samplingDiffuseRay(float& theta,float& phi)
{
    float thetaCDF=(float)distribution(generator),phiCDF=(float)distribution(generator);
    theta=thetaCDF*2*M_PI;
    phi=0.5*acos(1-2*phiCDF);
}
void samplingSpecRay(float& theta, float& alpha,int ns)
{
    float thetaCDF=(float)distribution(generator),alphaCDF=(float)distribution(generator);
    theta=thetaCDF*2*M_PI;
    alpha=acos(pow(alphaCDF,1.0/(ns+1.0)));
}

Vec3f toWorld(float theta, float phi, Vec3f baseZ)
{
    Vec3f res;
    baseZ.Normalize();
    Vec3f Z(0,0,1);
    Vec3f baseY;
    Vec3f::Cross3(baseY,baseZ,Z);
    if(baseY.Length()<eps) res.Set(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
    else{
        baseY.Normalize();
        Vec3f baseX;
        Vec3f::Cross3(baseX,baseY,baseZ);
        baseX.Normalize();
        res=sin(theta)*cos(phi)*baseX+sin(theta)*sin(phi)*baseY+cos(theta)*baseZ;
    }
    res.Normalize();
        return res;
}

Vec3f evaluateBRDF(Material* material, Vec3f wo, Vec3f norm, Vec3f wi)
{
    Vec3f diffuse=1.0/M_PI*material->getAlbedo();
    float cosine=wo.Dot3(norm);
    Vec3f Perfect=2*cosine*norm-wo;
    float alpha=acos(wi.Dot3(Perfect));
    int ns=material->getExponent();
    Vec3f specular=(float)0.5*pow(cos(alpha),ns*1.0)*(ns+2.0)/M_PI*material->getReflectance();
    return diffuse+specular;
}

Vec3f sampleBRDF(Material* material, Vec3f wo, Vec3f norm, float weight, Vec3f& wi, float& pdf)
{
    int ns=material->getExponent();
    float theta,phi,alpha;
    
    
    float d=distribution(generator);
    float cosine=wo.Dot3(norm);
    Vec3f Perfect=2*cosine*norm-wo;
    if(d<weight){
        samplingSpecRay(theta,alpha,ns);
        wi=toWorld(alpha,theta,Perfect);
        phi=acos(norm.Dot3(wi));
    }
    else{
        samplingDiffuseRay(theta,phi);
        wi=toWorld(phi,theta,norm);
        alpha=acos(Perfect.Dot3(wi));
    }
    float pdfD=cos(phi)/M_PI;
    float pdfS=0.5*(ns+1.0)*pow(cos(alpha),ns*1.0)/M_PI;
    pdf=weight*pdfS+(1-weight)*pdfD;
    return evaluateBRDF(material,wo,norm,wi);
}

Vec3f sampleGGX(Material* s,Vec3f norm, float& half, float& phi)
{
    float alpha2=s->getAlpha()*s->getAlpha();
    float x=distribution(generator),y=distribution(generator);
    phi=2*M_PI*x;
    half=acos(sqrt((1.0 - y) / (1.0 + (alpha2 - 1.0) * y)));
    return toWorld(half,phi,norm);
}

float GGX(float alpha2, float cosine)
{
    return alpha2/(M_PI * pow(pow(cosine,2) * (alpha2 - 1.0) + 1.0,2));
}

float pdfGGX(float alpha2, float cosine){
    return GGX(alpha2,cosine)*cosine;
}

Vec3f Fschlick(Vec3f F0, Vec3f wo, Vec3f wh)
{
    Vec3f one(1,1,1);
    float cosine=wo.Dot3(wh);
    if(cosine<0) cosine=0;
    return F0+(one-F0)*pow(1.0-cosine,5);
}

float G1schlick(float k, float cosw)
{
    return cosw * (1.0 - k) + k;
}

float Gschlick(float alpha,float cos_wo,float cos_wi)
{
    float k = 0.5 * alpha;
    return 1.0 /**cos_wo*cos_wi */ / (G1schlick(k, cos_wi) * G1schlick(k, cos_wo));
}

Vec3f evaluateBSDF(Material* s, Vec3f wo, Vec3f norm, Vec3f wi, Vec3f wh)
{
    float cos_wo=norm.Dot3(wo);
    float cos_wi=norm.Dot3(wi);
    float cos_wh=norm.Dot3(wh);
    float D=GGX(s->getAlpha()*s->getAlpha(),cos_wh);
    float G=Gschlick(s->getAlpha(),cos_wo, cos_wi);
    Vec3f F=Fschlick(s->getReflectance(),wo,wh);
    Vec3f one(1,1,1);
    Vec3f mental=one-F;
    Vec3f kd=mental*(1-s->getMentalness());
    Vec3f albedo=s->getAlbedo();
    Vec3f diffuse=1.0/M_PI*Vec3f(albedo.x()*kd.x(),albedo.y()*kd.y(),albedo.z()*kd.z());
    Vec3f specular=/* cos_wo*cos_wi* */D*G*0.25*F;
    return diffuse+specular;
}

Vec3f sampleBSDF(Material* s, Vec3f wo, Vec3f norm, float weight,Vec3f& wi, float& pdf)
{
    float randNum=distribution(generator);
    float theta, phi,half;
    float pdfD=0,pdfS=0;
    Vec3f wh;
    //sample specular
    if(randNum < weight){
        // sample D and G
        wh= sampleGGX(s,norm,half,phi);
        float cosineOH=wh.Dot3(wo);
        wi=2*cosineOH*wh-wo;
        theta=acos(norm.Dot3(wi));
    }
    //sample diffuse
    else{
        samplingDiffuseRay(phi,theta);
        wi=toWorld(theta,phi,norm);
        wh=wi+wo;
        wh.Normalize();
        half=acos(norm.Dot3(wh));
    }
    pdfS=pdfGGX(s->getAlpha()*s->getAlpha(),cos(half))/max(0.01, 4.0 * wi.Dot3(wh));
    pdfD=1.0/M_PI*cos(theta);
    pdf = pdfD*(1-weight)+pdfS*weight;
    return evaluateBSDF(s,wo,norm,wi,wh);
}

Vec3f RayTracer::traceRay(Ray& ray, float tmin, float cutdown, Hit& hit,bool appearance)
{
    Vec3f res(0,0,0);
    Group* group=scene->getGroup();
    group->intersect(ray,hit,tmin);
    Vec3f newPoint=hit.getIntersectionPoint();
    Vec3f rayDir=ray.getDirection();
    int lightCount=scene->getNumLights();
    
    //if ray does not hit any object, get environment light illumination
    if(hit.getT()==inf){
        for(int i=0;i<lightCount;i++){
            Light* light=scene->getLight(i);
            int lightType=light->getType();
            if(lightType==3){
                float dist=0;
                light->getIllumination(newPoint,rayDir,res,dist);
            }
            }
        return res;
    }
    
    //check direction of norm
    Vec3f norm=hit.getNormal();
    if(norm.Dot3(rayDir)>0) norm=(-1.0)*norm;
    
    //get material
    Material* material=hit.getMaterial();
    
    //if ray hits light, get light illumination
    if(hit.isLight())  {
        Camera* camera=scene->getCamera();
        Vec3f cameraPos=camera->getPos();
        Vec3f dir=cameraPos-newPoint;
        float coe=1.0/dir.Dot3(dir);
        return coe*material->getEmittance();
    }
    
    //sampling light
    for(int i=0;i<lightCount;i++)
   {
        Light* light=scene->getLight(i);
       float lightArea=light->getArea();
       int lightType=light->getType();
       if(lightArea>0){
        Vec3f lightPos;
        light->samplingLight(lightPos);
        Vec3f wi=lightPos-newPoint;
        float tHitLight=wi.Length();
        wi.Normalize();
        Ray shadowRay(newPoint,wi);
        Hit shadowHit(inf,NULL,-1.0*wi);
       if(!group->intersectShadowRay(shadowRay, tHitLight)){
        Vec3f Li;
        float dist;
        light->getIllumination(newPoint,lightPos,Li,dist);
           float cosine=norm.Dot3(wi);
           Vec3f Bsdf;
           if(!appearance) Bsdf=evaluateBRDF(material,-1.0*rayDir,norm,wi);
           else{
               Vec3f wo=-1.0*rayDir;
               Vec3f wh=wo+wi;
               wh.Normalize();
               Bsdf=evaluateBSDF(material,-1.0*rayDir,norm,wi,wh);
           }
           Vec3f Li_x_Bsdf(Li.x()*Bsdf.x(),Li.y()*Bsdf.y(),Li.z()*Bsdf.z());
        res+=lightArea*cosine*Li_x_Bsdf;
       }
       }
       //never used
       else if(lightType!=3) continue;
       //can choose sample environment light or not
       else {
           Vec3f container;
           light->samplingLight(container);
           //float dist=-1.0;
           Vec3f Emit=light->getEmit(container.x(),container.y());
          // cout<<container.z()<<endl;
           float theta=M_PI*container.y(),phi=2*M_PI*container.x();
           Vec3f wi(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
           float cosine=wi.Dot3(norm);
           Ray lightRay(newPoint,wi);
           if(cosine<0) cosine=0;
           if(!group->intersectShadowRay(lightRay, 1e-6)){
               Vec3f wh=wi-rayDir;
               wh.Normalize();
               Vec3f Bsdf;
               if(!appearance) Bsdf=evaluateBRDF(material,-1.0*rayDir,norm,wi);
               else Bsdf=evaluateBSDF(material,-1.0*rayDir,norm,wi,wh);
               Vec3f Li_x_Bsdf;
               Li_x_Bsdf.Set(Emit.x()*Bsdf.x(),Emit.y()*Bsdf.y(),Emit.z()*Bsdf.z());
           res+=1.0/container.z()*Li_x_Bsdf*cosine*sin(theta);
           }
           }
    }
    
    
    //sampling objects
    float pdf=1;
    Vec3f albedo=material->getAlbedo();
    Vec3f reflectance=material->getReflectance();
    float weight=reflectance.Length()/(albedo.Length()+reflectance.Length());
    if(appearance){
        float weightSpec=reflectance.Length();
        float weightDiffuse=albedo.Length()*(1-material->getMentalness());
        weight= weightSpec/(weightSpec+weightDiffuse);
    }
    Vec3f wi;
    Vec3f Bsdf;
    //appearance=0,sample phong brdf
    if(!appearance) Bsdf=sampleBRDF(material,-1.0*rayDir,norm,weight,wi,pdf);
    //else sample cook-torrance
    else Bsdf=sampleBSDF(material,-1.0*rayDir,norm,weight,wi,pdf);
    Ray newRay(newPoint,wi);
    Hit newHit(inf,NULL,-1.0*newRay.getDirection());
    float randNum=(float)distribution(generator);
    if(randNum<cutdown){
        Vec3f Li=traceRay(newRay,tmin,cutdown*0.95,newHit,appearance);
        if(newHit.isLight()==false) {
                float cosine=norm.Dot3(wi);
            Vec3f Li_x_Bsdf(Li.x()*Bsdf.x(),Li.y()*Bsdf.y(),Li.z()*Bsdf.z());
                res+=1.0*cosine/cutdown/pdf*Li_x_Bsdf;
                Vec3f text=material->sampleKd(hit);
                res+=text;
    }
    }
    return res;
}

void RayTracer::traceRay(Ray& ray,float tmin, int bounces,float weight,Hit& hit,int idx)
{
	bool shadeback=this->shadeback;
	
	Group* g=scene->getGroup();
	g->intersect(ray,hit,tmin);
	float tstop=hit.getT();
	Material* bg=new CookMaterial(scene->getBackgroundColor());
	Vec3f reflectiveColor=hit.getMaterial()->getReflectiveColor();
	Vec3f transparentColor=hit.getMaterial()->getTransparentColor();

	if(hit.getT()==inf) return;
		
	Vec3f nowPoint=hit.getIntersectionPoint(),dir_R=-1.0f*ray.getDirection(),n=hit.getNormal();
	dir_R.Normalize();n.Normalize();
	float judge=n.Dot3(dir_R);
	
	if(transparentColor.Length()>eps) shadeback=true;
	if(judge<-eps&&shadeback){
		 hit.set(hit.getT(),hit.getMaterial(),-1.0f*n,ray);
		 judge*=-1.0f;
		 n*=-1.0f;
	}
	if(judge<-eps)  return;
	
	assert(idx>=0&&idx<=2);
	if(idx==0) RayTree::SetMainSegment(ray,0, tstop);
	else if(idx==1) RayTree::AddReflectedSegment(ray,0,tstop);
	else RayTree::AddTransmittedSegment(ray,0,tstop);

	int num_lights=scene->getNumLights();
	
	Vec3f light_col,light_dir;
	float dist_to_light=0;
	
	if(shadows)
	for(int i=0;i<num_lights;i++){
		scene->getLight(i)->getIllumination(nowPoint,light_dir,light_col,dist_to_light);
		light_dir.Normalize();
		Ray r(nowPoint,light_dir,1);
		Hit ShadowHit(inf,bg,light_dir);
		if(!transparent_shadow){
       	if(g->intersect(r,ShadowHit,sqrt(epsilon))) RayTree::AddShadowSegment(r,0,ShadowHit.getT()); 
       	}
       	else{
       		 Vec3f shadowTerm(1,1,1);
      		vector<Hit> v;
      		g->intersectTShadowRay(r,shadowTerm,v,sqrt(epsilon));
       		if(!v.empty()) RayTree::AddShadowSegment(r,0,v[v.size()-1].getT());
       	}
	}
	
	if(reflectiveColor.Length()>eps&&bounces>0&&weight>cutoff_weight){
		Vec3f out=2*n.Dot3(dir_R)*n-dir_R;
		Ray r(nowPoint,out,2);
		Hit reflectHit(inf,bg,-1.0f*out);
		stack<float> tmp=refractionStack;
		traceRay(r,epsilon,bounces-1,weight*reflectiveColor.Length(),reflectHit,1);
		refractionStack=tmp;
	}
	
	if(transparentColor.Length()>eps&&weight>cutoff_weight)
	{
		bool trans_flag=0;//in
		float index_obj=hit.getMaterial()->getIndexOfRefraction(),relativeIndex=refractionStack.top()/index_obj;
		if(index_obj==bg->getIndexOfRefraction()) index_obj*=1.0001;
		if(index_obj==refractionStack.top()) {
			trans_flag=1;//out
			refractionStack.pop();
			relativeIndex=index_obj/refractionStack.top();
			refractionStack.push(index_obj);
			}
		float delta=1-relativeIndex*relativeIndex*(1-judge*judge);
		if(delta>0){
			Vec3f out=(relativeIndex*judge-(float)sqrt(delta))*n-relativeIndex*dir_R;
			Ray r(nowPoint,out,3);
			Hit refractionHit(inf,bg,-1.0f*out);
			if(trans_flag==0) refractionStack.push(index_obj);
			else {
				assert(!refractionStack.empty());
				refractionStack.pop();
			}
			cout<<"Now the relative index is:"<<relativeIndex<<endl;
			traceRay(r,epsilon,bounces-1,weight*transparentColor.Length(),refractionHit,2);
			}
		
	}
	if(bg) delete bg;
	bg=NULL;	
}

