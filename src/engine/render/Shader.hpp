#ifndef ASTRO_RENDER_SHADER_HPP
    #define ASTRO_RENDER_SHADER_HPP

    #include "../common/Type.hpp"
    #include "../common/Resource.hpp"

    namespace astro {
        namespace Gfx {

            namespace ShaderAttrType {
                enum ShaderAttr : int {
                    VEC2,
                    VEC3,
                    COLOR,
                    FLOAT,
                    INT,
                    MAT4, 
                    NONE
                };
            }

            struct ShaderAttr {
                int type;
                std::string name;
                ShaderAttr(){
                    this->type = ShaderAttrType::NONE;
                }
                ShaderAttr(const std::string &name){
                    this->type = ShaderAttrType::NONE;
                    this->name = name;
                }                
                void set(const std::string &name){
                    this->name = name;
                }
            };

            struct ShaderAttrVec2 : ShaderAttr {
                astro::Vec2<float> vec;
                ShaderAttrVec2(){
                    this->type = ShaderAttrType::VEC2;
                }
                ShaderAttrVec2(const astro::Vec2<float> &vec, const std::string &name = ""){
                    this->type = ShaderAttrType::VEC2;
                    this->vec = vec;
                    if(name.length() > 0) this->name = name;
                }                
                void set(const astro::Vec2<float> &vec){
                    this->vec = vec;
                }
            };

            struct ShaderAttrVec3 : ShaderAttr {
                astro::Vec3<float> vec;
                ShaderAttrVec3(){
                    this->type = ShaderAttrType::VEC3;
                }
                ShaderAttrVec3(const astro::Vec3<float> &vec, const std::string &name = ""){
                    this->type = ShaderAttrType::VEC3;
                    this->vec = vec;
                    if(name.length() > 0) this->name = name;
                }                
                void set(const astro::Vec3<float> &vec){
                    this->vec = vec;
                }
            };                    


            struct ShaderAttrFloat : ShaderAttr {
                float n;
                ShaderAttrFloat(){
                    this->type = ShaderAttrType::FLOAT;
                }
                ShaderAttrFloat(float n, const std::string &name = ""){
                    this->type = ShaderAttrType::FLOAT;
                    this->n = n;
                    if(name.length() > 0) this->name = name;
                }                
                void set(float n){
                    this->n = n;
                }
            };    

            struct ShaderAttrInt : ShaderAttr {
                int n;
                ShaderAttrInt(){
                    this->type = ShaderAttrType::INT;
                }
                ShaderAttrInt(int n, const std::string &name = ""){
                    this->type = ShaderAttrType::INT;
                    this->n = n;
                    if(name.length() > 0) this->name = name;
                }                
                void set(int n){
                    this->n = n;
                }
            };   

            struct ShaderAttrColor : ShaderAttr {
                astro::Color color;
                ShaderAttrColor(){
                    this->type = ShaderAttrType::COLOR;
                }
                ShaderAttrColor(const astro::Color &color, const std::string &name = ""){
                    this->type = ShaderAttrType::COLOR;
                    this->color = color;
                    if(name.length() > 0) this->name = name;
                }                
                void set(const astro::Color &color){
                    this->color = color;
                }
            };   

            struct ShaderAttrMat4 : ShaderAttr {
                astro::Mat<4, 4, float> mat;
                int n;
                ShaderAttrMat4(){
                    this->type = ShaderAttrType::MAT4;
                }
                ShaderAttrMat4(const astro::Mat<4, 4, float> &mat, const std::string &name = ""){
                    this->type = ShaderAttrType::MAT4;
                    this->mat = mat;
                    if(name.length() > 0) this->name = name;
                }                
                void set(const astro::Mat<4, 4, float> &mat){
                    this->mat = mat;
                }
            };                                    

            struct Shader : astro::Resource::Resource {
                std::string vertSrc;
                std::string fragSrc;
                int shaderId;
                Shader(){
                    rscType = astro::Resource::ResourceType::SHADER;
                    shaderId = 0;
                }
                std::shared_ptr<astro::Result> unload();
                std::shared_ptr<astro::Result> load(const std::shared_ptr<astro::Indexing::Index> &file);
            };

        }
    }

#endif