#include "../../hdr/util/Error.hpp"

namespace ns {
	Error::Error() :
		location(Location::Lexer),
		code(-1)
	{
	}
	Error::Error(Location location, int code, std::string message) :
		location(location),
		code(code),
		message(message) 
	{
	}

	const std::string Error::locations[static_cast<int>(Location::End)] = {
		"Lexer", "Parser", "Interpreter"
	}; 

	std::ostream& operator<<(std::ostream& ostream, const Error& error) {
		ostream << Error::locations[static_cast<int>(error.location)];
		ostream << " Error (" << error.code << "): "; 
		ostream << error.message << "\n"; 
		return ostream; 
	}
}