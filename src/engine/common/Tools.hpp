#ifndef ASTRO_TOOL_GENERAL_HPP
    #define ASTRO_TOOL_GENERAL_HPP

    #include <memory>
    #include <mutex>

    #include "Type.hpp"
    #include "Result.hpp"

    namespace astro {
        // time
        uint64 ticks();
        uint64 epoch();
        void sleep(uint64 t);
        // hash
        namespace Hash {
            std::string md5(const std::string &path, bool partial = false);
            std::string md5(char *data, size_t size, bool partial = false);
        }
        // file
        namespace File {
            enum ListType : int {
                File,
                Directory,
                Any
            };
            std::string dirSep();
            bool exists(const std::string &path);
            std::string format(const std::string &filename);
            std::string filename(const std::string &path);
            size_t size(const std::string &path);
            astro::Result list(const std::string &path, const std::string &format, int type, bool recursively, std::vector<std::string> &output);
        }
        // string
        namespace String {
            std::string toLower(const std::string &str);
            std::vector<std::string> split(const std::string &str, const std::string &sep);
            std::string format(const std::string &str, ...);
            std::string formatByes(int n);
            std::string str(float n);
            std::string str(double n);
            std::string str(uint8 n);
            std::string str(int8 n);
            std::string str(uint16 n);
            std::string str(int16 n);
            std::string str(uint32 n);
            std::string str(int32 n);
            std::string str(uint64 n);
            std::string str(int64 n);       
            
            template<typename T>
            std::string str(const astro::Mat<4, 4, T> &mat){
                std::string out;
                for(int x = 0; x < 4; ++x){
                    for(int y = 0; y < 4; ++y){
                        int i = x + y * 4;
                        out += str(static_cast<T>(mat.mat[i]))+" ";
                    }    
                }
                return out;
            } 

            template<typename T>
            std::string str(const astro::Mat<3, 3, T> &mat){
                std::string out;
                for(int x = 0; x < 3; ++x){
                    for(int y = 0; y < 3; ++y){
                        int i = x + y * 3;
                        out += str(static_cast<T>(mat.mat[i]));
                        if(x == 2){
                           out += "\n"; 
                        }else
                        if(x < 8){
                            out += ",\t";
                        }
                    }    
                }
                return out;
            }    

            template<typename T>
            std::string str(const astro::Vec2<T> &vec){
                return "(" + str(static_cast<T>(vec.x)) + ", " + str(static_cast<T>(vec.y)) + ")";
            }     

            template<typename T>
            std::string str(const astro::Vec3<T> &vec){
                return "(" + str(static_cast<T>(vec.x)) + ", " + str(static_cast<T>(vec.y)) + ", " + str(static_cast<T>(vec.z)) + ")";
            }      

            template<typename T>
            std::string str(const astro::Vec4<T> &vec){
                return "(" + str(static_cast<T>(vec.x)) + ", " + str(static_cast<T>(vec.y)) + ", " + str(static_cast<T>(vec.z)) + ", " + str(static_cast<T>(vec.w)) + ")";
            }                                   

        }
        // math
        namespace Math {
            static const float PI = 3.14159265358979323846;
            int random(int min, int max);
            float sqrt(float n);
            float sin(float n);
            float cos(float n);
            float tan(float n);
            float asin(float n);
            float acos(float n);
            float atan(float y, float x);
            float rads(float deg);
            float degs(float rads);
            astro::Mat<4, 4, float> scale(const astro::Vec3<float> &dir);
            astro::Mat<4, 4, float> perspective(float fovy, float aspRatio, float nearPlane, float farPlane);
            astro::Mat<4, 4, float> orthogonal(float left, float right, float bottom, float top);
            astro::Mat<4, 4, float> lookAt(const astro::Vec3<float> &to, const astro::Vec3<float> &dir, const Vec3<float> &up);  


            template<typename T>
            astro::Vec4<T> slerp(astro::Vec4<T> pStart, astro::Vec4<T> pEnd, T pFactor){
                // Taken from assimp source code
                // calc cosine theta
                T cosom = pStart.x * pEnd.x + pStart.y * pEnd.y + pStart.z * pEnd.z + pStart.w * pEnd.w;

                // adjust signs (if necessary)
                astro::Vec4<T> end = pEnd;
                if(cosom < static_cast<T>(0.0)){
                    cosom = -cosom;
                    end.x = -end.x;   // Reverse all signs
                    end.y = -end.y;
                    end.z = -end.z;
                    end.w = -end.w;
                }

                // Calculate coefficients
                T sclp, sclq;
                if((static_cast<T>(1.0) - cosom) > static_cast<T>(0.0001)){ // 0.0001 -> some epsillon{
                    // Standard case (slerp)
                    T omega, sinom;
                    omega = acos(cosom); // extract theta from dot product's cos theta
                    sinom = sin( omega);
                    sclp  = sin( (static_cast<T>(1.0) - pFactor) * omega) / sinom;
                    sclq  = sin( pFactor * omega) / sinom;
                }else{
                    // Very close, do linear interp (because it's faster)
                    sclp = static_cast<T>(1.0) - pFactor;
                    sclq = pFactor;
                }

                astro::Vec4<T> out;

                out.x = sclp * pStart.x + sclq * end.x;
                out.y = sclp * pStart.y + sclq * end.y;
                out.z = sclp * pStart.z + sclq * end.z;
                out.w = sclp * pStart.w + sclq * end.w; 

                return out;               
            }
        }
        // small packet
        static const size_t ASTRO_SMALLPACKET_SIZE = 64;
		struct SmallPacket {
            SmallPacket(const astro::SmallPacket &other);
            SmallPacket();
			~SmallPacket();
            std::mutex accesMutex;
			size_t index;
			char *data;				
			void copy(const astro::SmallPacket &other);
			void reset();
			void clear();
			void setIndex(size_t index);
			std::shared_ptr<astro::Result> write(const void *data, size_t Si);
			std::shared_ptr<astro::Result> read(std::string &str);
			std::shared_ptr<astro::Result> read(void *data, size_t Si);
			std::shared_ptr<astro::Result> write(const std::string str);
			astro::SmallPacket& operator= (const astro::SmallPacket &other);		
		};    
    }

#endif