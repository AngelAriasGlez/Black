#ifndef _ADJ_HORIZONTALLAYOUT_H
#define _ADJ_HORIZONTALLAYOUT_H

#include "LinearLayout.hpp"
#include <vector>

class HorizontalLayout : public LinearLayout{
	double mOffset;
public:

	HorizontalLayout() : LinearLayout(){
		initialize();
	}
	HorizontalLayout(mt::Rect r) : LinearLayout(r){
		initialize();
	}
	void initialize(){
		mOffset = 0;
	}
	virtual View *clone() const override { return new HorizontalLayout(*this); };

	virtual std::string toString() {
		return "HorizontalLayout::" + LinearLayout::toString();
	}

	void addView(View *view){

		//view->setPosition(getLeft()+mOffset, getTop());

		//view->setLeftMargin(mOffset);
		ViewGroup::addView(view);


	}

	double getChildsTotalWidth(){
		double width = 0;
		for (int i = 0; i<getViews().size(); i++){
			width += getViews()[i]->getWidth() + getViews()[i]->getLeftMargin() + getViews()[i]->getRightMargin();
		}
		return width;
	}

	int computeChildPositionLeft(View* view) override{
		double left = 0;

		if (getHorizontalGravity() == Gravity::CENTER){
			left = ((getWidth()/2.0) - (getChildsTotalWidth() /2.0) ) + mOffset + getLeft();
		}else if (getHorizontalGravity() == Gravity::RIGHT){
			int r = (getLeft() + getWidth() - getRightPadding());
			int vtw = view->getWidth() + view->getLeftMargin() + view->getRightMargin();
			left = r - vtw - mOffset;
		}else{
			left = mOffset + getLeft() + getLeftPadding();
		}
		mOffset += view->getWidth() + view->getRightMargin() + view->getLeftMargin();

		return left;
	}

	int computeChildPositionTop(View* view) override{
		int top = 0;

		if (getVerticalGravity() == Gravity::CENTER){
			top = (getTop() + (getHeight() / 2)) - (view->getHeight()/2);
		}
		else{
			top = ViewGroup::computeChildPositionTop(view);
		}

		return top;
	}

	void computeChildWidth(View* view) override{
		double width = view->getWidth();
		if (view->getWidthMode() == SizeMode::FILL){
			width = getWidth() - (getLeftPadding() + getRightPadding());
			

			if (view->getWeight() >= 1){
				width /= (double)view->getWeight();
			}
			
			for (int i = 0; i<getViews().size(); i++){
				if (getViews()[i]->getWidthMode() != SizeMode::FILL){
					width -= getViews()[i]->getWidth() + getViews()[i]->getLeftMargin() + getViews()[i]->getRightMargin();
				}
			}

			width -= view->getLeftMargin() + view->getRightMargin();
		}

		if (width != view->getWidth()){
			view->setWidth(width);
		}
	}

	void performLayout() override{
		mOffset = 0;
		ViewGroup::performLayout();
	}

};

#endif
