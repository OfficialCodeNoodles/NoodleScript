#pragma once

// Dependencies
#include "../backend/AST.hpp"

namespace ns {
	constexpr int numOfNativeFuncs = 17; 
	extern const char* nativeFuncNames[numOfNativeFuncs]; 
	class Scope; 

	struct RuntimeValue {
		ValueType valueType; 

		virtual ~RuntimeValue() {}; 
	};
	struct NullValue : public RuntimeValue {
		std::string value; 

		NullValue();
	};
	struct NumValue : public RuntimeValue {
		double value; 

		NumValue(); 
		NumValue(double value); 
	};
	struct BoolValue : public RuntimeValue {
		bool state; 

		BoolValue(); 
		BoolValue(bool state); 
	};
	struct StringValue : public RuntimeValue {
		std::string value; 

		StringValue(); 
		StringValue(const std::string& value); 
	};
	struct ListValue : public RuntimeValue {
		std::vector<RuntimeValue*> elements; 

		ListValue(); 
	};
	struct FuncValue : public RuntimeValue {
		std::string name;
		std::vector<std::string> parameters; 
		std::vector<Statement*> statements;
		Scope* scope; 
		bool nativeFunc; 

		FuncValue(); 
	};

	RuntimeValue* cloneRuntimeValue(RuntimeValue* runtimeValue); 

	std::ostream& operator<<(std::ostream& ostream, const RuntimeValue* runtimeValue); 
}