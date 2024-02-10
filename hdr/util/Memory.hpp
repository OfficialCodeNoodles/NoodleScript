#pragma once

// Dependencies
#include <vector>

namespace ns {
	class Memory {
	public:
		Memory(); 
		~Memory(); 

		template <typename Type> 
		Type* create() {
			Type* pointer = new Type();
			pointers.push_back(pointer);
			return pointer;
		}
		void clear(); 

		int getPointerCount() const;
	private:
		std::vector<void*> pointers; 
	};

	extern Memory* globalMemory; 
}