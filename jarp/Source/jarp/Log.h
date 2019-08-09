#pragma once

#include "spdlog/spdlog.h"

namespace jarp {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> CoreLogger;
		static std::shared_ptr<spdlog::logger> ClientLogger;
	};

}

#define JARP_CORE_TRACE(...)		::jarp::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define JARP_CORE_INFO(...)			::jarp::Log::GetCoreLogger()->info(__VA_ARGS__)
#define JARP_CORE_WARN(...)			::jarp::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define JARP_CORE_ERROR(...)		::jarp::Log::GetCoreLogger()->error(__VA_ARGS__)
#define JARP_CORE_CRITICAL(...)		::jarp::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define JARP_TRACE(...)				::jarp::Log::GetClientLogger()->trace(__VA_ARGS__)
#define JARP_INFO(...)				::jarp::Log::GetClientLogger()->info(__VA_ARGS__)
#define JARP_WARN(...)				::jarp::Log::GetClientLogger()->warn(__VA_ARGS__)
#define JARP_ERROR(...)				::jarp::Log::GetClientLogger()->error(__VA_ARGS__)
#define JARP_CRITICAL(...)			::jarp::Log::GetClientLogger()->critical(__VA_ARGS__)
