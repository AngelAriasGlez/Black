#ifndef _COLOR_H
#define _COLOR_H

#include "../core/String.hpp"
#include "../nanovg/nanovg.h"
class Color {
public:


	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	Color() :
		r(255),
		g(255),
		b(255),
		a(255){};
	Color(unsigned char red, unsigned char green, unsigned char blue) :
		r(red),
		g(green),
		b(blue),
		a(255){};
	Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) :
		r(red),
		g(green),
		b(blue),
		a(alpha){};
	Color(std::string color){
		parseHex(color);
	};

	float fr(){
		return 1. / 255. * r;
	}
	float fg(){
		return 1. / 255. * g;
	}
	float fb(){
		return 1. / 255. * b;
	}
	float fa(){
		return 1. / 255. * a;
	}

	void parseHex(std::string str){
		
		if (str.substr(0, 1) == "#"){
			str = str.substr(1, str.size());
		}

		if (str.size() <= 4) {
			if(str.size() == 4) str.insert(3, 1, str.at(3));
			str.insert(2, 1, str.at(2));
			str.insert(1, 1, str.at(1));
			str.insert(0, 1, str.at(0));
		}


		unsigned int intcolor;
		std::stringstream ss;
		ss << std::hex << str.data();
		ss >> intcolor;


		if (str.size() == 8){
			r = ((intcolor >> 24) & 0xFF);
			g = ((intcolor >> 16) & 0xFF);
			b = ((intcolor >> 8) & 0xFF);
			a = ((intcolor)& 0xFF);
		}
		else{
			r = ((intcolor >> 16) & 0xFF);
			g = ((intcolor >> 8) & 0xFF);
			b = ((intcolor)& 0xFF);
			a = 255;
		}


	}






	static Color WHITE;
	static Color BLACK;
	static Color RED;
	static Color GREEN;
	static Color BLUE;

	static Color YELLOW;
	static Color CYAN;
	static Color MAGENTA;
	static Color SILVER;
	static Color GRAY;
	static Color MAROON;
	static Color OLIVE;
	static Color PURPLE;
	static Color TEAL;
	static Color NAVY;

	static Color random() {
		return Color(rand() % 255, rand() % 255, rand() % 255, 255);
	}
	static Color aRandom() {
		return Color(rand() % 255, rand() % 255, rand() % 255, rand() % 255);
	}


	operator const NVGcolor () const {
		NVGcolor c;
		auto ca = *this;
		c.a = ca.fa();
		c.b = ca.fb();
		c.g = ca.fg();
		c.r = ca.fr();
		return c;
	}
};


#endif
