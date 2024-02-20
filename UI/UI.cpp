#include <GLFW/glfw3.h>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>

#include <Window/Window.hpp>

#include <UI/UI.hpp>

#include <Renderer/Renderer.hpp>

#include <Scene/Camera.hpp>

UI::UI(Renderer* ren) : info{ .flycam = ren->flycam, .visibility_testing = ren->visibility_testing, .time = ren->time, .cam = ren->cam, .tess_factor = ren->tess_factor, .tess_edge_size = ren->tess_edge_size, .n_indices = ren->n_indices }, dev(ren->dev) {
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
}

void UI::newFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::Begin("Rendering Info", nullptr);

	ImGui::Text("# of Indices: %zu", info.n_indices);
	ImGui::Text("FPS: %f", info.fps);
	ImGui::Text("Time: %f", info.time);
	ImGui::Checkbox("Fly Camera", &info.flycam);
	ImGui::Checkbox("Visibility Testing", &info.visibility_testing);
	ImGui::SliderFloat("Tessellation Factor", &info.tess_factor, 0.1, 10.0);
	ImGui::SliderFloat("Edge Size", &info.tess_edge_size, 0.0, 40.0);

	ImGui::End();
}

void UI::render(vk::CommandBuffer cmd) {
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}

UI::~UI() {
	dev.destroyDescriptorPool(desc_pool);
	
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}