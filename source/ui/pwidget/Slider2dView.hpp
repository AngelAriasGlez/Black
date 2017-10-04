#ifndef _ADJ_SLIDER2D_H
#define _ADJ_SLIDER2D_H

#include "../View.hpp"
#include "BasicView.hpp"

#include <algorithm>

#include "utils/Utils.hpp"
#include "../../graphics/Metrics.hpp"
#include "../../graphics/Canvas.hpp"

class Slider2dView :public BasicView, View::ITouchListener{
public:
	class IProgressListener {
	public:
		virtual void onProgressListener(Slider2dView* slider, double progressx, double progressy, bool fromUser) = 0;
		virtual void onDragListener(Slider2dView* slider, bool drag) = 0;
	};
private:
	int mThumbStartX;
	int mThumbStartY;
	bool mDrag;

	double mProgressX;
	double mProgressY;

	IProgressListener *mProgressListener;
public:

	Slider2dView(int width, int height): BasicView(width, height){
		mProgressListener = NULL;


		mDrag = false;
		mThumbStartX = 0;
		mThumbStartY = 0;

		mProgressX = 0;
		mProgressY = 0;

		//addView(mThumb);
	}

	~Slider2dView(){

	}


	bool onTouchEvent(TouchEvent& e){
		if (e.type == 0 || e.type == 2) {
			if (e.type == 0) {
				mDrag = true;
				if (mProgressListener) {
					mProgressListener->onDragListener(this, true);
				}
			}
			mProgressX = Utils::clamp(1. / getWidth() * e.x, 0, 1);
			mProgressY = Utils::clamp(1. / getHeight() * e.y, 0, 1);

			if (mProgressListener) {
				mProgressListener->onProgressListener(this, mProgressX, mProgressY, true);
			}
		}
		else if (e.type == 1) {
			mDrag = false;
			if (mProgressListener) {
				mProgressListener->onDragListener(this, false);
			}
		}
		
		return true;
	}


	void onDoubleClickListener(std::string label, View* view, TouchEvent& e)  {
		mProgressX = 0;
		mProgressY = 0;
		if (mProgressListener) {
			mProgressListener->onProgressListener(this, mProgressX, mProgressY, true);
		}
	}


	int getProgressX() {
		return mProgressX;
	}
	int getProgressY() {
		return mProgressY;
	}

	void setProgress(double valuex, double valuey) {
		mProgressX = Utils::clamp(valuex, 0., 1.);
		mProgressY = Utils::clamp(valuey, 0., 1.);

		if (mProgressListener) {
			mProgressListener->onProgressListener(this, mProgressX, mProgressY, true);
		}
	}

	void setOnProgressListener(IProgressListener *callback){
		mProgressListener = callback;
	}
	void draw(Canvas *canvas){
		BasicView::draw(canvas);
		int border = 4;
		canvas->drawQuad(Position(), getDimension(), border, Color("#333333"));

		int thumbsize = Metrics::dpToPx(15);
		int x = ((double)(getWidth() - (thumbsize * 2) - border) / 1. * mProgressX) + thumbsize + (border / 2);
		int y = ((double)(getHeight() - (thumbsize * 2) - border) / 1. * mProgressY) + thumbsize + (border / 2);

		/*canvas->drawLine(x, 0, x, y - thumbsize, 14, Color(255, 255, 255, 150));
		canvas->drawLine(x, y + thumbsize, x, getHeight(), 14, Color(255, 255, 255, 150));

		canvas->drawLine(0, y, x-thumbsize, y, 14, Color(255, 255, 255, 150));
		canvas->drawLine(x+thumbsize, y, getWidth(), y, 14, Color(255, 255, 255, 150));*/

		double linen = 15;
		/*int linesize = Metrics::dpToPx(3);
		int linen = 20;
		for (int i = 1;i < linen;i++) {
			canvas->drawLine((x - linesize), getHeight() / linen * i, (x - linesize) + (linesize *2), getHeight() / linen * i, 2, Color("#333333"));
			canvas->drawLine(getWidth() / linen * i, (y - linesize), getWidth() / linen * i, (y - linesize) + (linesize * 2), 2, Color("#333333"));

		}*/
		for (double i = 1;i < linen;i++) {
			//canvas->drawLine(0, (double)getHeight() / linen * i, getWidth(), (double)getHeight() / linen * i, 2, Color("#333333"));
			canvas->drawLine(Position((double)getWidth() / linen * i, 0), Position((double)getWidth() / linen * i, getHeight()), 2, Color("#333333"));

		}
		linen /= 2;
		for (double i = 1;i < linen;i++) {
			canvas->drawLine(Position(0, (double)getHeight() / linen * i), Position(getWidth(), (double)getHeight() / linen * i), 2, Color("#333333"));
			//canvas->drawLine((double)getWidth() / linen * i, 0, (double)getWidth() / linen * i, getHeight(), 2, Color("#333333"));

		}

		canvas->drawTorus(Position(x - (thumbsize), y - (thumbsize), getFront()+1), thumbsize, thumbsize*0.1, Color("#88ccdd"));
		canvas->drawCircle(Position(x - (thumbsize * 0.5), y - (thumbsize * 0.5), getFront() + 1), thumbsize *0.5, 0, Color("#88ccdd"));

		
	}

};

#endif
