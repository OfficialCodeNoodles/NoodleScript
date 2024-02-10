#include "../../hdr/runtime/Scope.hpp"
#include "../../hdr/util/Error.hpp"

namespace ns {
	Scope::Scope() : parent(nullptr) {
	}

	Scope* Scope::assignParent(Scope* parent) {
		this->parent = parent; 
		return parent; 
	}
	RuntimeValue* Scope::declareVariable(
		const std::string& variableName, RuntimeValue* runtimeValue, bool constant) 
	{
		// Checks if variable already exists
		if (variables.find(variableName) == variables.end())
			variables[variableName] = { constant, runtimeValue };
		else {
			throw Error(
				Error::Location::Interpreter,
				2,
				"Cannot create noodle " + variableName + " as it is already defined in this scope"
			);
		}

		return runtimeValue; 
	}
	RuntimeValue* Scope::assignVariable(
		const std::string& variableName, RuntimeValue* runtimeValue) 
	{
		Scope* scope = resolveVariable(variableName); 
		Variable& variable = scope->variables[variableName]; 
		
		// Ensures constants cannot be reassigned
		if (variable.constant)
			throw Error(
				Error::Location::Interpreter,
				3,
				"Cannot redeclare noodle " + variableName + " as it is a frozen noodle."
			); 
		
		variable.runtimeValue = runtimeValue; 
		return runtimeValue; 
	}
	Scope* Scope::resolveVariable(const std::string& variableName) {
		// Returns current scope if variable is found in variables map
		if (variables.find(variableName) != variables.end())
			return this; 

		if (parent == nullptr)
			throw Error(
				Error::Location::Interpreter, 
				4, 
				"Undefined noodle " + variableName + " referenced in scope."
			);

		return parent->resolveVariable(variableName); 
	}
	RuntimeValue* Scope::getVariableValue(const std::string& variableName) {
		Scope* scope = resolveVariable(variableName);
		return scope->variables[variableName].runtimeValue; 
	}
	bool Scope::isVariableConst(const std::string& variableName) {
		Scope* scope = resolveVariable(variableName); 
		const Variable variable = scope->variables[variableName]; 
		return variable.constant; 
	}
}