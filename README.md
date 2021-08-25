# jcc : js <=> c++ interface
The base project for the interaction between the c++ code and browser. The UI for c++ program in browser!

This project is very simple and easy to understand c++ <=> javascript interface via the localhost and http. The idea is to make c++ applications that will display the UI using the html/javascript. 

This library has no any external dependencies, consists only of headers, super-lightweight and easy to start. If you know html, js, c++, it is easy to mak c++ app that opens own UI in browser.

Look the examples in the **jcc_test.cpp**
It includes 3 headers - 
* httplib.h - responsible for the server code.
* json.h - very lightweight json/javascript - style object manipulation.
* jccServer.h - the interface layer that allows to open page in brobser and interact with the c++ code.

It also hes several samples, uncomment, run and disover each one.

