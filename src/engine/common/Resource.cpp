#include "Resource.hpp"
#include "../Core.hpp"


std::shared_ptr<astro::Result> astro::Resource::ResourceManager::load(const std::shared_ptr<astro::Indexing::Index> &file, const std::shared_ptr<Resource> &holder){
    if(file.get() == NULL){
        return  astro::makeResult(astro::ResultType::Failure, "no file provided");
    }
    std::unique_lock<std::mutex> lk(accesMutex);
    holder->setId(++lastId);
    holder->setLoaded(true);
    holder->setFile(file);
    this->resources[holder->id] = holder;        
    lk.unlock();
    auto r = holder->load(file);
    r->payload->write(&holder->id, sizeof(holder->id));
    return r;
}

std::shared_ptr<astro::Result> astro::Resource::ResourceManager::load(const std::string &name, const std::shared_ptr<Resource> &holder){
    auto indexer = astro::Core::getIndexer();
    auto file = indexer->findByName(name);
    if(file.get() == NULL){
        return astro::makeResult(astro::ResultType::Failure, "'"+name+"' doesn't exist");
    }
    return load(file, holder);
}

std::shared_ptr<astro::Resource::Resource> astro::Resource::ResourceManager::findByName(const std::string &name){
    auto resource = std::shared_ptr<astro::Resource::Resource>(NULL);
    std::unique_lock<std::mutex> lk(accesMutex);
    for(auto &it : resources){
        if(it.second->file->fname == name){
            resource = it.second;
            break;
        }
    }
    lk.unlock();
    return resource;
}

std::shared_ptr<astro::Resource::Resource> astro::Resource::ResourceManager::findByHash(const std::string &hash){
    auto resource = std::shared_ptr<astro::Resource::Resource>(NULL);
    std::unique_lock<std::mutex> lk(accesMutex);
    for(auto &it : resources){
        if(it.second->file->hash == hash){
            resource = it.second;
            break;
        }
    }
    lk.unlock();
    return resource;
}


std::shared_ptr<astro::Resource::Resource> astro::Resource::ResourceManager::findById(int id){
    auto resource = std::shared_ptr<astro::Resource::Resource>(NULL);
    std::unique_lock<std::mutex> lk(accesMutex);
    auto it = resources.find(id);
    if(it != resources.end()){
        resource = it->second;
    }
    lk.unlock();
    return resource;
}

astro::Resource::ResourceManager::ResourceManager(){
    lastId = astro::Math::random(101, 126);
}