#ifndef ASTRO_TYPE_HPP
    #define ASTRO_TYPE_HPP

    #include <string>
    #include <vector>
	#include <sys/types.h>
    
	#define int8 int8_t
	#define uint8 uint8_t
	#define int16 int16_t
	#define uin16 uint16_t
	#define int32 int32_t
	#define uint32 uint32_t
	#define int64 int64_t
	#define uint64 uint64_t    

    namespace astro {
        template<typename T>
        struct Vec2 {
            T x, y;
            
            Vec2(T x, T y){
                set(x, y);
            }
            
            Vec2(T c){
                set(c);
            }
            
            Vec2(){
                
            }
            
            void set(T x, T y){
                this->x = x; this->y = y;
            }

            void set(T c){
                this->x = c;
                this->y = c;
            }
        };

        template<typename T>
        struct Vec3 {
            T x, y, z;
            
            Vec3(T x, T y, T z){
                set(x, y, z);
            }
            
            Vec3(T c){
                set(c);
            }
            
            Vec3(){
                
            }
            
            void set(T x, T y, T z){
                this->x = x; this->y = y; this->z = z;
            }

            void set(T c){
                this->x = c;
                this->y = c;
                this->z = c;
            }
        };     


		struct Color {
			void operator=(float c);
			bool operator<(const astro::Color &v);
			bool operator>(const astro::Color &v);
			bool operator<=(const astro::Color &v);
			bool operator>=(const astro::Color &v);
			bool operator<(float c);
			bool operator>(float c);
			bool operator<=(float c);
			bool operator>=(float c);
			bool operator==(float c);
			bool operator==(const astro::Color &v);
			bool operator!=(float c);
			bool operator!=(const Color &v);
			astro::Color operator/(float c);
			astro::Color operator/(const astro::Color &v);
			astro::Color operator*(float c);
			astro::Color operator*(const astro::Color &v);
			astro::Color operator+(float c);
			astro::Color operator+(const astro::Color &v);
			astro::Color operator-(float c);
			astro::Color operator-(const astro::Color &v);
			astro::Color invert();
			Color(const std::string &hex);
			Color();
			Color(const astro::Color &color);
			Color(float r, float g, float b);
			Color(float r, float g, float b, float a);
			Color(float c);
			Color(float c, float a);
			void set(float c, float a);			
			void set(float c);
			void set(const std::string &hex);
			void set(float r, float g, float b, float a);
			void set(const astro::Color &color);
			float r;
			float g;
			float b;
			float a;		
			operator std::string();
			std::string str();
			std::string hex();
		};           
    }

#endif