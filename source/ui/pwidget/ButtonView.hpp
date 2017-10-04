#ifndef _ADJ_BUTTONVIEW_H
#define _ADJ_BUTTONVIEW_H

#include "TextView.hpp"
#include "../../utils/Assets.hpp"
#include "ImageView.hpp"

class ButtonView : public TextView{
private:
	Bitmap *mImage;
	Bitmap *mActiveImage;

	bool inc;
	int channel;

public:
	ButtonView() : TextView(){
		initialize();
	}
	ButtonView(Rect r) : TextView(r) {
		initialize();
	}
	virtual View *clone() const override { return new ButtonView(*this); };

	void initialize(){
		setColor(Color::WHITE);
		setClickColor(Color(50, 50, 50, 255));
		setActiveColor(Color("#0b0c0e"));


		setText("");
		setGravity(Gravity::CENTER, Gravity::CENTER);

		setBackground(Color("#303033"));
		setClickBackground(Color("#232424"));
		setActiveBackground(Color("#66e6ff"));

		//setBackground(Assets::getBitmap("button2.9.png"));
		//setClickBackground(Assets::getBitmap("button_amber.9.png"));
		//setActiveBackground(Assets::getBitmap("button_green.9.png"));

		updateBackground();

		mImage = NULL;
		mActiveImage = NULL;
	}
	virtual std::string toString() {
		return "ButtonView::" + TextView::toString();
	}

	void onInitialize(){
		View::onInitialize();
	}
	void setBitmap(Bitmap* bitmap) {
		mImage = bitmap;
	}
	void setActiveBitmap(Bitmap* bitmap) {
		mActiveImage = bitmap;
	}


	void onPositionChanged() {

	}


	void draw(Canvas *canvas) override{


		/*auto ctx = canvas->getNVGContext();

		auto cornerRadius = 2;
		auto mPushed = isTouchDown();
		auto borderlight = Color("#353636");
		auto borderdark = Color("#181a1a");

		auto bgtop = Color("#242525");
		auto bgbot = Color("#1c1d1d");

		Position p;
		Dimension d = mBounds.dimension;

		nvgBeginPath(ctx);
		nvgRoundedRect(ctx, p.x + 1, p.y + 1.0f, d.width - 2, d.height - 2, cornerRadius - 1);

		NVGpaint bg = nvgLinearGradient(ctx, p.x, p.y, p.x, d.width + d.height, bgtop, bgbot);

		nvgFillPaint(ctx, bg);
		nvgFill(ctx);

		nvgBeginPath(ctx);
		nvgStrokeWidth(ctx, 1.0f);
		nvgRoundedRect(ctx, p.x + 0.5f, p.y + (mPushed ? 0.5f : 1.5f), d.width - 1, d.height - 1 - (mPushed ? 0.0f : 1.0f), cornerRadius);
		nvgStrokeColor(ctx, borderlight);
		nvgStroke(ctx);

		nvgBeginPath(ctx);
		nvgRoundedRect(ctx, p.x + 0.5f, p.y + 0.5f, d.width - 1, d.height - 2, cornerRadius);
		nvgStrokeColor(ctx, borderdark);
		nvgStroke(ctx);*/



		TextView::draw(canvas);



		/*if (mBlink && mBackground.getBitmap()) {
			if (channel > 254) inc = false;
			if (channel < 150) inc = true;
			if (inc) channel += 10;
			if (!inc) channel -= 10;

			mBackground.getBitmap()->setColorMask(Color(channel, channel, channel, 255));
		}
		else {
			inc = true;
			channel = 254;
			mBackground.getBitmap()->setColorMask(Color(255, 255, 255, 255));
		}*/


		//mImage->setBounds(getBounds());
		//mActiveImage->setBounds(getBounds());
		if (isVisible()) {
			if ((isTouchDown() || mActive) && mActiveImage) {
				drawImage(canvas, mActiveImage);
			}
			else if (mImage) {
				drawImage(canvas, mImage);
			}
		}

	
	}

	void drawImage(Canvas* canvas, Bitmap* b) {
	
		int w = getWidth() - getLeftPadding() - getRightPadding();
		int h = getHeight() - getTopPadding() - getBottomPadding();
		int l = getLeftPadding();
		int t = getTopPadding();

		if (w > h) {
			int wx = b->Height / b->Width * h;
			l += (w - wx) / 2;
			w = wx;
		}
		else {
			int hx = b->Height / b->Width * w;
			t += (h - hx) / 2;
			h = hx;
		}
		canvas->drawBitmap(b, Rect(l, t, w, h), 0);
	
	}

	bool onTouchEvent(TouchEvent e) {

		return true;
	}
};

#endif
