#pragma once

#include "spdlog/spdlog.h"

class Log
{
public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return CoreLogger; }

private:
	static std::shared_ptr<spdlog::logger> CoreLogger;
};

#define JARP_CORE_TRACE(...) ::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define JARP_CORE_INFO(...) ::Log::GetCoreLogger()->info(__VA_ARGS__)
#define JARP_CORE_WARN(...) ::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define JARP_CORE_ERROR(...) ::Log::GetCoreLogger()->error(__VA_ARGS__)
#define JARP_CORE_CRITICAL(...) ::Log::GetCoreLogger()->critical(__VA_ARGS__)
