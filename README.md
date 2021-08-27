# JCCUI:  js <=> c++ interface
The base project for the interaction between the c++ code and browser. Completely cross-platform UI for c++ program in browser!

This project is very simple and easy to understand c++ <=> javascript interface via the localhost and http. The idea is to make c++ applications that will display the UI using the html/javascript. 

This library has no any external dependencies, consists only of headers, super-lightweight and easy to start. If you know html, js, c++, it is easy to mak c++ app that opens own UI in browser.

Look the examples in the **jcc_test.cpp**
It includes 3 headers - 
* httplib.h - responsible for the server code. Look https://github.com/yhirose/cpp-httplib for details.
* json.h - very lightweight json/javascript - style object manipulation. Look https://github.com/nbsdx/SimpleJSON for details.
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
## Samples
1. Simplest "Hello, world!"
```cpp
jcc::LocalServer sr;
sr.open("Hello world!");
sr.listen();
```
2. Opening custom page.  We get pattern.html, replacing "[[NAME]]" on "Andrew" and opening the modified html
```cpp
jcc::LocalServer sr;
jcc::Html h("examples/pattern.html");
h.Replace("[[NAME]]", "Andrew");
sr.open(h);
sr.listen();
```
3. The real example. Let you have ID <=> String correspondence in your app. And you want to edit/translate the text in the browser.
In this case the form helps to translate on Japanese language. The example opens page, waits for "Submit", then closes the page.
```cpp
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
	}, "/submit");
sr.open(h);
sr.listen();
```
