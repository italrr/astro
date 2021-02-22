#include <cmath>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../common/Type.hpp"
#include "../common/Result.hpp"
#include "../Core.hpp"
#include "Model.hpp"

static astro::Mat<4,4, float> aiMat2Astro(aiMatrix4x4 &mat){
    astro::Mat<4,4, float> result;

    result.mat[0 + 4 * 0] = mat.a1;
    result.mat[0 + 4 * 1] = mat.b1;
    result.mat[0 + 4 * 2] = mat.c1;
    result.mat[0 + 4 * 3] = mat.d1;


    result.mat[1 + 4 * 0] = mat.a2;
    result.mat[1 + 4 * 1] = mat.b2;
    result.mat[1 + 4 * 2] = mat.c2;
    result.mat[1 + 4 * 3] = mat.d2;


    result.mat[2 + 4 * 0] = mat.a3;
    result.mat[2 + 4 * 1] = mat.b3;
    result.mat[2 + 4 * 2] = mat.c3;
    result.mat[2 + 4 * 3] = mat.d3;


    result.mat[3 + 4 * 0] = mat.a4;
    result.mat[3 + 4 * 1] = mat.b4;
    result.mat[3 + 4 * 2] = mat.c4;
    result.mat[3 + 4 * 3] = mat.d4;

    return result;
}

// astro::Gfx::Bone::Bone(){

// }

// astro::Gfx::Bone::Bone(const std::shared_ptr<astro::Gfx::Mesh> &mesh, unsigned int id, const std::string &name, const astro::Mat<4, 4, float> &offsetMat){
//     this->id = id;
//     this->name = name;
//     this->offsetMat = offsetMat;
//     this->mesh = mesh;
//     this->parent = NULL;
// }

// astro::Mat<4, 4, float> astro::Gfx::Bone::getParentTrans(){
//      auto b = parent;

//     std::vector<astro::Mat<4, 4, float>> mats; 

//     while(b.get() != NULL){   
//         auto mat = b->trans;
//         mats.push_back(mat);
//         b = b->parent;
//     }

//     astro::Mat<4, 4, float> conc;

//     for(int i = mats.size()-1; i >= 0; i--){
//         conc =  conc * mats[i];
//     }

//     return conc;
// }




static void initMesh(   const unsigned int index,
                        const aiScene* pScene,
                        const aiMesh *mesh,
                        astro::Gfx::Mesh *amesh,
                        astro::Gfx::Model *model,
                        std::vector<astro::Gfx::Vertex> &verts,
                        std::vector<unsigned int> &ind,
                        std::vector<astro::Gfx::TextureDependency> &textures);
static void initScene(const aiScene* pScene, astro::Gfx::Model *model);
static void initBones(const unsigned int index, const aiMesh *mesh, astro::Gfx::Mesh *amesh, astro::Gfx::Model *model);
static void initTexture(const aiScene* scene, const aiMesh *mesh, std::vector<astro::Gfx::TextureDependency> &textures);
static void initAnimation(const aiScene* scene, astro::Gfx::Model *model);

static void initScene(const aiScene* scene, astro::Gfx::Model *model){
    int nmeshes = scene->mNumMeshes;
    model->meshes.resize(nmeshes);

	unsigned int nVert = 0;
	unsigned int nIndices = 0;

    // setup
	for (unsigned int i = 0; i < nmeshes; ++i) {
        // create meshes
        auto mesh = std::make_shared<astro::Gfx::Mesh>(astro::Gfx::Mesh());
        model->meshes[i] = mesh;
		model->meshes[i]->mIndex = scene->mMeshes[i]->mMaterialIndex;
        // metadata
		model->meshes[i]->nIndices = scene->mMeshes[i]->mNumFaces * 3;		
 		model->meshes[i]->bVertex = nVert;
		model->meshes[i]->bIndex = nIndices;
		nVert += scene->mMeshes[i]->mNumVertices;
		nIndices += model->meshes[i]->nIndices;
	}
    // init everything
    for(unsigned int i = 0; i < nmeshes; ++i){
        auto &mesh = model->meshes[i];
        initMesh(i, scene, scene->mMeshes[i], mesh.get(), model, mesh->vertices, mesh->indices, model->texDeps);
    }
    // init animation
    initAnimation(scene, model);
}

static void initBones(const unsigned int index, const aiMesh *mesh, astro::Gfx::Mesh *amesh, astro::Gfx::Model *model){
    for(unsigned int i = 0; i < mesh->mNumBones; ++i){
        unsigned int bIndex = 0;
        std::string name(mesh->mBones[i]->mName.data);

        auto bMapping = model->boneMapping.find(name);
        if(bMapping == model->boneMapping.end()){

            bIndex = amesh->nBones++;

            astro::Gfx::BoneInfo bi;
            model->boneInfo.push_back(bi);
        }else{
            bIndex = bMapping->second;
        }

        model->boneMapping[name] = bIndex;
        model->boneInfo[bIndex].offset = aiMat2Astro(mesh->mBones[i]->mOffsetMatrix);

		for(unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++){
            // aiVertexWeight weight = bone->mWeights[j];
            // unsigned int vertId = amesh->bVertex + mesh->mBones[i]->mWeights[j].mVertexId;
            unsigned int vertId =  mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			amesh->vertices[vertId].setBoneData(i, weight);
		}

    }
  
}

static void initTexture(const aiScene* pScene, const aiMesh *mesh, std::vector<astro::Gfx::TextureDependency> &textures){
    auto indexer = astro::Core::getIndexer();

    static auto process = [&](aiMaterial *mat, aiTextureType type, int rtypeName){
        for(unsigned int i = 0; i < mat->GetTextureCount(type); ++i){
            aiString str;
            mat->GetTexture(type, i, &str);
            astro::Gfx::TextureDependency dep;
            dep.role = rtypeName;
            dep.file = indexer->findByName(str.C_Str());
            if(dep.file.get() == NULL){
                return;
            }
            textures.push_back(dep);
        }            
    };  

    aiMaterial* material = pScene->mMaterials[mesh->mMaterialIndex];  
    process(material, aiTextureType_DIFFUSE, astro::Gfx::TextureRole::DIFFUSE);
    process(material, aiTextureType_SPECULAR, astro::Gfx::TextureRole::SPECULAR);
    process(material, aiTextureType_HEIGHT, astro::Gfx::TextureRole::NORMAL);
    process(material, aiTextureType_AMBIENT, astro::Gfx::TextureRole::HEIGHT); 
};


static void initMesh(   const unsigned int index,
                        const aiScene* pScene,
                        const aiMesh *mesh,
                        astro::Gfx::Mesh *amesh,
                        astro::Gfx::Model *model,
                        std::vector<astro::Gfx::Vertex> &verts,
                        std::vector<unsigned int> &ind,
                        std::vector<astro::Gfx::TextureDependency> &textures){




    // vertex data
    for(unsigned int i = 0; i < mesh->mNumVertices; ++i){
        astro::Gfx::Vertex vertex;
        // positions
        vertex.position.set(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        // normals
        if (mesh->HasNormals()){
            vertex.normal.set(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        // texture coordinates
        if(mesh->HasTextureCoords(0)){
            vertex.texCoords.set(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y); 
        }else{
            vertex.texCoords.set(0.0f, 0.0f);
        }
        // tangent and bittangent
        if(mesh->HasTangentsAndBitangents()){
            // tangent
            vertex.tangent.set(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            // bitangent
            vertex.bitangent.set(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }else{
            // TODO: maybe generate these if not available?
            vertex.tangent.set(0);
            vertex.bitangent.set(0);
        }
        verts.push_back(vertex);
    }

    // indices
    for(unsigned int i = 0; i < mesh->mNumFaces; ++i){
        aiFace face = mesh->mFaces[i];
        for(int j = 0; j < face.mNumIndices; ++j){
            ind.push_back(face.mIndices[j]);        
        }
    }    

    // bones
    if(mesh->HasBones()){
        initBones(index, mesh, amesh, model);
    }

    // textures
    initTexture(pScene, mesh, textures);

}

static void initAnimation(const aiScene* scene, astro::Gfx::Model *model){
    auto nanim = scene->mNumAnimations;
    typedef std::shared_ptr<astro::Gfx::SkeletalAnimation> asSkAnim;
    static std::function<void(asSkAnim &anim, const aiAnimation *aiAnim, const aiNode *node)> process = [&](asSkAnim &anim, const aiAnimation *aiAnim, const aiNode *node){
        std::string nodeName(node->mName.data);
        for(int i = 0; i < aiAnim->mNumChannels; ++i){
            const aiNodeAnim* pNodeAnim = aiAnim->mChannels[i];
            if (std::string(pNodeAnim->mNodeName.data) == nodeName){
                // rotation
                for(unsigned int j = 0; j < pNodeAnim->mNumRotationKeys; ++j){
                    astro::Gfx::SkeletalFrameRotation rot;
                    auto airot = pNodeAnim->mRotationKeys[j];
                    rot.time = (float)airot.mTime;
                    rot.rotation = astro::Vec4<float>(airot.mValue.x, airot.mValue.y, airot.mValue.z, airot.mValue.w);
                    anim->rotations.push_back(rot);
                }            
                // scaling
                for (unsigned int j = 0; j < pNodeAnim->mNumScalingKeys; ++j) {
                    astro::Gfx::SkeletalFrameScaling scaling;
                    auto aiscaling = pNodeAnim->mScalingKeys[j];
                    scaling.time = aiscaling.mTime;
                    scaling.scaling = astro::Vec3<float>(aiscaling.mValue.x, aiscaling.mValue.y, aiscaling.mValue.z);
                    anim->scalings.push_back(scaling);
                }            
                // translation 
                for (unsigned int j = 0; j < pNodeAnim->mNumPositionKeys; ++j) {
                    astro::Gfx::SkeletalFrameTranslation trans;
                    auto aitrans = pNodeAnim->mPositionKeys[j];
                    trans.time = aitrans.mTime;
                    trans.translation = astro::Vec3<float>(aitrans.mValue.x, aitrans.mValue.y, aitrans.mValue.z);
                    anim->translations.push_back(trans);
                } 
            } 
        }
        // process children
        for(unsigned int i = 0; i < node->mNumChildren; ++i){
            process(anim, aiAnim, node->mChildren[i]);
        }
    };

    for(unsigned int i = 0; i < nanim; ++i){
        auto anim = std::make_shared<astro::Gfx::SkeletalAnimation>(astro::Gfx::SkeletalAnimation());
        anim->name = std::string(scene->mAnimations[i]->mName.data);
        anim->ticksPerSecond = scene->mAnimations[i]->mTicksPerSecond;
        anim->duration = scene->mAnimations[i]->mDuration;
        process(anim, scene->mAnimations[i], scene->mRootNode);
        model->animations[anim->name] = anim;
    }

    if(nanim > 0){
        model->currentAnim = model->animations.begin()->second;
    }
}


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

        initScene(scene, this);

        auto rscmngr = Core::getResourceMngr();

        // create expect list
        std::vector<std::shared_ptr<astro::Result>> results;
        for(int i = 0; i < this->texDeps.size(); ++i){
            auto &tex = this->texDeps[i];
            results.push_back(rscmngr->load(tex.file, std::make_shared<astro::Gfx::Texture>(astro::Gfx::Texture())));
        }

        // expect all textures to load
        astro::expect(results, [&, result,rscmngr, results](astro::Job &ctx){
            if(!ctx.succDeps){
                std::string messages = "";
                for(int i =0; i < ctx.listeners.size(); ++i){
                    messages += astro::String::format("%s%s ", results[i]->msg.c_str(), (i < ctx.listeners.size()-1 ? "," : ""));
                }
                astro::log("Model::load: warning: not all textures were loaded succesfully: %s\n", messages.c_str());
            }

            for(int i = 0; i < this->texDeps.size(); ++i){
                auto &tex = this->texDeps[i];
                if(tex.file.get() == NULL){
                    continue;
                }
                auto texture = rscmngr->findByName(tex.file->fname);
                this->transform->textures.push_back(BindTexture(std::static_pointer_cast<astro::Gfx::Texture>(texture), tex.role));
            }

            auto jgfx = astro::findJob({"astro_gfx"});
            if(jgfx.get() == NULL){
                result->setFailure(astro::String::format("gfx job not found: cannot load model '%s'", file->fname.c_str()));
                return;
            }

            // load meshes to gpu
            jgfx->addBacklog([&, result](astro::Job &ctx){
                auto ren = astro::Gfx::getRenderEngine();
                for(int i = 0; i < meshes.size(); ++i){
                    auto meshres = ren->generateMesh(meshes[i]->vertices, meshes[i]->indices, this->transform->useBones);
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

void astro::Gfx::Model::updateAnim(float time){

    auto &anim = this->currentAnim;
	float tps = time * anim->ticksPerSecond;
	float animTime = std::fmod(tps, anim->duration);    
    std::vector<astro::Mat<4, 4, float>> transforms;

    static auto interpRotation = [&](){
        auto rot = astro::Vec4<float>();


        return rot;
    };

    static auto interpTrans = [&](){
        auto trans = astro::Vec3<float>();


        return trans;
    };

    static auto interpScaling = [&](){
        auto scaling = astro::Vec3<float>();


        return scaling;
    };        

}

void astro::Gfx::Model::render(){
    auto ren = astro::Gfx::getRenderEngine();

    // this->skeleton->update();
    // transform->shAttrs["bones[0]"] = std::make_shared<astro::Gfx::ShaderAttrMat4>(astro::Gfx::ShaderAttrMat4(this->skeleton->boneMats[0], "bones[0]"));

    for(int i = 0; i < meshes.size(); ++i){
        meshes[i]->transform = this->transform;
        ren->renderMesh(meshes[i].get());
    }
}

void astro::Gfx::Mesh::render(){
    auto ren = astro::Gfx::getRenderEngine();
    ren->renderMesh(this);
}