#pragma once
#include <functional>
#include <filesystem>

namespace httplib {
	class Server;
};

namespace jcc {
	///This is the simplified filesystem class to be used by the server
	class FileSystem {
		static std::string& _server();

		static void _define_server();
	public:
		///returns path to the system folder to store app-specific files. You need to create subfolder to that path.
		static const char* getTempFolder();
		///appends filename to the /public and returns the path.
		/// The public path defined by setServerFilesPlacement()  + "/public" or seek automatically.
		/// Pay attention, existance of /public folder is mandatory even if there are no files!
		static std::string _public(const char* filename);
		/// appends filename to the server files path
		static std::string path(const char* filename);
		/// Set the path to the folder that contains /public and /private folders 
		static void setServerFilesPlacement(const char* path);
		/// read the file to string
		static void read(const char* filename, std::string& to);
		/// write the string to file
		static void write(const char* filename, std::string& from);
		/// create all componentes of the path
		static void createPath(const char* path);
	};

	class Html {
	public:
		std::string body;
		Html();
		Html(const char* filepath);
		operator const char* ();
		operator std::string& ();
		void Replace(const char* ID, const char* text);
	};

	class LocalServer {
		httplib::Server* svr;
		std::function<std::string(const httplib::Request&)> _get;
		std::function<json::JSON(json::JSON&)> _exchange;
		int _port;
		Html home;
		bool _allowConsoleOutput;
		bool _autoTerminate;
		std::string dump_headers(const httplib::Headers& headers);
		std::string log(const httplib::Request& req, const httplib::Response& res);
	public:
		LocalServer(int preferred_port = -1);

		/// returns the port, assigned to the server
		int port();

		///
		void allowConsoleOutput(bool enable = true);

		/// opens the page in browser
		void open(const Html& page);
		/// opens the page from file in browser
		void openFile(const char* filepath);

		/// assign the callback for the get request. The page interacts with the host program via the get requests.
		void get(std::function<std::string(const httplib::Request&)> f);

		///exchange objects, you are getting object as input and should return the object as well
		void exchange(std::function<json::JSON(json::JSON&)> f);

		/// run once, it blocks execution and starts to listen the requests.
		void listen();
	};



	inline std::string LocalServer::dump_headers(const httplib::Headers& headers) {
		std::string s;
		char buf[BUFSIZ];

		for (auto it = headers.begin(); it != headers.end(); ++it) {
			const auto& x = *it;
			snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
			s += buf;
		}

		return s;
	}

	inline std::string LocalServer::log(const httplib::Request& req, const httplib::Response& res) {
		std::string s;
		if (_allowConsoleOutput) {
			char buf[BUFSIZ];

			s += "================================\n";

			snprintf(buf, sizeof(buf), "%s %s %s", req.method.c_str(),
				req.version.c_str(), req.path.c_str());
			s += buf;

			std::string query;
			for (auto it = req.params.begin(); it != req.params.end(); ++it) {
				const auto& x = *it;
				snprintf(buf, sizeof(buf), "%c%s=%s",
					(it == req.params.begin()) ? '?' : '&', x.first.c_str(),
					x.second.c_str());
				query += buf;
			}
			snprintf(buf, sizeof(buf), "%s\n", query.c_str());
			s += buf;

			s += dump_headers(req.headers);

			s += "--------------------------------\n";

			snprintf(buf, sizeof(buf), "%d %s\n", res.status, res.version.c_str());
			s += buf;
			s += dump_headers(res.headers);
			s += "\n";

			if (!res.body.empty()) { s += res.body; }

			s += "\n";
		}

		return s;
	}

	inline void EnsureTrailingSlash(std::string& s) {
		size_t L = s.length();
		if (L && s[L - 1] != '\\' && s[L - 1] != '/')s += "/";
	}

	inline void TrimSlash(std::string& s) {
		do {
			size_t L = s.length();
			if (L && (s[L - 1] == '\\' || s[L - 1] == '/'))s.pop_back();
			else break;
		} while (s.length());
	}

	inline void RemoveFileName(std::string& s) {
		do {
			size_t L = s.length();
			if (L && s[L - 1] != '\\' && s[L - 1] != '/')s.pop_back();
			else break;
		} while (s.length());
	}

	inline std::string& FileSystem::_server() {
		static std::string s;
		return s;
	}

	inline void FileSystem::_define_server() {
		if (_server().length() == 0) {
			_server() = std::filesystem::current_path().string().c_str();
			EnsureTrailingSlash(_server());
			do {
				std::string fp = _server();
				EnsureTrailingSlash(fp);
				fp += "/public";
				if (std::filesystem::exists(std::filesystem::path(fp.c_str()))) {
					break;
				}
				else {
					TrimSlash(_server());
					RemoveFileName(_server());
					EnsureTrailingSlash(_server());
				}
			} while (_server().length() > 3);
		}
	}

	inline const char* FileSystem::getTempFolder() {
		return "temp/";
	}


	inline std::string FileSystem::_public(const char* filename) {
		_define_server();
		std::string pub = _server();
		pub += "public/";
		pub += filename;
		return pub;
	}

	inline std::string FileSystem::path(const char* filename) {
		_define_server();
		std::string fp = _server();
		fp += filename;
		return fp;
	}

	inline void FileSystem::setServerFilesPlacement(const char* path) {
		_server() = path;
		EnsureTrailingSlash(_server());
	}

	inline void FileSystem::read(const char* filename, std::string& to) {
		std::string fp = _server() + filename;
		FILE* F = nullptr;
		fopen_s(&F, fp.c_str(), "rb");
		if (F) {
			fseek(F, 0, SEEK_END);
			size_t p = ftell(F);
			fseek(F, 0, SEEK_SET);
			char* cc = new char[p + 1];
			cc[p] = 0;
			fread(cc, 1, p, F);
			fclose(F);
			to = cc;
			delete[]cc;
		}
	}

	inline void FileSystem::write(const char* filename, std::string& from) {
		std::string fp = _server() + filename;
		FILE* F = nullptr;
		fopen_s(&F, fp.c_str(), "wb");
		if (F) {
			fwrite(from.c_str(), 1, from.length(), F);
			fclose(F);
		}
	}

	inline void FileSystem::createPath(const char* path) {

	}

	inline Html::Html() {

	}

	inline Html::Html(const char* filepath) {
		jcc::FileSystem::read(filepath, body);
	}

	inline Html::operator const char* () {
		return body.c_str();
	}

	inline Html::operator std::string& () {
		return body;
	}

	std::string _replace(std::string& str, const std::string& sub, const std::string& mod) {
		std::string tmp(str);
		auto fnd = tmp.find(sub);
		if (fnd != -1)tmp.replace(fnd, sub.length(), mod);
		return tmp;
	}
	
	inline void Html::Replace(const char* ID, const char* text) {
		body = _replace(body, ID, text);
	}

	inline LocalServer::LocalServer(int preferred_port) {
		_allowConsoleOutput = false;
		_autoTerminate = true;
		svr = new httplib::Server;
		_get = nullptr;
		_exchange = nullptr;
		if (!svr->is_valid()) {
			printf("server has an error...\n");
			return;
		}
		svr->Get("/ping", [=](const httplib::Request& req, httplib::Response& res) {
			res.set_content("alive", "text/html");
			});
		svr->Get("/home", [=](const httplib::Request& req, httplib::Response& res) {
			std::string h = home;
			res.set_content(h, "text/html");
			});
		std::string fpath = FileSystem::_public("");
		svr->set_mount_point("/", fpath);
		svr->Get("(.*?)", [=](const httplib::Request& req, httplib::Response& res) {
			if (_get) {
				res.set_content(_get(req), "text/html");
			}
			});
		svr->Post("/exchange", [=](const httplib::Request& req, httplib::Response& res) {
			if (_exchange) {
				json::JSON js = json::JSON::Load(req.body);
				std::string s1 = js.dump();
				json::JSON r = _exchange(js);
				res.set_content(r.dump(), "application/json");
			}
			});
		svr->set_error_handler([](const httplib::Request& req, httplib::Response& res) {
			const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
			char buf[BUFSIZ];
			snprintf(buf, sizeof(buf), fmt, res.status);
			res.set_content(buf, "text/html");
			});
		svr->set_logger([this](const httplib::Request& req, const httplib::Response& res) {
			printf("%s", log(req, res).c_str());
			});
		svr->set_file_request_handler([this](const httplib::Request& req, httplib::Response& res) {
			printf("File request: %s", log(req, res).c_str());
			});
		_port = preferred_port;
		if (preferred_port == -1)_port = svr->bind_to_any_port("localhost");
		else {
			if (!svr->bind_to_port("localhost", preferred_port)) {
				_port = svr->bind_to_any_port("localhost");
			}
		}
	}

	inline int LocalServer::port() {
		return _port;
	}

	inline void LocalServer::allowConsoleOutput(bool enable) {
		_allowConsoleOutput = enable;
	}

	inline void LocalServer::open(const Html& page) {
		home = page;
		std::string lib;
		FileSystem::read("public/main.js", lib);
		if (strstr(home, "<body>")) {
			std::string blib = "<body>\n<script>\n" + lib + "\n</script>";
			home.Replace("<body>", blib.c_str());
		}
		std::string h = "start http://localhost:" + std::to_string(_port) + "/home";
		std::system(h.c_str());
	}

	inline void LocalServer::openFile(const char* filepath) {
		Html p;
		FileSystem::read(filepath, p);
		open(p);
	}

	inline void LocalServer::get(std::function<std::string(const httplib::Request&)> f) {
		_get = f;
	}

	void LocalServer::exchange(std::function<json::JSON(json::JSON&)> f) {
		_exchange = f;
	}

	inline void LocalServer::listen() {
		svr->listen_after_bind();
	}
};