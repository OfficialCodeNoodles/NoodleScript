#pragma once

// Dependencies
#include "../util/Memory.hpp"
#include "AST.hpp"

namespace ns {
	class Parser {
	public:
		std::vector<Token>* tokens; 

		Parser(); 

		std::vector<Token>* lexSourceCode(const std::string& sourceCode); 
		Program* produceAST(); 
	private:
		Token prvsToken; 

		Statement* parseStatement(); 
		Statement* parseVarDeclaration(); 
		Statement* parseFuncDeclaration();
		Statement* parseIfStatement(); 
		Statement* parseWhileStatement(); 

		Expr* parseExpr(); 
		Expr* parseAssignmentExpr(); 
		Expr* parseConditionalExpr(); 
		Expr* parsePrimaryConditionalExpr(); 
		Expr* parseAdditiveExpr();
		Expr* parseMultiplicativeExpr(); 
		Expr* parseAdditiveUnaryExpr(); 
		Expr* parseFuncCallExpr(); 
		Expr* parsePrimaryExpr();

		Token getToken(int index = 0) const; 
		Token popToken(int index = 0); 
		Token expectToken(TokenType tokenType, int index = 0);
		bool atEOF() const; 
	};
}