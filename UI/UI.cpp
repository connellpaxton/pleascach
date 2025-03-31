#include <GLFW/glfw3.h>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <imgui/imgui_console.h>

#include <Window/Window.hpp>

#include <UI/UI.hpp>

#include <Renderer/Renderer.hpp>

#include <Scene/Camera.hpp>
#include <Scene/March.hpp>

/* this pains me to do, but its the only way :( */
Renderer* __ren;

static csys::ItemLog& operator<<(csys::ItemLog& log, ImVector<float>& vec) {
	if (!vec.size())
		return log << "vector<f32> {}";
	log << "vector<f32> { ";
	for (int i = 0; i < vec.size() - 1; i++)
		log << vec[i] << ", ";
	return log << vec[vec.size() - 1] << " }";
}

static void scene_load(csys::String fname) {
	__ren->objects = March::readFile(fname.m_String, __ren->scene_map);

	__ren->shader_buffer->upload(__ren->objects);
}

static void scene_write(csys::String fname) {
	March::writeFile(fname.m_String, __ren->objects, __ren->scene_map);
}

static void add_sphere() {

}

UI::UI(Renderer* ren) : ren(ren) {
	__ren = ren;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	/* just allocate a ton in case */
	std::vector<vk::DescriptorPoolSize> pool_sizes = {
		{ vk::DescriptorType::eSampler, 1024 },
		{ vk::DescriptorType::eCombinedImageSampler, 1024 },
		{ vk::DescriptorType::eSampledImage, 1024 },
		{ vk::DescriptorType::eStorageBuffer, 1024 },
		{ vk::DescriptorType::eUniformTexelBuffer, 1024 },
		{ vk::DescriptorType::eStorageTexelBuffer, 1024 },
		{ vk::DescriptorType::eUniformBuffer, 1024 },
		{ vk::DescriptorType::eStorageBuffer, 1024 },
		{ vk::DescriptorType::eUniformBufferDynamic, 1024 },
		{ vk::DescriptorType::eStorageBufferDynamic, 1024 },
		{ vk::DescriptorType::eInputAttachment, 1024 },
	};

	desc_pool = ren->dev.createDescriptorPool(vk::DescriptorPoolCreateInfo {
		.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			.maxSets = 1024,
			.poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
			.pPoolSizes = pool_sizes.data(),
	});
	
	ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)(ren->win.win), true);

	auto imgui_info = ImGui_ImplVulkan_InitInfo {
		.Instance = ren->instance,
		.PhysicalDevice = ren->phys_dev,
		.Device = ren->dev,
		.Queue = ren->queue,
		.DescriptorPool = desc_pool,
		.MinImageCount = 2,
		.ImageCount = static_cast<uint32_t>(ren->swapchain->images.size()),
		.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
	};

	ImGui_ImplVulkan_Init(&imgui_info, ren->render_pass->render_pass);

	CommandBuffer imgui_cmd_buffer(ren->dev, ren->queue_family);
	imgui_cmd_buffer.command_buffer.reset();

	imgui_cmd_buffer.begin();
	ImGui_ImplVulkan_CreateFontsTexture(imgui_cmd_buffer.command_buffer);

	imgui_cmd_buffer.end();

	auto imgui_fence = ren->dev.createFence(vk::FenceCreateInfo {
		.flags = vk::FenceCreateFlagBits::eSignaled,
	});

	ren->dev.resetFences(imgui_fence);

	ren->queue.submit(vk::SubmitInfo {
		.commandBufferCount = 1,
		.pCommandBuffers = &imgui_cmd_buffer.command_buffer,
	}, imgui_fence);

	(void)ren->dev.waitForFences(imgui_fence, vk::True, UINT64_MAX);

	ren->dev.destroyFence(imgui_fence);

	imgui_cmd_buffer.cleanup(ren->dev);

	ImGui_ImplVulkan_DestroyFontUploadObjects();
	ImGui::StyleColorsDark();

	/* set up input so we can use the keyboard */
	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	console = std::make_unique<ImGuiConsole>("developer console");
	console->System().RegisterCommand("pause", "Pauses or unpauses the engine", [this]() {
		this->ren->paused = !this->ren->paused;
		console->System().Log(csys::ItemType::eINFO) << "Paused: " << (this->ren->paused ? "True" : "False") << csys::endl;
	});

	console->System().RegisterCommand("quit", "Quits the engine", [this]() {
		this->ren->should_close = true;
		console->System().Log(csys::ItemType::eINFO) << "Quitting..." << csys::endl;
	});

	console->System().RegisterCommand("read_scene", "Adds elements from a scene file", [this](csys::String fname) {
		console->System().Log(csys::ItemType::eINFO) << "Loading file: " << fname.m_String << csys::endl;
		scene_load(fname);
	}, csys::Arg<csys::String>("file-name"));
	console->System().RegisterCommand("write_scene", "Writes elements to a scene file (destroys underlying data if present)", [this](csys::String fname) {
		Log::info("Writing to " + fname.m_String + "\n");
		console->System().Log(csys::ItemType::eINFO) << "Writing to file: " << fname.m_String << "\n";
		scene_write(fname);
	}, csys::Arg<csys::String>("file-name"));

	console->System().RegisterCommand("add_sphere", "Adds sphere to the scene.", [this](csys::String name, float cx, float cy, float cz, float rad) {
		March::add_sphere(name.m_String, glm::vec3(cx, cy, cz), rad, __ren);
	}, csys::Arg<csys::String>("Name"), csys::Arg<float>("center.x"), csys::Arg<float>("center.y"), csys::Arg<float>("center.z"), csys::Arg<float>("radius"));
	
	console->System().RegisterCommand("add_box", "Adds box to the scene.", [this](csys::String name, float cx, float cy, float cz, float dx, float dy, float dz) {
		March::add_box(name.m_String, glm::vec3(cx, cy, cz), glm::vec3(dx, dy, dz), __ren);
		}, csys::Arg<csys::String>("Name"),
			csys::Arg<float>("center.x"), csys::Arg<float>("center.y"), csys::Arg<float>("center.z"),
			csys::Arg<float>("dim.x"), csys::Arg<float>("dim.y"), csys::Arg<float>("dim.z"));

	console->System().RegisterCommand("list-vars", "List variables accessible from developer console", [this]() {
		const std::vector<std::string> names = {
//			"show_bboxes",
//			"visibility_testing",
			"flycam",
			"speed",
			"max_fps",
//			"wireframe",
//			"backface_culling",
		};

		for (const auto& name : names)
			console->System().Log(csys::ItemType::eINFO) << name << csys::endl;
		});

//	console->System().RegisterVariable("show_bboxes", ren->show_bboxes, csys::Arg<bool>("value"));
//	console->System().RegisterVariable("visibility_testing", ren->visibility_testing, csys::Arg<bool>("value"));
	console->System().RegisterVariable("flycam", ren->flycam, csys::Arg<bool>("value"));
	console->System().RegisterVariable("speed", ren->speed, csys::Arg<float>("value"));
	console->System().RegisterVariable("max_fps", ren->max_fps, csys::Arg<float>("value"));
//	console->System().RegisterVariable("wireframe", ren->wireframe_mode, pipeline_setter);
//	console->System().RegisterVariable("backface_culling", ren->backface_culling, pipeline_setter);

	console->System().Log(csys::ItemType::eINFO) << "Welcome to Pleascach!" << csys::endl;
}

void UI::newFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	
	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::Begin("Rendering Info", nullptr);

	ImGui::Text("FPS: %f", ren->fps);
	ImGui::Text("Time: %f", ren->time);
	ImGui::SliderFloat("Rad", &ren->rad, 0.0, 3.0);
	ImGui::SliderFloat("Theta", &ren->cam.theta, 0.0, glm::pi<float>());
	ImGui::SliderFloat("Phi", &ren->cam.phi, 0.0, glm::two_pi<float>());
	
	ImGui::SliderFloat("X", &ren->cam.pos.x, -1000.0, 1000.0);
	ImGui::SliderFloat("Y", &ren->cam.pos.y, -1000.0, 1000.0);
	ImGui::SliderFloat("Z", &ren->cam.pos.z, -1000.0, 1000.0);


	if(ImGui::CollapsingHeader("Objects")) {
		int anon = 0;
		for (const auto& obj : ren->objects) {
			bool found = false;
			auto name = March::find_name(ren->scene_map, obj.id, found);

			if (!found || name[0] == '#') {
				name = std::string("#") + std::to_string(anon++);
			}

			ImGui::Text("(%f %f %f %f) (%f %f %f %f) \"%s\"",
					obj.center.x, obj.center.y, obj.center.z, obj.center.w,
					obj.dimensions.x, obj.dimensions.y, obj.dimensions.z, obj.dimensions.w,
					name.c_str());
		}
	}

	if (ren->in_menu)
		console->Draw();

	ImGui::End();
}

void UI::render(vk::CommandBuffer cmd) {
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}

UI::~UI() {
	console.reset();
//	dev.destroyDescriptorPool(desc_pool);
	
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}