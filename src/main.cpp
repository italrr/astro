#include "engine/Core.hpp"
#include "engine/Graphics.hpp"
#include "engine/Input.hpp"
#include "engine/render/Model.hpp"

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

int main(int argc, const char *argv[]){
	
	astro::Core::init();
	bool started = false;


	auto model = std::make_shared<astro::Gfx::Model>(astro::Gfx::Model()); 
	auto prim = std::make_shared<astro::Gfx::RObj2DPrimitive>(astro::Gfx::RObj2DPrimitive());

	auto stt = astro::ticks();
	float c = 0;

// glm::transpose
// glm::inverse(glm::mat4() * float);
	// glm::scale


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
	pipeline.lights.push_back(lightspot);


	// auto lightdir = std::make_shared<astro::Gfx::DirLight>(astro::Gfx::DirLight());
	// lightdir->direction = astro::Vec3<float>(0.0f, 0.0f, 0.0f);
	// lightdir->ambient = astro::Vec3<float>(0.05f, 0.05f, 0.05f);
	// lightdir->diffuse = astro::Vec3<float>(0.4f, 0.4f, 0.4f);
	// lightdir->specular = astro::Vec3<float>(0.5f, 0.5f, 0.5f);
	// pipeline.lights.push_back(lightdir);


	pipeline.camera.setPosition(astro::Vec3<float>(0.0f, 0.0f, 3.0f));
	
	prim->transform->material.diffuse = 0;
	prim->transform->material.specular = 1;
	prim->transform->material.shininess = 64.0f;

	//
	// Rendering thread
	//
	auto gfxthrd = astro::spawn([&](astro::Job &ctx){ // loop
		uint64 delta = astro::ticks()-stt;
		c += 0.008f;


		model->transform->model = astro::MAT4Identity.rotate(astro::Math::rads(c), astro::Vec3<float>(0.0f, 0.5f, 0.0f)).scale(astro::Vec3<float>(0.025f, 0.025f, 0.025f)).translate(astro::Vec3<float>(0.0f, -1.5f, -5.0f));//.rotate(astro::Math::rads(c), astro::Vec3<float>(1.0f, 0.7f, 0.5f)).scale(astro::Vec3<float>(0.05f, 0.05f, 0.05f));
		// model = model.translate(astro::Vec3<float>(0.0f, 0.0f, 0.0f));


		float cameraSpeed = 0.02f; 
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
		astro::sleep(1000); 
	}

	auto indexer = astro::Core::getIndexer();

	indexer->asyncFindManyByName({"b_primitive_f.glsl", "nathan.fbx"}, [&](std::vector<std::shared_ptr<astro::Indexing::Index>> &files){
		auto fileShader = files[0];
		auto modelFile = files[1];

		auto rscmng = astro::Core::getResourceMngr();
		auto resultSh = rscmng->load(fileShader, std::make_shared<astro::Gfx::Shader>(astro::Gfx::Shader()));
		auto rmodel = rscmng->load(modelFile, model);

		rmodel->setOnSuccess([&, modelFile](const std::shared_ptr<astro::Result> &result){
			astro::log("loaded model '%s'\n", modelFile->fname.c_str());
		});
		rmodel->setOnFailure([&, modelFile](const std::shared_ptr<astro::Result> &result){
			astro::log("failed to load model '%s': '%s'\n", modelFile->fname.c_str(), result->msg.c_str());
		});


		resultSh->setOnSuccess([&, fileShader](const std::shared_ptr<astro::Result> &result){
			astro::log("loaded shader '%s'\n", fileShader->fname.c_str());
		});
		resultSh->setOnFailure([&, fileShader](const std::shared_ptr<astro::Result> &result){
			astro::log("failed to load shader '%s': '%s'\n", fileShader->fname.c_str(), result->msg.c_str());
		});


		astro::expect({rmodel, resultSh}, [&, rscmng, prim, model](astro::Job &ctx){

			if(!ctx.succDeps){
				astro::log("not all jobs were successful\n");
				return;
			}

			auto sh = rscmng->findByName("b_primitive_f.glsl");


			auto shader = std::static_pointer_cast<astro::Gfx::Shader>(sh);

			model->transform->shader = shader;

			pipeline.add(model);

		}, false);
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
