#include "Shader.hpp"
#include "../Graphics.hpp"
#include "../common/Log.hpp"

#include <fstream>

static std::string loadSource(const std::string &path){
    std::string src;
    std::ifstream file;
    std::string buffer;
    file.open(path.c_str());
    while (getline(file,buffer)){
        src += buffer+"\n";
    }
    return src;
}

std::shared_ptr<astro::Result> astro::Gfx::Shader::load(const std::shared_ptr<astro::Indexing::Index> &file){
    auto result = astro::makeResult(astro::ResultType::Waiting);
    result->job = astro::spawn([&, file, result](astro::Job &ctx){
        // infere other shader filename(have _v ? infere _f; have _f? infere _v)
        const static std::string fprex = "_f";
        const static std::string vprex = "_v";        
        std::string fragPath = file->fname.find(fprex) != -1 ? file->path : "";
        std::string vertPath = file->fname.find(vprex) != -1 ? file->path : "";
        if(fragPath.length() == 0 && vertPath.size() == 0){
            result->setFailure(astro::String::format("failed to find both fragment and vertex files for '%s'", file->fname.c_str()));
            return;
        }
        if(fragPath.length() == 0){
            fragPath = vertPath;
            fragPath.replace(fragPath.find(vprex), vprex.size(), fprex);
        }
        if(vertPath.length() == 0){
            vertPath = fragPath;
            vertPath.replace(vertPath.find(fprex), fprex.size(), vprex);
        }
        
        // piggyback this to gfx job since shaders need to be compiled in the graphics api context's
        auto jgfx = astro::findJob({"astro_gfx"});
        if(jgfx.get() == NULL){
            result->setFailure(astro::String::format("gfx job not found: cannot compile shader '%s'", file->fname.c_str()));
            return;
        }
        this->fragSrc = loadSource(fragPath);
        this->vertSrc = loadSource(vertPath);
        jgfx->addBacklog([&, result](astro::Job &ctx){
            auto ren = astro::Gfx::getRenderEngine();
            auto r = ren->compileShader(this->vertSrc, this->fragSrc);
            result->set(r->val, r->msg);
            if(r->val == ResultType::Success){
                r->payload->reset();
                r->payload->read(&shaderId, sizeof(shaderId));
            }
        });
    }, true, false, true);

    return result;
}

std::shared_ptr<astro::Result> astro::Gfx::Shader::unload(){
    auto result = astro::makeResult(astro::ResultType::Waiting);
    if(!loaded || loaded == 0){
        return astro::makeResult(astro::ResultType::noop);
    }
    result->job = astro::spawn([&, result](astro::Job &ctx){
        auto jgfx = astro::findJob({"astro_gfx"});
        if(jgfx.get() == NULL){
            result->setFailure(astro::String::format("gfx job not found: cannot unload shader '%s'", file->fname.c_str()));
            return;
        }
        jgfx->addBacklog([&, result](astro::Job &ctx){
            auto ren = astro::Gfx::getRenderEngine();
            auto r = ren->deleteShader(shaderId);
            result->set(r->val, r->msg);
        });
    }, true, false, true);
    return result;
}