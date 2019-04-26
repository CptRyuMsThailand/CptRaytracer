#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
class Triangle{
	public :
	unsigned vert0;
	unsigned vert1;
	unsigned vert2;
	vec3 albeido = vec3(0.9);
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
struct Material{
	char Matid[50];
	vec3 colorKd = vec3(0.0);
};
void parseMtlFile(std::ifstream &file,
std::vector<Material> &m){
	char junk[1024];
	Material Nm;
	while(!file.eof()){
		char ind[20];
		char a[20];
		char b[20];
		char c[20];
		file >> ind;
		if(strcmp(ind,"newmtl")==0){
			file >> Nm.Matid;
		}else if(strcmp(ind,"Kd")==0){
			file >> a ;
			file >> b ;
			file >> c ;
			Nm.colorKd = vec3(
				strtod(a,NULL),
				strtod(b,NULL),
				strtod(c,NULL)
			);
			m.push_back(Nm);
		}else{
			file.getline(junk,1024);
		}
	}
}
void parseObjFile(std::ifstream &file,std::vector<Triangle> &tris,std::vector<vec3> &vert){
	char junk[1024];
	bool isHaveMaterial=false;
	std::vector<Material> mtlArr;
	Material *CurrMat;
	//char* ind;
	while(!file.eof()){
		char ind[20];
		char a[20];
		char b[20];
		char c[20];
		file >> ind;
		//std::cout <<ind << std::endl;
		if(strcmp(ind,"mtllib")==0){
			char Path[50];
			file >> Path;
			std::ifstream mtl;
			mtl.open(Path);
			if(mtl.is_open()){
			parseMtlFile(mtl,mtlArr);
			mtl.close();
			isHaveMaterial=true;
			}
		}
		else if(strcmp(ind ,"v")==0){
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
			
			Triangle ttt = Triangle(ai,bi,ci);
			if(isHaveMaterial && CurrMat!=NULL){
				ttt.albeido = CurrMat->colorKd;
			}
			tris.push_back(ttt);
		}else if(isHaveMaterial && strcmp(ind,"usemtl")==0){
			char mName[50];
			file >> mName;
			for(unsigned i=0;i<mtlArr.size();++i){
				if(strcmp(mName,mtlArr[i].Matid)==0){
					CurrMat = &mtlArr[i];
					break;
				}
			}
		
		}else{
			file.getline(junk,1024);
		}
	}
}