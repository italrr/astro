#ifndef ASTRO_TOOL_GENERAL_HPP
    #define ASTRO_TOOL_GENERAL_HPP

    #include "Type.hpp"
    #include "Result.hpp"

    namespace astro {
        // time
        uint64 ticks();
        uint64 epoch();
        void sleep(uint64 t);
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
            std::string md5(const std::string &path);
            std::string md5(char *data, size_t size);
            astro::Result list(const std::string &path, const std::string &format, int type, bool recursively, std::vector<std::string> &output);
        }
        // string
        namespace String {
            std::string toLower(const std::string &str);
            std::vector<std::string> split(const std::string &str, const std::string &sep);
        }
        // math
        namespace Math {
            int random(int min, int max);
        }
    }

#endif