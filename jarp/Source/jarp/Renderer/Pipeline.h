#pragma once

namespace jarp {

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		static Pipeline* Create();
	};

}
