#include "String.hpp"


String operator+(const String& a, const String& b) { //outside the class
	return String(a.mString + b.mString);
}
String operator+(const String& a, const  char* b) { //outside the class
	return String(a.mString + b);
}
String operator+(const char* a, const  String& b) { //outside the class
	return String(a + b.mString);
}

String& String::operator+=(const String& a) { //outside the class
	mString += a.mString;
	return *this;
}
String& String::operator+=(const char* a) { //outside the class
	mString += a;
	return *this;
}


/*std::ostream& operator<<(std::ostream& ost, const std::string& ls)
{
	ost << ls.mstd::string;
	return ost;
}*/


