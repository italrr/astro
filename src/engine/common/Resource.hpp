#ifndef ASTRO_RESOURCES_HPP
    #define ASTRO_RESOURCES_HPP

    #include "Type.hpp"
    #include "Result.hpp"

    #include <unordered_map>
    #include <memory>
    #include <mutex>

    #include "Type.hpp"
    #include "Indexer.hpp"
    #include "Job.hpp"

    namespace astro {
        
        namespace Resource {

            namespace ResourceType {
                enum ResourceType : int {
                    SHADER,
                    TEXTURE,
                    MODEL,
                    NONE
                };
            }

            struct Resource {
                bool loaded;
                int type;
                int id;
                std::shared_ptr<astro::Indexing::Index> file;
                Resource(){
                    type = ResourceType::NONE;
                }
                virtual std::shared_ptr<astro::Result> load(const std::shared_ptr<astro::Indexing::Index> &file){
                    return astro::makeResult(astro::ResultType::Success);
                }
            };

            struct ResourceManager {
                int lastId;
                std::mutex accesMutex;
                std::unordered_map<int, std::shared_ptr<Resource>> resources;
                std::shared_ptr<astro::Result> load(const std::string &name, const std::shared_ptr<Resource> &holder);
                std::shared_ptr<astro::Result> load(const std::shared_ptr<astro::Indexing::Index> &file, const std::shared_ptr<Resource> &holder);
                std::shared_ptr<astro::Resource::Resource> findByName(const std::string &name);
                std::shared_ptr<astro::Resource::Resource> findByHash(const std::string &hash);
                std::shared_ptr<astro::Resource::Resource> findById(int id);
                ResourceManager();
            };

        }
    }

#endif