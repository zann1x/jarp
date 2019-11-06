#pragma once

#include "jarp/Renderer/CommandPool.h"

namespace jarp {

	class CommandBuffer
	{
	public:
		virtual ~CommandBuffer() = default;

		static CommandBuffer* Create(const std::shared_ptr<CommandPool>& commandPool);
	};

}
