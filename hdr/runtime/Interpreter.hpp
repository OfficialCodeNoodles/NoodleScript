#pragma once

// Dependencies
#include "eval/Expressions.hpp"

namespace ns {
	class Interpreter {
	public:
		Interpreter(); 
		~Interpreter(); 

		void generateProgram(const std::string& sourceCode); 
		void runProgram(); 
		void deleteProgramMemory(); 
		void logTokens() const; 
		void logAST() const; 

		void enableTokenLogging(bool shouldLogTokens); 
		void enableASTLogging(bool shouldLogAST); 
		void enableRuntimeValueLogging(bool shouldLogRuntimeValue); 

		bool isTokenLoggingEnabled() const; 
		bool isASTLoggingEnabled() const; 
		bool isRuntimeValueLoggingEnabled() const; 
	private:
		Memory memory; 
		Parser parser; 
		Program* program; 
		Scope* globalScope; 
		RuntimeValue* runtimeValue; 
		std::vector<Token> tokens; 
		bool shouldLogTokens; 
		bool shouldLogAST; 
		bool shouldLogRuntimeValue; 

		void initGlobalScope(bool reallocate = true);
		void copyTokens(const std::vector<Token>* tokens); 
		void logRuntimeValue(); 
	};
}