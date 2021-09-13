# JCCUI:  js <=> c++ interface
![CMake](https://github.com/AndrewShpagin/jccui/actions/workflows/cmake.yml/badge.svg)

The base project for the interaction between the c++ code and browser. Completely cross-platform UI for c++ program in browser!

This project is very simple and easy to understand c++ <=> javascript interface via the localhost and http. The idea is to make c++ applications that will display the UI using the html/javascript. 

This library has no any external dependencies, consists only of headers, super-lightweight and easy to start. If you know html, js, c++, it is easy to make c++ app that opens own UI in browser.

Look the examples in the **jcc_test.cpp**
It includes 3 headers - 
* httplib.h - responsible for the server code. Look https://github.com/yhirose/cpp-httplib for details.
* json.h - very lightweight json/javascript - style object manipulation. Look https://github.com/nbsdx/SimpleJSON for details. Pay attention, there used modified version of the SimpleJSON, several bugs fixed, better flexibility of types casting.
* jcc.h - the interface layer that allows to open page in brobser and interact with the c++ code.

It also has several samples, uncomment, run and disover each one.
How to start:

**Windows:**
Open VS 2019, Open local folder, run the project.

**Mac/Linux:**
Use CMake to build and run.

## Usage
Just include 3 headers into your project.
```
#include "httplib.h"
#include "json.h"
#include "jcc.h"
```
### Several patterns of usage:

The simplest one:
```cpp
jcc::LocalSelver ls;
jcc::Html h("test.html");//test.html placed in the same folder as the main.js
ls.exchange([](const json::JSON& incomingData)->json::JSON {
	json::JSON response;
	// handle the incoming data and send the response.
	return response;
});
ls.open(h);
ls.wait();
```
A bit more general:
```cpp
jcc::LocalSelver ls;
jcc::Html h("test.html");//test.html placed in the same folder as the main.js
ls.exchange([](const json::JSON& incomingData)->json::JSON {
	json::JSON response;
	// handle the incoming data and send the response. It looks similar to js syntax
	response["User"] = "Andrew";	
	return response;
});
ls.open(h);
do {
	// do something in cycle if need, 
	// send data to client if need, for example
	ls.eval() << "console.log('Hello from the c++ code!')" << sr;
	// and sleep if nothing todo.
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
} while(ls.alive());
// End all stuff
ls.finishGracefully();	
```
Pay attention the project should include **'main.js'**, it should be placed in the project's folder. The folder where the **main.js** placed
treated as the root folder. All publicly accessible resources like pictures should be places into the **public** folder. If you want to point 
to some specific folder whwre you placed **main.js**, ise the function
```cpp
jcc::LocalSelver ls;
ls.setServerFilesPlacement("c:/path_to_main_js/");
```
## Samples
1. Simplest "Hello, world!"
```cpp
jcc::LocalServer sr;
jcc::Html h;
h << "<body>Hello world!</body>";
sr.open(h);
sr.wait();
```
2. Open the page and execute js code there. The code passed later, after the page is open.
```cpp
jcc::LocalServer sr;
jcc::Html h;
h << "<body>Press CTRL SHIFT I, see the console.</body>";
sr.open(h);
sr.eval() << "console.log('Hello from the c++ code!')" << sr;
sr.wait();
```
3. Modify DOM element from the c++ server side.
```cpp
jcc::LocalServer sr;
jcc::Html h;
h << "<body><h1 id=\"main\"><h1></body>";
sr.open(h);
for(int i=0;sr.alive();i++){
	sr.el("main") << "Seconds passed: <b>" << std::to_string(i) << "</b>" << sr;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}
sr.stopGracefully();
```
4. Send something to the browser console.
```cpp
jcc::LocalServer sr;
jcc::Html h;
h << "<body>Press CTRL SHIFT I, see the console.</body>";
sr.open(h);
sr.console() << "Hello from the c++ code!'" << sr;
sr.wait();
```
5. Opening custom page.  We get pattern.html, replacing "[[NAME]]" on "Andrew" and opening the modified html.
See the [pattern.html](examples/pattern.html)
```cpp
jcc::LocalServer sr;
// if you read by relative path, you need to pass server reference as well
jcc::Html h("examples/pattern.html", sr);
h.Replace("[[NAME]]", "Andrew");
sr.open(h);
sr.wait();
```
6. Objects exchange, see the [test_request.html](examples/test_request.html)
```cpp
jcc::LocalServer sr;
sr.exchange([](const json::JSON& obj)->json::JSON {
	json::JSON js;
	js["User"] = obj;
	return js;
});
jcc::Html h("examples/test_request.html");
sr.open(h);
sr.wait();
```
7. The real example. Let you have ID <=> String correspondence in your app. And you want to edit/translate the text in the browser.
In this case the form helps to translate on Japanese language. The example opens page, waits for "Submit", then closes the page.
See the [edittext.html](examples/edittext.html)
```cpp
jcc::LocalServer sr;
json::JSON txt=json::Object();
const char* tl = "ja";
int idx = 0;
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
jcc::Html h("examples/edittext.html", sr);
h.Replace("{JSONTEXT}", txt.dump().c_str());
///pay attention, if the form has action <form action="/submit" method="get"> then the result of submitting will be passed there as json object, look the edittext.html
sr.get([&](const jcc::Request& req, jcc::Response& res) {
	printf("The translation result:\n%s", req.paramsToJson().dump().c_str());
	res = "<html><body><div>Text accepted! Please close the page if it is not closed automatically.</div><script>window.close();</script></body></html>";
	}, "/submit");
sr.open(h);
sr.wait();
```
