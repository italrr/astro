#define STB_IMAGE_IMPLEMENTATION
#include "../common/3rdparty/stb_image.h"
#include "../common/Log.hpp"
#include "../Graphics.hpp"
#include "Texture.hpp"

std::shared_ptr<astro::Result> astro::Gfx::Texture::load(const std::shared_ptr<astro::Indexing::Index> &file){
    auto result = astro::makeResult(astro::ResultType::Waiting);
    result->job = astro::spawn([&, file, result](astro::Job &ctx){        
        int width, height, nrChannels;
        unsigned char *data = stbi_load(file->path.c_str(), &width, &height, &nrChannels, 0); 

        if(!data){
            result->setFailure(astro::String::format("Texture::load: failed to load texture '%s': incompatible format?", file->fname.c_str()));
            return;
        }

        auto jgfx = astro::findJob({"astro_gfx"});
        if(jgfx.get() == NULL){
            result->setFailure(astro::String::format("Texture::load: gfx job not found: cannot load shader '%s'", file->fname.c_str()));
            return;
        }
        
        jgfx->addBacklog([&, result, data, width, height, nrChannels](astro::Job &ctx){

            int format;
            if (nrChannels == 1)
                format = ImageFormat::RED;
            else if (nrChannels == 3)
                format = ImageFormat::RGB;
            else if (nrChannels == 4)
                format = ImageFormat::RGBA;

            auto ren = astro::Gfx::getRenderEngine();
            auto r = ren->generateTexture2D(data, width, height, format);
            result->set(r->val, r->msg);
            if(r->val == ResultType::Success){
                r->payload->reset();
                r->payload->read(&textureId, sizeof(textureId));
            }
            stbi_image_free(data);
        });
    }, true, false, true);

    return result;
}

std::shared_ptr<astro::Result> astro::Gfx::Texture::unload(){
    auto result = astro::makeResult(astro::ResultType::Waiting);
    if(!rscLoaded || textureId == 0){
        return astro::makeResult(astro::ResultType::noop);
    }
    result->job = astro::spawn([&, result](astro::Job &ctx){
        auto jgfx = astro::findJob({"astro_gfx"});
        if(jgfx.get() == NULL){
            result->setFailure(astro::String::format("Texture::load: gfx job not found: cannot unload shader '%s'", file->fname.c_str()));
            return;
        }
        jgfx->addBacklog([&, result](astro::Job &ctx){
            auto ren = astro::Gfx::getRenderEngine();
            // auto r = ren->deleteShader(shaderId);
            // result->set(r->val, r->msg);
        });
    }, true, false, true);
    return result;
}