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
            std::string md5(const std::string &path);
            std::string md5(char *data, size_t size);
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
        }
        // math
        namespace Math {
            static const float PI = 3.14159265358979323846;
            int random(int min, int max);
            float sqrt(float n);
            float sin(float n);
            float cos(float n);
            float tan(float n);
            float atan(float y, float x);
            float rads(float deg);
            float degs(float rads);
            Mat<4, 4, float> perspective(float fovy, float aspRatio, float nearPlane, float farPlane);
            Mat<4, 4, float> orthogonal(float left, float right, float bottom, float top);
            Mat<4, 4, float> lookAt(const Vec3<float> &pos, const Vec3<float> &dir);        
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