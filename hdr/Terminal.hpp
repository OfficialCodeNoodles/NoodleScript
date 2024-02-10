#pragma once

// Dependencies
#include "runtime/Interpreter.hpp"

namespace ns {
	extern Interpreter interpreter;
	extern std::string sourceCode;
	extern std::string terminalInput; 
	extern bool terminalOpen; 
	extern bool insideConsole;
	extern bool jumpToConsole;

	void initNoodleScriptMsg(); 
	void initTerminalMsg(); 
	void initConsoleMsg(); 

	void prepareTerminalMsg(); 
	void prepareConsoleMsg(); 

	void executeTerminal(const std::string& input); 
	void executeConsole(const std::string& input); 

	void initTerminal(int argc, const char** argv); 
	bool isTerminalOpen(); 
	void runSourceFile(const std::string& filepath); 
	void updateTerminal(); 
	void closeTerminal(); 
}