#include "engine/Core.hpp"
#include "engine/Graphics.hpp"
#include "engine/Input.hpp"


int main(int argc, const char *argv[]){
	
	astro::Core::init();
	bool started = false;

	auto prim = std::make_shared<astro::Gfx::RObj2DPrimitive>(astro::Gfx::RObj2DPrimitive());

	auto stt = astro::ticks();
	float c = 0;


	auto pipeline = astro::Gfx::Pipeline();

	auto light = std::make_shared<astro::Gfx::PointLight>(astro::Gfx::PointLight());
	light->setPosition(astro::Vec3<float>(1.2f, 1.0f, 2.0f));
	light->ambient = astro::Vec3<float>(0.2f, 0.2f, 0.2f);
	light->diffuse = astro::Vec3<float>(0.5f, 0.5f, 0.5f);
	light->specular = astro::Vec3<float>(1.0f, 1.0f, 1.0f);
	light->constant = 1.0f;
	light->linear = 0.09f;
	light->quadratic = 0.032f;
	pipeline.lights.push_back(light);



	auto lightspot = std::make_shared<astro::Gfx::SpotLight>(astro::Gfx::SpotLight());
	lightspot->ambient = astro::Vec3<float>(0.0f, 0.0f, 0.0f);
	lightspot->diffuse = astro::Vec3<float>(1.0f, 1.0f, 1.0f);
	lightspot->specular = astro::Vec3<float>(1.0f, 1.0f, 1.0f);
	lightspot->constant = 1.0f;
	lightspot->linear = 0.09f;
	lightspot->quadratic = 0.032f;
	lightspot->cutOff = astro::Math::cos(astro::Math::rads(12.5f));
	lightspot->outerCutOff = astro::Math::cos(astro::Math::rads(15.0f));
	// pipeline.lights.push_back(lightspot);


	auto lightdir = std::make_shared<astro::Gfx::DirLight>(astro::Gfx::DirLight());
	lightdir->direction = astro::Vec3<float>(-0.2f, -1.0f, -0.3f);
	lightdir->ambient = astro::Vec3<float>(0.05f, 0.05f, 0.05f);
	lightdir->diffuse = astro::Vec3<float>(0.4f, 0.4f, 0.4f);
	lightdir->specular = astro::Vec3<float>(0.5f, 0.5f, 0.5f);
	// pipeline.lights.push_back(lightdir);


	pipeline.camera.setPosition(astro::Vec3<float>(0.0f, 0.0f, 3.0f));
	
	prim->transform->material.diffuse = 0;
	prim->transform->material.specular = 1;
	prim->transform->material.shininess = 64.0f;

	//
	// Rendering thread
	//
	auto gfxthrd = astro::spawn([&](astro::Job &ctx){ // loop
		// auto gfx = astro::Gfx::getRenderEngine();
		// gfx->objects.push_back(prim);
			
		// astro::Mat<4, 4, float> model = astro::MAT4Identity;
		// auto projection = astro::Math::perspective(astro::Math::rads(45.0f), (float)gfx->size.x / (float)gfx->size.y, 0.1f, 100.0f);


		uint64 delta = astro::ticks()-stt;

		c += 0.15f;
		


		prim->transform->model = astro::MAT4Identity.rotate(astro::Math::rads(c), astro::Vec3<float>(1.0f, 1.0f, 1.0f));
		// model = model.translate(astro::Vec3<float>(0.0f, 0.0f, 0.0f));


		float cameraSpeed = 0.2f; 
		if (astro::Input::keyboardCheck(astro::Input::Key::W))
			pipeline.camera.setPosition(pipeline.camera.position - astro::Vec3<float>(0.0f, 0.0f, 1.0f) * cameraSpeed);
		if (astro::Input::keyboardCheck(astro::Input::Key::S))
			pipeline.camera.setPosition(pipeline.camera.position + astro::Vec3<float>(0.0f, 0.0f, 1.0f) * cameraSpeed);

		if (astro::Input::keyboardCheck(astro::Input::Key::A))
			pipeline.camera.setPosition(pipeline.camera.position + astro::Vec3<float>(1.0f, 0.0f, 0.0f) * cameraSpeed);
		if (astro::Input::keyboardCheck(astro::Input::Key::D))
			pipeline.camera.setPosition(pipeline.camera.position - astro::Vec3<float>(1.0f, 0.0f, 0.0f) * cameraSpeed);			

		// if (astro::Input::keyboardCheck(astro::Input::Key::A))
		// 	pipeline.camera.position = pipeline.camera.position - pipeline.camera.front.cross(pipeline.camera.up).normalize() * cameraSpeed;

		// if (astro::Input::keyboardCheck(astro::Input::Key::D))
		// 	pipeline.camera.position = pipeline.camera.position + pipeline.camera.front.cross(pipeline.camera.up).normalize() * cameraSpeed;
		
		// pipeline.camera.lookAt(astro::Vec3<float>(0.0f, 0.0f,  -15.0f));

		pipeline.render();
		astro::Gfx::update();
		if(!astro::Gfx::isRunning()){
			ctx.stop();
		}		
	}, astro::JobSpec(true, true, true, {"astro_gfx"}));
	gfxthrd->setOnStart([&](astro::Job &ctx){ // init
		astro::Gfx::init("astro", astro::Gfx::RenderEngineType::OpenGL);
		pipeline.init(astro::Gfx::getRenderEngine(), astro::Math::rads(45.0f), 0.1f, 100.0f);
		started = true;		
	});
	gfxthrd->setOnEnd([&](astro::Job &ctx){ // end
		astro::Gfx::onEnd();
	});
	// wait for render thread to start
	while(!started){
		astro::Core::update();
		astro::sleep(100); 
	}
	prim->init({
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	}, 36, 8, true)->setOnSuccess([&, prim](const std::shared_ptr<astro::Result> &result){
		auto indexer = astro::Core::getIndexer();
		auto fileShader = indexer->findByName("b_primitive_f.glsl");
		auto fileTexShader = indexer->findByName("b_tex_primitive_f.glsl");
		auto fileTexture = indexer->findByName("container2.png");
		auto fileTexture2 = indexer->findByName("container2_specular.png");

        if(fileShader.get() != NULL && fileTexture.get() != NULL && fileTexture2.get() != NULL){
			auto rscmng = astro::Core::getResourceMngr();
            
			auto resultSh = rscmng->load(fileShader, std::make_shared<astro::Gfx::Shader>(astro::Gfx::Shader()));
			auto resultTexSh = rscmng->load(fileTexShader, std::make_shared<astro::Gfx::Shader>(astro::Gfx::Shader()));
			auto resultTex = rscmng->load(fileTexture, std::make_shared<astro::Gfx::Texture>(astro::Gfx::Texture()));
			auto resultTex2 = rscmng->load(fileTexture2, std::make_shared<astro::Gfx::Texture>(astro::Gfx::Texture()));

			resultTex->setOnSuccess([&, fileTexture](const std::shared_ptr<astro::Result> &result){
				astro::log("loaded texture '%s'\n", fileTexture->fname.c_str());
				
			});
			resultTex->setOnFailure([&, fileTexture](const std::shared_ptr<astro::Result> &result){
				astro::log("failed to load texture '%s'\n", result->msg.c_str());
			});

			resultTexSh->setOnSuccess([&, fileTexShader](const std::shared_ptr<astro::Result> &result){
				astro::log("loaded shader '%s'\n", fileTexShader->fname.c_str());
				
			});
			resultTexSh->setOnFailure([&, fileTexShader](const std::shared_ptr<astro::Result> &result){
				astro::log("failed to load shader '%s'\n", result->msg.c_str());
			});			


            resultSh->setOnSuccess([&, fileShader](const std::shared_ptr<astro::Result> &result){
				astro::log("loaded shader '%s'\n", fileShader->fname.c_str());
            });
            resultSh->setOnFailure([&, fileShader](const std::shared_ptr<astro::Result> &result){
                astro::log("failed to load shader '%s': '%s'\n", fileShader->fname.c_str(), result->msg.c_str());
            });


            resultTex2->setOnSuccess([&, fileTexture2](const std::shared_ptr<astro::Result> &result){
				astro::log("loaded texture '%s'\n", fileTexture2->fname.c_str());
            });
            resultTex2->setOnFailure([&, fileTexture2](const std::shared_ptr<astro::Result> &result){
                astro::log("failed to load texture '%s': '%s'\n", fileTexture2->fname.c_str(), result->msg.c_str());
            });


			astro::expect({resultSh, resultTex, resultTex2, resultTexSh}, [&, rscmng, prim](astro::Job &ctx){

				if(!ctx.succDeps){
					astro::log("not all jobs were successful\n");
					return;
				}

				auto sh = rscmng->findByName("b_primitive_f.glsl");
				auto shTex = rscmng->findByName("b_tex_primitive_f.glsl");
				auto tex = rscmng->findByName("container2.png");
				auto tex2 = rscmng->findByName("container2_specular.png");


				auto shader = std::static_pointer_cast<astro::Gfx::Shader>(sh);
				// auto shader = std::static_pointer_cast<astro::Gfx::Shader>(shTex);
				auto texture = std::static_pointer_cast<astro::Gfx::Texture>(tex);
				auto texture2 = std::static_pointer_cast<astro::Gfx::Texture>(tex2);

				prim->transform->shader = shader;
				prim->transform->textures.push_back(astro::Gfx::BindTexture(texture, astro::Gfx::TextureRole::NONE));
				prim->transform->textures.push_back(astro::Gfx::BindTexture(texture2, astro::Gfx::TextureRole::NONE));
				
				pipeline.add(prim);

			}, false);


				

        }

	});

	//
	// Main thread
	//
	do {
		astro::Core::update();
	} while(astro::Gfx::isRunning());

	astro::Core::onEnd();
	
	return 0;
}
