#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
class Triangle{
	public :
	unsigned vert0;
	unsigned vert1;
	unsigned vert2;
	Triangle(void){};
	Triangle(unsigned ve0,
	unsigned ve1,
	unsigned ve2){
		this->vert0 = ve0;
		this->vert1 = ve1;
		this->vert2 = ve2;
	}
	bool intersect(const vec3 &ro,
	const vec3 &rd,double &tnear,const
	std::vector<vec3> &vert)const {
		const vec3 &v0 = vert[this->vert0];
		const vec3 &v1 = vert[this->vert1];
		const vec3 &v2 = vert[this->vert2];
		double t = 1e10, u,v;
		
		if(rayTriangleIntersect(ro,rd,v0,v1,v2,t,u,v) && t < tnear && tnear > 1e-8){
			tnear = t;
			return true;
		}
		return false;
	}
	void surfaceProperty(vec3 &hitNorm,
	const std::vector<vec3> &vert)const{
		const vec3 &v0 = vert[this->vert0];
		const vec3 &v1 = vert[this->vert1];
		const vec3 &v2 = vert[this->vert2];
		hitNorm = (v1 - v0).cross(v2-v0);
		hitNorm.normalize();
	}
};
void parseObjFile(std::ifstream &file,std::vector<Triangle> &tris,std::vector<vec3> &vert){
	char junk[1024];
	//char* ind;
	while(!file.eof()){
		char ind[20];
		char a[20];
		char b[20];
		char c[20];
		file >> ind;
		//std::cout <<ind << std::endl;
		if(strcmp(ind ,"v")==0){
			//std::cout <<(char*)&ind << std::endl;
			vec3 ve (0.0,0.0,0.0);
			/*char a[10];
			char b[10];
			char c[10];*/
			file >> a;
			file >> b;
			file >> c;
			ve.x = strtod(a,NULL);
			ve.y = strtod(b,NULL);
			ve.z = strtod(c,NULL);
			vert.push_back(ve);
		}
		else if(strcmp(ind,"f")==0){
			int ai,bi,ci;
			//std::cout << true<<std::endl;
			file >> a;
			file >> b;
			file >> c;
			ai = atoi(a)-1;
			bi = atoi(b)-1;
			ci = atoi(c)-1;
			tris.push_back(Triangle(ai,bi,ci));
		}else{
			file.getline(junk,1024);
		}
	}
}