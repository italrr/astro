#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../common/Type.hpp"
#include "../common/Result.hpp"
#include "../Core.hpp"
#include "Model.hpp"

std::shared_ptr<astro::Result> astro::Gfx::Model::load(const std::shared_ptr<astro::Indexing::Index> &file){
    auto result = astro::makeResult(astro::ResultType::Waiting);
     // only use diffuse for now
    this->transform->resetMatMode();
    this->transform->enMatMode(Gfx::MaterialMode::DIFFUSE);
    result->job = astro::spawn([&, file, result](astro::Job &ctx){   

        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(file->path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);	

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            result->setFailure(astro::String::format("Model::load: failed to load model '%s': %s\n", file->fname.c_str(), import.GetErrorString()));
            return;
        }

        struct TextureDependency {
            std::shared_ptr<astro::Indexing::Index> file;
            int role;
        };

        std::unordered_map<std::string, TextureDependency> dependencies;

        auto indexer = Core::getIndexer();
        auto rscmngr = Core::getResourceMngr();

        auto processTexture = [&](aiMaterial *mat, aiTextureType type, int rtypeName){
            for(unsigned int i = 0; i < mat->GetTextureCount(type); ++i){
                aiString str;
                mat->GetTexture(type, i, &str);
                TextureDependency dep;
                dep.role = rtypeName;
                dep.file = indexer->findByName(str.C_Str());
                if(dep.file.get() == NULL){
                   astro::log("Model::load: failed to find texture '%s' for model '%s'\n", str.C_Str(), file->fname.c_str());
                }
                dependencies[dep.file->fname] = dep;
            }            
        };

        int WEIGHTS_PER_VERTEX = 4;

        auto processMesh = [&](aiMesh *mesh, const aiScene *scene){
            auto rmesh = std::make_shared<astro::Gfx::Mesh>(astro::Gfx::Mesh());
            std::vector<astro::Gfx::Vertex> vertices;
            std::vector<unsigned int> indices;
            // vertices
            for(int i = 0; i < mesh->mNumVertices; ++i){
                astro::Gfx::Vertex vertex;
                // positions
                vertex.position.x = mesh->mVertices[i].x;
                vertex.position.y = mesh->mVertices[i].y;
                vertex.position.z = mesh->mVertices[i].z;
                // normals
                if (mesh->HasNormals()){
                    vertex.normal.x = mesh->mNormals[i].x;
                    vertex.normal.y = mesh->mNormals[i].y;
                    vertex.normal.z = mesh->mNormals[i].z;
                }
                // texture coordinates
                if(mesh->HasTextureCoords(0)){
                    vertex.texCoords.x = mesh->mTextureCoords[0][i].x; 
                    vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
                }else{
                    vertex.texCoords = astro::Vec2<float>(0.0f, 0.0f);
                }
                // tangent and bittangent
                if(mesh->HasTangentsAndBitangents()){
                    // tangent
                    vertex.tangent.x = mesh->mTangents[i].x;
                    vertex.tangent.y = mesh->mTangents[i].y;
                    vertex.tangent.z = mesh->mTangents[i].z;
                    // bitangent
                    vertex.bitangent.x = mesh->mBitangents[i].x;
                    vertex.bitangent.y = mesh->mBitangents[i].y;
                    vertex.bitangent.z = mesh->mBitangents[i].z;
                }else{
                    // TODO: maybe generate it if not available?
                    vertex.tangent.set(0);
                    vertex.bitangent.set(0);
                }
                vertices.push_back(vertex);
            }
            // indices
            for(int i = 0; i < mesh->mNumFaces; ++i){
                aiFace face = mesh->mFaces[i];
                for(int j = 0; j < face.mNumIndices; ++j){
                    indices.push_back(face.mIndices[j]);        
                }
            }
            // process material
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];  
            processTexture(material, aiTextureType_DIFFUSE, TextureRole::DIFFUSE);
            processTexture(material, aiTextureType_SPECULAR, TextureRole::SPECULAR);
            processTexture(material, aiTextureType_HEIGHT, TextureRole::NORMAL);
            processTexture(material, aiTextureType_AMBIENT, TextureRole::HEIGHT);
            // process bones
            int boneArraysSize = mesh->mNumVertices*WEIGHTS_PER_VERTEX;
            std::vector<int> boneIDs;
            boneIDs.resize(boneArraysSize);            
            for(int i = 0; i < mesh->mNumBones; ++i){
                aiBone* bone = mesh->mBones[i]; 
                std::string bName(bone->mName.data);
                
            }

            rmesh->vertices = vertices;
            rmesh->indices = indices;

            return rmesh;
        };

        std::function<void(aiNode *node, const aiScene *scene)> processNode = [&](aiNode *node, const aiScene *scene){
            // process all the node's meshes (if any)
            for(int i = 0; i < node->mNumMeshes; i++){
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
                this->meshes.push_back(processMesh(mesh, scene));			
            }
            // then do the same for each of its children
            for(unsigned int i = 0; i < node->mNumChildren; i++){
                processNode(node->mChildren[i], scene);
            }            
        };


        processNode(scene->mRootNode, scene);


        std::vector<std::shared_ptr<astro::Result>> results;
        for(auto &it : dependencies){
            results.push_back(rscmngr->load(it.second.file, std::make_shared<astro::Gfx::Texture>(astro::Gfx::Texture())));
        }

        // expect all textures to load
        astro::expect(results, [&, dependencies, result,rscmngr, results](astro::Job &ctx){
            if(!ctx.succDeps){
                std::string messages = "";
                for(int i =0; i < ctx.listeners.size(); ++i){
                    messages += astro::String::format("%s\n", results[i]->msg.c_str());
                }
                astro::log("Model::load: warning: not all textures were loaded succesfully: %s\n", messages.c_str());
            }

            for(auto &it : dependencies){
                auto texture = rscmngr->findByName(it.second.file->fname);
                this->transform->textures.push_back(BindTexture(std::dynamic_pointer_cast<astro::Gfx::Texture>(texture), it.second.role));
            }

            auto jgfx = astro::findJob({"astro_gfx"});
            if(jgfx.get() == NULL){
                result->setFailure(astro::String::format("gfx job not found: cannot load shader '%s'", file->fname.c_str()));
                return;
            }

            // load meshes to gpu
            jgfx->addBacklog([&, result](astro::Job &ctx){
                auto ren = astro::Gfx::getRenderEngine();
                for(int i = 0; i < meshes.size(); ++i){
                    auto meshres = ren->generateMesh(meshes[i]->vertices, meshes[i]->indices);
                    meshres->payload->reset();
                    meshres->payload->read(&meshes[i]->vao, sizeof(meshes[i]->vao));
                    meshres->payload->read(&meshes[i]->vbo, sizeof(meshes[i]->vbo));
                    meshres->payload->read(&meshes[i]->ebo, sizeof(meshes[i]->ebo));
                }
                result->set(astro::ResultType::Success);
            });
        }, false);



    }, true, false, true);

    return result;
}

std::shared_ptr<astro::Result> astro::Gfx::Model::unload(){
    // TODO: unload code
    return astro::makeResult(astro::ResultType::Success);
}

void astro::Gfx::Model::render(){
    auto ren = astro::Gfx::getRenderEngine();
    for(int i = 0; i < meshes.size(); ++i){
        meshes[i]->transform = this->transform;
        ren->renderMesh(meshes[i].get());
    }
}

void astro::Gfx::Mesh::render(){
    auto ren = astro::Gfx::getRenderEngine();
    ren->renderMesh(this);
}