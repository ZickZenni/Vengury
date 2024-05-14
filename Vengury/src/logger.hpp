#pragma once

#include "common.hpp"

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

namespace Vengury
{
	template <typename TP>
	std::time_t ToTime_T(TP tp)
	{
		using namespace std::chrono;
		auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
			+ system_clock::now());
		return system_clock::to_time_t(sctp);
	}

	enum class LogColor : std::uint16_t
	{
		red = FOREGROUND_RED,
		green = FOREGROUND_GREEN,
		blue = FOREGROUND_BLUE,
		intensify = FOREGROUND_INTENSITY
	};

	enum LOG_FLAGS
	{
		FLAG_NO_DISK = (1 << 0),
		FLAG_NO_CONSOLE = (1 << 1)
	};

	static const int kEventValue = 400;
	static const int kRawValue =  600;
	inline constexpr auto max_padding_length = 26;
	inline constexpr auto level_padding_length = 8;

	const LEVELS INFO_TO_FILE{ INFO.value | FLAG_NO_CONSOLE, {"INFO"} },
		HACKER{ INFO.value, {"HACKER"} },
		EVENT{ kEventValue | FLAG_NO_CONSOLE, {"EVENT"} },
		RAW_GREEN_TO_CONSOLE{ kRawValue | FLAG_NO_DISK, {"RAW_GREEN_TO_CONSOLE"} },
		RAW_RED{ kRawValue, {"RAW_RED"} };

	inline LogColor operator|(LogColor a, LogColor b)
	{
		return static_cast<LogColor>(static_cast<std::underlying_type_t<LogColor>>(a) | static_cast<std::underlying_type_t<LogColor>>(b));
	}

	class Logger;
	inline Logger* g_logger{};

	class Logger
	{
	public:
		explicit Logger() :
			m_file_path(std::getenv("appdata")),
			m_worker(g3::LogWorker::createLogWorker())
		{
			if ((m_did_console_exist = AttachConsole(GetCurrentProcessId())) == false)
				AllocConsole();

			if ((m_console_handle = GetStdHandle(STD_OUTPUT_HANDLE)) != nullptr)
			{
				SetConsoleTitleA("Vengury");
				SetConsoleOutputCP(CP_UTF8);

				m_console_out.open("CONOUT$", std::ios_base::out | std::ios_base::app);
			}

			m_file_path /= "Vengury";
			std::filesystem::path m_backup_path = m_file_path;
			m_backup_path /= "Backup";
			try
			{
				if (!std::filesystem::exists(m_file_path))
				{
					std::filesystem::create_directory(m_file_path);
				}
				else if (!std::filesystem::is_directory(m_file_path))
				{
					std::filesystem::remove(m_file_path);
					std::filesystem::create_directory(m_file_path);
				}
				if (!std::filesystem::exists(m_backup_path))
				{
					std::filesystem::create_directory(m_backup_path);
				}
				else if (!std::filesystem::is_directory(m_backup_path))
				{
					std::filesystem::remove(m_backup_path);
					std::filesystem::create_directory(m_backup_path);
				}

				m_file_path /= "Vengury.log";

				if (std::filesystem::exists(m_file_path))
				{
					auto file_time = std::filesystem::last_write_time(m_file_path);
					auto timet = ToTime_T(file_time);
					auto local_time = std::localtime(&timet);

					auto timestamp = fmt::format("{:0>2}-{:0>2}-{}-{:0>2}-{:0>2}-{:0>2} Vengury.log", local_time->tm_mon + 1, local_time->tm_mday, local_time->tm_year + 1900, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);

					std::filesystem::copy_file(m_file_path, m_backup_path / timestamp);
				}

				m_file_out.open(m_file_path, std::ios_base::out | std::ios_base::trunc);

				m_worker->addSink(std::make_unique<log_sink>(), &log_sink::Callback);
				g3::initializeLogging(m_worker.get());
			}
			catch (std::filesystem::filesystem_error const& error)
			{
				m_console_out << error.what();
			}

			g_logger = this;
		}

		~Logger()
		{
			m_worker.reset();
			if (!m_did_console_exist)
				FreeConsole();

			g_logger = nullptr;
		}

		struct log_sink
		{
			std::map<std::string, LogColor> LogColors = {
				{INFO.text, LogColor::blue | LogColor::intensify},
				{WARNING.text, LogColor::red},
				{HACKER.text, LogColor::green | LogColor::intensify},
				{FATAL.text, LogColor::red | LogColor::intensify},
				{G3LOG_DEBUG.text, LogColor::blue},
				{RAW_RED.text, LogColor::red},
				{RAW_GREEN_TO_CONSOLE.text, LogColor::green | LogColor::intensify}
			};

			void Callback(g3::LogMessageMover log)
			{
				g3::LogMessage log_message = log.get();
				int level_value = log_message._level.value;

				bool is_raw = level_value == RAW_RED.value || level_value == RAW_GREEN_TO_CONSOLE.value;

				if (!(level_value & FLAG_NO_CONSOLE))
				{
					SetConsoleTextAttribute(g_logger->m_console_handle, static_cast<std::uint16_t>(LogColors[log_message._level.text]));
					g_logger->m_console_out << log_message.toString(is_raw ? FormatRaw : FormatConsole) << std::flush;
				}
				
				if (!(level_value & FLAG_NO_DISK))
				{
					g_logger->m_file_out << log_message.toString(is_raw ? FormatRaw : FormatFile) << std::flush;
				}
			}

			static std::string FormatFile(const g3::LogMessage& msg)
			{
				std::string file_name_with_line = "[" + msg.file() + ":" + msg.line() + "]";
				std::stringstream out;
				out << "[" << msg.timestamp("%H:%M:%S") << "] [" << std::left << std::setw(level_padding_length) << msg.level().append("]") << std::setw(max_padding_length) << file_name_with_line;
				return out.str();
			}

			static std::string FormatConsole(const g3::LogMessage& msg)
			{
				std::stringstream out;
				out << "[" << msg.timestamp("%H:%M:%S") << "] ";
				return out.str();
			}

			static std::string FormatRaw(const g3::LogMessage& msg)
			{
				return "";
			}
		};

		template <typename ...Args>
		inline void LogNow(std::string_view format, Args const& ...args)
		{
			auto message = fmt::format(format, args...);
			if (m_file_out)
				m_file_out << message << std::endl << std::flush;
		}
		
	private:
		bool m_did_console_exist{};
		HANDLE m_console_handle{};
		std::ofstream m_console_out;
		std::filesystem::path m_file_path;
		std::filesystem::path m_event_file_path;
		std::ofstream m_file_out;
		std::unique_ptr<g3::LogWorker> m_worker;
	};


#define LOG_NOW(format, ...) g_logger->LogNow(format, __VA_ARGS__)
#define HEX_TO_UPPER(value) "0x" << std::hex << std::uppercase << (DWORD64)value << std::dec << std::nouppercase
}
