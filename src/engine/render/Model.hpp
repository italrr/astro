#ifndef ASTRO_RENDER_MODEL_HPP
    #define ASTRO_RENDER_MODEL_HPP

    #include "Texture.hpp"
    #include "../Graphics.hpp"

    namespace astro {
        namespace Gfx {

            struct SkeletalFrameRotation {
                astro::Vec4<float> rotation;
                float time;
            };

            struct SkeletalFrameTranslation {
                astro::Vec3<float> translation;
                float time;
            };

            struct SkeletalFrameScaling {
                astro::Vec3<float> scaling;
                float time;
            };                        

            struct Bone {
                std::string name;
                unsigned int index;
                std::vector<astro::Gfx::SkeletalFrameRotation> rotations;
                std::vector<astro::Gfx::SkeletalFrameTranslation> translations;
                std::vector<astro::Gfx::SkeletalFrameScaling> scalings;   
            };

            struct SkeletalHierarchy {
                std::string name;
                astro::Mat<4, 4, float> mat;
                std::shared_ptr<astro::Gfx::SkeletalHierarchy> parent;
                std::vector<std::shared_ptr<astro::Gfx::SkeletalHierarchy>> children;
                astro::Mat<4, 4, float> getTransform(){
                    auto b = this->parent.get();
                    std::vector<astro::Mat<4, 4, float>> mats;
                    while(b != NULL){
                        mats.push_back(b->mat);
                        b = b->parent.get();
                    }
                    astro::Mat<4, 4, float> total = MAT4Identity;
                    for(int i = 0; i < mats.size(); ++i){
                        total = total * mats[mats.size() - 1 - i];
                    }
                    return total;
                }
                SkeletalHierarchy(){
                    parent = std::shared_ptr<astro::Gfx::SkeletalHierarchy>(NULL);
                }
            };

            struct BoneInfo {
                std::shared_ptr<astro::Gfx::SkeletalHierarchy> skeleton;
                astro::Mat<4,4, float> transf;
                astro::Mat<4,4, float> offset;
                std::string name;
            };        

            struct SkeletalAnimation {
                std::unordered_map<std::string, std::shared_ptr<astro::Gfx::Bone>> bones;
                std::vector<astro::Mat<4, 4, float>> hierarchy;
                std::string name;
                float ticksPerSecond;
                float duration;
            };

            struct Mesh : astro::Gfx::RenderObject {
                unsigned int vao;
                unsigned int vbo;
                unsigned int ebo;      
                unsigned int mIndex;
                unsigned int nIndices;
                unsigned int bVertex;
                unsigned int bIndex;
                std::vector<astro::Gfx::Vertex> vertices;
                std::vector<unsigned int> indices; 
                Mesh(){
                    rObjtype = RenderObjectType::MESH;
                    vao = 0;
                    vbo = 0;
                    ebo = 0;
                    mIndex = 0;
                    nIndices = 0;
                    bVertex = 0;
                    bIndex = 0;                    
                }
                void render();
            };

            struct TextureDependency {
                std::shared_ptr<astro::Indexing::Index> file;
                int role;
            };

            struct Model : astro::Resource::Resource, astro::Gfx::RenderObject {
                std::vector<BoneInfo> boneInfo;   
                std::unordered_map<std::string, std::shared_ptr<astro::Gfx::SkeletalAnimation>> animations;
                std::shared_ptr<astro::Gfx::SkeletalAnimation> currentAnim;
                std::unordered_map<std::string, unsigned int> boneMapping;
                std::vector<astro::Gfx::TextureDependency>  texDeps; // quick ref to the expected texture files
                astro::Mat<4,4, float> gInvTrans;
                std::unordered_map<std::string, std::shared_ptr<astro::Gfx::SkeletalHierarchy>> skeleton;
                std::shared_ptr<astro::Gfx::SkeletalHierarchy> skeletonRoot;
                unsigned int nBones;
                std::vector<std::shared_ptr<astro::Gfx::Mesh>> meshes;       
                std::shared_ptr<astro::Result> unload();
                std::shared_ptr<astro::Result> load(const std::shared_ptr<astro::Indexing::Index> &file);
                void updateAnim(float time);
                void render();
                Model(){
                    nBones = 0;
                    rObjtype = astro::Gfx::RenderObjectType::MODEL;
                    rscType = astro::Resource::ResourceType::MODEL;
                }                       
            };        

        }
    }

#endif