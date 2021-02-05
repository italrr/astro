#ifndef ASTRO_INDEXING_HPP
    #define ASTRO_INDEXING_HPP

    #include "Type.hpp"
    #include "Result.hpp"

    #include <unordered_map>
    #include <memory>
    #include <mutex>

    namespace astro {
        
        namespace Indexing {

            struct Index {
                std::string hash;
                std::string fname;
                std::string path; //abs
                size_t size;
                std::vector<std::string> tags;
                void read(const std::string &path);
                bool isIt(const std::string &tag);
                void autotag();
            };

            static const std::vector<std::string> INDEX_STRUCTURE = {
                "texture", "font", "model", "shader"
            };

            struct Indexer {
                std::string path;
                std::mutex accesMutex;
                std::unordered_map<std::string, std::shared_ptr<Index>> resources;
                std::shared_ptr<astro::Result> scan(const std::string &root);
                std::shared_ptr<Index> findByHash(const std::string &hash);
                std::shared_ptr<Index> findByName(const std::string &name);

                std::shared_ptr<astro::Result> asyncFindByHash(const std::string &hash, std::function<void(std::shared_ptr<Index> &file)> callback);
                std::shared_ptr<astro::Result> asyncFindByName(const std::string &name, std::function<void(std::shared_ptr<Index> &file)> callback); 

                std::shared_ptr<astro::Result> asyncFindManyByHash(const std::vector<std::string> &hashes, std::function<void(std::vector<std::shared_ptr<Index>> &files)> callback);
                std::shared_ptr<astro::Result> asyncFindManyByName(const std::vector<std::string> &names, std::function<void(std::vector<std::shared_ptr<Index>> &files)> callback);
            };
        }

    }

#endif