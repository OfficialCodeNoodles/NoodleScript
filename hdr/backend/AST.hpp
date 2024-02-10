#pragma once

// Dependencies
#include "Lexer.hpp"

namespace ns {
	enum class NodeType {
		// Statements
		Program,
		VarDeclaration,
		FuncDeclaration,
		IfStatement,
		WhileStatement,

		// Expressions
		NullLiteral,
		NumLiteral,
		StringLiteral,
		ListLiteral,
		Identifier,
		ListAccesser, 
		UnaryExpr, 
		BinaryExpr,
		AssignmentExpr, 
		FuncCall
	};
	enum class ValueType {
		Null,
		Number,
		Bool,
		String,
		List, 
		FuncValue
	};

	struct Statement {
		NodeType nodeType; 

		virtual ~Statement() {}
	};
	struct Expr : public Statement {};

	struct Program : public Statement {
		std::vector<Statement*> statements; 

		Program(); 
	};
	struct VarDeclaration : public Statement {
		bool constant; 
		ValueType valueType; 
		std::string identifier; 
		Expr* expr; 

		VarDeclaration(); 
	};
	struct FuncDeclaration : public Statement {
		std::string name; 
		bool nativeFunc; 
		std::vector<std::string> parameters; 
		std::vector<Statement*> statements; 

		FuncDeclaration(); 
	};
	struct IfStatement : public Statement {
		Expr* condition; 
		std::vector<Statement*> ifStatements;
		std::vector<Statement*> elseStatements; 

		IfStatement(); 
	};
	struct WhileStatement : public Statement {
		Expr* condition; 
		std::vector<Statement*> statements; 

		WhileStatement(); 
	};

	struct NullLiteral : public Expr {
		NullLiteral(); 
	};
	struct NumLiteral : public Expr {
		double value; 

		NumLiteral(); 
	};
	struct StringLiteral : public Expr {
		std::string value; 

		StringLiteral();
	};
	struct ListLiteral : public Expr {
		std::vector<Expr*> elements; 

		ListLiteral(); 
	};
	struct Identifier : public Expr {
		std::string name; 

		Identifier(); 
	};
	struct ListAccesser : public Expr {
		std::string name; 
		int index; 

		ListAccesser(); 
	};
	struct UnaryExpr : public Expr {
		Expr* expr; 
		std::string operation; 

		UnaryExpr(); 
	};
	struct BinaryExpr : public Expr {
		Expr* leftExpr;
		Expr* rightExpr;
		std::string operation;

		BinaryExpr(); 
	};
	struct AssignmentExpr : public Expr {
		Expr* assigne; 
		Expr* value; 

		AssignmentExpr(); 
	};
	struct FuncCall : public Expr {
		std::string caller; 
		std::vector<Expr*> arguments; 

		FuncCall(); 
	};

	std::ostream& operator<<(std::ostream& ostream, const Statement* statement); 
}