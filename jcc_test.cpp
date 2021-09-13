#include "httplib.h"
#include "json.h"
#include "jcc.h"

/// simplest example ever
void example_HelloWorld() {
	jcc::LocalServer sr;
	jcc::Html h;
	h << "Hello world!";
	sr.open(h);
	sr.wait();
}

/// eval in browser example
void example_eval() {
	jcc::LocalServer sr;
	jcc::Html h;
	h << "<body>Press CTRL SHIFT I, see the console.</body>";
	sr.open(h);
	sr.eval() << "console.log('Hello from the c++ code!')" << sr;
	sr.wait();
}

/// dom access from the c++
void example_dom() {
	jcc::LocalServer sr;
	jcc::Html h;
	h << "<body><h1 id=\"main\"><h1></body>";
	sr.open(h);
	for(int i=0;sr.alive();i++){
		sr.el("main") << "Seconds passed: <b>" << std::to_string(i) << "</b>" << sr;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	sr.stopGracefully();
}

/// console in browser example
void example_console() {
	jcc::LocalServer sr;
	jcc::Html h;
	h << "<body>Press CTRL SHIFT I, see the console.</body>";
	sr.open(h);
	sr.console() << "Hello from the c++ code!'" << sr;
	sr.wait();
}

/// we get pattern.html, replacing [[NAME]] on "Andrew" and opening the modified html
void example_HtmlPattern() {
	jcc::LocalServer sr;
	jcc::Html h("examples/pattern.html");
	h.Replace("[[NAME]]", "Andrew");
	sr.open(h);
	sr.wait();
}

/// we get test_request.html, it sends request using sendObject (defined in main.js), we are modifying object and returning the resulting js object.
void example_ObjectsExchange() {
	jcc::LocalServer sr;
	sr.exchange([](const json::JSON& obj)->json::JSON {
		json::JSON js;
		js["User"] = obj;
		return js;
	});
	jcc::Html h("examples/test_request.html");
	sr.open(h);
	sr.wait();
}

/// The first real example. Let you have ID <=> String correspondence in your app. And you want to edit/translate the text in the browser.
/// In this case the form helps to translate on Japanese language. The example opens page, waits for "Submit", then closes the page.
void example_Translate() {
	jcc::LocalServer sr;
	json::JSON txt=json::Object();
	const char* tl = "ja";
	int idx = 0;
	auto tag = [&](const char* id, const char* value) {
		txt[id] = value;
	};
	auto add = [&](const char* id, const char* text) {
		std::string s = "TextItem" + std::to_string(idx++);
		txt[s] = json::Object();
		txt[s]["ID"] = id;
		txt[s]["Text"] = text;
		txt[s]["English"] = text;
		txt[s]["tl"] = tl;
	};
	txt["Language"] = "Japanese";
	txt["author"] = "Andrew";
	txt["mail"] = "andrewshpagin@gmail.com";
	txt["author"] = "Andrew";
	add("HELLO", "Hello world!");
	add("JCC", "js and c++ interface.");
	txt["Button"] = "Submit";
	jcc::Html h("examples/edittext.html");
	h.Replace("{JSONTEXT}", txt.dump().c_str());
	///pay attention, if the form has action <form action="/submit" method="get"> then the result of submitting will be passed there as json object, look the edittext.html
	sr.get([&](const jcc::Request& req, jcc::Response& res) {
		printf("The translation result:\n%s", req.paramsToJson().dump().c_str());
		res = "<html><body><div>Text accepted! Please close the page if it is not closed automatically.</div><script>window.close();</script></body></html>";
		sr.signalToStop();
		}, "/submit");
	sr.open(h);
	sr.wait();
}

int main(){
	//example_HelloWorld();
	example_eval();
	//example_console();
	//example_dom();
	//example_HtmlPattern();
	//example_ObjectsExchange();
	//example_Translate();
	return 0;
}
