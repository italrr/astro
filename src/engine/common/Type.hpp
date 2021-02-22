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
		
		namespace Math {
			float sqrt(float n);
			float sin(float n);
			float cos(float n);
		}

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


			astro::Vec2<T> operator*(const astro::Vec2<T> &vec) const{
				return astro::Vec2<T>(this->x * vec.x, this->y * vec.y);
			}

			astro::Vec2<T> operator/(const astro::Vec2<T> &vec) const{
				return astro::Vec2<T>(this->x / vec.x, this->y / vec.y);
			}

			astro::Vec2<T> operator+(const astro::Vec2<T> &vec) const{
				return astro::Vec2<T>(this->x + vec.x, this->y + vec.y);
			}

			astro::Vec2<T> operator-(const astro::Vec2<T> &vec) const{
				return astro::Vec2<T>(this->x - vec.x, this->y - vec.y);
			}			

			astro::Vec2<T> normalize() const{
				astro::Vec2<T> normalized = *this;
				T sqr = normalized.x * normalized.x + normalized.y * normalized.y;
				if(sqr == 1 || sqr == 0){
					return normalized;
				}
				T invrt = 1.0f/astro::Math::sqrt(sqr);
				normalized.x *= invrt;
				normalized.y *= invrt;		
				return normalized;
			}	

			T dot(const astro::Vec2<T> &vec) const{
				T product = 0; 
				product += x * vec.x; 
				product += y * vec.y; 
				return product; 
			}

			astro::Vec2<T> cross(const astro::Vec2<T> &vec) const{
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

			astro::Vec3<T> normalize() const {
				astro::Vec3<T> normalized = *this;
				float sqr = normalized.dot(normalized);
				if(sqr == 1 || sqr == 0){
					return normalized;
				}
				float invrt = 1.0f / astro::Math::sqrt(sqr);
				normalized.x *= invrt;
				normalized.y *= invrt;
				normalized.z *= invrt;				
				return normalized;
			}	

			astro::Vec3<T> operator*(const astro::Vec3<T> &vec) const{
				return astro::Vec3<T>(this->x * vec.x, this->y * vec.y, this->z * vec.z);
			}

			astro::Vec3<T> operator/(const astro::Vec3<T> &vec) const{
				return astro::Vec3<T>(this->x / vec.x, this->y / vec.y, this->z / vec.z);
			}

			astro::Vec3<T> operator+(const astro::Vec3<T> &vec) const{
				return astro::Vec3<T>(this->x + vec.x, this->y + vec.y, this->z + vec.z);
			}

			astro::Vec3<T> operator-(const astro::Vec3<T> &vec) const{
				return astro::Vec3<T>(this->x - vec.x, this->y - vec.y, this->z - vec.z);
			}						

			T dot(const astro::Vec3<T> &vec) const{
				return this->x * vec.x + this->y * vec.y + this->z * vec.z;
			}

			astro::Vec3<T> cross(const astro::Vec3<T> &vec) const{
				astro::Vec3<T> out;
				out.x = this->y * vec.z - vec.y * this->z;
				out.y = -(this->x * vec.z - vec.x * this->z);
				out.z = this->x * vec.y - vec.x * this->y;
				return out;
			}


        };     


        template<typename T>
        struct Vec4 {
            T x, y, z, w;
            
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

			astro::Vec4<T> operator*(const astro::Vec4<T> &vec) const{
				return astro::Vec4<T>(this->x * vec.x, this->y * vec.y, this->z * vec.z, this->w * vec.w);
			}

			astro::Vec4<T> operator/(const astro::Vec4<T> &vec) const{
				return astro::Vec4<T>(this->x / vec.x, this->y / vec.y, this->z / vec.z, this->w / vec.w);
			}

			astro::Vec4<T> operator+(const astro::Vec4<T> &vec) const{
				return astro::Vec4<T>(this->x + vec.x, this->y + vec.y, this->z + vec.z, this->w + vec.w);
			}

			astro::Vec4<T> operator-(const astro::Vec4<T> &vec) const{
				return astro::Vec4<T>(this->x - vec.x, this->y - vec.y, this->z - vec.z, this->w - vec.w);
			}									

			astro::Vec4<T> normalize() const{
				astro::Vec4<T> normalized = *this;
				float sqr = normalized.x * normalized.x + normalized.y * normalized.y + normalized.z * normalized.z;
				if(sqr == 1 || sqr == 0){
					return normalized;
				}
				float invrt = 1.0f/astro::Math::sqrt(sqr);
				normalized.x *= invrt;
				normalized.y *= invrt;
				normalized.z *= invrt;				
				return normalized;
			}

			float dot(const astro::Vec4<T> &vec) const{
				return this->x * vec.x + this->y * vec.y + this->z * vec.z + this->w * vec.w;
			}

			astro::Vec4<T> cross(const astro::Vec4<T> &vec) const{
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

			astro::Mat<4, 4, T> operator*(const astro::Mat<4, 4, T> &m2) const{
				astro::Mat<4, 4, T> out(MAT4FIdentity);
				unsigned int row, column, row_offset;

				for (row = 0, row_offset = row * 4; row < 4; ++row, row_offset = row * 4)
					for (column = 0; column < 4; ++column)
						out.mat[row_offset + column] =
							(this->mat[row_offset + 0] * m2.mat[column + 0]) +
							(this->mat[row_offset + 1] * m2.mat[column + 4]) +
							(this->mat[row_offset + 2] * m2.mat[column + 8]) +
							(this->mat[row_offset + 3] * m2.mat[column + 12]);

				return out;
			}

			astro::Mat<4, 4, T> operator*(T s) const{
				Mat<4, 4, T> out(*this);


				for(int i = 0; i < 4*4; ++i){
					out.mat[i] *= s;
				}


				return out;
			}			

			astro::Mat<4, 4, T> translate(const astro::Vec3<T> &vec) const{
				Mat<4, 4, T> out(*this);
				
				out.mat[12] = vec.x;
				out.mat[13] = vec.y;
				out.mat[14] = vec.z;

				return out;
			}

			astro::Mat<4, 4, T> scale(const astro::Vec3<T> &vec) const{
				Mat<4, 4, T> out;
				
				out.mat[0 + 4*0] = this->mat[0 + 4*0] * vec.x;
				out.mat[0 + 4*1] = this->mat[0 + 4*1] * vec.x;
				out.mat[0 + 4*2] = this->mat[0 + 4*2] * vec.x;
				out.mat[0 + 4*3] = this->mat[0 + 4*3] * vec.x;

				out.mat[1 + 4*0] = this->mat[1 + 4*0] * vec.y;
				out.mat[1 + 4*1] = this->mat[1 + 4*1] * vec.y;
				out.mat[1 + 4*2] = this->mat[1 + 4*2] * vec.y;
				out.mat[1 + 4*3] = this->mat[1 + 4*3] * vec.y;

				out.mat[2 + 4*0] = this->mat[2 + 4*0] * vec.z;
				out.mat[2 + 4*1] = this->mat[2 + 4*1] * vec.z;
				out.mat[2 + 4*2] = this->mat[2 + 4*2] * vec.z;
				out.mat[2 + 4*3] = this->mat[2 + 4*3] * vec.z;				

				out.mat[3 + 4*0] = this->mat[3 + 4*0];
				out.mat[3 + 4*1] = this->mat[3 + 4*1];
				out.mat[3 + 4*2] = this->mat[3 + 4*2];
				out.mat[3 + 4*3] = this->mat[3 + 4*3];

				return out;
			}

			astro::Mat<4, 4, T> transpose(){
				astro::Mat<4, 4, T> result;

				result.mat[0 + 4 * 0] = this->mat[0 + 4 * 0];
				result.mat[0 + 4 * 1] = this->mat[1 + 4 * 0];
				result.mat[0 + 4 * 2] = this->mat[2 + 4 * 0];
				result.mat[0 + 4 * 3] = this->mat[3 + 4 * 0];

				result.mat[1 + 4 * 0] = this->mat[0 + 4 * 1];
				result.mat[1 + 4 * 1] = this->mat[1 + 4 * 1];
				result.mat[1 + 4 * 2] = this->mat[2 + 4 * 1];
				result.mat[1 + 4 * 3] = this->mat[3 + 4 * 1];

				result.mat[2 + 4 * 0] = this->mat[0 + 4 * 2];
				result.mat[2 + 4 * 1] = this->mat[1 + 4 * 2];
				result.mat[2 + 4 * 2] = this->mat[2 + 4 * 2];
				result.mat[2 + 4 * 3] = this->mat[3 + 4 * 2];

				result.mat[3 + 4 * 0] = this->mat[0 + 4 * 3];
				result.mat[3 + 4 * 1] = this->mat[1 + 4 * 3];
				result.mat[3 + 4 * 2] = this->mat[2 + 4 * 3];
				result.mat[3 + 4 * 3] = this->mat[3 + 4 * 3];

				return result;				
			}

			astro::Mat<4, 4, T> inverse(){
				T c00 = this->mat[2 + 4*2] * this->mat[3 + 4*3] - this->mat[3 + 4*2] * this->mat[2 + 4*3];
				T c02 = this->mat[1 + 4*2] * this->mat[3 + 4*3] - this->mat[3 + 4*2] * this->mat[1 + 4*3];
				T c03 = this->mat[1 + 4*2] * this->mat[2 + 4*3] - this->mat[2 + 4*2] * this->mat[1 + 4*3];

				T c04 = this->mat[2 + 4*1] * this->mat[3 + 4*3] - this->mat[3 + 4*1] * this->mat[2 + 4*3];
				T c06 = this->mat[1 + 4*1] * this->mat[3 + 4*3] - this->mat[3 + 4*1] * this->mat[1 + 4*3];
				T c07 = this->mat[1 + 4*1] * this->mat[2 + 4*3] - this->mat[2 + 4*1] * this->mat[1 + 4*3];

				T c08 = this->mat[2 + 4*1] * this->mat[3 + 4*2] - this->mat[3 + 4*1] * this->mat[2 + 4*2];
				T c10 = this->mat[1 + 4*1] * this->mat[3 + 4*2] - this->mat[3 + 4*1] * this->mat[1 + 4*2];
				T c11 = this->mat[1 + 4*1] * this->mat[2 + 4*2] - this->mat[2 + 4*1] * this->mat[1 + 4*2];

				T c12 = this->mat[2 + 4*0] * this->mat[3 + 4*3] - this->mat[3 + 4*0] * this->mat[2 + 4*3];
				T c14 = this->mat[1 + 4*0] * this->mat[3 + 4*3] - this->mat[3 + 4*0] * this->mat[1 + 4*3];
				T c15 = this->mat[1 + 4*0] * this->mat[2 + 4*3] - this->mat[2 + 4*0] * this->mat[1 + 4*3];

				T c16 = this->mat[2 + 4*0] * this->mat[3 + 4*2] - this->mat[3 + 4*0] * this->mat[2 + 4*2];
				T c18 = this->mat[1 + 4*0] * this->mat[3 + 4*2] - this->mat[3 + 4*0] * this->mat[1 + 4*2];
				T c19 = this->mat[1 + 4*0] * this->mat[2 + 4*2] - this->mat[2 + 4*0] * this->mat[1 + 4*2];

				T c20 = this->mat[2 + 4*0] * this->mat[3 + 4*1] - this->mat[3 + 4*0] * this->mat[2 + 4*1];
				T c22 = this->mat[1 + 4*0] * this->mat[3 + 4*1] - this->mat[3 + 4*0] * this->mat[1 + 4*1];
				T c23 = this->mat[1 + 4*0] * this->mat[2 + 4*1] - this->mat[2 + 4*0] * this->mat[1 + 4*1];

				Vec4<T> fac0(c00, c00, c02, c03);
				Vec4<T> fac1(c04, c04, c06, c07);
				Vec4<T> fac2(c08, c08, c10, c11);
				Vec4<T> fac3(c12, c12, c14, c15);
				Vec4<T> fac4(c16, c16, c18, c19);
				Vec4<T> fac5(c20, c20, c22, c23);

				Vec4<T> vec0(this->mat[1 + 4*0], this->mat[0 + 4*0], this->mat[0 + 4*0], this->mat[0 + 4*0]);
				Vec4<T> vec1(this->mat[1 + 4*1], this->mat[0 + 4*1], this->mat[0 + 4*1], this->mat[0 + 4*1]);
				Vec4<T> vec2(this->mat[1 + 4*2], this->mat[0 + 4*2], this->mat[0 + 4*2], this->mat[0 + 4*2]);
				Vec4<T> vec3(this->mat[1 + 4*3], this->mat[0 + 4*3], this->mat[0 + 4*3], this->mat[0 + 4*3]);

				Vec4<T> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
				Vec4<T> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
				Vec4<T> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
				Vec4<T> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

				Vec4<T> signA(+1, -1, +1, -1);
				Vec4<T> signB(-1, +1, -1, +1);

				Mat<4, 4, T> inv;

				Vec4<T> col1 = inv0 * signA;
				Vec4<T> col2 = inv1 * signB;
				Vec4<T> col3 = inv2 * signA;
				Vec4<T> col4 = inv3 * signB;

				inv.mat[0 + 4*0] = col1.x;
				inv.mat[0 + 4*1] = col1.y;
				inv.mat[0 + 4*2] = col1.z;
				inv.mat[0 + 4*3] = col1.w;


				inv.mat[1 + 4*0] = col2.x;
				inv.mat[1 + 4*1] = col2.y;
				inv.mat[1 + 4*2] = col2.z;
				inv.mat[1 + 4*3] = col2.w;				
				
			
				inv.mat[2 + 4*0] = col3.x;
				inv.mat[2 + 4*1] = col3.y;
				inv.mat[2 + 4*2] = col3.z;
				inv.mat[2 + 4*3] = col3.w;


				inv.mat[3 + 4*0] = col4.x;
				inv.mat[3 + 4*1] = col4.y;
				inv.mat[3 + 4*2] = col4.z;
				inv.mat[3 + 4*3] = col4.w;


				Vec4<T> row0(inv.mat[0 + 4*0], inv.mat[1 + 4*0], inv.mat[2 + 4*0], inv.mat[3 + 4*0]);

				Vec4<T> dot0(this->mat[0+0*4] * row0.x, this->mat[0+1*4] * row0.y, this->mat[0+2*4] * row0.z, this->mat[0+3*4] * row0.w);
				T dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

				T ood = ((T)1) / dot1;

				return inv * ood;				
			}

			astro::Mat<4, 4, T> rotate(T angle, const astro::Vec3<T> &vec) const{
				T const a = angle;
				T const c = Math::cos(a);
				T const s = Math::sin(a);

				astro::Vec3<T> axis = vec.normalize();
				float cdiff = (T)1 - c;
				astro::Vec3<T> temp(axis.x * cdiff, axis.y * cdiff, axis.z * cdiff);

				Mat<4, 4, T> rotate(MAT4FIdentity);
				
				rotate.mat[0 + 0*4] = c + temp.x * axis.x;
				rotate.mat[0 + 1*4] = temp.x * axis.y + s * axis.z;
				rotate.mat[0 + 2*4] = temp.x * axis.z - s * axis.y;

				rotate.mat[1 + 0*4] = temp.y * axis.x - s * axis.z;
				rotate.mat[1 + 1*4] = c + temp.y * axis.y;
				rotate.mat[1 + 2*4] = temp.y * axis.z + s * axis.x;

				rotate.mat[2 + 0*4] = temp.z * axis.x + s * axis.y;
				rotate.mat[2 + 1*4] = temp.z * axis.y - s * axis.x;
				rotate.mat[2 + 2*4] = c + temp.z * axis.z;

				Mat<4, 4, T> out(MAT4FIdentity);

				auto &m = this->mat;
				
				out.mat[0 + 0*4] = m[0 + 0*4] * rotate.mat[0 + 0*4] + m[1 + 0*4] * rotate.mat[0 + 1*4] + m[2 + 0*4] * rotate.mat[0 + 2*4];
				out.mat[0 + 1*4] = m[0 + 1*4] * rotate.mat[0 + 0*4] + m[1 + 1*4] * rotate.mat[0 + 1*4] + m[2 + 1*4] * rotate.mat[0 + 2*4];
				out.mat[0 + 2*4] = m[0 + 2*4] * rotate.mat[0 + 0*4] + m[1 + 2*4] * rotate.mat[0 + 1*4] + m[2 + 2*4] * rotate.mat[0 + 2*4];

				out.mat[1 + 0*4] = m[0 + 0*4] * rotate.mat[1 + 0*4] + m[1 + 0*4] * rotate.mat[1 + 1*4] + m[2 + 0*4] * rotate.mat[1 + 2*4];
				out.mat[1 + 1*4] = m[0 + 1*4] * rotate.mat[1 + 0*4] + m[1 + 1*4] * rotate.mat[1 + 1*4] + m[2 + 1*4] * rotate.mat[1 + 2*4];
				out.mat[1 + 2*4] = m[0 + 2*4] * rotate.mat[1 + 0*4] + m[1 + 2*4] * rotate.mat[1 + 1*4] + m[2 + 2*4] * rotate.mat[1 + 2*4];

				out.mat[2 + 0*4] = m[0 + 0*4] * rotate.mat[2 + 0*4] + m[1 + 0*4] * rotate.mat[2 + 1*4] + m[2 + 0*4] * rotate.mat[2 + 2*4];
				out.mat[2 + 1*4] = m[0 + 1*4] * rotate.mat[2 + 0*4] + m[1 + 1*4] * rotate.mat[2 + 1*4] + m[2 + 1*4] * rotate.mat[2 + 2*4];
				out.mat[2 + 2*4] = m[0 + 2*4] * rotate.mat[2 + 0*4] + m[1 + 2*4] * rotate.mat[2 + 1*4] + m[2 + 2*4] * rotate.mat[2 + 2*4];
		
				out.mat[3 + 0*4] = m[3 + 0*4];
				out.mat[3 + 1*4] = m[3 + 1*4];
				out.mat[3 + 2*4] = m[3 + 2*4];
				out.mat[3 + 3*4] = m[3 + 3*4];
				return out;
			}

			astro::Vec4<T> operator*(const astro::Vec4<T> &vec) const{
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