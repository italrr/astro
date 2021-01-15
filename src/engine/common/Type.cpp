#include "Tools.hpp"
#include "Type.hpp"

/*
================
color
================
*/
astro::Color::Color(){
	r = 0;
	g	= 0;
	b	= 0;
	a	= 1.0f;


}

astro::Color::Color(const astro::Color &c){
	r = c.r;
	g = c.g;
	b = c.b;
	a = c.a;
}


astro::Color::Color(float r, float g, float b){
	this->r = r;
	this->g = g;
	this->b = b;
	this->a	= 1.0f;

}

astro::Color::Color(float r, float g, float b, float a){
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

astro::Color::Color(const std::string &hex){
	set(hex);
}

void astro::Color::set(const std::string &hex){
	char *p;
	int hexValue = strtol(hex.substr(1).c_str(), &p, 16);
  r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
  g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
  b = ((hexValue) & 0xFF) / 255.0;        // Extract the BB byte
  a = 1.0f;
}

std::string astro::Color::hex(){
	int ri = r * 255.0f; int gi = g * 255.0f; int bi = b * 255.0f;
	int hex = ((ri & 0xff) << 16) + ((gi & 0xff) << 8) + (bi & 0xff); // loses alpha though
	char hexString[4*sizeof(int)+1];
	return std::string(hexString);
}

void astro::Color::set(float r, float g, float b, float a){
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

void astro::Color::set(const astro::Color &color){
	this->r = color.r;
	this->g	= color.g;
	this->b	= color.b;
	this->a	= color.a;
}

astro::Color::Color(float c){
	set(c);
}

astro::Color::Color(float c, float a){
	set(c, a);
}

void astro::Color::set(float c, float a){
	this->r = c;
	this->g	= c;
	this->b	= c;
	this->a	= a;
}

void astro::Color::set(float c){
	this->r = c;
	this->g	= c;
	this->b	= c;
	this->a	= c;	
}

astro::Color::operator std::string(){
	return str();
}

std::string astro::Color::str(){
    return astro::String::format("(%f, %f, %f, %f)", r, g, b, a);
}

void astro::Color::operator=(float c){
	r = c;
	g = c;
	b = c;
	a = c;
}

bool astro::Color::operator<(const astro::Color &v){
	return r < v.r && g < v.g && b < v.b && a < v.a;
}

bool astro::Color::operator>(const astro::Color &v){
	return r > v.r && g > v.g && b > v.b && a > v.a;
}

bool astro::Color::operator<=(const astro::Color &v){
	return r <= v.r && g <= v.g && b <= v.b && a <= v.a;
}

bool astro::Color::operator>=(const astro::Color &v){
	return r >= v.r && g >= v.g && b >= v.b && a >= v.a;
}

bool astro::Color::operator<(float c){
	return r < c && g < c && b < c && a < c;
}

bool astro::Color::operator>(float c){
	return r > c && g > c && b > c && a > c;
}

bool astro::Color::operator<=(float c){
	return r <= c && g <= c && b <= c && a <= c;
}

bool astro::Color::operator>=(float c){
	return r >= c && g >= c && b >= c && a >= c;
}

bool astro::Color::operator==(float c){
	return r == c && g == c && b == c && a == c;
}

bool astro::Color::operator==(const astro::Color &v){
	return r == v.r && g == v.g && b == v.b && a == v.a;
}

bool astro::Color::operator!=(float c){
	return r != c && g != c && b != c && a != c;
}

bool astro::Color::operator!=(const Color &v){
	return r != v.r && g != v.g && b != v.b && a != v.a;
}

astro::Color astro::Color::operator/(float c){
	auto b = *this;
	b.r /= c;
	b.g /= c;
	b.b /= c;
	return b;
}

astro::Color astro::Color::operator/(const astro::Color &v){
	auto b = *this;
	b.r /= v.r;
	b.g /= v.g;
	b.b /= v.b;
	return b;
}

astro::Color astro::Color::operator*(float c){
	auto b = *this;
	b.r *= c;
	b.g *= c;
	b.b *= c;
	return b;
}

astro::Color astro::Color::operator*(const astro::Color &v){
	auto b = *this;
	b.r *= v.r;
	b.g *= v.g;
	b.b *= v.b;
	return b;
}

astro::Color astro::Color::operator+(float c){
	auto b = *this;
	b.r += c;
	b.g += c;
	b.b += c;
	return b;
}

astro::Color astro::Color::operator+(const astro::Color &v){
	auto b = *this;
	b.r += v.r;
	b.g += v.g;
	b.b += v.b;
	return b;
}

astro::Color astro::Color::operator-(float c){
	auto b = *this;
	b.r -= c;
	b.g -= c;
	b.b -= c;
	return b;
}

astro::Color astro::Color::operator-(const astro::Color &v){
	auto b = *this;
	b.r -= v.r;
	b.g -= v.g;
	b.b -= v.b;
	return b;
}

astro::Color astro::Color::invert(){
	auto col = astro::Color();
	col.r = 1.0 - r;
	col.g = 1.0 - g;
	col.b = 1.0 - b;
	return col;
}