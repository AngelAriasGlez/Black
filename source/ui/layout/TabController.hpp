#ifndef TABVIEW_H
#define TABVIEW_H

#include "../ViewGroup.hpp"
#include "../pwidget/ButtonView.hpp"
#include <map>

class TabController : View::ITouchListener {
	std::map<ButtonView*, View*> mItems;
	ButtonView *mSelected;
	//View* mContainer;
public:


	TabController() {
		initialize();
	}


	void initialize() {
		mSelected = NULL;
		//mContainer = NULL;
	}

	/*void setContainer(View* cont) {
		mContainer = cont;
	}*/

	void addTab(ButtonView *button, View* view) {
		mItems[button] = view;
		view->setVisibility(View::GONE);
		button->addOnTouchListener(this);
		//mItems.begin()->second->setVisibility(View::VISIBLE);
	}

	void onClickListener(std::string label, View* view, TouchEvent& e)  {

		/*if (mContainer) {
			if (mSelected == view) {
				mContainer->setVisibility(View::GONE);
				mSelected->setActive(false);
				mSelected = NULL;
				return true;
			}
			else {
				mContainer->setVisibility(View::VISIBLE);
			}
		}*/
		if (view == mSelected) {
			((ButtonView *)view)->setActive(false);
			mItems[mSelected]->setVisibility(View::GONE);
			mSelected = NULL;
			return;
		}

		std::map<ButtonView*, View*>::iterator iter;
		for (iter = mItems.begin(); iter != mItems.end(); ++iter) {
			iter->first->setActive(false);
			iter->second->setVisibility(View::GONE);
		}

		if (mItems.find((ButtonView *)view) != mItems.end()) {
			mSelected = (ButtonView *)view;
			((ButtonView *)view)->setActive(true);
			mItems[(ButtonView *)view]->setVisibility(View::VISIBLE);
		}
	}







};

#endif
