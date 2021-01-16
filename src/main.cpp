#include "engine/Core.hpp"
#include "engine/Graphics.hpp"
#include "engine/Input.hpp"


int main(int argc, const char *argv[]){
	
	astro::Core::init();
	bool started = false;

	auto prim = std::make_shared<astro::Gfx::RObj2DPrimitive>(astro::Gfx::RObj2DPrimitive());
	bool renderIt = false;

	auto setRenderIt = [&](){
		renderIt = true;
	};
	auto stt = astro::ticks();
	float c = 0;

	// camera
	auto cameraPos   = astro::Vec3<float>(0.0f, 0.0f,  3.0f);
	auto cameraFront = astro::Vec3<float>(0.0f, 0.0f, -1.0f);
	auto cameraUp    = astro::Vec3<float>(0.0f, 1.0f,  0.0f);


	//
	// Rendering thread
	//
	auto gfxthrd = astro::spawn([&](astro::Job &ctx){ // loop
		if(renderIt){
			auto gfx = astro::Gfx::getRenderEngine();
			gfx->objects.push_back(prim);
				
			astro::Mat<4, 4, float> model = astro::MAT4Identity;
			astro::Mat<4, 4, float> projection;

			projection = astro::Math::perspective(astro::Math::rads(45.0f), (float)gfx->size.x / (float)gfx->size.y, 0.1f, 100.0f);

			uint64 delta = astro::ticks()-stt;

			c += 0.15f;

			model = model.rotate(astro::Math::rads(c), astro::Vec3<float>(1.0f, 0.3f, 0.5f));
			model = model.translate(astro::Vec3<float>(0.0f, 0.0f, 0.0f));


			float cameraSpeed = 0.2f; 
			if (astro::Input::keyboardCheck(astro::Input::Key::W))
				cameraPos = cameraPos + cameraFront * cameraSpeed;
			if (astro::Input::keyboardCheck(astro::Input::Key::S))
				cameraPos = cameraPos - cameraFront * cameraSpeed;

			if (astro::Input::keyboardCheck(astro::Input::Key::A))
				cameraPos = cameraPos - cameraFront.cross(cameraUp).normalize() * cameraSpeed;

			if (astro::Input::keyboardCheck(astro::Input::Key::D))
				cameraPos = cameraPos + cameraFront.cross(cameraUp).normalize() * cameraSpeed;

			auto k = (cameraPos + cameraFront);
			auto view = astro::Math::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

			prim->transform->shAttrs["model"] = std::make_shared<astro::Gfx::ShaderAttrMat4>(astro::Gfx::ShaderAttrMat4(model, "model"));
			prim->transform->shAttrs["view"] = std::make_shared<astro::Gfx::ShaderAttrMat4>(astro::Gfx::ShaderAttrMat4(view, "view"));
			prim->transform->shAttrs["projection"] = std::make_shared<astro::Gfx::ShaderAttrMat4>(astro::Gfx::ShaderAttrMat4(projection, "projection"));

		}
		astro::Gfx::update();
		if(!astro::Gfx::isRunning()){
			ctx.stop();
		}		
	}, astro::JobSpec(true, true, true, {"astro_gfx"}));
	gfxthrd->setOnStart([&](astro::Job &ctx){ // init
		astro::Gfx::init("astro", astro::Gfx::RenderEngineType::OpenGL);
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
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
	},{ }, true)->setOnSuccess([&, prim](const std::shared_ptr<astro::Result> &result){
		auto indexer = astro::Core::getIndexer();
		auto fileShader = indexer->findByName("b_primitive_f.glsl");
		auto fileTexture = indexer->findByName("wall.jpg");
		auto fileTexture2 = indexer->findByName("wall2.jpg");

        if(fileShader.get() != NULL && fileTexture.get() != NULL && fileTexture2.get() != NULL){
			auto rscmng = astro::Core::getResourceMngr();
            
			auto resultSh = rscmng->load(fileShader, std::make_shared<astro::Gfx::Shader>(astro::Gfx::Shader()));
			auto resultTex = rscmng->load(fileTexture, std::make_shared<astro::Gfx::Texture>(astro::Gfx::Texture()));
			auto resultTex2 = rscmng->load(fileTexture2, std::make_shared<astro::Gfx::Texture>(astro::Gfx::Texture()));

			resultTex->setOnSuccess([&, fileTexture](const std::shared_ptr<astro::Result> &result){
				astro::log("loaded texture '%s'\n", fileTexture->fname.c_str());
				
			});
			resultTex->setOnFailure([&, fileTexture](const std::shared_ptr<astro::Result> &result){
				astro::log("failed to load texture '%s'\n", result->msg.c_str());
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


			astro::expect({resultSh, resultTex, resultTex2}, [&, rscmng, prim](astro::Job &ctx){

				if(!ctx.succDeps){
					astro::log("not all jobs were successful\n");
					return;
				}

				auto rsc = rscmng->findByName("b_primitive_f.glsl");
				auto tex = rscmng->findByName("wall.jpg");
				auto tex2 = rscmng->findByName("wall2.jpg");


				auto shader = std::static_pointer_cast<astro::Gfx::Shader>(rsc);
				auto texture = std::static_pointer_cast<astro::Gfx::Texture>(tex);
				auto texture2 = std::static_pointer_cast<astro::Gfx::Texture>(tex2);

				prim->transform->shader = shader;
				prim->transform->texture = texture;
				prim->transform->texture2 = texture2;
				
				setRenderIt();

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
