#include "../../hdr/util/Memory.hpp"

namespace ns {
	Memory::Memory() {
	}
	Memory::~Memory() {
		clear(); 
	}

	void Memory::clear() {
		while (pointers.size() > 0) {
			delete pointers.back();
			pointers.pop_back();
		}
	}

	int Memory::getPointerCount() const {
		return pointers.size(); 
	}

	Memory* globalMemory = nullptr;
}