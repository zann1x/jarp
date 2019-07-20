#include "jarppch.h"
#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace jarp {

	std::shared_ptr<spdlog::logger> Log::CoreLogger;

	void Log::Init()
	{
		auto Sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		CoreLogger = std::make_shared<spdlog::logger>("jarp", Sink);
		CoreLogger->set_level(spdlog::level::trace);
		// start_color_range
		//  [year-month-day hour:minute:second.millisecond] [logger_name] message
		// end_color_range
		CoreLogger->set_pattern("%^[%Y-%m-%d %T.%e] [%n] %v%$");

		JARP_CORE_INFO("Initialized core logger!");
	}

}
