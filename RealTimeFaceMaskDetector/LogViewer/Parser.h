#ifndef LOGVIEWER_PARSER_H
#define LOGVIEWER_PARSER_H
#include <string>
#include <vector>
#include <fstream>
#include <regex>

namespace logger {
	enum class LogOptions {
		INFO,
		WARNING,
		ERROR,
		DEBUG,
		FATAL
	};

	struct LogStruct {
		std::string time;
		LogOptions option;
		std::string trace_track;
		std::string message;
	};

	class Parser {
	public:
		Parser(const std::string& path_) : path{ path_ } {
			open_file();
			parse_all();
		}

		Parser(const std::string& path_, const std::vector<LogOptions>& options_) : 
			options{ options_ }, path{path_} 
		{
			open_file();
			parse_with_options();
		}

		~Parser() {
			fs.close();
		}
		const std::vector<LogStruct>& get_logs() const { return logs; }
	private:
		void open_file();
		void parse_all();
		void parse_with_options();
		void find_type();
		bool match{ false };
		std::string path;
		std::vector<LogOptions> options;
		std::vector<LogStruct> logs;
		std::fstream fs;
		std::string line;
		std::regex info_pat{R"(\[INFO\]{1})"};
		std::regex warning_pat{ R"(\[WARNING\]{1})" };
		std::regex error_pat{ R"(\[ERROR\]{1})" };
		std::regex debug_pat{ R"(\[DEBUG\]{1})" };
		std::regex fatal_pat{ R"(\[FATAL\]{1})" };
		LogStruct current_log;
	};

}

#endif //LOGVIEWVER_PARSER_H