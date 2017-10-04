#ifndef _ACCORDIONMODULE_H
#define _ACCORDIONMODULE_H

#include "VerticalLayout.h"
#include "SizeAnimation.h"
#include "TextView.h"

#include "Metrics.h"



class AccordionModule : public VerticalLayout, View::ITouchListener, Timer::IStatusListener{
private:
	SizeAnimation *mSizeAnimation;
	bool mOpened;
	int mClosedWidth;
	int mOpenedWidth;

	View* mClosedView;
	View* mOpenedView;

	TextView *mLabel;
public:
	AccordionModule(int width, int height) : VerticalLayout(width, height){
		setPadding(Metrics::dpToPx(4));

		mSizeAnimation = new SizeAnimation();
		mSizeAnimation->setView(this);
		mSizeAnimation->setListener(this);
		mSizeAnimation->setMs(200);

		mClosedWidth = getWidth();
		mOpenedWidth = mClosedWidth;

		mOpened = false;
		mOpenedView = NULL;
		mClosedView = NULL;

		mLabel = new TextView(View::FILL, Metrics::dpToPx(30));
		mLabel->setBackground(Color(255, 0, 0));
		mLabel->setGravity(Gravity::CENTER, Gravity::CENTER);
		mLabel->setOnTouchListener(this);
		addView(mLabel);
	}
	void onInitialize(){
		VerticalLayout::onInitialize();

		mLabel->setText(getLabel());

	}
	void setOpenedView(View* view){
		mOpenedView = view;
		mOpenedWidth = mOpenedView->getWidth();
		if (mOpened){
			setWidth(mOpenedWidth);
		}else{
		}
		addView(mOpenedView);
		mOpenedView->setVisibility(HIDDEN);
	}
	void setClosedView(View* view){
		mClosedView = view;
		mClosedWidth = mOpenedView->getWidth();
		if (mOpened){
			mClosedView->setVisibility(HIDDEN);
		}else{
			setWidth(mClosedWidth);
		}
		addView(mClosedView);
		mClosedView->setVisibility(HIDDEN);
	}
	void draw(Canvas *canvas){
		VerticalLayout::draw(canvas);

	}
	void open(){
		if (!mSizeAnimation->isRunning()){
			mClosedWidth = getWidth();
		}
		mSizeAnimation->run(mOpenedWidth, getHeight(), false);
		mOpened = true;

	}
	void close(){
		mSizeAnimation->run(mClosedWidth, getHeight(), false);
		mOpened = false;

	}


	void onClickListener(std::string label, View* view, TouchEvent& e) {
		if (mOpened){
			close();
		}else{
			open();
		}


	}

	void complete(){
		if (mOpenedView){
			mOpenedView->setVisible(mOpened);
		}
		if (mClosedView){
			mClosedView->setVisible(mOpened);
		}
	}

	void running(){};
	void reset(){};
	void tick(){};
};
#endif