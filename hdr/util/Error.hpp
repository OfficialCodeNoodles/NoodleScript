#pragma once

// Dependencies
#include <iostream>

namespace ns {
	class Error {
	public:
		enum class Location { Lexer, Parser, Interpreter, End } location;
		int code; 
		std::string message; 

		Error();
		Error(Location location, int code, std::string message); 

		friend std::ostream& operator<<(std::ostream& ostream, const Error& error);
	private:
		static const std::string locations[static_cast<int>(Location::End)]; 
	};

	std::ostream& operator<<(std::ostream& ostream, const Error& error); 
}