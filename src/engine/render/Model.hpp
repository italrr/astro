#ifndef ASTRO_RENDER_MODEL_HPP
    #define ASTRO_RENDER_MODEL_HPP

    #include "Texture.hpp"
    #include "../Graphics.hpp"

    namespace astro {
        namespace Gfx {

            struct BoneInfo {
                astro::Mat<4,4, float> transf;
                astro::Mat<4,4, float> offset;
            };

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

            struct SkeletalAnimation {
                std::vector<astro::Gfx::SkeletalFrameRotation> rotations;
                std::vector<astro::Gfx::SkeletalFrameTranslation> translations;
                std::vector<astro::Gfx::SkeletalFrameScaling> scalings;
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
                unsigned int nBones;      
                std::vector<astro::Gfx::Vertex> vertices;
                std::vector<unsigned int> indices; 
                Mesh(){
                    rObjtype = RenderObjectType::MESH;
                    vao = 0;
                    vbo = 0;
                    ebo = 0;
                    nBones = 0;
                    mIndex = 0;
                    nIndices = 0;
                    bVertex = 0;
                    bIndex = 0;                    
                }
                void render();
            };

            // struct Skeleton;
            // struct Bone {
            //     unsigned int id;
            //     std::string name;
            //     // std::shared_ptr<astro::Gfx::Mesh> mesh;
            //     std::shared_ptr<astro::Gfx::Bone> parent;
            //     // astro::Gfx::Skeleton *skeleton;
            //     astro::Mat<4, 4, float> offsetMat;
            //     astro::Mat<4, 4, float> trans;
            //     astro::Mat<4, 4, float> mTransformation;
            //     Bone();
            //     // Bone(const std::shared_ptr<astro::Gfx::Mesh> &mesh, unsigned int id, const std::string &name, const astro::Mat<4, 4, float> &offsetMat);
            //     astro::Mat<4, 4, float> getParentTrans();
            // };            

            // struct Skeleton {
            //     std::unordered_map<std::string, std::shared_ptr<astro::Gfx::Bone>> bones;
            //     astro::Mat<4,4, float> gInvTrans;
            //     std::vector<astro::Mat<4,4, float>> boneMats;
                
            //     std::shared_ptr<astro::Gfx::Bone> findBone(const std::string &name){
            //         auto it = bones.find(name);
            //         if(it != bones.end()){
            //             return it->second;
            //         }
            //         return std::shared_ptr<astro::Gfx::Bone>(NULL);
            //     }
                
                
            //     void init(const std::unordered_map<std::string, std::shared_ptr<astro::Gfx::Bone>> &bones, const astro::Mat<4,4, float> &gInvTrans){
            //         this->bones = bones;
            //         this->gInvTrans = gInvTrans;
            //         // for(int i = 0; i < this->bones.size(); ++i){
            //         //     this->bones[i]->skeleton = this;
            //         // }
            //     }
            //     void update(){
            //         // if(bones.size() == 0){
            //         //     return;
            //         // }
            //         // boneMats.clear();
            //         // for(int i = 0; i < 100; ++i){
            //         //     if(i > bones.size() - 1){
            //         //         boneMats.push_back(astro::Mat<4, 4, float>(MAT4Identity));
            //         //         continue;
            //         //     }
            //         //     auto conc = bones[i]->getParentTrans() * bones[i]->trans;
            //         //     boneMats.push_back(gInvTrans * conc * bones[i]->offsetMat);
            //         // }
            //     }
            // };

            struct TextureDependency {
                std::shared_ptr<astro::Indexing::Index> file;
                int role;
            };

            struct Model : astro::Resource::Resource, astro::Gfx::RenderObject {
                std::vector<BoneInfo> boneInfo;   
                std::unordered_map<std::string, std::shared_ptr<astro::Gfx::SkeletalAnimation>> animations;
                std::shared_ptr<astro::Gfx::SkeletalAnimation> currentAnim;
                std::unordered_map<std::string, unsigned int> boneMapping;
                std::vector<astro::Gfx::TextureDependency>  texDeps; // quick ref to the expect texture files
                astro::Mat<4,4, float> gInvTrans;
                std::vector<std::shared_ptr<astro::Gfx::Mesh>> meshes;       
                std::shared_ptr<astro::Result> unload();
                std::shared_ptr<astro::Result> load(const std::shared_ptr<astro::Indexing::Index> &file);
                void updateAnim(float time);
                void render();
                Model(){
                    rObjtype = astro::Gfx::RenderObjectType::MODEL;
                    rscType = astro::Resource::ResourceType::MODEL;
                }                       
            };        

        }
    }

#endif