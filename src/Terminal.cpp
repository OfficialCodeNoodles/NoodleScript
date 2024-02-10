#include "../hdr/Terminal.hpp"

#include <fstream>

namespace ns {
	Interpreter interpreter;
	std::string sourceCode;
	std::string terminalInput;
	bool terminalOpen = false;
	bool insideConsole = false;
	bool jumpToConsole = false;

	void initNoodleScriptMsg() {
		std::cout << "\nNoodleScript Terminal - Created by CodeNoodles (2024)\n\n";

		std::cout << " @@@@@@@@@@@@@@@@@@@@@\n";
		std::cout << " @@@@@@@@@@@@@@@@@@@@@\n"; 
		std::cout << " @@@@@@@@@@@@@@@@@@@@@\n";
		std::cout << " @@@@@@@@@@@@@@@@@@@@@\n";
		std::cout << " @@@@@@@@@@@@@@@@@@@@@\n";
		std::cout << " @@@@@@@@ @@@ @@@   @@\n";
		std::cout << " @@@@@@@@  @@ @@  @@@@\n";
		std::cout << " @@@@@@@@ @ @ @@@  @@@\n";
		std::cout << " @@@@@@@@ @@  @@@@  @@\n";
		std::cout << " @@@@@@@@ @@@ @@   @@@\n";
		std::cout << " @@@@@@@@@@@@@@@@@@@@@\n\n";

		std::cout << "type `help` to view a list of commands.\n\n";
	}
	void initTerminalMsg() {
		std::cout << "\nNoodleScript Terminal\n\n";
		insideConsole = false;
	}
	void initConsoleMsg() {
		std::cout << "\nNoodleScript Console\n\n";
		insideConsole = true;
	}

	void prepareTerminalMsg() {
		std::cout << "> ";
	}
	void prepareConsoleMsg() {
		if (!jumpToConsole)
			std::cout << ">>> ";
	}

	void executeTerminal(const std::string& input) {
		const std::vector<std::string> words = extractWords(input);
		const int wordCount = words.size();

		if (wordCount == 0)
			return;

		if (words[0] == "help") {
			std::cout << "\nHere are a list of commands:\n\n"
				" console -> Enters a console where code can be written\n"
				" terminal -> Reenters the terminal\n"
				" run [string] -> Runs a .ns script, if the file path is given\n"
				" set -> Allows for the modification of enviroment variables\n"
				"     logTokens [true | false] -> Specifies whether lexed tokens should be outputed\n"
				"     logAST [true | false] -> Specifies whether to output the AST\n"
				"     logRuntimeValue [true | false] -> Specifies whether to ouput the program value\n"
				" show -> Shows the state of certain enviroment variables\n"
				"     logTokens -> Shows whether lexed tokens should be outputed\n"
				"     logAST -> Shows whether to output the AST\n"
				"     logRuntimeValue -> Shows whether to ouput the program value\n"
				"     globalMemorySize -> Shows how many runtime values are currently allocated\n"
				" clear -> Clears the terminal\n"
				" exit -> Closes the terminal\n\n";
		}
		else if (words[0] == "console")
			initConsoleMsg();
		else if (words[0] == "run") {
			if (wordCount < 2)
				return;

			runSourceFile(words[1]); 
		}
		else if (words[0] == "set") {
			if (wordCount < 3)
				return;

			const bool condition = words[2] == "true";

			if (words[1] == "logTokens")
				interpreter.enableTokenLogging(condition);
			else if (words[1] == "logAST")
				interpreter.enableASTLogging(condition);
			else if (words[1] == "logRuntimeValue")
				interpreter.enableRuntimeValueLogging(condition);
		}
		else if (words[0] == "show") {
			auto showEnviromentVariable = [](const std::string& variableName,
				const std::string& value) -> void
			{
				std::cout << " Enviroment variable `" << variableName << "` is currently set to " 
					<< value << "\n"; 
			};

			if (wordCount < 2)
				return; 

			if (words[1] == "logTokens")
				showEnviromentVariable(
					"shouldLogTokens", interpreter.isTokenLoggingEnabled() ? "true" : "false"
				); 
			else if (words[1] == "logAST")
				showEnviromentVariable(
					"shouldLogAST", interpreter.isASTLoggingEnabled() ? "true" : "false"
				);
			else if (words[1] == "logRuntimeValue")
				showEnviromentVariable(
					"shouldLogRuntimeValue", interpreter.isRuntimeValueLoggingEnabled() 
						? "true" : "false"
				);
			else if (words[1] == "globalMemorySize") {
				const int allocatedRuntimeValues = globalMemory->getPointerCount();
				std::cout << " Currently, there are " << allocatedRuntimeValues
					<< " runtimeValues allocated\n"; 
			}
		}
		else if (words[0] == "clear") {
			// Clears terminal in Windows
			system("cls");
			initNoodleScriptMsg();
		}
	}
	void executeConsole(const std::string& input) {
		if (input == "terminal") {
			initTerminalMsg();
			return;
		}

		if (sourceCode == "")
			sourceCode = input;

		interpreter.generateProgram(sourceCode);
		interpreter.runProgram();

		sourceCode = "";
		jumpToConsole = false;
	}

	void initTerminal(int argc, const char** argv) {
		terminalOpen = true;

		// Attempts running a source file if an extra parameter is passed into the command prompt
		if (argc == 2) {
			const std::string filepath = argv[1];
			runSourceFile(filepath); 
		}
		else 
			initNoodleScriptMsg();
	}
	bool isTerminalOpen() {
		return terminalOpen; 
	}
	void runSourceFile(const std::string& filepath) {
		insideConsole = true;

		std::ifstream sourceFile(filepath);
		std::string line;

		while (std::getline(sourceFile, line)) {
			if (line.empty())
				continue;

			// Adds \n to ensure an EOL token is lexed
			sourceCode += line + " \n";
		}

		sourceFile.close();
		initConsoleMsg();
		jumpToConsole = true;
	}
	void updateTerminal() {
		if (insideConsole) prepareConsoleMsg();
		else prepareTerminalMsg();

		// Skips input handling when a script is ready to be run
		if (!jumpToConsole) {
			std::getline(std::cin, terminalInput);

			// Exits terminal when `exit` is typed
			if (terminalInput == "exit") {
				terminalOpen = false;
				return; 
			}
		}

		if (insideConsole) executeConsole(terminalInput);
		else executeTerminal(terminalInput);
	}
	void closeTerminal() {
	}
}