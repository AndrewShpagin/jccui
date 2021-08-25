﻿#include "httplib.h"
#include "json.h"
#include "jcc.h"

/// simplest example ever
void example_HelloWorld() {
	jcc::LocalServer sr;
	sr.open("Hello world!");
	sr.listen();
}

/// we get pattern.html, replacing [[NAME]] on "Andrew" and opening the modified html
void example_HtmlPattern() {
	jcc::LocalServer sr;
	jcc::Html h("examples/pattern.html");
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
	jcc::Html h("examples/test_request.html");
	sr.open(h);
	sr.listen();
}

/// The first real example. Let you have ID <=> String correspondence in your app. And you want to edit/translate the text in the browser.
/// In this case the form helps to translate on Japanese language
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
	sr.exchange([](json::JSON& data)->json::JSON {
		printf("%s", data.dump().c_str());
		return data;
		});
	sr.open(h);
	sr.listen();
}

int main(){
	//example_HelloWorld();
	//example_HtmlPattern();
	//example_ObjectsExchange();
	example_Translate();
	return 0;
}
