#ifndef ASTRO_TYPE_HPP
    #define ASTRO_TYPE_HPP

    #include <string>
	#include <sys/types.h>

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