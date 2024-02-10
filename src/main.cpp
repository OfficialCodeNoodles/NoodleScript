#include "../hdr/Terminal.hpp"

int main(int argc, const char** argv) {
	ns::initTerminal(argc, argv); 

	while (ns::isTerminalOpen())
		ns::updateTerminal(); 

	ns::closeTerminal(); 
	return 0; 
}