#ifndef _FONT_H
#define _FONT_H

#include "../core/String.hpp"


class Font {
private:
	String mName;
	int mSize;
public:
	Font();
	Font(String name);
	Font(String name, int size);
	Font(int size);
	int getSize();
	void setSize(int size);
	String getName();

	bool compare(const Font& f);

};
#endif
