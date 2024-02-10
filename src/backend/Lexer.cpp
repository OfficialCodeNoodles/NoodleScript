#include "../../hdr/backend/Lexer.hpp"
#include "../../hdr/util/Error.hpp"

namespace ns {
	TokenIdentifier TokenIdentifier::tokenIdentifiers[static_cast<int>(TokenType::Invalid)] = {
		{ {} },
		{ {} },
		{ {} },
		{ { "frozen" } },
		{ { "noodle" } },
		{ { "recipe" } },
		{ { "eat" } },
		{ { "if" } },
		{ { "else" } },
		{ { "while" } },
		{ { "=" } },
		{ { "++", "--", "!" } },
		{ { "+", "-", "*", "/", "%", "==", "!=", ">", "<", ">=", "<=", "**", "&&", "||" } },
		{ { "(" } },
		{ { ")" } },
		{ { "[" } },
		{ { "]" } }, 
		{ {} },
		{ { "#" } },
		{ { "\n", "\r", ";" } }
	};

	void processEscapeCharacters(std::string& string) {
		for (int chrIndex = 0; chrIndex < string.size(); chrIndex++) {
			const char chr = string[chrIndex]; 

			// Checks for potential escape characters if a slash is found
			if (chr == '\\' && chrIndex < string.size() - 1) {
				const char nextChr = string[chrIndex + 1]; 
				
				char replacementChr = ' ';

				// Replace specific characters with escape characters
				switch (nextChr) {
				case 'n':
					replacementChr = '\n'; 
					break; 
				case 't':
					replacementChr = '\t'; 
					break; 
				}

				if (replacementChr != ' ') {
					// Set the slash character to the escape character
					string[chrIndex] = replacementChr; 
					// Erase the extra character
					string.erase(string.begin() + chrIndex + 1); 
				}
			}
		}
	}
	void unprocessEscapeCharacters(std::string& string) {
		for (int chrIndex = 0; chrIndex < string.size(); chrIndex++) {
			const char chr = string[chrIndex]; 
			
			std::string insertionString; 

			// Replace specific escape characters with their visual text representation
			switch (chr) {
			case '\n':
				insertionString = "\\n"; 
				break; 
			case '\t':
				insertionString = "\\t"; 
				break; 
			}

			if (!insertionString.empty()) {
				// Erase the escape character
				string.erase(string.begin() + chrIndex); 
				// Insert new string
				string.insert(chrIndex, insertionString); 
				chrIndex++; 
			}
		}
	}
	std::string extractNextIdentifier(std::string& line, char seperator) {
		// Characters that are irrelevent to lexing, and are thus skipped over
		const char skippableChrs[] = { ' ', '\t', '\0' };
		// Charactes that cannot be chained with other characters
		const char nonChainableChrs[] = { '(', ')', '[', ']' };
		enum class IdentifierType { Alpha, Numeric, Symbolic, Unknown };

		auto isSkippableChar = [&skippableChrs](char chr) -> bool {
			for (auto skippableChr : skippableChrs) {
				if (skippableChr == chr)
					return true;
			}

			return false;
		};
		auto isUnchainableChar = [&nonChainableChrs](char chr) -> bool {
			for (auto nonChainableChr : nonChainableChrs) {
				if (nonChainableChr == chr)
					return true; 
			}

			return false; 
		}; 
		auto determineIdentifierType = [](char chr) -> IdentifierType {
			return (chr == '_' || (chr >= 'A' && chr <= 'Z') // _ A-Z a-z
					|| (chr >= 'a' && chr <= 'z')) ? IdentifierType::Alpha 
				: ((chr == '.'|| chr == '-' || (chr >= '0' && chr <= '9')  // . 0-9
					? IdentifierType::Numeric : IdentifierType::Symbolic)); 
		}; 

		IdentifierType identifierType = IdentifierType::Unknown; 
		std::string nextIdentifier;
		size_t nextIdentifierEnd = 0;
		bool isString = false; 
		bool isComment = false; 

		for (auto chr : line) {
			if (isComment) {
				nextIdentifierEnd++;

				if (chr == '\n')
					break;

				continue;
			}

			if (isUnchainableChar(chr)) {
				if (nextIdentifierEnd == 0) {
					nextIdentifierEnd++;
					nextIdentifier += chr;
				}

				break;
			}

			if (!isSkippableChar(chr) || isString) {
				if (!isComment) {
					if (chr == '\"') {
						if (isString) {
							nextIdentifierEnd++;
							break;
						}

						isString = true;
					}
					// Handle comment token
					else if (chr == '#') {
						isComment = true;

						if (nextIdentifier.size() > 0)
							break;
						else 
							nextIdentifierEnd++; 

						continue; 
					}

					if (isString) {
						nextIdentifierEnd++;
						nextIdentifier += chr;
						continue;
					}
				}

				IdentifierType currentIdentifierType = determineIdentifierType(chr); 

				if (identifierType == IdentifierType::Unknown)
					identifierType = currentIdentifierType; 
				else {
					if (identifierType == IdentifierType::Numeric) {
						// Handles -- operator
						if (nextIdentifier[0] == '-' && chr == '-') {
							nextIdentifierEnd++;
							nextIdentifier += chr;
							break;
						}
						else if (nextIdentifier.size() > 0 && (chr == '-' || currentIdentifierType 
								!= IdentifierType::Numeric))
							break;
					}

					if ((identifierType == IdentifierType::Symbolic
							&& currentIdentifierType != IdentifierType::Symbolic)
						|| (identifierType != IdentifierType::Symbolic
							&& (currentIdentifierType == IdentifierType::Symbolic || chr == '-')))
						break;
				}

				nextIdentifier += chr;
			}
			else if (nextIdentifier.size() > 0)
				break;

			nextIdentifierEnd++;
		}

		line = line.substr(nextIdentifierEnd);

		return nextIdentifier;
	}
	std::vector<std::string>& extractWords(const std::string& string) {
		static std::vector<std::string> words;

		words.clear();

		using WordPair = std::pair<std::string, std::string>;
		auto seperate = [](const std::string& string, char seperator) -> WordPair {
			const size_t seperatorIndex = string.find(seperator);
			return seperatorIndex == std::string::npos ? WordPair(string, "")
				: WordPair(
					string.substr(0, seperatorIndex),
					string.substr(seperatorIndex + 1)
				);
		};

		const char seperator = ' ';
		WordPair wordPair = seperate(string, seperator);

		// Keeps seperating words until empty
		while (wordPair.first != "") {
			words.push_back(wordPair.first);
			wordPair = seperate(wordPair.second, seperator);
		}

		return words;
	}
	Token determineTokenType(const std::string& nextIdentifier) {
		TokenType tokenType = static_cast<TokenType>(0);
		Token nextToken{ TokenType::Invalid, nextIdentifier};

		enum class TokenIdentifyMethod { Number, ReservedKeyword };
		const int firstChr = nextIdentifier[0];
		const int secondChr = nextIdentifier.size() > 1 ? nextIdentifier[1] : ' '; 

		// Immediately returns strings
		if (firstChr == '\"') return { TokenType::String, nextIdentifier  };

		const TokenIdentifyMethod tokenIdentificationMethod =
			(firstChr == '-' && std::isdigit(secondChr)) || std::isdigit(firstChr) 
				? TokenIdentifyMethod::Number :
					TokenIdentifyMethod::ReservedKeyword;

		switch (tokenIdentificationMethod) {
		case TokenIdentifyMethod::Number:
			nextToken.type = TokenType::Number; 
			goto ReturnToken;
		case TokenIdentifyMethod::ReservedKeyword:
			for (auto& tokenIdentifier : TokenIdentifier::tokenIdentifiers) {
				// Checks if identifier is reserved in the tokenIdentifiers array
				for (auto& identifier : tokenIdentifier.identifiers) {
					if (nextIdentifier == identifier) {
						nextToken = { tokenType, nextIdentifier };
						goto ReturnToken;
					}
				}

				tokenType = static_cast<TokenType>(static_cast<int>(tokenType) + 1);
			}

			nextToken.type = TokenType::Identifier; 
			break;
		}

	ReturnToken:
		return nextToken;
	}
	std::vector<Token>& tokenize(const std::string& sourceCode) {
		static std::vector<Token> tokens;

		tokens.clear();

		// New source string that be modified
		std::string remainingSource = sourceCode;
		
		// Inserts escape characters
		processEscapeCharacters(remainingSource); 

		// Continues processing tokens while source code still remains
		while (remainingSource.size() > 0) {
			std::string nextIdentifier = extractNextIdentifier(remainingSource);
			
			if (nextIdentifier.empty()) continue; 

			Token nextToken = determineTokenType(nextIdentifier);

			if (nextToken.type == TokenType::Invalid) {
				throw Error(
					Error::Location::Lexer,
					10,
					"Invalid identifier " + nextToken.string + " found in source code"
				); 
			} 

			tokens.push_back(nextToken);
		}

		if (tokens.size() > 0) {
			// Adds EOL token if not placed at end of tokens vector
			if (tokens.back().type != TokenType::EndOfLine)
				tokens.push_back({ TokenType::EndOfLine, "EOL" }); 
		}
		
		// Add EOF token to notify the parser when the tokens vector is over
		tokens.push_back({ TokenType::EndOfFile, "EOF" });
		return tokens;
	}

	std::ostream& operator<<(std::ostream& ostream, Token token) {
		std::string value = (token.type == TokenType::EndOfLine) ? "EOL" : token.string; 
		unprocessEscapeCharacters(value); 
		ostream << " { Value: " << value << ", Type: " 
			<< static_cast<int>(token.type) << " }\n"; 
		return ostream; 
	}
	std::ostream& operator<<(std::ostream& ostream, const std::vector<Token>& tokens) {
		for (auto& token : tokens)
			ostream << token; 
		return ostream;
	}
}