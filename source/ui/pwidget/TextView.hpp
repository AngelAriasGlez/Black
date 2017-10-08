#ifndef _ADJ_TEXTVIEW_H
#define _ADJ_TEXTVIEW_H

#include "../../graphics/Metrics.hpp"
#include "../../graphics/Canvas.hpp"
#include "../View.hpp"
#include "../../graphics/Font.hpp"
#include <time.h>

class TextView : public View{
protected:
	String mText;

	//TODO: Make Template for store this
	Background mClickBackground;
	Background mNormalBackground;
	Background mActiveBackground;
	Background mBlinkBackground;

	Color mColor;
	Color mNormalColor;
	Color mClickColor;
	Color mActiveColor;
	Color mDisabledColor;
	Color mBlinkColor;
    
	bool mActive;

	Font mFont;

	int mLeft;
	int mTop;


	bool mBlink;
	bool mBlinkActive;
	int mBlinkPrevTime;
public:

	TextView(): View(){
		initialize();
	}
	TextView(Rect r) : View(r) {
		initialize();
	}
	virtual std::string toString() {
		return "TextView::" + View::toString();
	}
	virtual View *clone() const override { return new TextView(*this); };

	Font getFont() {
		return mFont;
	}
	void setFont(Font font) {
		if (mFont.compare(font)) {
			mFont = font;
			invalidate();
		}
	}

	void initialize(){
		setGravity(Gravity::LEFT, Gravity::CENTER);

		mActive = false;
		mBlink = false;
		mBlinkActive = false;

		mBlinkColor = Color::WHITE;
		mBlinkBackground = Color::RED;

		//mClickBackground = Color::GREEN;

		mBlinkPrevTime = 0;

		mRetained = true;
	}
	void setText(String text) {
		if (text != mText) {
			mText = text;
			invalidate();
		}
	}

	String getText() {
		return mText;
	}



	void setActive(bool active){
		if (mActive != active) {
			mActive = active;
			invalidate();
		}
	}
	bool isActive(){
		return mActive;
	}


    void setColor(Color color){
		mNormalColor = color;
		invalidate();
    }
	void setActiveColor(Color color){
		mActiveColor = color;
		invalidate();
	}
	void setClickColor(Color color){
		mClickColor = color;
		invalidate();
	}
	void setDisabledColor(Color color) {
		mDisabledColor = color;
		invalidate();
	}
	void setBlinkColor(Color color) {
		mBlinkColor = color;
	}

	void setBlink(bool blink) {
		mBlinkActive = blink;
		if (!blink && mBlink) {
			mBlink = false;
			invalidate();
		}
	}


	void setActiveBackground(Background bg){
		//bg.getBitmap()->setColorMask(Color(0, 255, 0, 255));
		mActiveBackground = bg;
	}
	void setClickBackground(Background bg){
		mClickBackground = bg;
	}
	void setBackground(Background bg){
		mNormalBackground = bg;
	}

	void setBlinkBackground(Background bg) {
		mBlinkBackground = bg;
	}

	void updateBackground(){
		if (isTouchDown()){
			View::setBackground(mClickBackground);
		}
		else if (mBlink) {
			View::setBackground(mBlinkBackground);
		}
		else if (mActive) {
			View::setBackground(mActiveBackground);
		}
		else{
			View::setBackground(mNormalBackground);
		}
		mBackground.setCornerRadius(mNormalBackground.getCornerRadius());
	}
	void updateColor(){
		if (!isEnabled()) {
			mColor = mDisabledColor;
		}else if (isTouchDown()){
			mColor = mClickColor;
		}
		else if (mBlink) {
			mColor = mBlinkColor;
		}
		else if (mActive){
			mColor = mActiveColor;
		}
		else{
			mColor = mNormalColor;
		}
	}



	bool onTouchEvent(TouchEvent& e){
		if (e.type == TouchEvent::DOWN){
			invalidate();
		}
		else if (e.type == TouchEvent::UP){
			invalidate();
		}
		return View::onTouchEvent(e);
	}



	void calc(Canvas* canvas) {



		float bounds[4];
		canvas->calculateFontBounds(mFont, mText, bounds);

		int xsize = bounds[2];
		int ysize = bounds[3];

		mLeft = getLeftPadding();
		if (getHorizontalGravity() == Gravity::CENTER) {
			mLeft = (getWidth() / 2) - (xsize / 2);
		}
		else if (getHorizontalGravity() == Gravity::RIGHT) {
			mLeft = (getWidth() - getRightPadding()) - xsize;
		}

		mTop = getTopPadding();
		if (getVerticalGravity() == Gravity::CENTER) {
			mTop = (getHeight() / 2) - (ysize/2);
		}
		else if (getVerticalGravity() == Gravity::BOTTOM) {
			mTop = (getHeight() - getBottomPadding()) - ysize;
		}



	}
    virtual void draw(Canvas* canvas) override{
        updateBackground();

        updateColor();
		View::draw(canvas);

            
        calc(canvas);
		//canvas->drawQuad(Position(), this->getDimension(), 0, Color::aRandom());
		canvas->drawFont(Point(mLeft, mTop), mText, mFont, mColor);
        
    }

	virtual void update(Canvas *canvas)  override {
		if (isVisible()) {
			if (mBlinkActive) {
				int time = clock() - mBlinkPrevTime;
				if (time > 500) {
					mBlink = !mBlink;
					invalidate();
					mBlinkPrevTime = clock();
				}
			}
		}
	}
};

#endif
