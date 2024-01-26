#include <Memory/Memory.hpp>

namespace mem {
	u32 choose_heap(vk::PhysicalDevice& phys_dev, const vk::MemoryRequirements& requirements, vk::MemoryPropertyFlags req_flags, vk::MemoryPropertyFlags pref_flags) {
		auto dev_props = phys_dev.getMemoryProperties();

		if(static_cast<int>(pref_flags.operator VkImageCreateFlags()) != -1) {
			/* try to find an exact match for preferred flags first */
			for (u32 memory_type = 0; memory_type < 32; memory_type++) {
				if (requirements.memoryTypeBits & (1 << memory_type)) {
					const auto& type = dev_props.memoryTypes[memory_type];
					if ((type.propertyFlags & pref_flags) == pref_flags) {
						return type.heapIndex;
					}
				}
			}
		}
		
		for (u32 memory_type = 0; memory_type < dev_props.memoryTypeCount; memory_type++) {
			if (requirements.memoryTypeBits & (1 << memory_type)) {
				const auto& type = dev_props.memoryTypes[memory_type];
				if ((type.propertyFlags & req_flags) == req_flags) {
					return memory_type;
				}
			}
		}

		return -1;
	}
}