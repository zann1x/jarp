#pragma once

namespace jarp {

	class CommandPool
	{
	public:
		virtual ~CommandPool() = default;

		static CommandPool* Create();
	};

}
