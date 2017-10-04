#ifndef _ADJ_VERTICALLAYOUT_H
#define _ADJ_VERTICALLAYOUT_H

#include "LinearLayout.hpp"
#include <vector>
#include <algorithm>

class VerticalLayout : public LinearLayout{
	double mOffset;

public:
	VerticalLayout() : LinearLayout(){
		initialize();
	}
	VerticalLayout(mt::Rect r) : LinearLayout(r){
		initialize();
	}
	void initialize(){
		mOffset = 0;
	}

	virtual std::string toString() {
		return "VerticalLayout::" + LinearLayout::toString();
	}
	inline void addView(View *view){


		ViewGroup::addView(view);

	}
	double getChildsTotalHeight(){
		double height = 0;
		for (int i = 0; i<getViews().size(); i++){
			height += getViews()[i]->getHeight() + getViews()[i]->getTopMargin() + getViews()[i]->getBottomMargin();
		}
		return height;
	}

	int computeChildPositionTop(View* view){
		double top = 0;

		if (getVerticalGravity() == Gravity::CENTER){
			top = ((getHeight() / 2) - (getChildsTotalHeight() / 2)) + mOffset + getTop();
		}
		else if (getVerticalGravity() == Gravity::BOTTOM){
			
			int r = (getTop() + (getHeight() - getBottomPadding()));
			int vtw = view->getHeight() + view->getTopMargin() + view->getBottomMargin();
			top = r - mOffset - vtw;
		}
		else{
			top = mOffset + getTop() + getTopPadding();
		}
		mOffset += view->getHeight() + view->getTopMargin() + view->getBottomMargin();

		return top;
	}

	int computeChildPositionLeft(View* view){
		int left = 0;

		if (getHorizontalGravity() == Gravity::CENTER){
			left = (getLeft() + (getWidth() / 2)) - (view->getWidth() / 2);
		}
		else{
			left = ViewGroup::computeChildPositionLeft(view);
		}

		return left;
	}


	void computeChildHeight(View* view){
		double height = view->getHeight();
		if (view->getHeightMode() == SizeMode::FILL) {
			height = getHeight() - (getTopPadding() + getBottomPadding());


			if (view->getWeight() > 1) {
				height /= (double)view->getWeight();
			}

			for (int i = 0; i < getViews().size(); i++) {
				if (getViews()[i]->getHeightMode() != SizeMode::FILL) {
					height -= getViews()[i]->getHeight() + getViews()[i]->getTopMargin() + getViews()[i]->getBottomMargin();
				}
			}
			height -= view->getTopMargin() + view->getBottomMargin();

			view->setHeight(std::max(height, 0.0));
		}
		else if (view->getHeightMode() == SizeMode::WRAP) {

		}


		//LOGX() << view->getLabel() << " " << height << " " << getHeight() << " " << view->getHeightType() << ENDL();



		if (height != view->getHeight()){
			view->setHeight(height);
		}
	}

	void performLayout() override{
		mOffset = 0;
		ViewGroup::performLayout();
	}
};

#endif
