#ifndef _ADJ_LISTVIEW_H
#define _ADJ_LISTVIEW_H

#include "../layout/VerticalLayout.hpp"
#include "TextView.hpp"
#include <vector>

#include <algorithm>

#include <typeinfo>
#include "../../utils/Utils.hpp"

#include "BasicView.hpp"

#include "../anim/SizeAnimation.hpp"
#include "../../core/Timer.hpp"

class ListView : public ViewGroup, View::IResizeListener {

private:

	std::vector<int> mTopOffsets;

	int mDragPrevY;
	double mDragPrevTime;
	bool mDrag;

	int mBarDragPrevY;
	int mBarDragStartY;
	bool mBarDrag;
	int mBarTopPos;

	/*bool mClick;
	int mClickId;*/

	double mPos;

	double mVelocity;
	double mVelocityPrev;

	bool mNu;

	View* mBar;

	/*SizeAnimation mBarAnimation;
	Timer mBarTimer;*/

public:
	ListView(Rect r) :
		//mItems(std::vector<View *>()),
		mTopOffsets(std::vector<int>()),
		mDragPrevY(0),
		mDragPrevTime(0),
		mPos(0),
		mVelocity(0),
		mVelocityPrev(0),
		mNu(false),
		mBar(new BasicView()),
		mBarDrag(false),
		mBarDragStartY(0),
		mBarTopPos(0),
		mDrag(false),
		ViewGroup(r) {

		mBar->setBackground(Color("#53575d"));
		




		setOnResizeListener(this);
	}
	virtual ~ListView() {

		clear();
	}

	virtual std::string toString() {
		return "ListView::" + ViewGroup::toString();
	}

	int computeChildPositionTop(View* view) override{
		return view->getBounds().rect.y;
	}

	/*int computeChildPositionLeft(View* view) override {
		return 0;
	}*/

	void onResizeListener(std::string label, View* view, int w, int h) {

	}

	void addItem(View *v) {
		Bounds bds = v->getBounds();
		bds.rect.y = getTop() + totalHeight();
		v->setBounds(bds);
		addView(v);
		mTopOffsets.push_back(totalHeight() + iTotalHeight(v));
	}
	void addItems(std::vector<View*> vs) {
		for (auto v : vs) {
			addItem(v);
		}
	}

	void clear() {
		/*for (int i = 0; i < mItems.size(); i++) {
			if (mItems[i]) {
				delete mItems[i];
			}
		}

		mItems.clear();*/
		removeChilds();

		mTopOffsets.clear();
		mPos = 0;
		mVelocity = 0;
	}
	double totalHeight() {
		if (mTopOffsets.size() >= 1) {
			return mTopOffsets.back();
		}
		return 0;
	}

	int getTopVisible() {
		int p;
		for (p = 0; p < mTopOffsets.size(); p++) {
			if (mTopOffsets[p] >= abs(mPos)) break;
		}
		if (p >= mTopOffsets.size()) {
			p = mTopOffsets.size() - 1;
		}
		return p;
	}
	int iTotalHeight(View * view) {
		return view->getTopMargin() + view->getHeight() + view->getBottomMargin();
	}

	long long prev;
	virtual void process() {


			if (mTopOffsets.size() >= 1) {


				long long c = clock();

				if (!mDrag && mVelocity != 0) {
					double elapsed = (double)(c - prev);
					//LOGE("e %f", mPos);

					if (!mTouchDown) {
						mPos += mVelocity * elapsed;
						mPos = std::min(mPos, 0.0);
						mPos = std::max(mPos, -(totalHeight() - getHeight()));
					}


					double mvel = elapsed / 300;
					if (mVelocity < 0) {
						mVelocity = std::min(0., mVelocity + mvel);
					}else {
						mVelocity = std::max(0., mVelocity - mvel);
					}
					invalidate();
				}
				prev = c;
			}

			if (totalHeight() < getHeight()) {
				mBar->setVisibility(View::HIDDEN);
			}
			else {
				mBar->setVisibility(View::VISIBLE);
				int minSize = 50;
				double barHeight = ((double)getHeight()) / (((double)totalHeight()) / ((double)getHeight()));
				barHeight = Utils::max(minSize, barHeight);

				barHeight = Utils::clamp(barHeight, barHeight, getHeight());
				double barPath = (((double)getHeight()) - barHeight) - getTopPadding() - getBottomPadding();


				if (!mBarDrag) {
					mBarTopPos = barPath * (((double)abs(mPos)) / ((double)(totalHeight() - getHeight()))) + getTopPadding();
				}
				else {
					int pos = ((double)(totalHeight() - getHeight()) / barPath) * (double)(mBar->getTop() - getTop());
					mPos = -pos;
				}

				int barWidth = 10;

				Bounds bounds;
				bounds.rect = Rect(getWidth() - mBar->getWidth() - getLeftPadding(), mBarTopPos, barWidth, barHeight);
				mBar->setBounds(bounds);
			}

			/*mBarAnimation.setView(mBar);

			if (abs(mVelocity) > 0.01 && mBar->getWidth() < barWidth && !mBarAnimation.isRunning()) {
				mBarAnimation.start(barWidth, mBar->getHeight(), false);
			}
			else if (mVelocity == 0 && mBarTimer.isComplete() && mBar->getWidth() != 0 && !mBarAnimation.isRunning()) {
				mBarAnimation.start(0, mBar->getHeight(), true);
			}
			mBarAnimation.update();
			//mBarTimer.update();*/




	}
	void update(Canvas* canvas) {
		process();
	}
	void invokeDraw(Canvas* canvas) override{
		View::invokeDraw(canvas);

		canvas->translate(Point(0, +mPos));

		for (auto c : mViews) {
			if (getTop() < c->getBottom()+mPos
				&& getBottom() > c->getTop()+mPos) {
				c->invokeDraw(canvas);
			}
		}
		canvas->translate(Point(0, -mPos));


		mBar->invokeDraw(canvas);


	}


	/*void onTouchListener(std::string label, View* view, TouchEvent& e) override {
		if (view == mBar) {
			if (e.type == 0) {
				mBarDrag = true;
				mBarDragStartY = mBarDragPrevY = e.rawY;
			}
			else if (e.type == 2) {
				//mPos += (double)(e.rawY - mBarDragStartY) * (((double)totalHeight()) / ((double)getHeight()));
				mBarTopPos = getTop() + (e.rawY - mBarDragStartY);
			}
			else if (e.type == 1) {
				mBarDrag = false;
			}
			mBarDragPrevY = e.rawY;

		}


	}*/
	/*void dispatchTouchEvent(TouchEvent e) override {

	}*/

	void processTouch(TouchEvent& e) {

		if (e.type == 0) {
			mDragPrevY = e.rawY;
			mDragPrevTime = e.time;
			mVelocity = 0;
			mVelocityPrev = 0;
			mDrag = true;
			//LOGE("DOWN");

			/*int p;
			for (p = 0; p < mSizes.size(); p++){
			if (mSizes[p] >= abs(mPos) + e.y) break;
			}

			LOGE("%d", p);
			mClickId = p;
			mClick = false;*/

		}
		else if (mDrag && e.type == 2) {
			//mClick = true;


			int d = (e.rawY - mDragPrevY);
			if (d == 0) return;
			double elapsed = (e.time - mDragPrevTime);
			elapsed = std::max(1., elapsed);
			double v = ((double)d / elapsed);
			if (abs(mVelocity) > 1) {
				if (v > 0) {
					v = std::min(mVelocityPrev * 2., v);
				}
				else {
					v = std::max(mVelocityPrev * 2., v);
				}
			}
			mVelocity = (mVelocityPrev + v) / 2.;
			mVelocityPrev = v;
			//mVelocity = ((mVelocity * 9.) + v) / 10.;

			//LOGE("MOVE %f %f", elapsed, v);

			//LOGE("%f %f %d", mVelocity, ((double)d / elapsed), d);

			mPos += d;
			mPos = std::max(mPos, -(totalHeight() - getHeight()));
			mPos = std::min(mPos, 0.0);

		}
		else if (e.type == 1) {
			//LOGE("UP");
			/*if (!mClick){

			}*/
			mDrag = false;

		}
		mDragPrevTime = e.time;
		mDragPrevY = e.rawY;
	}
	bool onTouchEvent(TouchEvent& e) {
		processTouch(e);
		invalidate();
		return true;

	}

	bool dispatchTouchEvent(TouchEvent e) override {
		TouchEvent e2(e);
		e2.y += abs(mPos);
		e2.rawY += abs(mPos);
		return ViewGroup::dispatchTouchEvent(e2);

	}



};

#endif
