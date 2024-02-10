#include "../../hdr/backend/AST.hpp"
#include "../../hdr/runtime/Values.hpp"

namespace ns {
	Program::Program() {
		nodeType = NodeType::Program; 
	}
	VarDeclaration::VarDeclaration() : expr(nullptr) {
		nodeType = NodeType::VarDeclaration; 
	}
	FuncDeclaration::FuncDeclaration() : nativeFunc(false) {
		nodeType = NodeType::FuncDeclaration; 
	}
	IfStatement::IfStatement() {
		nodeType = NodeType::IfStatement; 
	}
	WhileStatement::WhileStatement() {
		nodeType = NodeType::WhileStatement; 
	}

	NullLiteral::NullLiteral() {
		nodeType = NodeType::NullLiteral; 
	}
	NumLiteral::NumLiteral() {
		nodeType = NodeType::NumLiteral; 
	} 
	StringLiteral::StringLiteral() {
		nodeType = NodeType::StringLiteral; 
	}
	ListLiteral::ListLiteral() {
		nodeType = NodeType::ListLiteral; 
	}
	Identifier::Identifier() {
		nodeType = NodeType::Identifier; 
	}
	ListAccesser::ListAccesser() : index(0) {
		nodeType = NodeType::ListAccesser; 
	}
	UnaryExpr::UnaryExpr() {
		nodeType = NodeType::UnaryExpr; 
	}
	BinaryExpr::BinaryExpr() {
		nodeType = NodeType::BinaryExpr; 
	}
	AssignmentExpr::AssignmentExpr() {
		nodeType = NodeType::AssignmentExpr; 
	}
	FuncCall::FuncCall() {
		nodeType = NodeType::FuncCall; 
	}

	std::ostream& operator<<(std::ostream& ostream, const Statement* statement) {
		static int indentSize = 0; 
		static bool skipNodeEnd = false; 

		auto indent = [&]() -> void { 
			ostream << std::string(indentSize * 2, ' '); 
		};
		auto printNodeStart = [&](const std::string& nodeType, bool multiLine = false) -> void {
			ostream << (multiLine ? "{\n" : "{ "); 
			
			if (multiLine) {
				indentSize++;
				indent();
			}

			ostream << "NodeType: \"" << nodeType << "\"";
		}; 
		auto printAttribute = [&](const std::string& attribute, const std::string& value, 
			bool multiLine = false) -> void 
		{
			ostream << ","; 
			ostream << (multiLine ? "\n" : " ");
			if (multiLine) indent(); 
			ostream << attribute << ": " << value; 
		};
		auto printSubStatement = [&](const std::string& attribute, const Statement* statement, 
			bool multiLine = false) -> void 
		{
			ostream << ",";
			ostream << (multiLine ? "\n" : " ");
			if (multiLine) indent();
			ostream << attribute << ": " << statement;
		}; 
		auto printNodeEnd = [&](bool multiLine = false) -> void {
			if (skipNodeEnd) {
				ostream << " }"; 
				skipNodeEnd = false; 
				return; 
			}

			if (multiLine) {
				indentSize--;
				indent();
				ostream << "},\n";
			}
			else ostream << " },\n"; 
		};

		if (statement == nullptr) return ostream; 

		switch (statement->nodeType) {
		case NodeType::Program:
		{
			const Program* program = dynamic_cast<const Program*>(statement);

			printNodeStart("Program", true); 

			ostream << ",\n"; 
			indent(); 
			ostream << "Statements: [\n";
			indentSize++; 

			for (auto& statement : program->statements) {
				indent(); 
				ostream << statement;
			}

			indentSize--; 
			indent(); 
			ostream << "]\n"; 
			
			printNodeEnd(true);
			break; 
		}
		case NodeType::VarDeclaration:
		{
			const VarDeclaration* varDeclaration = dynamic_cast<const VarDeclaration*>(statement); 

			printNodeStart("NoodleDeclaration", true); 
			printAttribute("Constant", (varDeclaration->constant ? "true" : "false"), true); 
			printAttribute(
				"ValueType", std::to_string(static_cast<int>(varDeclaration->valueType)), true
			);
			printAttribute("Identifier", "\"" + varDeclaration->identifier + "\"", true);
			printSubStatement("Expr", varDeclaration->expr, true); 
			printNodeEnd(true); 
			break; 
		}
		case NodeType::FuncDeclaration:
		{
			const FuncDeclaration* funcDeclaration = dynamic_cast<const FuncDeclaration*>(statement); 
			
			printNodeStart("RecipeDeclaration", true); 
			printAttribute("Name", "\"" + funcDeclaration->name + "\"", true);
			
			ostream << ",\n"; 
			indent(); 
			ostream << "Parameters: [\n"; 
			indentSize++; 

			for (auto& parameter : funcDeclaration->parameters) {
				indent(); 
				ostream << "Name: \"" << parameter << "\",\n";
			}

			indentSize--;
			indent();
			ostream << "]";

			ostream << ",\n";
			indent();
			ostream << "Statements: [\n";
			indentSize++;
			
			for (auto& statement : funcDeclaration->statements) {
				indent(); 
				ostream << statement; 
			}
			
			indentSize--;
			indent();
			ostream << "]\n";

			printNodeEnd(true); 
			break; 
		}
		case NodeType::IfStatement:
		{
			const IfStatement* ifStatement = dynamic_cast<const IfStatement*>(statement); 

			printNodeStart("IfStatement", true); 
			printSubStatement("Condition", ifStatement->condition, true); 

			indent();
			ostream << "IfStatements: [\n";
			indentSize++;

			for (auto& statement : ifStatement->ifStatements) {
				indent();
				ostream << statement;
			}

			indentSize--;
			indent();
			ostream << "],\n";

			indent();
			ostream << "ElseStatements: [\n";
			indentSize++;

			for (auto& statement : ifStatement->elseStatements) {
				indent();
				ostream << statement;
			}

			indentSize--;
			indent();
			ostream << "]\n"; 

			printNodeEnd(true);
			break; 
		}
		case NodeType::WhileStatement:
		{
			const WhileStatement* whileStatement = dynamic_cast<const WhileStatement*>(statement); 

			printNodeStart("WhileStatement", true); 
			printSubStatement("Condition", whileStatement->condition, true); 

			indent();
			ostream << "Statements: [\n";
			indentSize++;

			for (auto& statement : whileStatement->statements) {
				indent();
				ostream << statement;
			}

			indentSize--;
			indent();
			ostream << "]\n";

			printNodeEnd(true); 
			break; 
		}
		case NodeType::NullLiteral:
		{
			const NullLiteral* nullLiteral = dynamic_cast<const NullLiteral*>(statement); 

			printNodeStart("NullLiteral");
			printNodeEnd(); 
			break; 
		}
		case NodeType::NumLiteral:
		{
			const NumLiteral* numLiteral = dynamic_cast<const NumLiteral*>(statement); 

			printNodeStart("NumLiteral");
			printAttribute("Value", std::to_string(numLiteral->value)); 
			printNodeEnd(); 
			break;
		}
		case NodeType::StringLiteral:
		{
			const StringLiteral* stringLiteral = dynamic_cast<const StringLiteral*>(statement); 

			std::string value = stringLiteral->value; 
			unprocessEscapeCharacters(value); 

			printNodeStart("StringLiteral"); 
			printAttribute("Value", value + "\"");
			printNodeEnd(); 
			break; 
		}
		case NodeType::ListLiteral:
		{
			const ListLiteral* listLiteral = dynamic_cast<const ListLiteral*>(statement); 

			printNodeStart("ListLiteral", true); 

			ostream << ",\n";
			indent();
			ostream << "Elements: [\n";
			indentSize++;

			for (auto& element : listLiteral->elements) {
				indent();
				ostream << element;
			}

			indentSize--;
			indent();
			ostream << "]";
			ostream << "\n";

			printNodeEnd(true); 
			break; 
		}
		case NodeType::Identifier:
		{
			const Identifier* identifier = dynamic_cast<const Identifier*>(statement);

			printNodeStart("Identifier");
			printAttribute("Name", "\"" + identifier->name + "\"");
			printNodeEnd();
			break;
		}
		case NodeType::ListAccesser:
		{
			const ListAccesser* listAccesser = dynamic_cast<const ListAccesser*>(statement); 
			
			printNodeStart("ListAccesser"); 
			printAttribute("Name", "\"" + listAccesser->name + "\""); 
			printAttribute("Index", std::to_string(listAccesser->index)); 
			printNodeEnd(); 
			break; 
		}
		case NodeType::UnaryExpr:
		{
			const UnaryExpr* unaryExpr = dynamic_cast<const UnaryExpr*>(statement); 

			printNodeStart("UnaryExpr", true);
			skipNodeEnd = true;
			printSubStatement("Expr", unaryExpr->expr, true); 
			printAttribute("Operator", "\"" + unaryExpr->operation + "\"", true);
			ostream << "\n";
			printNodeEnd(true); 
			break; 
		}
		case NodeType::BinaryExpr:
		{
			const BinaryExpr* binaryExpr = dynamic_cast<const BinaryExpr*>(statement); 

			printNodeStart("BinaryExpr", true);
			skipNodeEnd = true;
			printSubStatement("LeftExpr", binaryExpr->leftExpr, true);
			skipNodeEnd = true;
			printSubStatement("RightExpr", binaryExpr->rightExpr, true);
			printAttribute("Operator", "\"" + binaryExpr->operation + "\"", true);
			ostream << "\n";
			printNodeEnd(true);
			break; 
		}
		case NodeType::AssignmentExpr:
		{
			const AssignmentExpr* assignmentExpr = dynamic_cast<const AssignmentExpr*>(statement);

			printNodeStart("AssignmentExpr", true); 
			skipNodeEnd = true;
			printSubStatement("Assigne", assignmentExpr->assigne, true); 
			skipNodeEnd = true;
			printSubStatement("Value", assignmentExpr->value, true); 
			ostream << "\n";
			printNodeEnd(true); 
			break; 
		}
		case NodeType::FuncCall:
		{
			const FuncCall* funcCall = dynamic_cast<const FuncCall*>(statement); 

			printNodeStart("RecipeCall", true); 
			printAttribute("Name", funcCall->caller, true); 
			
			ostream << ",\n";
			indent();
			ostream << "Arguments: [\n";
			indentSize++;

			for (auto& argument : funcCall->arguments) {
				indent();
				ostream << argument; 
			}

			indentSize--;
			indent();
			ostream << "]";
			ostream << "\n"; 

			printNodeEnd(true);
		}
			break; 
		default:
			ostream << "Unrecognized statement " << static_cast<int>(statement->nodeType);
			ostream << "\n"; 
			break; 
		}

		return ostream; 
	}
}