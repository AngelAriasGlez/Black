#include "Font.hpp"
#include "../graphics/Metrics.hpp"


Font::Font() {
	mSize = Metrics::dpToPx(10);
	mName = "sans";
	}
Font::Font(int size) {
	mName = "sans";
	mSize = size;
}
Font::Font(String name) {
	mName = name;
	mSize = Metrics::dpToPx(10);
}
Font::Font(String name, int size) {
		mName = name;
		mSize = size;
	}
	int Font::getSize() {
		return mSize;
	}
	void Font::setSize(int size) {
		mSize = size;
	}
	String Font::getName() {
		return mName;
	}

	bool Font::compare(const Font& f) {
		return f.mName != mName || f.mSize != mSize;
	
	}



