#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace jarp {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

		// Initialize core
		s_CoreLogger = std::make_shared<spdlog::logger>("jarp", sink);
		s_CoreLogger->set_level(spdlog::level::trace);
		// start_color_range
		//  [year-month-day hour:minute:second.millisecond] [logger_name] message
		// end_color_range
		s_CoreLogger->set_pattern("%^[%Y-%m-%d %T.%e] [%n] %v%$");

		JARP_CORE_INFO("Initialized core logger!");

		// Initialize client
		s_ClientLogger = std::make_shared<spdlog::logger>("App", sink);
		s_ClientLogger->set_level(spdlog::level::trace);
		// start_color_range
		//  [year-month-day hour:minute:second.millisecond] [logger_name] message
		// end_color_range
		s_ClientLogger->set_pattern("%^[%Y-%m-%d %T.%e] [%n] %v%$");

		JARP_CORE_INFO("Initialized client logger!");
	}

}
