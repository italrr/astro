#ifndef ASTRO_TYPE_HPP
    #define ASTRO_TYPE_HPP

    #include <string>
    #include <vector>
	#include <sys/types.h>
	#include <cmath>
    
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

			Vec2(const astro::Vec2<T> &vec){
				this->x = vec.x; this->y = vec.y;
			}
            
            Vec2(){
                set(0);
            }
            
            void set(T x, T y){
                this->x = x; this->y = y;
            }

            void set(T c){
                this->x = c;
                this->y = c;
            }

			astro::Vec2<T> normalize(){
				astro::Vec2<T> normalized = *this;
				T sqr = normalized.x * normalized.x + normalized.y * normalized.y;
				if(sqr == 1 || sqr == 0){
					return normalized;
				}
				T invrt = 1.0f/std::sqrt(sqr);
				normalized.x *= invrt;
				normalized.y *= invrt;		
				return normalized;
			}	

			T dot(const astro::Vec2<T> &vec) {
				T product = 0; 
				product += x * vec.x; 
				product += y * vec.y; 
				return product; 
			}

			astro::Vec2<T> cross(const astro::Vec2<T> &vec) {
				return this->x * vec.y - this->y * vec.x;
			}

			astro::Vec2<T> cross(T sc) {
				return astro::Vec2<T>(sc * this->y, -sc * this->x);
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
            
			Vec3(const astro::Vec3<T> &vec){
				this->x = vec.x; this->y = vec.y; this->z = vec.z;
			}

            Vec3(){
                set(0);
            }
            
            void set(T x, T y, T z){
                this->x = x; this->y = y; this->z = z;
            }

            void set(T c){
                this->x = c;
                this->y = c;
                this->z = c;
            }

			astro::Vec3<T> normalize(){
				astro::Vec3<T> normalized = *this;
				float sqr = normalized.x * normalized.x + normalized.y * normalized.y + normalized.z * normalized.z;
				if(sqr == 1 || sqr == 0){
					return normalized;
				}
				float invrt = 1.0f/std::sqrt(sqr);
				normalized.x *= invrt;
				normalized.y *= invrt;
				normalized.z *= invrt;				
				return normalized;
			}	

			T dot(const astro::Vec3<T> &vec) {
				return this->x * vec.x + this->y * vec.y + this->z * vec.z;
			}

			astro::Vec3<T> cross(const astro::Vec3<T> &vec) {
				astro::Vec3<T> out;
				out.x = this->y * vec.z - vec.y * this->z;
				out.y = -(this->x * vec.z - vec.x * this->z);
				out.z = this->x * vec.y - vec.x * this->y;
				return out;
			}


        };     


        template<typename T>
        struct Vec4 {
            T x, y, z;
            
            Vec4(T x, T y, T z, T w){
                set(x, y, z, w);
            }
            
            Vec4(T c){
                set(c);
            }
            
            Vec4(){
                set(0);
            }

			Vec4(const astro::Vec4<T> &vec){
				this->x = vec.x;
				this->y = vec.y;
				this->z = vec.z;
				this->w = vec.w;
			}			
            
            void set(T x, T y, T z, T w){
                this->x = x; this->y = y; this->z = z; this->w = w;
            }

            void set(T c){
                this->x = c;
                this->y = c;
                this->z = c;
				this->w = c;
            }

			astro::Vec4<T> normalize(){
				astro::Vec4<T> normalized = *this;
				float sqr = normalized.x * normalized.x + normalized.y * normalized.y + normalized.z * normalized.z;
				if(sqr == 1 || sqr == 0){
					return normalized;
				}
				float invrt = 1.0f/sqrt(sqr);
				normalized.x *= invrt;
				normalized.y *= invrt;
				normalized.z *= invrt;				
				return normalized;
			}

			float dot(const astro::Vec4<T> &vec) {
				return this->x * vec.x + this->y * vec.y + this->z * vec.z + this->w * vec.w;
			}

			astro::Vec4<T> cross(const astro::Vec4<T> &vec) {
				astro::Vec4<T> out;
				out.x = this->y*vec.z - this->z*vec.y;
				out.y = this->z*vec.x - this->x*vec.z;
				out.z = this->x*vec.y - this->y*vec.x;
				return out;
			}

        };  

		static const float MAT4FIdentity[16] = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1		
		};		

		template<unsigned w, unsigned h, typename T>
		struct Mat {
			unsigned size;
			T mat[w * h];
            
			Mat(T c){
				set(c);
            }

			Mat(){
				
            }			

            Mat(const T mat[w*h]){
				set(mat);
            }		

            void set(const T mat[w*h]){
				for(unsigned i = 0; i < w * h; ++i){
					this->mat[i] = mat[i];
				}
            }	

			void set(const T c){
				for(unsigned i = 0; i < w * h; ++i){
					this->mat[i] = c;
				}
			}

			astro::Mat<4, 4, T> operator*(const astro::Mat<4, 4, T> &m2){
				astro::Mat<4, 4, T> out(MAT4FIdentity);
				unsigned int row, column, row_offset;

				for (row = 0, row_offset = row * 4; row < 4; ++row, row_offset = row * 4)
					for (column = 0; column < 4; ++column)
						out.m[row_offset + column] =
							(this->mat[row_offset + 0] * m2->mat[column + 0]) +
							(this->mat[row_offset + 1] * m2->mat[column + 4]) +
							(this->mat[row_offset + 2] * m2->mat[column + 8]) +
							(this->mat[row_offset + 3] * m2->mat[column + 12]);

				return out;
			}

			astro::Vec4<T> operator*(const astro::Vec4<T> &vec){
				astro::Vec4<T> out;
				for(int i = 0; i < 4; ++i) {
					switch(i){
						case 0: {
							out.x = (vec.x * this->mat[i + 0]) + (vec.y * this->mat[i + 4]) + (vec.z * this->mat[i + 8]) + (vec.w * this->mat[i + 12]);
						} break;
						case 1: {
							out.y = (vec.x * this->mat[i + 0]) + (vec.y * this->mat[i + 4]) + (vec.z * this->mat[i + 8]) + (vec.w * this->mat[i + 12]);
						} break;
						case 2: {
							out.z = (vec.x * this->mat[i + 0]) + (vec.y * this->mat[i + 4]) + (vec.z * this->mat[i + 8]) + (vec.w * this->mat[i + 12]);
						} break;
						case 3: {
							out.w = (vec.x * this->mat[i + 0]) + (vec.y * this->mat[i + 4]) + (vec.z * this->mat[i + 8]) + (vec.w * this->mat[i + 12]);
						} break;																		
					}
				}

				return out;
			}
		};

		static const astro::Mat<4, 4, float>MAT4Identity(MAT4FIdentity);

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