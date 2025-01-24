#include "Hooks.h"
#include "Settings.h"

static void InitializeLog(std::string_view pluginName)
{
	auto path = logger::log_directory();
	if (!path)
	{
		stl::report_and_fail("Failed to find standard logging directory"sv);
	}
	*path /= std::format("{}.log", pluginName);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifdef DEBUG
	const auto level = spdlog::level::debug;
#else
	const auto level = spdlog::level::info;
#endif

	log->set_level(level);
	log->flush_on(level);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S:%e] %v"s);
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
	const auto name{ plugin->GetName() };
	const auto version{ plugin->GetVersion() };

	InitializeLog(name);

	SKSE::AllocTrampoline(2 << 3);
	SKSE::Init(a_skse);

	TimeFormatChanger::LoadSettings();
	SKSE::log::info("Settings loaded.");

	TimeFormatChanger::InstallHook();
	SKSE::log::info("Hooks installed.");

	return true;
}
