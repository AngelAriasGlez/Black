#ifndef _ADJ_TABLAYOUT_H
#define _ADJ_TABLAYOUT_H

#include "VerticalLayout.h"
#include "HorizontalLayout.h"
#include "ButtonView.h"
#include "Metrics.h"
#include <string>
#include <vector>
#include <map>

class TabLayout : public HorizontalLayout, View::ITouchListener{
private:

	ViewGroup *mContainer;
	VerticalLayout *mLabelContainer;

	std::map<View*, View*> mTabList;

public:
	TabLayout(int width, int height) : HorizontalLayout(width, height){
		
		mContainer = new ViewGroup(Size::FILL, Size::FILL);
		mContainer->setPadding(Metrics::dpToPx(6));
		mContainer->setBackground(Assets::getBitmap("deck-flat-left-bg.9.png"));

		mLabelContainer = new VerticalLayout(Metrics::dpToPx(50), Size::FILL);
		mLabelContainer->setBackground(Assets::getBitmap("deck-flat-left-bg.9.png"));
		mLabelContainer->setPadding(Metrics::dpToPx(3));
		addView(mLabelContainer);
		addView(mContainer);

	}
	~TabLayout(){
		std::map<View*, View*>::iterator iter;
		for (iter = mTabList.begin(); iter != mTabList.end(); ++iter) {
			delete iter->first;
		}

		delete mContainer;
		delete mLabelContainer;
	}

	void addTab(View * view, std::string label){
		mContainer->addView(view);
		ButtonView *labelb = new ButtonView(Size::FILL, Size::FILL);
		labelb->setText(label);
		

		mTabList.insert(std::pair<View*, View*>(labelb, view));

		std::map<View*, View*>::iterator iter;
		for (iter = mTabList.begin(); iter != mTabList.end(); ++iter) {
			iter->first->setWeight(4);
		}

		mLabelContainer->addView(labelb);

		labelb->setOnTouchListener(this);

		if (mContainer->getViews().size() > 1){
			view->setVisibility(GONE);
		}else{
			onClickListener(labelb, TouchEvent());
		}
	}
    void onClickListener(std::string str, View *view, TouchEvent& e){
		if (view == mLabelContainer){

			std::map<View*, View*>::iterator iter;

			for (iter = mTabList.begin(); iter != mTabList.end(); ++iter) {
				if (iter->first == view){
					((ButtonView *)iter->first)->setActive(true);
					iter->second->setVisibility(VISIBLE);
				}
				else{
					((ButtonView *)iter->first)->setActive(false);
					iter->second->setVisibility(GONE);
				}
			}
		}
	}

};

#endif
