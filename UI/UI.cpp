#include <GLFW/glfw3.h>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <imgui/imgui_console.h>

#include <Window/Window.hpp>

#include <UI/UI.hpp>

#include <Renderer/Renderer.hpp>

#include <Scene/Camera.hpp>

static csys::ItemLog& operator<<(csys::ItemLog& log, ImVector<float>& vec) {
	if (!vec.size())
		return log << "vector<f32> {}";
	log << "vector<f32> { ";
	for (int i = 0; i < vec.size() - 1; i++)
		log << vec[i] << ", ";
	return log << vec[vec.size() - 1] << " }";
}

/*static void vec_setter(ImVector<float>& v, std::vector<float> in) {
	v.reserve(in.size());
	std::memcpy(v.Data, in.data(), sizeof(float) * in.size());
}*/

UI::UI(Renderer* ren) : ren(ren), dev(ren->dev) {
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
		console->System().Log(csys::ItemType::eINFO) << "Paused: " << (this->ren->paused? "True" : "False") << csys::endl;
	});
	
	console->System().RegisterCommand("quit", "Quits the engine", [this]() {
		this->ren->should_close = true;
		console->System().Log(csys::ItemType::eINFO) << "Quitting..." << csys::endl;
	});

	console->System().RegisterCommand("list-vars", "List variables accessible from developer console", [this]() {
		const std::vector<std::string> names = {
			"show_bboxes",
			"visibility_testing",
			"speed",
			"flycam",
		};

		for(const auto& name : names)
			console->System().Log(csys::ItemType::eINFO) << name << csys::endl;
	});

	console->System().RegisterVariable("show_bboxes", ren->show_bboxes, csys::Arg<bool>("value"));
	console->System().RegisterVariable("visibility_testing", ren->visibility_testing, csys::Arg<bool>("value"));
	console->System().RegisterVariable("speed", ren->speed, csys::Arg<float>("value"));
	console->System().RegisterVariable("flycam", ren->flycam, csys::Arg<bool>("value"));

	console->System().Log(csys::ItemType::eINFO) << "Welcome to Pleascach!" << csys::endl;
}

void UI::newFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::Begin("Rendering Info", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoFocusOnAppearing);

	ImGui::Text("# of Indices: %zu", ren->n_indices);
	ImGui::Text("FPS: %f", ren->fps);
	ImGui::Text("Time: %f", ren->time);

	if(ren->in_menu)
		console->Draw();

	ImGui::End();
}

void UI::render(vk::CommandBuffer cmd) {
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}

UI::~UI() {
	dev.destroyDescriptorPool(desc_pool);
	
	console.reset();
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}