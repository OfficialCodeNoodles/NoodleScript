#include "../../hdr/backend/Parser.hpp"
#include "../../hdr/util/Error.hpp"
#include "../../hdr/runtime/Values.hpp"

namespace ns {
	Parser::Parser() : 
		tokens(nullptr),
		prvsToken({ TokenType::EndOfLine })
	{
	}

	std::vector<Token>* Parser::lexSourceCode(const std::string& sourceCode) {
		tokens = &tokenize(sourceCode); 
		return tokens; 
	}
	Program* Parser::produceAST() {
		if (tokens == nullptr)
			throw Error(
				Error::Location::Parser,
				1, 
				"Cannot create AST without generating tokens first."
			);

		Program* program = new Program(); 

		// Continues parsing program until EOF
		while (!atEOF()) {
			Statement* statement = parseStatement();

			// Don't add empty statements to program
			if (statement != nullptr)
				program->statements.push_back(statement);
		}

		return program; 
	}

	Statement* Parser::parseStatement() {
		switch (getToken().type) {
		case TokenType::Var: // noodle ...
			return parseVarDeclaration();
		case TokenType::Const: // frozen noodle ...
			return parseVarDeclaration();
		case TokenType::Func: // recipe ...
			return parseFuncDeclaration();
		case TokenType::If: // if ...
			return parseIfStatement();
		case TokenType::While: // while ...
			return parseWhileStatement();
		default:
			// Parse everything else
			return parseExpr();
		}
	}
	Statement* Parser::parseVarDeclaration() {
		const Token firstToken = popToken(); 
		const bool isConstant = firstToken.type == TokenType::Const; 

		auto determineValueType = [](NodeType nodeType) -> ValueType {
			switch (nodeType) {
			case NodeType::NumLiteral:
				return ValueType::Number;
			default:
				return ValueType::Null; 
			}
		}; 

		const Token typeToken = isConstant ? expectToken(TokenType::Var) : firstToken; 
		const std::string identifier = expectToken(TokenType::Identifier).string; 
		
		// Stops evaluation early if EOF token found
		if (getToken().type == TokenType::EndOfLine) {
			popToken(); 

			if (isConstant) 
				throw "Must assign a value to constant."; 

			VarDeclaration* varDeclaration = globalMemory->create<VarDeclaration>();
			
			varDeclaration->constant = isConstant; 
			varDeclaration->valueType = ValueType::Null; 
			varDeclaration->identifier = identifier; 
			varDeclaration->expr = globalMemory->create<NullLiteral>();
			
			return varDeclaration; 
		}

		// Continue evaluation
		expectToken(TokenType::Equals); 

		VarDeclaration* varDeclaration = globalMemory->create<VarDeclaration>(); 
		
		varDeclaration->constant = isConstant; 
		varDeclaration->identifier = identifier; 
		varDeclaration->expr = parseExpr(); 
		varDeclaration->valueType = determineValueType(varDeclaration->expr->nodeType);

		expectToken(TokenType::EndOfLine); 
		return varDeclaration;
	}
	Statement* Parser::parseFuncDeclaration() {
		popToken();

		FuncDeclaration* funcDeclaration = globalMemory->create<FuncDeclaration>(); 

		funcDeclaration->name = expectToken(TokenType::Identifier).string;

		expectToken(TokenType::OpenParen);

		// Enum that handles the state of the parameter parsing
		enum class ParameterPosition { Specifier, Identifier, End } 
			parameterPosition = ParameterPosition::Specifier;  
		std::string parameter; 

		while (true) {
			Token nextToken = getToken(); 

			if (nextToken.type == TokenType::CloseParen) {
				popToken(); 
				break;
			}
			else if (nextToken.type == TokenType::EndOfFile) {
				popToken();
				throw Error(
					Error::Location::Parser,
					15,
					"Parameter list for recipe " + funcDeclaration->name + " incomplete"
				);
			}

			switch (parameterPosition) {
			case ParameterPosition::Specifier:
				expectToken(TokenType::Var); 
				break; 
			case ParameterPosition::Identifier:
				nextToken = expectToken(TokenType::Identifier);
				parameter = nextToken.string; 
				funcDeclaration->parameters.push_back(parameter); 
				break;
			}

			// Increments the parameter position
			parameterPosition = static_cast<ParameterPosition>(
				(static_cast<int>(parameterPosition) + 1) 
					% static_cast<int>(ParameterPosition::End)
			); 
		}

		std::vector<Statement*>& statements = funcDeclaration->statements; 

		// Parse body statements until EOF or eat token is found
		while (!atEOF() && getToken().type != TokenType::EndStatement) {
			Statement* statement = parseStatement();

			if (statement != nullptr)
				statements.push_back(statement);
		}
		
		expectToken(TokenType::EndStatement); 
		return funcDeclaration; 
	}
	Statement* Parser::parseIfStatement() {
		popToken(); 

		IfStatement* ifStatement = globalMemory->create<IfStatement>(); 

		ifStatement->condition = parseExpr(); 

		std::vector<Statement*>& ifStatements = ifStatement->ifStatements; 

		// Parse statements until EOF, eat, or else token is found
		while (!atEOF() && getToken().type != TokenType::EndStatement
			&& getToken().type != TokenType::Else)
		{
			Statement* statement = parseStatement(); 

			if (statement != nullptr)
				ifStatements.push_back(statement); 
		}

		// Check for eat token
		if (getToken().type == TokenType::EndStatement) {
			popToken(); 
			return ifStatement;
		}

		// Otherwise, continue with else statement
		expectToken(TokenType::Else);

		std::vector<Statement*>& elseStatements = ifStatement->elseStatements; 

		// Parse statements until EOF or eat token is found
		while (!atEOF() && getToken().type != TokenType::EndStatement) {
			Statement* statement = parseStatement();

			if (statement != nullptr)
				elseStatements.push_back(statement);
		}

		expectToken(TokenType::EndStatement); 
		return ifStatement; 
	}
	Statement* Parser::parseWhileStatement() {
		popToken(); 

		WhileStatement* whileStatement = globalMemory->create<WhileStatement>(); 
		 
		whileStatement->condition = parseExpr(); 

		std::vector<Statement*>& statements = whileStatement->statements; 

		// Parse statements until EOF or eat token is found
		while (!atEOF() && getToken().type != TokenType::EndStatement) {
			Statement* statement = parseStatement(); 

			if (statement != nullptr)
				statements.push_back(statement); 
		}

		expectToken(TokenType::EndStatement); 
		return whileStatement; 
	}

	Expr* Parser::parseExpr() {
		return parseAssignmentExpr(); 
	}
	Expr* Parser::parseAssignmentExpr() {
		Expr* leftExpr = parseConditionalExpr(); 

		if (getToken().type == TokenType::Equals) {
			popToken(); 
			
			Expr* value = parseAssignmentExpr(); 
			AssignmentExpr* assignmentExpr = globalMemory->create<AssignmentExpr>(); 
			
			assignmentExpr->assigne = leftExpr; 
			assignmentExpr->value = value; 
			
			return assignmentExpr; 
		}

		return leftExpr; 
	}
	Expr* Parser::parseConditionalExpr() {
		Expr* leftExpr = parsePrimaryConditionalExpr(); 

		const std::string tokenString = getToken().string; 

		if (tokenString == "&&" || tokenString == "||") {
			const std::string operation = popToken().string;

			Expr* rightExpr = parseConditionalExpr(); 
			BinaryExpr* binaryExpr = globalMemory->create<BinaryExpr>(); 

			binaryExpr->leftExpr = leftExpr; 
			binaryExpr->rightExpr = rightExpr; 
			binaryExpr->operation = operation; 
			leftExpr = binaryExpr; 
		}

		return leftExpr;
	}
	Expr* Parser::parsePrimaryConditionalExpr() {
		if (getToken().string == "!") {
			const std::string operation = popToken().string; 

			Expr* expr = parsePrimaryConditionalExpr(); 
			UnaryExpr* unaryExpr = globalMemory->create<UnaryExpr>(); 

			unaryExpr->expr = expr; 
			unaryExpr->operation = operation; 
			return unaryExpr; 
		}

		Expr* leftExpr = parseAdditiveExpr();

		const std::string tokenString = getToken().string;

		if (tokenString == "==" || tokenString == "!=" || tokenString == ">" || tokenString == "<"
			|| tokenString == ">=" || tokenString == "<=")
		{
			const std::string operation = popToken().string;

			Expr* rightExpr = parsePrimaryConditionalExpr();
			BinaryExpr* binaryExpr = globalMemory->create<BinaryExpr>();

			binaryExpr->leftExpr = leftExpr;
			binaryExpr->rightExpr = rightExpr;
			binaryExpr->operation = operation;
			leftExpr = binaryExpr;
		}

		return leftExpr;
	}
	Expr* Parser::parseAdditiveExpr() {
		Expr* leftExpr = parseMultiplicativeExpr(); 

		if (getToken().string == "+" || getToken().string == "-") {
			const std::string operation = popToken().string; 

			Expr* rightExpr = parseMultiplicativeExpr(); 
			BinaryExpr* binaryExpr = globalMemory->create<BinaryExpr>(); 
			
			binaryExpr->leftExpr = leftExpr; 
			binaryExpr->rightExpr = rightExpr; 
			binaryExpr->operation = operation; 
			leftExpr = binaryExpr;
		}

		return leftExpr; 
	}
	Expr* Parser::parseMultiplicativeExpr() {
		Expr* leftExpr = parseAdditiveUnaryExpr();

		if (getToken().string == "*" || getToken().string == "/" || getToken().string == "%"
			|| getToken().string == "**")
		{
			const std::string operation = popToken().string;

			Expr* rightExpr = parseAdditiveUnaryExpr();
			BinaryExpr* binaryExpr = globalMemory->create<BinaryExpr>(); 
			
			binaryExpr->leftExpr = leftExpr;
			binaryExpr->rightExpr = rightExpr;
			binaryExpr->operation = operation;
			leftExpr = binaryExpr;
		}

		return leftExpr;
	}
	Expr* Parser::parseAdditiveUnaryExpr() {
		Expr* expr; 

		if (getToken().string == "++" || getToken().string == "--") {
			const std::string operation = popToken().string; 

			UnaryExpr* unaryExpr = globalMemory->create<UnaryExpr>(); 
			Identifier* identifier = globalMemory->create<Identifier>(); 

			identifier->name = expectToken(TokenType::Identifier).string;
			unaryExpr->expr = identifier; 
			unaryExpr->operation = operation; 
			expr = unaryExpr; 
		}
		else if (getToken().type == TokenType::Identifier && (getToken(1).string == "++"
			|| getToken(1).string == "--"))
		{
			const std::string identifierName = popToken().string; 

			UnaryExpr* unaryExpr = globalMemory->create<UnaryExpr>();
			Identifier* identifier = globalMemory->create<Identifier>();

			identifier->name = identifierName; 
			unaryExpr->expr = identifier;
			unaryExpr->operation = popToken().string; 
			expr = unaryExpr;
		}
		else 
			expr = parseFuncCallExpr(); 

		return expr; 
	}
	Expr* Parser::parseFuncCallExpr() {
		if (prvsToken.type != TokenType::Func
			&& getToken().type == TokenType::Identifier
			&& getToken(1).type == TokenType::OpenParen)
		{
			FuncCall* funcCall = globalMemory->create<FuncCall>();

			funcCall->caller = popToken().string;
			popToken();

			// Parse arguments until EOF or closed paren token is found
			while (!atEOF() && getToken().type != TokenType::CloseParen) 
				funcCall->arguments.push_back(parseExpr()); 

			expectToken(TokenType::CloseParen); 

			return funcCall; 
		}

		return parsePrimaryExpr();
	}
	Expr* Parser::parsePrimaryExpr() {
		const Token token = getToken();

		switch (token.type) {
		case TokenType::Number:
		{
			NumLiteral* numLiteral = globalMemory->create<NumLiteral>(); 
			numLiteral->value = std::stod(popToken().string); 
			return numLiteral; 
		}
		case TokenType::String:
		{
			StringLiteral* stringLiteral = globalMemory->create<StringLiteral>(); 
			stringLiteral->value = popToken().string; 
			return stringLiteral; 
		}
		case TokenType::BinaryOperator:
			if (token.string == "-") {
				popToken(); 
		
				if (getToken().type == TokenType::Identifier) {
					UnaryExpr* unaryExpr = globalMemory->create<UnaryExpr>(); 
					unaryExpr->expr = parseExpr(); 
					unaryExpr->operation = "-"; 
					return unaryExpr; 
				}
			}
		
			goto UnexpectedToken;
		case TokenType::OpenParen:
		{
			popToken(); 
			Expr* value = parseExpr();
			expectToken(TokenType::CloseParen); 
			return value; 
		}
		case TokenType::OpenBracket:
		{
			popToken(); 
			ListLiteral* listLiteral = globalMemory->create<ListLiteral>(); 

			// Parse elements until EOF or closed bracket token is found
			while (!atEOF() && getToken().type != TokenType::CloseBracket) 
				listLiteral->elements.push_back(parseExpr()); 

			expectToken(TokenType::CloseBracket); 
			return listLiteral; 
		}
		case TokenType::Identifier:
		{
			const std::string identifierName = popToken().string; 

			if (getToken().type == TokenType::OpenBracket) {
				ListAccesser* listAccessor = globalMemory->create<ListAccesser>(); 

				listAccessor->name = identifierName; 
				
				const Expr* insideExpr = parseExpr(); 
				
				if (insideExpr->nodeType == NodeType::NumLiteral) {
					const NumLiteral* indexExpr = dynamic_cast<const NumLiteral*>(insideExpr); 
					listAccessor->index = indexExpr->value; 
				}

				expectToken(TokenType::CloseBracket);
				return listAccessor; 
			} 
				
			Identifier* identifier = globalMemory->create<Identifier>();
			identifier->name = identifierName; 
			return identifier; 
		}
		case TokenType::EndOfLine:
			popToken(); 
			return nullptr;
		default:
UnexpectedToken:
			throw Error(
				Error::Location::Parser,
				12,
				"Unexpected token " + getToken().string + " found during parsing." 
			); 
		}
	}

	Token Parser::getToken(int index) const {
		return (*tokens)[index]; 
	}
	Token Parser::popToken(int index) {
		prvsToken = getToken(index); 
		tokens->erase(tokens->begin() + index); 
		return prvsToken; 
	}
	Token Parser::expectToken(TokenType tokenType, int index) {
		const Token token = popToken(index); 

		if (token.type != tokenType) {
			throw Error(
				Error::Location::Parser,
				12,
				"Unexpected token " + getToken().string + " found during parsing."
			);
		}

		return token; 
	}
	bool Parser::atEOF() const {
		return getToken().type == TokenType::EndOfFile; 
	}
}