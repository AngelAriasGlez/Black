#ifndef _ASSETS_H
#define _ASSETS_H



class Bitmap;
class ViewGroup;
class FtFont;

#include "../core/String.hpp"
#include <map>

class Asset {
public:
	String name;
	String data;
	Asset() {

	}
	Asset(std::string name) {
		load(name);
	}
	~Asset() {

	}
	bool load(std::string name);
};

class Assets{
private:
	static std::map<std::string, FtFont*> mFonts;
public:
	static Bitmap* getBitmap(std::string file);
	static ViewGroup* getLayout(std::string name);


};

#endif

