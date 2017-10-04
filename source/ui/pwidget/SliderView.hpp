#ifndef _ADJ_SLIDER_H
#define _ADJ_SLIDER_H

#include "../../utils/Utils.hpp"

#include "../../graphics/Metrics.hpp"

#include <algorithm>

#include "SliderBase.hpp"
#include "../../graphics/Canvas.hpp"


class SliderView : public SliderBase{
public:
	enum Orientation {
		HORIZONTAL,
		VERTICAL
	};
protected:
	bool mDrawRail;

private:
	Orientation mOrientation;
public:


	SliderView(Rect r) : SliderBase(r){
		mOrientation = VERTICAL;
		mDrawRail = true;
	}
	/*void setOrientation(Orientation o) {
		mOrientation = o;
	}*/


	void draw(Canvas *canvas){
		BasicView::draw(canvas);

		//canvas->drawFont(0, 0, "MID", 30, Color("#333333"));

		if (getType() == LINEAR_INF) {

			int railMargin = getWidth() / 4.5;

			int thumbsize = Metrics::dpToPx(9);
			int linesize = Metrics::dpToPx(2);
			int linemargin = 10;

			canvas->drawQuad(Rect(railMargin, 0, getWidth() - (railMargin * 2), getHeight()), 4, Color("#333333"));
			canvas->drawLine(Point(railMargin *1.3, getHeight() / 2), Point(getWidth() - (railMargin * 1.3), getHeight() / 2), 4, Color("#88ccdd"));
			canvas->drawQuad(Rect(railMargin * 1.3, (getHeight() / 2), getWidth() - (railMargin * 2.6), ((getHeight() / 2) / 100. * -getProgress())), 0, Color("#88ccdd66"));


			canvas->drawFont(Point(), String(getProgress()), Font(), Color::WHITE);



		}else if (getType() == LINEAR) {


			int railMargin = getWidth() / 4.5;

			int thumbsize = Metrics::dpToPx(Utils::max(9, getWidth() / 20));
			int linesize = thumbsize/3;
			int linemargin = 10;

			if (getOrigin() == CENTER) {


				canvas->drawQuad(Rect(railMargin, 0, getWidth() - (railMargin * 2), getHeight()), 4, Color("#333333"));
				canvas->drawLine(Point(railMargin *1.3, getHeight() / 2), Point(getWidth() - (railMargin * 1.3), getHeight() / 2), 4, Color("#88ccdd"));
				if(mDrawRail)canvas->drawQuad(Rect(railMargin * 1.3, (getHeight() / 2), getWidth() - (railMargin * 2.6), ((getHeight() / 2) / 100. * -getProgress())), 0, Color("#88ccdd66"));


				{
					int y = (getHeight() - thumbsize) - ((getHeight() - thumbsize) / 200. * (getProgress() + 100));
					canvas->drawRoundQuad(Rect(0, y, getWidth(), thumbsize), Metrics::dpToPx(1), 0, Color("#88ccdd"));
					canvas->drawQuad(Rect(linemargin, y + (thumbsize / 2) - (linesize / 2), getWidth() - (linemargin * 2), linesize), 0, Color("#6d9fa7"));

				}


			}else {
				for (int i = 0; i <= 8; i++) {
					canvas->drawLine(Point(20, (double)i * ((double)getHeight() / 8.)), Point(getWidth() - 20, (double)i * ((double)getHeight() / 8.)), 2, Color("#333333"));
				}

				canvas->drawQuad(Rect(railMargin, getHeight(), getWidth() - (railMargin * 2), -(getHeight() / 100. * getProgress())), 0, Color("#88ccdd66"));

				canvas->drawLine(Point(0, getHeight()), Point(getWidth(), getHeight()), 8, Color("#333333"));
				
				{
					int y = (getHeight() - thumbsize) - ((getHeight() - thumbsize) / 100. * getProgress());
					canvas->drawRoundQuad(Rect(0, y, getWidth(), thumbsize), Metrics::dpToPx(1), 0, Color("#88ccdd"));
					canvas->drawQuad(Rect(linemargin, y + (thumbsize / 2) - (linesize / 2), getWidth() - (linemargin * 2), linesize), 0, Color("#6d9fa7"));

				}
			
			}

		}else if(getType() == CIRCULAR){
			
			/*int m = Utils::max(getBounds().getInnerDimension().width, getBounds().getInnerDimension().height);

			if (getWidthMode() == WRAP) {
				setWidth(m);
			}
			if (getHeightMode() == WRAP) {
				setHeight(m);
			}


			int d = std::min((getWidth() / 2) - getLeftPadding() - getRightPadding(), (getHeight() / 2) - getTopPadding() - getBottomPadding());

			int xoffset = ((getWidth() / 2) - (d)), yoffset = ((getHeight() / 2) - (d));

			canvas->translateProjection(Position(xoffset, yoffset));

			canvas->drawCircle(Position(), d, 0, Color("#151515dd"));
			canvas->drawTorus(Position(), d, d / 2, Color("#222222ee"));
			//canvas->drawCircle(0, 0, d, 0, Color(100, 100, 100, 50));
			//canvas->drawCircle(0, 0, d, 1, Color("#333333"));
			//canvas->drawCircle(d/2, d/2, d/2, 1, Color("#333333"));




			canvas->translateProjection(Position(d, d));
			int loff = (d / 2) + ((d / 2) / 3);
			for (int i = 0; i < 20; i++) {
				canvas->drawLine(Position(loff, 0), Position(loff + ((d / 2) / 3), 0), 2, Color("#333333"));
				canvas->rotateProjection(36 / 2, Position());
			}
			resetProjection(canvas);
			canvas->translateProjection(Position(xoffset, yoffset));

			if (isTouchDown()) {
				int dc = d / 4;


				int x = (d - dc) + (d - dc) * sin((mPreviusAngle) * M_PI / 180);
				int y = (d - dc) + (d - dc) * cos((mPreviusAngle) * M_PI / 180);

				canvas->drawCircle(Position(x, y), dc, 0, Color("#88ccdd"));
			}
			canvas->drawFont(Position(), String(getProgress()), Font(), Color::WHITE);*/
		}

	}




};

#endif
