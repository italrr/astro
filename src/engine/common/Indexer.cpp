#include "Indexer.hpp"
#include "Tools.hpp"
#include "Log.hpp"
#include "Job.hpp"

std::shared_ptr<astro::Result> astro::Indexing::Indexer::scan(const std::string &root){
    auto result = astro::makeResult(astro::ResultType::Waiting);
    result->job = astro::spawn([&, root, result](astro::Job &ctx){
        std::unique_lock<std::mutex> lk(accesMutex);
        int found = 0;
        int bytes = 0;
        for(int i = 0; i < INDEX_STRUCTURE.size(); ++i){
            std::string path = root+astro::File::dirSep()+INDEX_STRUCTURE[i];
            std::vector<std::string> files;
            astro::File::list(path, "", astro::File::ListType::Any, true, files);
            for(int j = 0; j < files.size(); ++j){
                auto res = std::make_shared<astro::Indexing::Index>(astro::Indexing::Index());
                res->read(files[j]);
                this->resources[res->hash] = res;
                bytes += res->size;
                ++found;
            }
        }
        result->set(ResultType::Success);
        astro::log("[IND] found %i files(s) | total %i byte(s)\n", found, bytes);
        lk.unlock();
    }, astro::JobSpec(true, false, true, {"indexing", root}));
    return result;
}

std::shared_ptr<astro::Indexing::Index> astro::Indexing::Indexer::findByHash(const std::string &hash){
    std::unique_lock<std::mutex> lk(accesMutex);
    auto it = this->resources.find(hash);
    if(it == this->resources.end()){
        lk.unlock();
        return std::shared_ptr<astro::Indexing::Index>(NULL);
    }
    auto rs = it->second;
    lk.unlock();
    return it->second;
}

std::shared_ptr<astro::Indexing::Index> astro::Indexing::Indexer::findByName(const std::string &name){
    std::unique_lock<std::mutex> lk(accesMutex);
    for(auto &it : this->resources){
        if(it.second->fname == name){
            auto rs = it.second;
            lk.unlock();
            return rs;
        }
    }
    lk.unlock();
    return std::shared_ptr<astro::Indexing::Index>(NULL); 
}


void astro::Indexing::Index::read(const std::string &path){
    this->path = path;
    this->size = File::size(path);
    this->fname = File::filename(path);
    this->hash = Hash::md5(path);
    this->autotag();   
}

void astro::Indexing::Index::autotag(){
    tags.clear();
    if(!astro::File::exists(path)){
        return;
    }
    auto format = astro::File::format(path);
    if(format == "png" || format == "jpg" || format == "jpeg" || format == "bmp"){
        tags.push_back("image");
    }
    if(format == "mp3" || format == "wav" || format == "ogg" || format == "flac"){
        tags.push_back("sound");
    }
    if(format == "json"){
        tags.push_back("json");
    }
    if(format == "json"){
        tags.push_back("json");
    }    
    if(format == "glsl"){
        tags.push_back("shader");
    }       
}

bool astro::Indexing::Index::isIt(const std::string &tag){
    for(int i = 0; i < tags.size(); ++i){
        if(tags[i] == tag){
            return true;
        }
    }
    return false;
}