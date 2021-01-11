#include "Resource.hpp"
#include "../Core.hpp"


std::shared_ptr<astro::Result> astro::Resource::ResourceManager::load(const std::shared_ptr<astro::Indexing::Index> &file, const std::shared_ptr<Resource> &holder){
    auto result = astro::makeResult(astro::ResultType::Waiting);
    if(file.get() == NULL){
        result->setFailure("no file provided");
        return result;
    }
    result->job = astro::spawn([&, holder, result, file](astro::Job &ctx){
        ctx.spec.result = result;
        std::unique_lock<std::mutex> lk(accesMutex);
        auto r = holder->load(file);
        if(r->isSuccessful()){
            holder->id = ++lastId;
            this->resources[holder->id] = holder;
            result->setSuccess(String::format("loaded '%s'", file->path.c_str()));
            ctx.spec.payload->write(&holder->id, sizeof(holder->id));
        }else{
            result->setFailure(String::format("failed to load '%s': %s", file->path.c_str(), r->msg.c_str()));
        }
        lk.unlock();
    }, astro::JobSpec(true, false, true, {"resource_load", file->fname}));
    return result;
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