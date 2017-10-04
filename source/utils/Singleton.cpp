#include "Singleton.hpp"

Singleton* Singleton::pinstance = 0;
Singleton* Singleton::getInstance () {   
	if (pinstance == 0){     
		pinstance = new Singleton; 
	}   
	return pinstance; 
} 
Singleton::Singleton() { }
