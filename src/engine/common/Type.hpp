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
    }

#endif