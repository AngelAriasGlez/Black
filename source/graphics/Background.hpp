#ifndef _BACKGROUND_H
#define _BACKGROUND_H

#include "Bitmap.hpp"
#include "Color.hpp"
#include "LinearGradient.hpp"

class Background{
private:
	Color mStartColor;
	Color mEndColor;
	//double mColorAngle = 0;
	double mGradientMiddle = 0.5;

	bool mGradientSetted = false;
	bool mSolidSetted = false;

	int mCornerRadius = 0;
	int mBorderWidth = 0;
	Color mBorderColor;
public:
	Background(){
		mStartColor.a = 0;
		mEndColor.a = 0;
	}
	~Background() {

	}
	Background(Color color){
		setSolidColor(color);
	}
	Background(Color start, Color end) {
		setGradient(start, end);
	}
	void setSolidColor(Color color){
		mSolidSetted = true;
		mStartColor = color;
	}
	void setGradient(Color start, Color end){
		mGradientSetted = true;
		mStartColor = start;
		mEndColor = end;
	}

	Color getSolidColor() {
		return mStartColor;
	}

	Color getGradientStartColor(){
		return mStartColor;
	}
	Color getGradientEndColor() {
		return mEndColor;
	}
	/*void setAngle(double angle) {
		mColorAngle = angle;
	}
	double getAngle() {
		return mColorAngle;
	}*/

	void setGradientMiddle(double angle) {
		mGradientMiddle = angle;
	}
	double getGradientMiddle() {
		return mGradientMiddle;
	}


	bool isSolidSetted() {
		return mSolidSetted;
	}
	bool isGradientSetted() {
		return mGradientSetted;
	}

	void setCornerRadius(int radius) {
		mCornerRadius = radius;
	}
	int getCornerRadius() {
		return mCornerRadius;
	}
	void setBorderWidth(int w){
		mBorderWidth = w;
	}
	void setBorderColor(Color c) {
		mBorderColor = c;
	}
	int getBorderWidth() {
		return mBorderWidth;
	}
	Color getBorderColor() {
		return mBorderColor;
	}


};


#endif
