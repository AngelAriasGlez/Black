#ifndef _STRINGSTREAM_H
#define _STRINGSTREAM_H

#include <sstream>


class StringStream : public std::stringstream{
public:
	operator std::string() const;

};



#endif