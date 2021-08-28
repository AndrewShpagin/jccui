#pragma once
#include <functional>
#include <filesystem>

namespace httplib {
	class Server;
};

namespace jcc {
	class Html;

	class Request:public httplib::Request {
	public:
		///tries to convert params to json, if empty -  converts body to JSON
		operator json::JSON() const;
		///explicitly converts params of the request to JSON
		json::JSON paramsToJson() const;
		///explicitly converts the body of the request to JSON
		json::JSON bodyToJson() const;
	};
	class Response:public httplib::Response {
	public:
		///Assign the json directly as the request response
		void operator = (const json::JSON& js);
		///Assign the HTML as the request response;
		void operator = (const Html& h);
		///Assign text as the response result
		void operator = (const std::string& string);
		///Assign text as the response result
		void operator = (const char* string);
	};
	typedef std::function<void(const Request&, Response&)> requestHandler;
	
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
		std::function<json::JSON(json::JSON&)> _exchange;
		int _port;
		Html home;
		bool _allowConsoleOutput;
		bool _autoTerminate;
		bool _stop;
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

		/// assign the callback for the get request. The page may interact with the host program via the get requests.
		void get(requestHandler f, const char* pattern = nullptr);

		/// assign the callback for the post request. The page may interact with the host program via the post requests.
		void post(requestHandler f, const char* pattern = nullptr);

		/// assign the callback for the put request.
		void put(requestHandler f, const char* pattern = nullptr);

		///exchange objects, you are getting object as input and should return the object as well. The call of exchange initiated on browser's side by sendObject, see the example
		void exchange(std::function<json::JSON(const json::JSON&)> f);

		/// run once, it blocks execution and starts to listen the requests. Create the thread if no need blocking.
		void listen();

		/// stop the server and exit the listen cycle. This function may be called in the response body. Server will be stopped only after the sending the response.
		void signalToStop();
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

	inline Request::operator json::JSON() const {
		if (params.size()) {
			return paramsToJson();
		}else {
			return bodyToJson();
		}
	}

	inline json::JSON Request::paramsToJson() const {
		json::JSON js;
		for (auto it = params.begin(); it != params.end(); it++) {
			js[it->first] = it->second;
		}
		return js;
	}

	inline json::JSON Request::bodyToJson() const {
		json::JSON js = json::JSON::Load(body.c_str());
		return js;
	}

	inline void Response::operator=(const json::JSON& js) {
		set_content(js.dump(), "application/json");
	}

	inline void Response::operator=(const Html& h) {
		set_content(h.body.c_str(), "text/html");
	}

	inline void Response::operator=(const std::string& str) {
		set_content(str.c_str(), "text/html");
	}

	inline void Response::operator=(const char* str) {
		set_content(str, "text/html");
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
		std::ifstream  fs(fp, std::fstream::in | std::fstream::binary);
		if (fs.is_open()) {
			std::stringstream buffer;
			buffer << fs.rdbuf();
			to = buffer.str();
			fs.close();
		}
	}

	inline void FileSystem::write(const char* filename, std::string& from) {
		std::string fp = _server() + filename;
		std::fstream fs(fp, std::fstream::out | std::fstream::binary);
		if(fs.is_open()){
			fs << from;
			fs.close();
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
		_stop = false;
		svr = new httplib::Server;
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
		svr->set_post_routing_handler([this](const httplib::Request& req, httplib::Response& res) {
			if(_stop) {
				svr->stop();
			}
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
		FileSystem::read("main.js", lib);
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

	inline void LocalServer::get(requestHandler f, const char* pattern) {
		if(svr)svr->Get(pattern ? pattern : "(.*?)", [=](const httplib::Request& req, httplib::Response& res) {
			const Request* _req = static_cast<const Request*>(&req);
			Response* _res = (Response*)(&res);
			f(*_req, *_res);
			res.status = 200;
		});
	}

	inline void LocalServer::post(requestHandler f, const char* pattern) {
		if (svr)svr->Post(pattern ? pattern : "(.*?)", [=](const httplib::Request& req, httplib::Response& res) {
			const Request* _req = static_cast<const Request*>(&req);
			Response* _res = static_cast<Response*>(&res);
			f(*_req, *_res);
			res.status = 200;
		});
	}

	inline void LocalServer::put(requestHandler f, const char* pattern) {
		if (svr)svr->Put(pattern ? pattern : "(.*?)", [=](const httplib::Request& req, httplib::Response& res) {
			const Request* _req = static_cast<const Request*>(&req);
			Response* _res = static_cast<Response*>(&res);
			f(*_req, *_res);
			res.status = 200;
			});
	}

	void LocalServer::exchange(std::function<json::JSON(const json::JSON&)> f) {
		if (svr)svr->Post("/exchange", [=](const httplib::Request& req, httplib::Response& res) {
			json::JSON js = json::JSON::Load(req.body);
			std::string s1 = js.dump();
			json::JSON r = f(js);
			res.set_content(r.dump(), "application/json");
			res.status = 200;
		});
	}

	inline void LocalServer::listen() {
		if(svr)svr->listen_after_bind();
	}

	inline void LocalServer::signalToStop() {
		if(svr) {
			_stop = true;
		}
	}
};
