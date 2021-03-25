#include <cmath>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../common/Type.hpp"
#include "../common/Result.hpp"
#include "../Core.hpp"
#include "Model.hpp"

static astro::Mat<4,4, float> aiMat2Astro(const aiMatrix4x4 &mat){
    astro::Mat<4,4, float> result;

    result.mat[0 + 0*4] = mat.a1; result.mat[0 + 1*4] = mat.a2; result.mat[0 + 2*4] = mat.a3; result.mat[0 + 3*4] = mat.a4;
    result.mat[1 + 0*4] = mat.b1; result.mat[1 + 1*4] = mat.b2; result.mat[1 + 2*4] = mat.b3; result.mat[1 + 3*4] = mat.b4;
    result.mat[2 + 0*4] = mat.c1; result.mat[2 + 1*4] = mat.c2; result.mat[2 + 2*4] = mat.c3; result.mat[2 + 3*4] = mat.c4;
    result.mat[3 + 0*4] = mat.d1; result.mat[3 + 1*4] = mat.d2; result.mat[3 + 2*4] = mat.d3; result.mat[3 + 3*4] = mat.d4;

    return result;
}

static astro::Mat<4,4, float> aiMat2Astro(const aiMatrix3x3 &mat){
    astro::Mat<4,4, float> result;

    result.mat[0 + 0*4] = mat.a1; result.mat[0 + 1*4] = mat.a2; result.mat[0 + 2*4] = mat.a3; result.mat[0 + 3*4] = 0.0f;
    result.mat[1 + 0*4] = mat.b1; result.mat[1 + 1*4] = mat.b2; result.mat[1 + 2*4] = mat.b3; result.mat[1 + 3*4] = 0.0f;
    result.mat[2 + 0*4] = mat.c1; result.mat[2 + 1*4] = mat.c2; result.mat[2 + 2*4] = mat.c3; result.mat[2 + 3*4] = 0.0f;
    result.mat[3 + 0*4] = 0.0f; result.mat[3 + 1*4] = 0.0f; result.mat[3 + 2*4] = 0.0f; result.mat[3 + 3*4] = 1.0f;

    return result;
}

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

    model->gInvTrans = aiMat2Astro(scene->mRootNode->mTransformation).inverse();  

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

    for(int i = 0; i < mesh->mNumBones; ++i){
        auto aiBone = mesh->mBones[i];
        std::string name(aiBone->mName.data);
        int boneIndex = model->boneInfo.size();
        
        if(model->boneMapping.find(name) == model->boneMapping.end()){
            // map index
            model->boneMapping[name] = boneIndex;
            // add bone info
            astro::Gfx::BoneInfo boneInfo;
            model->boneInfo.push_back(boneInfo);   
        }else{
            boneIndex = model->boneMapping[name];      
        }       

        model->boneInfo[boneIndex].offset = aiMat2Astro(aiBone->mOffsetMatrix);
        model->boneInfo[boneIndex].name = name;    

        // add weights
        for(int j = 0; j < aiBone->mNumWeights; ++j){
            unsigned int vertId = aiBone->mWeights[j].mVertexId;
            float weight = aiBone->mWeights[j].mWeight;
            amesh->vertices[vertId].setBoneData(boneIndex, weight);
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




    // TODO: this can be parallelized

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

    std::unordered_map<std::string, std::shared_ptr<astro::Gfx::SkeletalHierarchy>> skelHierarchy;
    typedef std::shared_ptr<astro::Gfx::SkeletalAnimation> asSkAnim;
    typedef std::shared_ptr<astro::Gfx::SkeletalHierarchy> asSkHier;

    std::function<asSkHier(const aiNode *node, const asSkHier &parent)> readHierarchy = [&](const aiNode *node, const asSkHier &parent){
        std::string nodeName(node->mName.data);
        // add to hierarchy
        auto current = std::make_shared<astro::Gfx::SkeletalHierarchy>(astro::Gfx::SkeletalHierarchy());
        current->mat = aiMat2Astro(node->mTransformation);
        current->parent = parent;
        current->name = nodeName;
        skelHierarchy[nodeName] = current;
        // iterate children
        for(int i = 0; i < node->mNumChildren; ++i){
            current->children.push_back(readHierarchy(node->mChildren[i], current));
        }
        return current;
    };

    std::function<void(const aiNode *node, const aiAnimation *aiAnim, asSkAnim &anim)> readAnimations = [&](const aiNode *node, const aiAnimation *aiAnim, asSkAnim &anim){
        std::string nodeName(node->mName.data);
        // find channels
        for(int i = 0; i < aiAnim->mNumChannels; ++i){
            const aiNodeAnim* pNodeAnim = aiAnim->mChannels[i]; 
            if(nodeName != std::string(pNodeAnim->mNodeName.data)){
                continue;
            }
            auto fbone = anim->bones.find(nodeName);
            if(fbone == anim->bones.end()){
                auto animbone = std::make_shared<astro::Gfx::Bone>(astro::Gfx::Bone());
                animbone->name = nodeName;           
                anim->bones[nodeName] = animbone; 
                fbone = anim->bones.find(nodeName);

            }
            if(fbone != anim->bones.end()){
                // rotation
                for(unsigned int j = 0; j < pNodeAnim->mNumRotationKeys; ++j){
                    astro::Gfx::SkeletalFrameRotation rot; 
                    auto airot = pNodeAnim->mRotationKeys[j];
                    rot.time = (float)airot.mTime;
                    rot.rotation = astro::Vec4<float>(airot.mValue.x, airot.mValue.y, airot.mValue.z, airot.mValue.w);
                    fbone->second->rotations.push_back(rot);
                }            
                // scaling
                for (unsigned int j = 0; j < pNodeAnim->mNumScalingKeys; ++j) {
                    astro::Gfx::SkeletalFrameScaling scaling;
                    auto aiscaling = pNodeAnim->mScalingKeys[j];
                    scaling.time = (float)aiscaling.mTime;
                    scaling.scaling = astro::Vec3<float>(aiscaling.mValue.x, aiscaling.mValue.y, aiscaling.mValue.z);
                    fbone->second->scalings.push_back(scaling);
                }            
                // translation 
                for (unsigned int j = 0; j < pNodeAnim->mNumPositionKeys; ++j) {
                    astro::Gfx::SkeletalFrameTranslation trans;
                    auto aitrans = pNodeAnim->mPositionKeys[j];
                    trans.time = (float)aitrans.mTime;
                    trans.translation = astro::Vec3<float>(aitrans.mValue.x, aitrans.mValue.y, aitrans.mValue.z);
                    fbone->second->translations.push_back(trans);
                }                 
            }
        }
        // iterate children
        for(int i = 0; i < node->mNumChildren; ++i){
            readAnimations(node->mChildren[i], aiAnim, anim);
        }
    };

    // build hierarchy
    model->skeletonRoot = readHierarchy(scene->mRootNode, asSkHier(NULL));
    model->skeleton = skelHierarchy;

    // read animations
    for(unsigned int i = 0; i < nanim; ++i){
        auto anim = std::make_shared<astro::Gfx::SkeletalAnimation>(astro::Gfx::SkeletalAnimation());
        anim->name = std::string(scene->mAnimations[i]->mName.data);
        anim->ticksPerSecond = scene->mAnimations[i]->mTicksPerSecond;
        anim->duration = scene->mAnimations[i]->mDuration;
        // init bones for this animation
        for(int i = 0; i < model->boneInfo.size(); ++i){
            auto &bi = model->boneInfo[i];
            auto bone = std::make_shared<astro::Gfx::Bone>(astro::Gfx::Bone());
            bone->name = bi.name;        
            anim->bones[bi.name] = bone; 
        }
        // read anim keys
        readAnimations(scene->mRootNode, scene->mAnimations[i], anim);
        model->animations[anim->name] = anim;
    }

    // asign default animation (if any)
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
        const aiScene *scene = import.ReadFile(file->path.c_str(),
        		aiProcess_JoinIdenticalVertices |
                aiProcess_SortByPType | 
                aiProcess_Triangulate | 
                aiProcess_GenSmoothNormals | 
                aiProcess_FlipUVs |
                aiProcess_LimitBoneWeights);	

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

    static auto findRotation = [&](const std::shared_ptr<astro::Gfx::Bone> &bone, float animTime){
        
        for(int i = 0; i < bone->rotations.size() - 1; i++){
            if(animTime < bone->rotations[i + 1].time){
                return i;
            }
        }
        return 0;
    };

    static auto findTrans = [&](const std::shared_ptr<astro::Gfx::Bone> &bone, float animTime){
        for(int i = 0; i < bone->translations.size() - 1; i++){
            if(animTime < bone->translations[i + 1].time){
                return i;
            }
        }
        return 0;
    };   

    static auto interpRotation = [&](const std::shared_ptr<astro::Gfx::Bone> &bone){
        if(bone->rotations.size() == 1){
            auto start = aiQuaternion(bone->rotations[0].rotation.w, bone->rotations[0].rotation.x, bone->rotations[0].rotation.y, bone->rotations[0].rotation.z);
            return aiMat2Astro(start.GetMatrix());
        }

        int current = findRotation(bone, animTime);
        int next = current + 1;

        float dt = bone->rotations[next].time - bone->rotations[current].time;
        float factor = (animTime - bone->rotations[current].time) / dt;

        auto start = aiQuaternion(bone->rotations[current].rotation.w, bone->rotations[current].rotation.x, bone->rotations[current].rotation.y, bone->rotations[current].rotation.z);
        auto end = aiQuaternion(bone->rotations[next].rotation.w, bone->rotations[next].rotation.x, bone->rotations[next].rotation.y, bone->rotations[next].rotation.z);

        aiQuaternion result;
        aiQuaternion::Interpolate(result, start, end, factor);
        result = result.Normalize();

        return aiMat2Astro(result.GetMatrix());
    };

    static auto interpTrans = [&](const std::shared_ptr<astro::Gfx::Bone> &bone){
        if(bone->translations.size() == 1){
            return astro::MAT4Identity.translate(bone->translations[0].translation);        
        }

        int current = findTrans(bone, animTime);
        int next = current + 1;
        
        float dt = bone->translations[next].time - bone->translations[current].time;
        float factor = (animTime - bone->translations[current].time) / dt;

        const aiVector3D start = aiVector3D(bone->translations[current].translation.x, bone->translations[current].translation.y, bone->translations[current].translation.z);
        const aiVector3D end = aiVector3D(bone->translations[next].translation.x, bone->translations[next].translation.y, bone->translations[next].translation.z);
        aiVector3D delta = end - start;
        auto result = start + factor * delta;

        return astro::MAT4Identity.translate(astro::Vec3<float>(result.x, result.y, result.z));
    };
    
    //  calc
    std::unordered_map<std::string, astro::Mat<4, 4, float>> builtSkel;
    typedef std::function<void(const std::shared_ptr<astro::Gfx::SkeletalHierarchy> &top, const astro::Mat<4, 4, float> &parent)> bCHType;
    static bCHType buildCurrentHirarchy = [&](const std::shared_ptr<astro::Gfx::SkeletalHierarchy> &top, const astro::Mat<4, 4, float> &parent){
        auto node = top->mat;

        auto fbone = anim->bones.find(top->name);
        if(fbone != anim->bones.end() && fbone->second->rotations.size() > 0 && fbone->second->translations.size() > 0){
            auto &bone = fbone->second;
            auto trans = interpTrans(bone);
            auto rot = interpRotation(bone);
            node = trans * rot;
        }

        auto transform =  parent * node;

        builtSkel[top->name] = transform;
        for(int i = 0; i < top->children.size(); ++i){
            buildCurrentHirarchy(top->children[i], transform);
        }
    };

    buildCurrentHirarchy(skeletonRoot, astro::MAT4Identity);

    for(int i = 0; i < boneInfo.size(); ++i){
        auto &bi = boneInfo[i];

        // build skeleton based on current animation
        astro::Mat<4, 4, float> globalTrans = builtSkel[bi.name];

        // final transformation
        bi.transf = gInvTrans * globalTrans * bi.offset;
    }
}

void astro::Gfx::Model::render(){
    auto ren = astro::Gfx::getRenderEngine();

    updateAnim(ren->currentTime);

    for(int i = 0; i < boneInfo.size(); ++i){
        std::string name = astro::String::format("gBones[%i]", i);
        this->transform->shAttrs[name] = std::make_shared<astro::Gfx::ShaderAttrMat4>(boneInfo[i].transf);
    }
    for(int i = 0; i < meshes.size(); ++i){
        meshes[i]->transform = this->transform;
        ren->renderMesh(meshes[i].get());
    }
}

void astro::Gfx::Mesh::render(){
    auto ren = astro::Gfx::getRenderEngine();
    ren->renderMesh(this);
}