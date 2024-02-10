#pragma once

// Depdencencies
#include <string>
#include <vector>
#include <iostream>

namespace ns {
	enum class TokenType {
		Number,
		Bool, 
		String,
		Const, 
		Var, 
		Func, 
		EndStatement,  
		If,
		Else,
		While, 
		Equals,
		UnaryOperator, 
		BinaryOperator,
		OpenParen,
		CloseParen,
		OpenBracket, 
		CloseBracket, 
		Identifier,
		Comment, 
		EndOfLine, 
		EndOfFile,
		Invalid
	};
	struct TokenIdentifier {
		std::vector<std::string> identifiers; 

		static TokenIdentifier tokenIdentifiers[static_cast<int>(TokenType::Invalid)];
	};
	struct Token {
		TokenType type; 
		std::string string; 
	};

	void processEscapeCharacters(std::string& string); 
	void unprocessEscapeCharacters(std::string& string); 
	std::string extractNextIdentifier(std::string& line, char seperator = ' ');
	std::vector<std::string>& extractWords(const std::string& string);
	Token determineTokenType(const std::string& nextIdentifier);
	std::vector<Token>& tokenize(const std::string& sourceCode);

	std::ostream& operator<<(std::ostream& ostream, Token token); 
	std::ostream& operator<<(std::ostream& ostream, const std::vector<Token>& tokens);
}