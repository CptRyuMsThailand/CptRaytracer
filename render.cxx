#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <random>
#include "vec3.cxx"
#include "bmp.cxx"
#include "modelOpen.cxx"
#define lightpos vec3(20.0,10.0,-2.0)
#define PI 3.1415926536
#define max(x,y) ((x) > (y) ? (x) : (y))
#define min(x,y) ((x) < (y) ? (x) : (y))
#define clamp(x,y,z) (min(max((x),(y)),(z)))
typedef double db;

db norm(db a,db b,db c){
	return (a-b)/(c-b);
}
db lerp(db a,db b,db t){
	return (1-t)*a+t*b;
}
db map(db a,db b,db c,db u,db v){
	return lerp(u,v,norm(a,b,c));
}
struct Option{
	unsigned width=620;
	unsigned height=480;
	double fov = 70;
	double bias = 0.01;
	vec3 bgColor = vec3(1.0,1.0,1.0);
};
using namespace std;

double sky(const vec3 &ro,const vec3 &rd){
	
	vec3 sund =lightpos - ro;
	sund.normalize();
	
	return max(sund.dot(rd),0);
}
vec3 uniformSampHemisphere(const double &r1,const double &r2){
	double amp = sqrt(1.0-r1*r1);
	double phi = r2*2.0*PI;
	vec3 tt = vec3(amp*cos(phi),r1,amp*sin(phi));
	tt.normalize();
return tt;
}
void createCoordSys(const vec3 &N,vec3 &T,vec3 &B){
	if(abs(N.x) > abs(N.y)){
		T = vec3(N.z,0,-N.x) * (1.0/sqrt(N.x*N.x+N.z*N.z));
	}else{
		T = vec3(0,-N.z,N.y)*(1.0/sqrt(N.y*N.y+N.z*N.z));
	}
	B = N.cross(T);
}
bool trace(const vec3 &ro,const vec3 &rd,const vector<Triangle> &objects,
const vector<vec3> &vert,double &tNear,
unsigned &ptr){
	//hitObject = nullptr;
	bool isHit = false;
	unsigned s = objects.size();
	for(unsigned k=0;k<s;++k){
		double tNearT = 1e10;
		if(objects[k].intersect(ro,rd,tNearT,vert) && tNearT < tNear){
			isHit = true;
			ptr = k;
			tNear = tNearT;
		}
	}
	return (isHit);
};
vec3 castRay(const vec3 &ro,const vec3 &rd,const vector<Triangle> &objects,
const vector<vec3> &vert,const Option &options,int depth){
	if(depth == 0)return vec3(0.0,0.0,0.0);
	vec3 hitColor = vec3(0);
	double tNear = 1000000;
	unsigned hitObject;
//Diffuse
	if(trace(ro,rd,objects,vert,tNear,hitObject)
	){
		vec3 directL (0);
		vec3 indirL (0);
		vec3 phit = ro+rd*tNear;
		vec3 norm(0.0);
		objects[hitObject].surfaceProperty(norm,vert);
		vec3 toLight = lightpos-phit;
		toLight.normalize();
		double ltnear = 1000000;
		bool vis = !trace(phit+norm*options.bias,toLight,objects,vert,ltnear,hitObject);
		double nv = vis*max(0,norm.dot(toLight));
		vec3 ref = rd.reflect(norm);
		double sv = vis*max(0,ref.dot(toLight));
	    directL = options.bgColor*nv*0.5+options.bgColor*sv*0.5;
	    default_random_engine generator;
uniform_real_distribution<double> distribution(0.0,1.0);
	    
	    vec3 Nt,Nb;
	    createCoordSys(norm,Nt,Nb);
	    unsigned int N =16;
	    for(unsigned i=0;i<N;++i){
	    	double r1 = distribution(generator);
	    	double r2 = distribution(generator);
	    	vec3 samp = uniformSampHemisphere(r1,r2);
	    	vec3 sampW (
	    	samp.x*Nb.x + samp.y*norm.x + samp.z*Nt.x,
	    	samp.x*Nb.y + samp.y*norm.y + samp.z*Nt.y,
	    	samp.x*Nb.z + samp.y*norm.z + samp.z*Nt.z
	    	);
	    	sampW.normalize();
	    	indirL = indirL+(castRay(phit+sampW*options.bias,sampW,objects,vert,options,depth-1)*r1);
	    }
	    indirL = indirL*(1.0/(N));
	    hitColor = ((directL)+(indirL))*(objects[hitObject].albeido);
	}else{
		hitColor =0.4;//options.bgColor * sky(ro,rd);
	}
	
	return hitColor;
}
void render(Option &options,
vector<Triangle> &objects,
vector<vec3> &vert,clock_t &timer){
	const int w = options.width;
	const int h = options.height;
	vec3 minpos (1e15);
	vec3 maxpos(-1e15);
	for(unsigned k=0;k<vert.size();++k){
		vec3 *n = &vert[k];
		minpos.x = min(minpos.x,n->x);
		minpos.y = min(minpos.y,n->y);
		minpos.z = min(minpos.z,n->z);
		maxpos.x = max(maxpos.x,n->x);
		maxpos.y = max(maxpos.y,n->y);
		maxpos.z = max(maxpos.z,n->z);
	}
	int padding = w%4;
	
	
	vec3 *buff = new vec3[w*h];
	vec3 *pix = buff;	
	vec3 UP (0.0,1.0,0.0);
	
	/*
	 Camera Origin Edit Here!
	*/
	vec3 ro = vec3(4.0,6.0,4.0);
	
	/*
	Camera Look At 
	*/
	vec3 focusAt = (minpos+maxpos)*0.5;
	
	
	vec3 lookD = focusAt-ro;
	lookD.normalize();
	vec3 vpR = lookD.cross(UP);
	vec3 vpU = vpR.cross(lookD);
	double scale = tan((options.fov * 0.5)*PI/180.0);
	
	double asp = (double)w/ (double)h;
	int progress = 10;
	for(int y=0;y<h;y++){
	for(int x=0;x<w;x++)
	{
		db fx = map((db)x,0.0,(db)w,-scale*asp,scale*asp);
		db fy = map((db)y,0.0,(db)h,-scale,scale);
		vec3 rd = (vpR*fx+vpU*fy+lookD);
		rd.normalize();
		//cout<<rd.x<<" : "<<rd.y<<":"<< rd.z << endl;
		*pix = castRay(ro,rd,objects,vert,options,2);
		pix++;
		int prog = map((db)(x+y*w),0,h*w,0,100);
		if(prog > progress){
			double ntime =(double) (clock()-timer);
			cout << "Progress : "<<progress<<"%"<<endl;
			cout << "Passed : "<<(ntime/CLOCKS_PER_SEC)<<" sec\n";
			progress+=10;
		}
		//cout << c.y << endl;
	//outp<<(char)(clamp(c.y*255,0,255));
	//outp<<(char)(clamp(c.x*255,0,255));
		}
		
	}
	timer = clock()-timer;
	cout << "Rendering Complete"<<endl;
	cout << "It's Took : "<<(double)timer/CLOCKS_PER_SEC<< " sec"<<endl;
	cout << "Saving Into Image File"<<endl;
	timer = clock();
	ofstream outp ;
	outp.open("output.bmp");
	BMPheader(outp,w,h);
	for(int y=0;y<h;++y){
	for(int x=0;x<w;++x){
		int pos = (y*w)+x;
		char r = (char)(clamp(buff[pos].x,0,1)*255);
		char g = (char)(clamp(buff[pos].y,0,1)*255);
		char b = (char)(clamp(buff[pos].z,0,1)*255);
		outp << b << g << r;
	}
	for(int i=0;i<padding;++i)outp<<(char)0;
	}
	outp.close();
	delete [] buff;
}
int main(){
	Option options;
	vector<Triangle> tris;
	vector<vec3> vert;
	clock_t timer;
	/*vert.push_back(vec3(0.0,0.0,0.0));
	vert.push_back(vec3(-1.0,1.0,0.0));
	vert.push_back(vec3(1.0,1.0,0.0));
	tris.push_back(Triangle(0,1,2));*/
	
	cout << "Loading Model"<<endl;
	timer = clock();
	ifstream imodel;
	imodel.open("F-14-Model.obj");
	parseObjFile(imodel,tris,vert);
	imodel.close();
	timer = clock()-timer;
	cout << "Model is loaded" << endl;
	cout << "It's Took : " << (double)(timer)/CLOCKS_PER_SEC <<" sec"<< endl;
	cout << "Info : "<<endl;
	cout << "Polygon Count : "<<tris.size();
	cout << endl;
	cout << "Vertice Count : "<<vert.size();
	cout << endl;
	cout << "Start Rendering"<<endl;
	timer = clock();
	render(options,tris,vert,timer);
	timer = clock()-timer;
	cout << "Save Complete"<<endl;
	cout << "It's Took : " << (double)(timer)/CLOCKS_PER_SEC <<" sec"<< endl;
}