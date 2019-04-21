#include <fstream>
void BMPheader(std::ofstream &ifile,int w,int h){
	//This Is BMP header
	int padding = w%4;
	ifile << (char)'B';
	ifile << (char)'M';
	int v0 =  (54+w*3*h+padding*h) ;
	ifile << (char)(v0) <<(char) (v0 >> 8);
	ifile << (char)(v0>>16)<<(char)(v0>>24);
	ifile << (char)0 << (char)0 << (char)0 << (char)0;
	ifile << (char)54 << (char)0 <<(char) 0 << (char)0;
	
	//This is DIP
	ifile << (char)40 << (char)0 <<(char) 0 << (char)0;
	ifile << (char)(w) << (char)(w >> 8) << (char)0 << (char)0;
	ifile << (char)(h) << (char)(h >> 8) << (char)0 << (char)0;
	ifile << (char)1 << (char)0;
	ifile << (char)24 << (char)0;
	ifile << (char)0<<(char)0<<(char)0<<(char)0;
	int v1 = (int)w*3*h+padding*h;
	ifile << (char)(v1) << (char)(v1 >> 8) << (char)(v1 >> 16) << (char)(v1 >> 24);
	ifile << (char)28 << (char)35 << (char)0 << (char)0;
	ifile << (char)28 << (char)35 << (char)0 << (char)0;
	ifile << (char)0 << (char)0 << (char)0 << (char)0;
	ifile << (char)0 << (char)0 << (char)0 << (char)0;
	//ifile << (int)0 << (int)0;
	
	
}