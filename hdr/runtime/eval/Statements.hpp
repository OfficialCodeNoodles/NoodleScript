#pragma once

// Dependencies
#include "../Scope.hpp"
#include "../../backend/Parser.hpp"

namespace ns {
	RuntimeValue* evaluateProgramNode(Program* program, Scope* scope);
	RuntimeValue* evaluateVarDeclarationNode(VarDeclaration* varDeclaration, Scope* scope);
	RuntimeValue* evaluateFuncDeclaration(FuncDeclaration* funcDeclaration, Scope* scope); 
	RuntimeValue* evaluateIfStatementNode(IfStatement* ifStatement, Scope* scope); 
	RuntimeValue* evaluateWhileStatementNode(WhileStatement* whileStatement, Scope* scope); 
	RuntimeValue* evaluateASTNode(Statement* astNode, Scope* scope);
}