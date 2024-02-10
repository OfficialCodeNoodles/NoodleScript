#include "../../hdr/runtime/Interpreter.hpp"
#include "../../hdr/util/Error.hpp"

namespace ns {
	Interpreter::Interpreter() :
		program(nullptr),
		globalScope(nullptr),
		runtimeValue(nullptr),
		shouldLogTokens(false),
		shouldLogAST(false),
		shouldLogRuntimeValue(false)
	{
		globalMemory = &memory; 

		try {
			initGlobalScope();
		}
		catch (const Error& error) {
			std::cout << error;
		}
	}
	Interpreter::~Interpreter() {
		if (program != nullptr)
			delete program; 
	}

	void Interpreter::generateProgram(const std::string& sourceCode) {
		if (program != nullptr) {
			delete program;
			program = nullptr; 
		}

		try {
			const std::vector<Token>* tokens = parser.lexSourceCode(sourceCode);
			copyTokens(tokens); 

			if (shouldLogTokens) logTokens();
			program = parser.produceAST(); 

			if (shouldLogAST) logAST(); 
		}
		catch (const Error& error) {
			std::cout << error; 
			
			delete program;
			program = nullptr; 
		}
	}
	void Interpreter::runProgram() { 
		try {
			if (program == nullptr)
				throw Error(
					Error::Location::Interpreter, 
					11, 
					"Unable to run program, as it does not exist."
				); 

			runtimeValue = evaluateASTNode(program, globalScope); 

			if (shouldLogRuntimeValue) logRuntimeValue();  
		}
		catch (const Error& error) {
			std::cout << error; 
		}
	}
	void Interpreter::deleteProgramMemory() {
		memory.clear(); 
	}
	void Interpreter::logTokens() const {
		std::cout << "\nLexed Tokens:\n";
		std::cout << tokens;
	}
	void Interpreter::logAST() const {
		if (program == nullptr)
			throw Error(
				Error::Location::Interpreter,
				7,
				"Cannot log AST, as it is undefined."
			);

		std::cout << "\nGenerated AST:\n";
		std::cout << program;
		std::cout << "\n";
	}

	void Interpreter::enableTokenLogging(bool shouldLogTokens) {
		std::cout << " Updated interpreter variable `shouldLogTokens` to "; 
		std::cout << (shouldLogTokens ? "true" : "false") << "\n";
		this->shouldLogTokens = shouldLogTokens; 
	}
	void Interpreter::enableASTLogging(bool shouldLogAST) {
		std::cout << " Updated interpreter variable `shouldLogAST` to ";
		std::cout << (shouldLogAST ? "true" : "false") << "\n";
		this->shouldLogAST = shouldLogAST; 
	}
	void Interpreter::enableRuntimeValueLogging(bool shouldLogRuntimeValue) {
		std::cout << " Updated interpreter variable `shouldLogRuntimeValue` to ";
		std::cout << (shouldLogRuntimeValue ? "true" : "false") << "\n";
		this->shouldLogRuntimeValue = shouldLogRuntimeValue; 
	}

	bool Interpreter::isTokenLoggingEnabled() const {
		return shouldLogTokens;
	}
	bool Interpreter::isASTLoggingEnabled() const {
		return shouldLogAST; 
	}
	bool Interpreter::isRuntimeValueLoggingEnabled() const {
		return shouldLogRuntimeValue; 
	}

	void Interpreter::initGlobalScope(bool reallocate) {
		globalScope = memory.create<Scope>(); 
		globalScope->declareVariable("null", memory.create<NullValue>(), true);

		BoolValue* trueValue = memory.create<BoolValue>(); 
		trueValue->state = true; 
		BoolValue* falseValue = memory.create<BoolValue>(); 
		falseValue->state = false; 

		globalScope->declareVariable("true", trueValue, true);
		globalScope->declareVariable("false", falseValue, true);

		// Create all of the native functions
		for (int nativeFuncNameIndex = 0; nativeFuncNameIndex < numOfNativeFuncs; 
			nativeFuncNameIndex++) 
		{
			const char* nativeFuncName = nativeFuncNames[nativeFuncNameIndex]; 
			
			FuncValue* funcValue = memory.create<FuncValue>();

			funcValue->name = nativeFuncName; 
			globalScope->declareVariable(funcValue->name, funcValue, true); 
		}
	}
	void Interpreter::copyTokens(const std::vector<Token>* tokens) {
		if (tokens == nullptr)
			throw Error(
				Error::Location::Interpreter,
				8,
				"Cannot log tokens, as they are undefined."
			);

		this->tokens.clear(); 

		for (auto& token : *tokens) 
			this->tokens.push_back(token); 
	}
	void Interpreter::logRuntimeValue() {
		if (runtimeValue == nullptr)
			throw Error(
				Error::Location::Interpreter,
				9,
				"Cannot log runtime value, as it is undefined. "
			); 

		std::cout << runtimeValue; 
		std::cout << "\n"; 
	}
}