#if defined(_WIN64) | defined(_WIN32)
#include "Platform.hpp"
#include "Application.hpp"



int main(void) {

	if (Platform::create()){
		return Platform::start();
	}
}


#endif
