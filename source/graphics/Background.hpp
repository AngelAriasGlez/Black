#ifndef _BACKGROUND_H
#define _BACKGROUND_H

#include "Bitmap.hpp"
#include "Color.hpp"

class Background{
private:
	Bitmap *mBitmap;
	Color mColor;
	bool mSolid;
	int mRadius = 0;
public:
	Background(){
		mSolid = false;
		mBitmap = NULL;
	}
	Background(Color color){
		mColor = color;
		mSolid = true;
		mBitmap = NULL;
	}
	Background(Bitmap *bitmap){
		mBitmap = bitmap;
		mSolid = false;
	}
	void setColor(Color color){
		mColor = color;
		mSolid = true;
	}
	void setBitmap(Bitmap *bitmap){
		mBitmap = bitmap;
	}

	Color getColor(){
		return mColor;
	}

	Bitmap *getBitmap(){
		return mBitmap;
	}

	bool isSolid(){
		return mSolid;
	}
	bool isBitmap(){
		return (mBitmap && mBitmap->Id);
	}

	void setRadius(int radius) {
		mRadius = radius;
	}
	int getRadius() {
		return mRadius;
	}

};


#endif
