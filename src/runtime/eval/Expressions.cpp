#include "../../../hdr/runtime/eval/Expressions.hpp"
#include "../../../hdr/util/Error.hpp"

namespace ns {
	RuntimeValue* evaluateFuncCallExpr(FuncCall* funcCall, Scope* scope) {
		FuncValue* funcValue = dynamic_cast<FuncValue*>(
			scope->getVariableValue(funcCall->caller)
		); 
		
		// Handle native functions seperately
		if (funcValue->nativeFunc) 
			return evaluateNativeFuncCallExpr(funcValue, funcCall, scope); 
		
		Scope* funcScope = globalMemory->create<Scope>(); 

		funcScope->assignParent(scope); 

		// Declare all of the parameters in the function scope
		for (int parameterIndex = 0; parameterIndex < funcValue->parameters.size(); 
			parameterIndex++) 
		{
			const std::string& parameter = funcValue->parameters[parameterIndex]; 
			Expr* argument = funcCall->arguments[parameterIndex]; 

			funcScope->declareVariable(parameter, evaluateASTNode(argument, scope)); 
		}

		RuntimeValue* result = globalMemory->create<NullValue>(); 

		// Evaluate each function statement
		for (auto& statement : funcValue->statements)
			result = evaluateASTNode(statement, funcScope); 

		return result; 
	}
	RuntimeValue* evaluateNativeFuncCallExpr(
		FuncValue* funcValue, FuncCall* funcCall, Scope* scope) 
	{
		auto verifyArgumentCount = [&](int count) -> void {
			const int argumentCount = funcCall->arguments.size(); 

			// Throw error if argument counts aren't equal
			if (argumentCount != count) {
				throw Error(
					Error::Location::Interpreter,
					14,
					"Cannot call recipe " + funcCall->caller + " with " 
						+ std::to_string(argumentCount) 
							+ (argumentCount == 1 ? " argument" : " arguments")
				);
			}
		}; 

		RuntimeValue* result = globalMemory->create<NullValue>(); 
		
		if (funcValue->name == nativeFuncNames[0]) { // serve
			auto printEvaluatedArgument = [](const RuntimeValue* evaluatedArgument) -> void {
				auto printEvaluatedArgumentImpl = [](const RuntimeValue* evaluatedArgument, 
					auto& caller) -> void 
				{
					switch (evaluatedArgument->valueType) {
					case ValueType::Number:
					{
						const NumValue* numberValue = dynamic_cast<const NumValue*>(
							evaluatedArgument
						);
						std::cout << numberValue->value;
						break;
					}
					case ValueType::Bool:
					{
						const BoolValue* boolValue = dynamic_cast<const BoolValue*>(evaluatedArgument);
						std::cout << (boolValue->state ? "true" : "false");
						break;
					}
					case ValueType::String:
					{
						const StringValue* stringValue = dynamic_cast<const StringValue*>(
							evaluatedArgument
						);
						std::cout << stringValue->value;
						break;
					}
					case ValueType::List:
					{
						const ListValue* listValue = dynamic_cast<const ListValue*>(evaluatedArgument);

						std::cout << "[";

						for (int elementIndex = 0; elementIndex < listValue->elements.size(); 
							elementIndex++) 
						{
							const RuntimeValue* element = listValue->elements[elementIndex]; 
							caller(element, caller); 

							if (elementIndex < listValue->elements.size() - 1)
								std::cout << " ";
						}

						std::cout << "]";
						break;
					}
					}
				}; 

				printEvaluatedArgumentImpl(evaluatedArgument, printEvaluatedArgumentImpl); 
			}; 

			// Loop through the argument list and call local lambda
			for (auto* argument : funcCall->arguments) {
				const RuntimeValue* evaluatedArgument = evaluateASTNode(argument, scope); 
				printEvaluatedArgument(evaluatedArgument); 
			}
		}
		else if (funcValue->name == nativeFuncNames[1]) { // serveRaw
			for (auto* argument : funcCall->arguments) 
				std::cout << argument;
		}
		else if (funcValue->name == nativeFuncNames[2]) { // receive
			std::string input; 
			std::getline(std::cin, input); 

			StringValue* stringValue = globalMemory->create<StringValue>(); 

			stringValue->value = input; 
			result = stringValue; 
		}
		else if (funcValue->name == nativeFuncNames[3]) { // floor
			verifyArgumentCount(1); 

			const RuntimeValue* evaluatedArgument = evaluateASTNode(funcCall->arguments[0], scope);

			switch (evaluatedArgument->valueType) {
			case ValueType::Number:
			{
				const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
					evaluatedArgument
				); 

				NumValue* numberValue = globalMemory->create<NumValue>(); 

				numberValue->value = std::floor(evaluatedNumValue->value); 
				result = numberValue; 
			}
				break;
			}
		}
		else if (funcValue->name == nativeFuncNames[4]) { // ceil
			verifyArgumentCount(1);

			const RuntimeValue* evaluatedArgument = evaluateASTNode(funcCall->arguments[0], scope);

			switch (evaluatedArgument->valueType) {
			case ValueType::Number:
			{
				const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
					evaluatedArgument
				);

				NumValue* numberValue = globalMemory->create<NumValue>();

				numberValue->value = std::ceil(evaluatedNumValue->value);
				result = numberValue;
			}
				break;
			}
		}
		else if (funcValue->name == nativeFuncNames[5]) { // round
			verifyArgumentCount(1);

			const RuntimeValue* evaluatedArgument = evaluateASTNode(funcCall->arguments[0], scope);

			switch (evaluatedArgument->valueType) {
			case ValueType::Number:
			{
				const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
					evaluatedArgument
				);

				NumValue* numberValue = globalMemory->create<NumValue>();

				numberValue->value = std::round(evaluatedNumValue->value);
				result = numberValue;
			}
				break;
			}
		}
		else if (funcValue->name == nativeFuncNames[6]) { // int
			verifyArgumentCount(1);

			const RuntimeValue* evaluatedArgument = evaluateASTNode(funcCall->arguments[0], scope);

			switch (evaluatedArgument->valueType) {
			case ValueType::Number:
			{
				const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
					evaluatedArgument
				);

				NumValue* numberValue = globalMemory->create<NumValue>();

				numberValue->value = evaluatedNumValue->value;
				result = numberValue;
				break;
			}
			case ValueType::String:
			{
				const StringValue* evaluatedStringValue = dynamic_cast<const StringValue*>(
					evaluatedArgument
				);

				NumValue* numberValue = globalMemory->create<NumValue>(); 
				numberValue->value = 0; 
				result = numberValue; 

				try {
					numberValue->value = std::stod(evaluatedStringValue->value); 
				}
				catch (...) {}

				break;
			}
			}
		}
		else if (funcValue->name == nativeFuncNames[7]) { // bool
			verifyArgumentCount(1);

			const RuntimeValue* evaluatedArgument = evaluateASTNode(funcCall->arguments[0], scope);

			switch (evaluatedArgument->valueType) {
			case ValueType::Number:
			{
				const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
					evaluatedArgument
				);

				BoolValue* boolValue = globalMemory->create<BoolValue>();

				boolValue->state = evaluatedNumValue->value > 0;
				result = boolValue;
				break;
			}
			case ValueType::String:
			{
				const StringValue* evaluatedStringValue = dynamic_cast<const StringValue*>(
					evaluatedArgument
				);

				BoolValue* boolValue = globalMemory->create<BoolValue>();

				boolValue->state = evaluatedStringValue->value == "true"; 
				result = boolValue;
				break;
			}
			}
		}
		else if (funcValue->name == nativeFuncNames[8]) { // str
			verifyArgumentCount(1);

			const RuntimeValue* evaluatedArgument = evaluateASTNode(funcCall->arguments[0], scope);

			switch (evaluatedArgument->valueType) {
			case ValueType::Number:
			{
				const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
					evaluatedArgument
				);

				StringValue* stringValue = globalMemory->create<StringValue>();

				stringValue->value = std::to_string(evaluatedNumValue->value); 
				result = stringValue;
				break;
			}
			case ValueType::Bool:
			{
				const BoolValue* evaluatedBoolValue = dynamic_cast<const BoolValue*>(
					evaluatedArgument
				);

				StringValue* stringValue = globalMemory->create<StringValue>();

				stringValue->value= evaluatedBoolValue->state ? "true" : "false"; 
				result = stringValue;
				break;
			}
			}
		}
		else if (funcValue->name == nativeFuncNames[9]) { // pow
			verifyArgumentCount(2);

			const RuntimeValue* evaluatedArgument1 = evaluateASTNode(funcCall->arguments[0], scope);
			const RuntimeValue* evaluatedArgument2 = evaluateASTNode(funcCall->arguments[1], scope);

			if (evaluatedArgument1->valueType == ValueType::Number
				&& evaluatedArgument2->valueType == ValueType::Number)
			{
				const NumValue* evaluatedNumValue1 = dynamic_cast<const NumValue*>(
					evaluatedArgument1
				);
				const NumValue* evaluatedNumValue2 = dynamic_cast<const NumValue*>(
					evaluatedArgument2
				);

				NumValue* numberValue = globalMemory->create<NumValue>(); 

				numberValue->value = std::pow(
					evaluatedNumValue1->value, evaluatedNumValue2->value
				); 
				result = numberValue; 
			}
		}
		else if (funcValue->name == nativeFuncNames[10]) { // sqrt
			verifyArgumentCount(1);

			const RuntimeValue* evaluatedArgument = evaluateASTNode(funcCall->arguments[0], scope);

			if (evaluatedArgument->valueType == ValueType::Number) {
				const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
					evaluatedArgument
				);
		
				NumValue* numberValue = globalMemory->create<NumValue>();

				numberValue->value = std::sqrt(evaluatedNumValue->value); 
				result = numberValue;
			}
		}
		else if (funcValue->name == nativeFuncNames[11]) { // len
			verifyArgumentCount(1);

			const RuntimeValue* evaluatedArgument = evaluateASTNode(funcCall->arguments[0], scope);
			
			if (evaluatedArgument->valueType == ValueType::String) {
				const StringValue* evaluatedStringValue = dynamic_cast<const StringValue*>(
					evaluatedArgument
				);

				NumValue* numberValue = globalMemory->create<NumValue>();

				numberValue->value = evaluatedStringValue->value.size(); 
				result = numberValue;
			}
			else if (evaluatedArgument->valueType == ValueType::List) {
				const ListValue* evaluatedListValue = dynamic_cast<const ListValue*>(
					evaluatedArgument
				);

				NumValue* numberValue = globalMemory->create<NumValue>();

				numberValue->value = evaluatedListValue->elements.size(); 
				result = numberValue;
			}
		}
		else if (funcValue->name == nativeFuncNames[12]) { // append
			verifyArgumentCount(2);

			RuntimeValue* evaluatedArgument1 = evaluateASTNode(funcCall->arguments[0], scope);
			RuntimeValue* evaluatedArgument2 = evaluateASTNode(funcCall->arguments[1], scope);

			if (evaluatedArgument1->valueType == ValueType::String) {
				StringValue* evaluatedStringValue1 = dynamic_cast<StringValue*>(
					evaluatedArgument1
				);

				if (evaluatedArgument2->valueType == ValueType::String) {
					const StringValue* evaluatedStringValue2 = dynamic_cast<const StringValue*>(
						evaluatedArgument2
					);

					evaluatedStringValue1->value += evaluatedStringValue2->value; 
					result = evaluatedArgument1; 
				}
			}
			else if (evaluatedArgument1->valueType == ValueType::List) {
				ListValue* evaluatedListValue = dynamic_cast<ListValue*>(
					evaluatedArgument1
				);

				evaluatedListValue->elements.push_back(evaluatedArgument2); 
				result = evaluatedArgument1; 
			}
		}
		else if (funcValue->name == nativeFuncNames[13]) { // at
			verifyArgumentCount(2); 

			RuntimeValue* evaluatedArgument1 = evaluateASTNode(funcCall->arguments[0], scope);
			RuntimeValue* evaluatedArgument2 = evaluateASTNode(funcCall->arguments[1], scope);

			if (evaluatedArgument1->valueType == ValueType::String) {
				StringValue* evaluatedStringValue = dynamic_cast<StringValue*>(
					evaluatedArgument1
				);

				if (evaluatedArgument2->valueType == ValueType::Number) {
					const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
						evaluatedArgument2
					);

					StringValue* resultStringValue = globalMemory->create<StringValue>(); 

					resultStringValue->value = evaluatedStringValue->value[evaluatedNumValue->value]; 
					result = resultStringValue; 
				}
			}
			else if (evaluatedArgument1->valueType == ValueType::List) {
				ListValue* evaluatedListValue = dynamic_cast<ListValue*>(
					evaluatedArgument1
				);

				if (evaluatedArgument2->valueType == ValueType::Number) {
					const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
						evaluatedArgument2
					);

					return evaluatedListValue->elements[evaluatedNumValue->value]; 
				}
			}
		}
		else if (funcValue->name == nativeFuncNames[14]) { // set 
			verifyArgumentCount(3);

			if (funcCall->arguments[0]->nodeType == NodeType::Identifier) {
				const Identifier* identifier = dynamic_cast<const Identifier*>(
					funcCall->arguments[0]
				);
				
				// If the variable is const, try to assign a value to it to throw an error
				if (scope->isVariableConst(identifier->name)) 
					scope->assignVariable(identifier->name, nullptr); 
			}

			RuntimeValue* evaluatedArgument1 = evaluateASTNode(funcCall->arguments[0], scope);
			RuntimeValue* evaluatedArgument2 = evaluateASTNode(funcCall->arguments[1], scope);
			RuntimeValue* evaluatedArgument3 = evaluateASTNode(funcCall->arguments[2], scope);

			if (evaluatedArgument1->valueType == ValueType::String) {
				StringValue* evaluatedStringValue = dynamic_cast<StringValue*>(
					evaluatedArgument1
				);

				if (evaluatedArgument2->valueType == ValueType::Number) {
					const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
						evaluatedArgument2
					);

					if (evaluatedArgument3->valueType == ValueType::String) {
						const StringValue* evaluatedStringValue2 = dynamic_cast<StringValue*>(
							evaluatedArgument3
						);

						evaluatedStringValue->value[evaluatedNumValue->value] = 
							evaluatedStringValue2->value[0]; 
					}
				}
			}
			else if (evaluatedArgument1->valueType == ValueType::List) {
				ListValue* evaluatedListValue = dynamic_cast<ListValue*>(evaluatedArgument1);

				if (evaluatedArgument2->valueType == ValueType::Number) {
					const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
						evaluatedArgument2
					);

					evaluatedListValue->elements[evaluatedNumValue->value] = 
						cloneRuntimeValue(evaluatedArgument3); 
				}
			}
		}
		else if (funcValue->name == nativeFuncNames[15]) { // pop
			verifyArgumentCount(2); 

			if (funcCall->arguments[0]->nodeType == NodeType::Identifier) {
				const Identifier* identifier = dynamic_cast<const Identifier*>(
					funcCall->arguments[0]
				);

				// If the variable is const, try to assign a value to it to throw an error
				if (scope->isVariableConst(identifier->name))
					scope->assignVariable(identifier->name, nullptr);
			}

			RuntimeValue* evaluatedArgument1 = evaluateASTNode(funcCall->arguments[0], scope);
			RuntimeValue* evaluatedArgument2 = evaluateASTNode(funcCall->arguments[1], scope);

			if (evaluatedArgument1->valueType == ValueType::String) {
				StringValue* evaluatedStringValue = dynamic_cast<StringValue*>(
					evaluatedArgument1
				);

				if (evaluatedArgument2->valueType == ValueType::Number) {
					const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
						evaluatedArgument2
					);
					const int popIndex = evaluatedNumValue->value; 

					StringValue* resultStringValue = globalMemory->create<StringValue>(); 
					resultStringValue->value = evaluatedStringValue->value[popIndex]; 
					evaluatedStringValue->value.erase(
						evaluatedStringValue->value.begin() + popIndex
					);
					result = resultStringValue; 
				}
			}
			else if (evaluatedArgument1->valueType == ValueType::List) {
				ListValue* evaluatedListValue = dynamic_cast<ListValue*>(evaluatedArgument1);

				if (evaluatedArgument2->valueType == ValueType::Number) {
					const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
						evaluatedArgument2
					);
					const int popIndex = evaluatedNumValue->value; 

					result = evaluatedListValue->elements[popIndex]; 
					evaluatedListValue->elements.erase(
						evaluatedListValue->elements.begin() + popIndex
					); 
				}
			}
		}
		else if (funcValue->name == nativeFuncNames[16]) { // abs
			verifyArgumentCount(1); 

			const RuntimeValue* evaluatedArgument = evaluateASTNode(funcCall->arguments[0], scope); 

			if (evaluatedArgument->valueType == ValueType::Number) {
				const NumValue* evaluatedNumValue = dynamic_cast<const NumValue*>(
					evaluatedArgument
				); 
				
				NumValue* resultNumValue = globalMemory->create<NumValue>(); 

				resultNumValue->value = std::abs(evaluatedNumValue->value); 
				result = resultNumValue; 
			}
		}

		return result;	
	}
	RuntimeValue* evaluateAssignmentExpr(AssignmentExpr* assignmentExpr, Scope* scope) {
		if (assignmentExpr->assigne->nodeType != NodeType::Identifier)
			throw Error(
				Error::Location::Interpreter,
				6,
				"Invalid assigne referenced in assignment expression."
			); 

		const Identifier* identifier = dynamic_cast<const Identifier*>(assignmentExpr->assigne);
		return scope->assignVariable(
			identifier->name, cloneRuntimeValue(evaluateASTNode(assignmentExpr->value, scope))
		);
	}
	RuntimeValue* evaluateUnaryExprNode(UnaryExpr* unaryExpr, Scope* scope) {
		RuntimeValue* nodeEvaluated = evaluateASTNode(unaryExpr->expr, scope); 

		if (nodeEvaluated->valueType == ValueType::Number) {
			Identifier* identifier = dynamic_cast<Identifier*>(unaryExpr->expr);
			NumValue* numberValue = dynamic_cast<NumValue*>(
				scope->getVariableValue(identifier->name)
			);

			return evaluateNumericUnaryExpr(numberValue, unaryExpr->operation);
		}
		else if (nodeEvaluated->valueType == ValueType::Bool) {
			BoolValue* boolValue = dynamic_cast<BoolValue*>(nodeEvaluated); 

			return evaluateConditionalUnaryExpr(boolValue, unaryExpr->operation); 
		}

		return globalMemory->create<NullValue>(); 
	}
	RuntimeValue* evaluateNumericUnaryExpr(NumValue* numberValue, const std::string& operation) {
		double& result = numberValue->value; 

		if (operation == "-") {
			NumValue* numberValueResult = globalMemory->create<NumValue>(); 
			numberValueResult->value = -result;
			return numberValueResult; 
		}
		else if (operation == "++")
			result += 1.0;
		else if (operation == "--")
			result -= 1.0; 

		return numberValue; 
	}
	RuntimeValue* evaluateConditionalUnaryExpr(BoolValue* boolValue, const std::string& operation) {
		bool& state = boolValue->state; 

		if (operation == "!") {
			BoolValue* boolValueResult = globalMemory->create<BoolValue>(); 
			boolValueResult->state = !state; 
			return boolValueResult; 
		}

		return globalMemory->create<NullValue>();
	}
	RuntimeValue* evaluateBinaryExprNode(BinaryExpr* binaryExpr, Scope* scope) {
		RuntimeValue* leftNodeEvaluated = evaluateASTNode(binaryExpr->leftExpr, scope);
		RuntimeValue* rightNodeEvaluated = evaluateASTNode(binaryExpr->rightExpr, scope);

		const ValueType leftNodeValueType = leftNodeEvaluated->valueType; 
		const ValueType rightNodeValueType = rightNodeEvaluated->valueType; 

		// Evaluates numeric binary expressions
		if (leftNodeValueType == ValueType::Number && rightNodeValueType == ValueType::Number) {
			NumValue* leftNumValue = dynamic_cast<NumValue*>(leftNodeEvaluated);
			NumValue* rightNumValue = dynamic_cast<NumValue*>(rightNodeEvaluated);

			return evaluateNumericBinaryExpr(
				leftNumValue, rightNumValue, binaryExpr->operation
			);
		}
		// Evaluates conditional binary expressions
		else if (leftNodeValueType == ValueType::Bool && rightNodeValueType == ValueType::Bool) {
			BoolValue* leftBoolValue = dynamic_cast<BoolValue*>(leftNodeEvaluated); 
			BoolValue* rightBoolValue = dynamic_cast<BoolValue*>(rightNodeEvaluated); 

			return evaluateConditionalBinaryExpr(
				leftBoolValue, rightBoolValue, binaryExpr->operation
			);
		}
		// Evaluates string binary expressions
		else if (leftNodeValueType == ValueType::String 
			&& rightNodeValueType == ValueType::String) 
		{
			StringValue* leftStringValue = dynamic_cast<StringValue*>(leftNodeEvaluated); 
			StringValue* rightStringValue = dynamic_cast<StringValue*>(rightNodeEvaluated); 

			return evaluateStringBinaryExpr(
				leftStringValue, rightStringValue, binaryExpr->operation
			); 
		}

		return globalMemory->create<NullValue>();
	}
	RuntimeValue* evaluateNumericBinaryExpr(
		NumValue* leftNumValue, NumValue* rightNumValue, const std::string& operation)
	{
		RuntimeValue* result = globalMemory->create<NullValue>(); 

		if (operation == "+") {
			NumValue* numberResult = globalMemory->create<NumValue>(); 
			numberResult->value = leftNumValue->value + rightNumValue->value;
			result = numberResult; 
		}
		else if (operation == "-") {
			NumValue* numberResult = globalMemory->create<NumValue>();
			numberResult->value = leftNumValue->value - rightNumValue->value;
			result = numberResult;
		}
		else if (operation == "*") {
			NumValue* numberResult = globalMemory->create<NumValue>();
			numberResult->value = leftNumValue->value * rightNumValue->value;
			result = numberResult;
		}
		else if (operation == "/") {
			NumValue* numberResult = globalMemory->create<NumValue>();

			// Check if denominator is zero. If so, just return zero
			if (rightNumValue->value == 0.0) {
				numberResult = globalMemory->create<NumValue>();
				numberResult->value = 0.0;
			}
			else {
				numberResult = globalMemory->create<NumValue>();
				numberResult->value = leftNumValue->value / rightNumValue->value;
			}

			result = numberResult;
		}
		else if (operation == "%") {
			NumValue* numberResult = globalMemory->create<NumValue>();

			// Check if denominator is zero. If so, just return zero
			if (rightNumValue->value == 0.0) numberResult->value = 0.0;
			else numberResult->value = std::fmod(leftNumValue->value, rightNumValue->value);

			result = numberResult;
		} 
		else if (operation == "==") {
			BoolValue* boolResult = globalMemory->create<BoolValue>(); 
			boolResult->state = leftNumValue->value == rightNumValue->value;
			result = boolResult; 
		}
		else if (operation == "!=") {
			BoolValue* boolResult = globalMemory->create<BoolValue>(); 
			boolResult->state = leftNumValue->value != rightNumValue->value; 
			result = boolResult; 
		}
		else if (operation == ">") {
			BoolValue* boolResult = globalMemory->create<BoolValue>(); 
			boolResult->state = leftNumValue->value > rightNumValue->value; 
			result = boolResult; 
		}
		else if (operation == "<") {
			BoolValue* boolResult = globalMemory->create<BoolValue>(); 
			boolResult->state = leftNumValue->value < rightNumValue->value; 
			result = boolResult; 
		}
		else if (operation == ">=") {
			BoolValue* boolResult = globalMemory->create<BoolValue>(); 
			boolResult->state = leftNumValue->value >= rightNumValue->value; 
			result = boolResult; 
		}
		else if (operation == "<=") {
			BoolValue* boolResult = globalMemory->create<BoolValue>();
			boolResult->state = leftNumValue->value <= rightNumValue->value;
			result = boolResult;
		}
		else if (operation == "**") {
			NumValue* numberResult = globalMemory->create<NumValue>(); 
			numberResult->value = std::pow(leftNumValue->value, rightNumValue->value); 
			result = numberResult; 
		}

		return result;
	}
	RuntimeValue* evaluateConditionalBinaryExpr(
		BoolValue* leftBoolValue, BoolValue* rightBoolValue, const std::string& operation) 
	{
		BoolValue* result = globalMemory->create<BoolValue>(); 
		
		if (operation == "&&")
			result->state = leftBoolValue->state && rightBoolValue->state;
		else if (operation == "||")
			result->state = leftBoolValue->state || rightBoolValue->state; 
		 
		return result;
	}
	RuntimeValue* evaluateStringBinaryExpr(
		StringValue* leftStringValue, StringValue* rightStringValue, 
		const std::string& operation) 
	{
		RuntimeValue* result = globalMemory->create<NullValue>();

		if (operation == "+") {
			StringValue* stringResult = globalMemory->create<StringValue>();
		    stringResult->value = leftStringValue->value + rightStringValue->value;
			result = stringResult; 
		}
		else if (operation == "==") {
			BoolValue* boolResult = globalMemory->create<BoolValue>(); 
			boolResult->state = leftStringValue->value == rightStringValue->value; 
			result = boolResult; 
		}
		else if (operation == "!=") {
			BoolValue* boolResult = globalMemory->create<BoolValue>();
			boolResult->state = leftStringValue->value != rightStringValue->value;
			result = boolResult;
		}

		return result; 
	}
	RuntimeValue* evaluateIdentifier(Identifier* identifier, Scope* scope) {
		RuntimeValue* value = scope->getVariableValue(identifier->name); 
		return value; 
	}
}