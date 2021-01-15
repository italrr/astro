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

	//
	// Rendering thread
	//
	auto gfxthrd = astro::spawn([&](astro::Job &ctx){ // loop
		if(renderIt){
			auto gfx = astro::Gfx::getRenderEngine();
			gfx->objects.push_back(prim);
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
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	},{ 
		0, 1, 3, 
		1, 2, 3 
	}, true)->setOnSuccess([&, prim](const std::shared_ptr<astro::Result> &result){
		auto indexer = astro::Core::getIndexer();
		auto fileShader = indexer->findByName("b_primitive_f.glsl");
		auto fileTexture = indexer->findByName("wall.jpg");

        if(fileShader.get() != NULL && fileTexture.get() != NULL){
			auto rscmng = astro::Core::getResourceMngr();
            auto resultSh = rscmng->load(fileShader, std::make_shared<astro::Gfx::Shader>(astro::Gfx::Shader()));
			auto resultTex = rscmng->load(fileTexture, std::make_shared<astro::Gfx::Texture>(astro::Gfx::Texture()));

			resultSh->setOnSuccess([&, fileTexture](const std::shared_ptr<astro::Result> &result){
				astro::log("loaded texture '%s'\n", fileTexture->fname.c_str());
				
			});
			resultSh->setOnFailure([&, fileTexture](const std::shared_ptr<astro::Result> &result){
				astro::log("failed to load texture '%s'\n", result->msg.c_str());
			});


            resultTex->setOnSuccess([&, fileShader](const std::shared_ptr<astro::Result> &result){
				astro::log("loaded shader '%s'\n", fileShader->fname.c_str());
            });
            resultTex->setOnFailure([&, fileShader](const std::shared_ptr<astro::Result> &result){
                astro::log("failed to load shader '%s': '%s'\n", fileShader->fname.c_str(), result->msg.c_str());
            });



			astro::expect({resultSh, resultTex}, [&, rscmng, prim](astro::Job &ctx){

				if(!ctx.succDeps){
					astro::log("not all jobs were succesful\n");
					return;
				}

				auto rsc = rscmng->findByName("b_primitive_f.glsl");
				auto tex = rscmng->findByName("wall.jpg");


				auto shader = std::static_pointer_cast<astro::Gfx::Shader>(rsc);
				auto texture = std::static_pointer_cast<astro::Gfx::Texture>(tex);

				prim->transform->shader = shader;
				prim->transform->texture = texture;
				// prim->transform->shAttrs["p_color"] = std::make_shared<astro::Gfx::ShaderAttrColor>(astro::Gfx::ShaderAttrColor(astro::Color(1.0f, 0.0f, 1.0f, 1.0f), "p_color"));
				
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
