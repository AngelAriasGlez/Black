#ifndef _IMAGEVIEW_H
#define _IMAGEVIEW_H

#include "../../graphics/Bitmap.hpp"
#include "../../graphics/Canvas.hpp"

class ImageView : public View{
public:
	Bitmap *mBitmap;

	bool mKeepRatio;
	double mDegrees;

	bool mAutoFree;

	ImageView() : View(){
		initialize();
	}
	ImageView(Rect r) : View(r) {
		initialize();
	}
	~ImageView(){
		if (mAutoFree) {
			if (mBitmap)
				delete mBitmap;
			mBitmap = 0;
		}
	}
	virtual View *clone() const override { return new ImageView(*this); };

	void initialize(){
		mBitmap = NULL;
		mKeepRatio = true;
		setGravity(View::CENTER, View::CENTER);
		mDegrees = 0;
		mAutoFree = true;
	}
	void rotate(double degrees) {
		mDegrees = degrees;
	}

	void setBitmap(Bitmap *bitmap){
		mBitmap = bitmap;
	}

	void setAutoFree(bool val) {
		mAutoFree = val;
	}


	Bitmap *getBitmap(){
		return mBitmap;
	}
	void draw(Canvas *canvas){
		View::draw(canvas);
		if (mBitmap && mBitmap->Id){

			if (getWidth() <= 0 && getWidthMode() == View::WRAP){
				setWidth(mBitmap->Width);
			}
			if (getHeight() <= 0 && getHeightMode() == View::WRAP){
				setHeight(mBitmap->Height);
			}

			int width = getWidth() - getLeftPadding() - getRightPadding(); 
			int height = getHeight() - getTopPadding() - getBottomPadding();

			if (mKeepRatio) {
				height = mBitmap->Height / mBitmap->Width * width;
			}



			float left = getLeftPadding();
			if (width > getWidth()) {
				left = 0;
			}
			else if (getHorizontalGravity() == Gravity::CENTER){
				left = ((float)getWidth() / 2.0) - (width / 2.0);
			}
			else if (getHorizontalGravity() == Gravity::RIGHT){
				left = (getWidth() - getRightPadding()) - (width / 2.0);
			}


			float top = getTopPadding();
			if (height > getHeight()) {
				top = 0;
			}
			else if (getVerticalGravity() == Gravity::CENTER){
				top = ((float)getHeight() / 2.0) - (height / 2.0);
			}
			else if (getVerticalGravity() == Gravity::BOTTOM){
				top = (Utils::min(getHeight(), height) - getBottomPadding()) - (height / 2.0);
			}
			//resetProjection(canvas);
			canvas->drawBitmap(mBitmap, Rect(left, top, width, height), mDegrees);
		}
	}
};

#endif
