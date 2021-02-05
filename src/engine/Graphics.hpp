#ifndef ASTRO_GRAPHICS_HPP
	#define ASTRO_GRAPHICS_HPP

	#include <memory>

	#include "common/Type.hpp"
	#include "common/Result.hpp"
	#include "common/Tools.hpp"

	#include "render/RenderLayer.hpp"
	#include "render/Shader.hpp"
	#include "render/Texture.hpp"

	namespace astro {
		namespace Gfx {

			namespace ImageFormat {
				enum ImageFormat : int {
					RED,
					GREEN,
					BLUE,
					RGB,
					RGBA
				};
			}

			namespace LightType {
				enum ImageFormat : int {
					SPOT,
					POINT,
					DIRECTIONAL
				};
			}			

			namespace RenderEngineType {
				enum RenderEngineType : int {
					Undefined = -1,
					OpenGL = 0,
					Vulkan,
					DirectX,
					Software
				};
				static RenderEngineType value(const std::string &name){
					if(astro::String::toLower(name) == "opengl"){
						return RenderEngineType::OpenGL;
					}else
					if(astro::String::toLower(name) == "vulkan"){
						return RenderEngineType::Vulkan;
					}else
					if(astro::String::toLower(name) == "directx"){
						return RenderEngineType::DirectX;
					}else
					if(astro::String::toLower(name) == "software"){
						return RenderEngineType::Software;
					}else{
						return RenderEngineType::Undefined;
					}																	
				}
				static std::string name(int type){
					switch(type){
						case RenderEngineType::OpenGL:
							return "OpenGL";
						case RenderEngineType::Vulkan:
							return "Vulkan";
						case RenderEngineType::DirectX:
							return "DirectX";
						case RenderEngineType::Software:
							return "Software";							
						default:
							return "Undefined";
					}
				}
			}


			namespace RenderObjectType {
				enum RenderObjectType : int {
					MODEL,
					PRIMITIVE,
					MESH,
					NONE
				};
				static std::string name(int type){
					switch(type){
						case RenderObjectType::MODEL: 
							return "Model";
						case RenderObjectType::PRIMITIVE:
							return "Primitive";
						case RenderObjectType::MESH:
							return "Mesh";							
						case RenderObjectType::NONE:
							return "None";							
						default:
							return "Undefined";
					}
				}
			}

            namespace TextureRole {
                enum TextureRole : int {
                    DIFFUSE,
                    SPECULAR,
					NORMAL,
					HEIGHT,
                    NONE
                };
            }

            struct BindTexture {
                std::shared_ptr<astro::Gfx::Texture> texture;
                int role;
				BindTexture(const std::shared_ptr<astro::Gfx::Texture> &texture, int role){
					this->texture = texture;
					this->role = role;
				}
                BindTexture(){
                    this->role = TextureRole::NONE;
                }
            };

			namespace MaterialMode {
				enum MaterialMode : int {
					DIFFUSE = 1,
					SPECULAR = 2,
					NORMAL = 4,
					HEIGHT = 8,
				};
			}

			struct Material {
				int diffuse;
				int specular;
				float shininess;
				Material(){

				}
				Material(int diffuse, int specular, float shininess){
					this->diffuse = diffuse;
					this->specular = specular;
					this->shininess = shininess;
				}
			};

            struct Vertex {
                astro::Vec3<float> position;
                astro::Vec3<float> normal;
                astro::Vec2<float> texCoords;
				astro::Vec3<float> tangent;
				astro::Vec3<float> bitangent;
				// bone data
				float weight[4];
				unsigned int id[4];
            };			

			struct RenderTransform {
				int matMode;
				astro::Gfx::Material material;
				astro::Mat<4, 4, float> model;
				astro::Vec3<float> position;
				astro::Color color;
				std::shared_ptr<astro::Gfx::Shader> shader;
				std::vector<astro::Gfx::BindTexture> textures;
				std::unordered_map<std::string, std::shared_ptr<astro::Gfx::ShaderAttr>> shAttrs;
				RenderTransform(){
					this->shader = std::make_shared<astro::Gfx::Shader>(astro::Gfx::Shader());
					this->model = astro::MAT4Identity;
					this->matMode = 0;
					enMatMode(Gfx::MaterialMode::DIFFUSE);
					enMatMode(Gfx::MaterialMode::SPECULAR);
					enMatMode(Gfx::MaterialMode::NORMAL);
					enMatMode(Gfx::MaterialMode::HEIGHT);
				}
				void enMatMode(int mode){
					this->matMode = this->matMode | (1 << mode);
				}
				void disMatMode(int mode){
					this->matMode = this->matMode & (~mode);
				}
				void resetMatMode(){
					this->matMode = 0;
				}
				bool hasMatMode(int mode){
					return this->matMode & (1 << mode); 
				}
			};

			struct RenderObject {
				RenderObject(){
					rObjtype = RenderObjectType::NONE;
					transform = std::make_shared<astro::Gfx::RenderTransform>(astro::Gfx::RenderTransform());
				}
				int rObjtype;
				int rObjId;
				std::shared_ptr<astro::Gfx::RenderTransform> transform;
				virtual void render(){
					return;
				}
			};

			struct Light {
				int type;
				virtual void apply(const std::shared_ptr<RenderObject> &object){ return; }
			};

			struct PointLight : Light {
				astro::Vec3<float> position;
				float constant;
				float linear;
				float quadratic;
				astro::Vec3<float> ambient;
				astro::Vec3<float> diffuse;
				astro::Vec3<float> specular;
				PointLight(){
					this->type = LightType::POINT;
				}
				void setPosition(const astro::Vec3<float> &position){
					this->position = position;
				}
			};			

			struct DirLight : Light {

				astro::Vec3<float> direction;
				astro::Vec3<float> ambient;
				astro::Vec3<float> diffuse;
				astro::Vec3<float> specular;
				DirLight(){
					this->type = LightType::DIRECTIONAL;
				}				
			};

			struct SpotLight : Light {
				astro::Vec3<float> position;
				astro::Vec3<float> direction;
				float cutOff;
				float outerCutOff;
				float constant;
				float linear;
				float quadratic;
				astro::Vec3<float> ambient;
				astro::Vec3<float> diffuse;
				astro::Vec3<float> specular;
				SpotLight(){
					this->type = LightType::SPOT;
				}				
			};

			struct RObj2DPrimitive : RenderObject {
				std::vector<float> vertices;
				std::vector<int> indices;
				std::shared_ptr<astro::Result> init(const std::vector<float> &verts, unsigned int nverts, unsigned int strides, bool textured);
				unsigned int vbo;
				unsigned int vao;
				unsigned int nverts;
				unsigned int strides;
				void render();
				RObj2DPrimitive(){
					rObjtype = RenderObjectType::PRIMITIVE;
				}
			};

			struct RenderEngine {
				void *window;
				std::vector<std::shared_ptr<astro::Gfx::RenderLayer>> layers;
				std::vector<std::shared_ptr<astro::Gfx::RenderObject>> objects;
				RenderEngine(){
					type = RenderEngineType::Undefined;
					window = NULL;
				}
				int type;
				astro::Vec2<int> size;
				virtual std::shared_ptr<astro::Result> init(){ return astro::makeResult(ResultType::Success); }
				virtual std::shared_ptr<astro::Result> end(){ return astro::makeResult(ResultType::Success); }
				virtual std::shared_ptr<astro::Result> createWindow(const std::string &title, Vec2<int> size){ return astro::makeResult(ResultType::Success); }
				virtual std::shared_ptr<astro::Result> isSupported(){ return astro::makeResult(ResultType::Success); }
				virtual int render(){ return 0; } 
				virtual std::shared_ptr<astro::Result> compileShader(const std::string &vert, const std::string &frag){ return astro::makeResult(ResultType::Success); }
				virtual std::shared_ptr<astro::Result> deleteShader(int id){ return astro::makeResult(ResultType::Success); }
				virtual void readjustViewport(const astro::Vec2<int> &origin, const astro::Vec2<int> &size) { return; }
				virtual std::shared_ptr<astro::Gfx::RenderLayer> addRenderLayer(astro::Gfx::RenderLayerType type, const std::string &tag, int order = -1);
				// generators
				virtual std::shared_ptr<astro::Result> generatePrimVertexBuffer(
														const std::vector<float> &verts,
														unsigned int nverts,
														unsigned int strides,
														bool textured){ return astro::makeResult(ResultType::Success); }
				virtual std::shared_ptr<astro::Result> generateTexture2D(unsigned char *data, int w, int h, int format){ return astro::makeResult(ResultType::Success); }
				virtual std::shared_ptr<astro::Result> generateMesh(const std::vector<astro::Gfx::Vertex> &vertices, const std::vector<unsigned int> &indices){ return astro::makeResult(ResultType::Success); }

				// renders
				virtual bool renderPrimVertBuffer(astro::Gfx::RenderObject *obj){ return true; }
				virtual bool renderMesh(astro::Gfx::RenderObject *obj){ return true; }
			};
			
			struct Camera {
				astro::Gfx::RenderEngine *renderer;
				astro::Vec3<float> position;
				astro::Vec3<float> front;
				astro::Vec3<float> up;
				astro::Vec3<float> right;
				astro::Vec3<float> worldUp;
				float yaw;
				float pitch;
				void setPosition(const astro::Vec3<float> &pos);
				void setFront(const astro::Vec3<float> &front);
				void update();
				void setUp(const astro::Vec3<float> &up);
				void init(astro::Gfx::RenderEngine *render);
				astro::Mat<4, 4, float> getView();
			};			

			struct Pipeline {
				unsigned int lastId;
				astro::Mat<4, 4, float> projection;
				Pipeline(const Pipeline &pip);
				Pipeline();
				std::mutex accesMutex;
				astro::Gfx::RenderEngine *renderer;
				astro::Gfx::Camera camera;
				std::vector<std::shared_ptr<astro::Gfx::Light>> lights;
				std::unordered_map<int, std::shared_ptr<astro::Gfx::RenderObject>> objects;
				void init(astro::Gfx::RenderEngine *renderer, float fov, float nearp, float farp);
				void add(const std::shared_ptr<astro::Gfx::RenderObject> &obj);
				void clear();
				void remove(int id);
				void remove(const std::shared_ptr<astro::Gfx::RenderObject> &obj);
				void render();
			};

			struct RenderInitSettings {
				RenderInitSettings(){
					size = astro::Vec2<int>(0);
					title = "";
					ret = RenderEngineType::Undefined;
					resizeable = false;
					vsync = false;
					fullscreen = false;
				}
				std::string title;
				astro::Vec2<int> size;
				bool fullscreen;
				bool resizeable;
				bool vsync;
				RenderEngineType::RenderEngineType ret;
			};

			void init(const std::string &title, astro::Vec2<int> size = astro::Vec2<int>(0));
			void init(const std::string &title, RenderEngineType::RenderEngineType ret = RenderEngineType::Undefined);
			void init(const std::string &title, RenderInitSettings &sett);
			void init(const std::string &title);
			void init(RenderInitSettings &sett);
			void update();
			bool isRunning();
			void onEnd();
			void end();
			
			RenderEngine *getRenderEngine();

		}
	}

#endif