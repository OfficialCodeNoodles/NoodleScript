#include "../../hdr/runtime/Values.hpp"
#include "../../hdr/util/Memory.hpp"

namespace ns {
	const char* nativeFuncNames[numOfNativeFuncs] = {
		"serve", 
		"serveRaw", 
		"receive",
		"floor",
		"ceil", 
		"round", 
		"num", 
		"bool", 
		"str",
		"pow",
		"sqrt",
		"len", 
		"append",
		"at", 
		"set", 
		"pop",
		"abs"
	};

	NullValue::NullValue() {
		valueType = ValueType::Null; 
	}
	NumValue::NumValue() {
		valueType = ValueType::Number; 
	}
	NumValue::NumValue(double value) : value(value) {
		valueType = ValueType::Number; 
	}
	BoolValue::BoolValue() {
		valueType = ValueType::Bool; 
	}
	BoolValue::BoolValue(bool state) : state(state) {
		valueType = ValueType::Bool; 
	}
	StringValue::StringValue() {
		valueType = ValueType::String; 
	}
	StringValue::StringValue(const std::string& value) : value(value) {
		valueType = ValueType::String; 
	}
	ListValue::ListValue() {
		valueType = ValueType::List; 
	}
	FuncValue::FuncValue() {
		valueType = ValueType::FuncValue; 
	}

	RuntimeValue* cloneRuntimeValue(RuntimeValue* runtimeValue) {
		switch (runtimeValue->valueType) {
		case ValueType::Number:
		{
			const NumValue* numValue = dynamic_cast<const NumValue*>(runtimeValue);
			NumValue* clonedNumValue = globalMemory->create<NumValue>();
			clonedNumValue->value = numValue->value;
			return clonedNumValue;
		}
		case ValueType::Bool:
		{
			const BoolValue* boolValue = dynamic_cast<const BoolValue*>(runtimeValue);
			BoolValue* clonedBoolValue = globalMemory->create<BoolValue>();
			clonedBoolValue->state = boolValue->state;
			return clonedBoolValue;
		}
		case ValueType::String:
		{
			const StringValue* stringValue = dynamic_cast<const StringValue*>(runtimeValue);
			StringValue* clonedStringValue = globalMemory->create<StringValue>();
			clonedStringValue->value = stringValue->value;
			return clonedStringValue;
		}
		default:
			return runtimeValue;
		}
	}

	std::ostream& operator<<(std::ostream& ostream, const RuntimeValue* runtimeValue) {
		switch (runtimeValue->valueType) {
		case ValueType::Null:
		{
			const NullValue* nullValue = dynamic_cast<const NullValue*>(runtimeValue);

			ostream << " { ValueType: Null, Value: " << nullValue->value;
			ostream << " }\n";
			break; 
		}
		case ValueType::Number:
		{
			const NumValue* numberValue = dynamic_cast<const NumValue*>(runtimeValue);

			ostream << " { ValueType: Number, Value: " << numberValue->value; 
			ostream << " }\n";
			break; 
		}
		case ValueType::Bool:
		{
			const BoolValue* boolValue = dynamic_cast<const BoolValue*>(runtimeValue); 

			ostream << " { ValueType: Bool, Value: " << (boolValue->state ? "true" : "false");
			ostream << " }\n";
			break; 
		}
		case ValueType::String:
		{
			const StringValue* stringValue = dynamic_cast<const StringValue*>(runtimeValue); 

			ostream << " { ValueType: String, Value: " << stringValue->value;
			ostream << " }\n";
			break;
		}
		case ValueType::List:
		{
			const ListValue* listValue = dynamic_cast<const ListValue*>(runtimeValue); 

			ostream << " { ValueType: List }\n"; 
			break; 
		}
		case ValueType::FuncValue:
		{
			const FuncValue* funcValue = dynamic_cast<const FuncValue*>(runtimeValue); 

			ostream << " { ValueType: FuncValue, Name: " << funcValue->name << " }\n";
			break; 
		}
		}

		return ostream; 
	}
}