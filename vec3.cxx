#include <cmath>
#define kEpsilon 1e-8
class vec3{
	public:
	double x,y,z;
	vec3(void){
		this->x = this-> y = this-> z = 0.0;
	}
	vec3(double a){
		this->x=a;
		this->y=a;
		this->z=a;
	}
	vec3(double x,double y,double z){
		this->x = x;
		this->y = y;
		this->z = z;
	};
	vec3 operator + (const vec3 &that) const{
		return vec3(this->x + that.x,
		this->y + that.y,
		this->z + that.z);
	}
	vec3 operator - (const vec3 &that) const{
		return vec3(this->x - that.x,
		this->y - that.y,
		this->z - that.z);
		}
	vec3 operator * (const vec3 &that) const{
		return vec3(this->x * that.x,
		this->y - that.y,
		this->z - that.z);
	}
	vec3 operator * (const double &that) const{
		return vec3(this->x * that,this->y * that,this->z * that);
	}
	vec3 cross(const vec3 &that) const{
		return vec3(
		(this->y * that.z) - (this->z * that.y),
		(this->z * that.x) - (this->x * that.z),
		(this->x * that.y) - (this->y * that.x)
		);
	}
	double dot(const vec3 &that)const{
		return this->x * that.x +
		this-> y * that.y +
		this-> z * that.z;
	}
	
	double length(){
		return sqrt(this->dot(*this));
	}
	/*vec3 normalize(){
		double l = this->length();
		vec3 *t = this;
		return (*t) *(1/l);
	}*/
	void normalize(){
		double l = this->length();
		this->x /= l;
		this->y /= l;
		this->z /= l;
	}
	vec3 reflect(vec3 n)const{
		return (*this) - n * (double)( (2* this->dot(n)));
	}
};

bool rayTriangleIntersect(const vec3 &ro,
const vec3 &rd,const vec3 &v0,
const vec3 &v1,const vec3 &v2,
double &t,double &u,double &v){
	vec3 v0v1 = v1 - v0;
	vec3 v0v2 = v2 - v0;
	vec3 pvec = rd.cross(v0v2);
	double det = v0v1.dot(pvec);
	if(abs(det) < kEpsilon)return false;
	double invDet = 1.0/det;
	vec3 tvec = ro - v0;
	u = tvec.dot(pvec) * invDet;
	if(u < 0 || u>1)return false;
	
	vec3 qvec = tvec.cross(v0v1);
	v = rd.dot(qvec) * invDet;
	if(v<0 || v+u>1)return false;
	t = v0v2.dot(qvec)*invDet;
	return t > kEpsilon;
}

