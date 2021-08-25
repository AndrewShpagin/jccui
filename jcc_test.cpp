#include "httplib.h"
#include "json.h"
#include "jccServer.h"

/// simplest example ever
void example_HelloWorld() {
	jcc::LocalServer sr;
	sr.open("Hello world!");
	sr.listen();
}

/// we get pattern.html, replacing [[NAME]] on "Andrew" and opening the modified html
void example_HtmlPattern() {
	jcc::LocalServer sr;
	jcc::Html h("pattern.html");
	h.Replace("[[NAME]]", "Andrew");
	sr.open(h);
	sr.listen();
}

/// we get test_request.html, it sends request using sendObject (defined in main.js), we are modifying object and returning the resulting js object.
void example_ObjectsExchange() {
	jcc::LocalServer sr;
	sr.exchange([](json::JSON& obj)->json::JSON {
		json::JSON js;
		js["User"] = obj;
		return js;
	});
	jcc::Html h("test_request.html");
	sr.open(h);
	sr.listen();
}

int main(){
	//example_HelloWorld();
	//example_HtmlPattern();
	example_ObjectsExchange();
	return 0;
}
