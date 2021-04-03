#include "group.h"
#define eps 1e-6
extern int how_to_intersect;

Group::Group(int _num):num(_num){
	arr= new Object3D*[num];
    intersected= new bool [num];
    minPoint.Set(inf,inf,inf);
    maxPoint.Set(-inf,-inf,-inf);
}

Group::~Group(){
	delete[] arr;
	arr=NULL;
}

void Group::addObject(int index, Object3D* obj)
{
		arr[index]=obj;
}
void Group::computeBoundingBox(int _resolution)
{
    resolution=_resolution;
    shapeList = new vector<int>** [resolution];
    for(int i=0;i<resolution;i++) shapeList[i]= new vector<int>* [resolution];
    for(int i=0;i<resolution;i++)
    for(int j=0;j<resolution;j++) shapeList[i][j] = new vector<int> [resolution];
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
    stepX=(maxPoint.x()-minPoint.x())/resolution;
    stepY=(maxPoint.y()-minPoint.y())/resolution;
    stepZ=(maxPoint.z()-minPoint.z())/resolution;
    for(int i=0;i<num;i++){
        Vec3f Min,Max;
        arr[i]->getMinMax(Min,Max);
        int startX=(Min.x()-minPoint.x())/stepX ,endX= (Max.x()-minPoint.x())/stepX;
        int startY=(Min.y()-minPoint.y())/stepY ,endY=(Max.y()-minPoint.y())/stepY ;
        int startZ=(Min.z()-minPoint.z())/stepZ ,endZ=(Max.z()-minPoint.z())/stepZ ;
        for(int x= startX; x<=endX; x++)
        for(int y=startY;y<=endY;y++)
        for(int z=startZ;z<=endZ;z++) shapeList[x][y][z].push_back(i);
    }
}

bool intersectFacet(const Ray& ray, Vec3f& minPoint, Vec3f& maxPoint, Hit& hit)
{
    Vec3f dir=ray.getDirection(),origin=ray.getOrigin();
    float t;
    if(abs(dir.x())>1e-6){
        if(origin.x()>maxPoint.x()||origin.x()<minPoint.x()){
        if(dir.x()>0) t=(minPoint.x()-origin.x())/dir.x();
        else t=(maxPoint.x()-origin.x())/dir.x();
        }
        else{
            if(dir.x()<0) t=(minPoint.x()-origin.x())/dir.x();
            else t=(maxPoint.x()-origin.x())/dir.x();
        }
        if(t>0){
            Vec3f intersectPoint=origin+t*dir;
            if(intersectPoint.y()>minPoint.y()&&intersectPoint.y()<maxPoint.y()&&intersectPoint.z()>minPoint.z()
               &&intersectPoint.z()<maxPoint.z()){
                hit.set(t,ray);
                return 1;
            }
        }
    }
    if(abs(dir.y())>1e-6){
        if(origin.y()>maxPoint.y()||origin.y()<minPoint.y()){
        if(dir.y()>0) t=(minPoint.y()-origin.y())/dir.y();
        else t=(maxPoint.y()-origin.y())/dir.y();
        }
        else{
            if(dir.y()<0) t=(minPoint.y()-origin.y())/dir.y();
            else t=(maxPoint.y()-origin.y())/dir.y();
        }
        if(t>0){
            Vec3f intersectPoint=origin+t*dir;
            if(intersectPoint.x()>minPoint.x()&&intersectPoint.x()<maxPoint.x()&&intersectPoint.z()>minPoint.z()
               &&intersectPoint.z()<maxPoint.z()){
                hit.set(t,ray);
                return 1;
            }
        }
    }
    if(abs(dir.z())>1e-6){
        if(origin.z()>maxPoint.z()||origin.z()<minPoint.z()){
        if(dir.z()>0) t=(minPoint.z()-origin.z())/dir.z();
        else t=(maxPoint.z()-origin.z())/dir.z();
        }
        else{
            if(dir.z()<0) t=(minPoint.z()-origin.z())/dir.z();
            else t=(maxPoint.z()-origin.z())/dir.z();
        }
        if(t>0){
            Vec3f intersectPoint=origin+t*dir;
            if(intersectPoint.y()>minPoint.y()&&intersectPoint.y()<maxPoint.y()&&intersectPoint.x()>minPoint.x()
               &&intersectPoint.x()<maxPoint.x()) {
                hit.set(t,ray);
                return 1;
            }
        }
    }
    return 0;
}

bool Group::initializeMarch(Ray& ray)
{
    Hit hit(inf,NULL,-1.0*ray.getDirection());
    bool intersected=intersectFacet(ray,minPoint,maxPoint,hit);
    if(!intersected) return 0;
    Vec3f startPoint=hit.getIntersectionPoint();
    float Epsilon=0.0001*min(maxPoint.z()-minPoint.z(),min(maxPoint.x()-minPoint.x(),maxPoint.y()-minPoint.y()));
    Vec3f rayDir=ray.getDirection();
    startPoint+=Epsilon*rayDir;
    ray.setOrigin(startPoint);
    return 1;
}

bool Group::cameraIn(const Vec3f& cameraPos){
    if(cameraPos.x()>minPoint.x()&&cameraPos.x()<maxPoint.x()&&
       cameraPos.y()>minPoint.y()&&cameraPos.y()<maxPoint.y()&&
       cameraPos.z()>minPoint.z()&&cameraPos.z()<maxPoint.z()) return 1;
    return 0;
}
bool Group::intersect (const Ray& ray, Hit & hit, float tmin)
{
    if(!how_to_intersect){
    fill(intersected,intersected+num,0);
    Vec3f orig=ray.getOrigin(),direction=ray.getDirection();
    float lengthX=orig.x()-minPoint.x();
    float lengthY=orig.y()-minPoint.y();
    float lengthZ=orig.z()-minPoint.z();
    int idx=(int)(lengthX/stepX);
    int idy=(int)(lengthY/stepY);
    int idz=(int)(lengthZ/stepZ);
    
    int signX=(direction.x()>0)?1:(-1);
    int signY=(direction.y()>0)?1:(-1);
    int signZ=(direction.z()>0)?1:(-1);
    
    float dt_x=inf;
    if(abs(direction.x())>1e-6) dt_x=stepX/abs(direction.x());
    float dt_y=inf;
    if(abs(direction.y())>1e-6) dt_y=stepY/abs(direction.y());
    float dt_z=inf;
    if(abs(direction.z())>1e-6) dt_z=stepZ/abs(direction.z());
    
    float remainTX=inf;
    if(abs(direction.x())>1e-6) {
        if(signX==1)
        remainTX=(stepX*(idx+1)-lengthX)/abs(direction.x());
        else remainTX=(lengthX-stepX*idx)/abs(direction.x());
    }
    float remainTY=inf;
    if(abs(direction.y())>1e-6) {
        if(signY==1)
        remainTY=(stepY*(idy+signY)-lengthY)/abs(direction.y());
        else remainTY=(lengthY-stepY*idy)/abs(direction.y());
    }
    float remainTZ=inf;
    if(abs(direction.z())>1e-6) {
        if(signZ==1)
        remainTZ=(stepZ*(idz+signZ)-lengthZ)/abs(direction.z());
        else  remainTZ=(lengthZ-stepZ*idz)/abs(direction.z());
    }
    //cout<<remainTX<<" "<<remainTY<<" "<<remainTZ<<endl;
    while(idx<resolution&&idy<resolution&&idz<resolution&&idx>=0&&idy>=0&&idz>=0){
        bool has_intersected= false;
        //cout<<shapeList[idx][idy][idz].size()<<endl;
        for(unsigned int i=0;i<shapeList[idx][idy][idz].size();i++)
        if(!intersected[shapeList[idx][idy][idz][i]]) {
         has_intersected |= arr[shapeList[idx][idy][idz][i]]->intersect(ray,hit,tmin);
            //Object3D* Debug=arr[shapeList[idx][idy][idz][i]];
            intersected[shapeList[idx][idy][idz][i]]=1;
            //Vec3f currentPoint=hit.getIntersectionPoint();
            //cout<<currentPoint<<endl;
        }
        Vec3f currentPoint=hit.getIntersectionPoint();
        if(has_intersected&&within(currentPoint,idx,idy,idz)) return 1;
        float remainT=min(remainTX,min(remainTY,remainTZ));
        if(remainTX==remainT){
            idx+=signX;
            remainTX+=dt_x;
        }
        else if(remainTY==remainT){
            idy+=signY;
            remainTY+=dt_y;
        }
        else if(remainTZ==remainT){
            idz+=signZ;
            remainTZ+=dt_z;
        }
    }
        return 0;
    }
    else return tree->intersect(ray, hit, tmin);
}

bool Group::within(const Vec3f& p, int idx, int idy, int idz){
    float x=stepX*idx,y=stepY*idy,z=stepZ*idz;
    if(p.x()>x&&p.x()<x+stepX&&p.y()>y&&p.y()<y+stepY&&p.z()>z&&p.z()<z+stepZ) return 1;
    return 0;
}


void Group::paint()
{
	for(int i=0;i<num;i++) arr[i]->paint();
}

bool Group::intersectShadowRay(const Ray& ray, float tHitLight)
{
    if(!how_to_intersect){
    fill(intersected,intersected+num,0);
    Vec3f orig=ray.getOrigin(),direction=ray.getDirection();
    float lengthX=orig.x()-minPoint.x();
    float lengthY=orig.y()-minPoint.y();
    float lengthZ=orig.z()-minPoint.z();
    int idx=(int)(lengthX/stepX);
    int idy=(int)(lengthY/stepY);
    int idz=(int)(lengthZ/stepZ);
    
    int signX=(direction.x()>0)?1:(-1);
    int signY=(direction.y()>0)?1:(-1);
    int signZ=(direction.z()>0)?1:(-1);
    
    
    float dt_x=inf;
    if(abs(direction.x())>1e-6) dt_x=stepX/abs(direction.x());
    float dt_y=inf;
    if(abs(direction.y())>1e-6) dt_y=stepY/abs(direction.y());
    float dt_z=inf;
    if(abs(direction.z())>1e-6) dt_z=stepZ/abs(direction.z());
    
    
    
    float remainTX=inf;
    if(abs(direction.x())>1e-6) {
        if(signX==1)
        remainTX=(stepX*(idx+1)-lengthX)/abs(direction.x());
        else remainTX=(lengthX-stepX*idx)/abs(direction.x());
    }
    float remainTY=inf;
    if(abs(direction.y())>1e-6) {
        if(signY==1)
        remainTY=(stepY*(idy+signY)-lengthY)/abs(direction.y());
        else remainTY=(lengthY-stepY*idy)/abs(direction.y());
    }
    float remainTZ=inf;
    if(abs(direction.z())>1e-6) {
        if(signZ==1)
        remainTZ=(stepZ*(idz+signZ)-lengthZ)/abs(direction.z());
        else  remainTZ=(lengthZ-stepZ*idz)/abs(direction.z());
    }
    Hit hit(inf,NULL,-1.0*ray.getDirection());
    while(idx<resolution&&idy<resolution&&idz<resolution&&idx>=0&&idy>=0&&idz>=0){
        //cout<<shapeList[idx][idy][idz].size()<<endl;
        for(unsigned int i=0;i<shapeList[idx][idy][idz].size();i++)
        if(!intersected[shapeList[idx][idy][idz][i]]) {
            if(arr[shapeList[idx][idy][idz][i]]->intersectShadowRay(ray,tHitLight)) return 1;
            //Object3D* Debug=arr[shapeList[idx][idy][idz][i]];
            intersected[shapeList[idx][idy][idz][i]]=1;
            //Vec3f currentPoint=hit.getIntersectionPoint();
            //cout<<currentPoint<<endl;
        }
        float remainT=min(remainTX,min(remainTY,remainTZ));
        if(remainTX==remainT){
            idx+=signX;
            remainTX+=dt_x;
        }
       else if(remainTY==remainT){
            idy+=signY;
            remainTY+=dt_y;
        }
       else if(remainTZ==remainT){
            idz+=signZ;
            remainTZ+=dt_z;
        }
    }
    return 0;
    }
    else return tree->intersectShadowRay(ray,tHitLight);
}

void Group::intersectTShadowRay(const Ray& r, Vec3f& term,vector<Hit>& v ,float tmin)
{
    for(int i=0;i<num;i++) {
    	arr[i]->intersectTShadowRay(r,term,v,tmin);
    	if(term.Length()<=eps) break;
    }
}
