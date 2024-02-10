#include "../../../hdr/runtime/eval/Statements.hpp"
#include "../../../hdr/runtime/eval/Expressions.hpp"
#include "../../../hdr/util/Error.hpp"

namespace ns {
	RuntimeValue* evaluateProgramNode(Program* program, Scope* scope) {
		RuntimeValue* lastNodeEvaluated = globalMemory->create<NullValue>(); 

		// Evaluate each statement and keep track of the previous runtime value
		for (auto& statement : program->statements) 
			lastNodeEvaluated = evaluateASTNode(statement, scope);

		return lastNodeEvaluated;
	}
	RuntimeValue* evaluateVarDeclarationNode(VarDeclaration* varDeclaration, Scope* scope) {
		RuntimeValue* value = varDeclaration->expr->nodeType != NodeType::NullLiteral
			? evaluateASTNode(varDeclaration->expr, scope) : globalMemory->create<NullValue>(); 
		return scope->declareVariable(varDeclaration->identifier, value, varDeclaration->constant);
	}
	RuntimeValue* evaluateFuncDeclaration(FuncDeclaration* funcDeclaration, Scope* scope) {
		FuncValue* funcValue = globalMemory->create<FuncValue>(); 

		funcValue->name = funcDeclaration->name; 
		funcValue->nativeFunc = funcDeclaration->nativeFunc; 
		funcValue->parameters = funcDeclaration->parameters; 
		funcValue->statements = funcDeclaration->statements;
		funcValue->scope = scope; 

		scope->declareVariable(funcValue->name, funcValue, true); 

		return funcValue; 
	}
	RuntimeValue* evaluateIfStatementNode(IfStatement* ifStatement, Scope* scope) {
		const RuntimeValue* evaluatedCondition = evaluateASTNode(ifStatement->condition, scope);

		RuntimeValue* result = globalMemory->create<NullValue>();

		// Make sure the evaluated condition is a bool
		if (evaluatedCondition->valueType == ValueType::Bool) {
			const BoolValue* condition = dynamic_cast<const BoolValue*>(evaluatedCondition);

			// If condition is true, evaluate its if statements
			if (condition->state) {
				for (auto& statement : ifStatement->ifStatements)
					result = evaluateASTNode(statement, scope);
			}
			// Otherwise, evaluate its else statements
			else {
				for (auto& statement : ifStatement->elseStatements)
					result = evaluateASTNode(statement, scope);
			}
		}

		return result; 
	}
	RuntimeValue* evaluateWhileStatementNode(WhileStatement* whileStatement, Scope* scope) {
		RuntimeValue* result = globalMemory->create<NullValue>(); 

		while (true) { 
			const RuntimeValue* evaluatedCondition = evaluateASTNode(whileStatement->condition, scope); 
			
			// Make sure the evaluated condition is a bool
			if (evaluatedCondition->valueType == ValueType::Bool) {
				const BoolValue* condition = dynamic_cast<const BoolValue*>(evaluatedCondition); 

				// If condition is true, evaluate its statements
				if (condition->state) {
					for (auto& statement : whileStatement->statements) 
						result = evaluateASTNode(statement, scope); 
				}
				else break; 
			}
		}

		return result;
	}
	RuntimeValue* evaluateASTNode(Statement* astNode, Scope* scope) {
		switch (astNode->nodeType) {
		case NodeType::Program:
		{
			Program* program = dynamic_cast<Program*>(astNode);
			return evaluateProgramNode(program, scope);
		}
		case NodeType::VarDeclaration:
		{
			VarDeclaration* varDeclaration = dynamic_cast<VarDeclaration*>(astNode);
			return evaluateVarDeclarationNode(varDeclaration, scope);
		}
		case NodeType::FuncDeclaration:
		{
			FuncDeclaration* funcDeclaration = dynamic_cast<FuncDeclaration*>(astNode); 
			return evaluateFuncDeclaration(funcDeclaration, scope); 
		}
		case NodeType::IfStatement:
		{
			IfStatement* ifStatement = dynamic_cast<IfStatement*>(astNode); 
			return evaluateIfStatementNode(ifStatement, scope); 
		}
		case NodeType::WhileStatement:
		{
			WhileStatement* whileStatement = dynamic_cast<WhileStatement*>(astNode); 
			return evaluateWhileStatementNode(whileStatement, scope); 
		}
		case NodeType::NullLiteral:
		{
			const NullLiteral* nullLiteral = dynamic_cast<const NullLiteral*>(astNode); 
			return globalMemory->create<NullValue>();
		}
		case NodeType::NumLiteral:
		{
			const NumLiteral* numLiteral = dynamic_cast<const NumLiteral*>(astNode);
			NumValue* numberValue = globalMemory->create<NumValue>();
			numberValue->value = numLiteral->value;
			return numberValue;
		}
		case NodeType::StringLiteral:
		{
			const StringLiteral* stringLiteral = dynamic_cast<const StringLiteral*>(astNode); 
			StringValue* stringValue = globalMemory->create<StringValue>(); 
			stringValue->value = stringLiteral->value.substr(1); 
			return stringValue; 
		}
		case NodeType::ListLiteral:
		{
			const ListLiteral* listLiteral = dynamic_cast<const ListLiteral*>(astNode); 
			ListValue* listValue = globalMemory->create<ListValue>(); 

			// Add each expression to the list once evaluated
			for (auto runtimeValue : listLiteral->elements)
				listValue->elements.push_back(evaluateASTNode(runtimeValue, scope)); 

			return listValue; 
		}
		case NodeType::FuncCall: 
		{
			FuncCall* funcCall = dynamic_cast<FuncCall*>(astNode); 
			return evaluateFuncCallExpr(funcCall, scope); 
		}
		case NodeType::Identifier:
		{
			Identifier* identifier = dynamic_cast<Identifier*>(astNode);
			return evaluateIdentifier(identifier, scope);
		}
		case NodeType::UnaryExpr:
		{
			UnaryExpr* unaryExpr = dynamic_cast<UnaryExpr*>(astNode); 
			return evaluateUnaryExprNode(unaryExpr, scope); 
		}
		case NodeType::BinaryExpr:
		{
			BinaryExpr* binaryExpr = dynamic_cast<BinaryExpr*>(astNode);
			return evaluateBinaryExprNode(binaryExpr, scope);
		}
		case NodeType::AssignmentExpr:
		{
			AssignmentExpr* assignmentExpr = dynamic_cast<AssignmentExpr*>(astNode);
			return evaluateAssignmentExpr(assignmentExpr, scope);
		}
		default:
			throw Error(
				Error::Location::Interpreter,
				5,
				"Unimplemented AST node " + std::to_string(static_cast<int>(astNode->nodeType)) 
					+ " found."
			);
		}
	}
}