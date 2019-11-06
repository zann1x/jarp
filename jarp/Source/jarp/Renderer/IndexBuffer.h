#pragma once

#include "jarp/Renderer/CommandBuffer.h"

namespace jarp {

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind(const std::shared_ptr<CommandBuffer> commandBuffer) = 0;

		static IndexBuffer* Create(const std::vector<uint32_t>& indices, uint32_t size);
	};

}
