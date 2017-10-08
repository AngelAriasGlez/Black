#ifndef _ADJ_WCOMBOVIEW_H
#define _ADJ_WCOMBOVIEW_H

#include "../ViewGroup.hpp"

#include "../../graphics/Canvas.hpp"
#include <vector>
#include <string>
#include "ListView.hpp"
#include "TextView.hpp"
#include "../../platform/Platform.hpp"

class ComboView : public ViewGroup {
public:
	class IChangeListener {
	public:
		virtual void onChangeListener(ComboView* slider, String selected, int selectedindex, bool fromUser) = 0;
	};
private:
	ListView* mFloatListView;
	TextView* mLabel;
	static ListView* mVisible;

	IChangeListener* mListener;

	int selectedIndex;
public:
	ComboView(Rect r) : ViewGroup(r) {
		mLabel = new TextView(Rect(0,0,View::FILL, View::FILL));
		mLabel->setGravity(Gravity::LEFT, Gravity::CENTER);
		mLabel->setPadding(Metrics::dpToPx(4));
		mLabel->setLabel("combolabel");
		addView(mLabel);

		//addOnTouchListener(this);
        mFloatListView = new ListView(mt::Rect(0, 0, r.getWidth(), Metrics::px(50)));
		mFloatListView->setBackground(Color("#222233"));
		mFloatListView->setVisibility(View::HIDDEN);
		//mFloatListView->setPadding(Spacing(20, 0));

		addView(mFloatListView);

		mFloatListView->setLeftPadding(4);


		mListener = NULL;

		selectedIndex = 0;
	}
	~ComboView() {

	}
	virtual std::string toString() {
		return "ComboView::" + ViewGroup::toString();
	}

	void setOnChangeListener(IChangeListener* listener) {
		mListener = listener;
	}



	void setSelected(int index) {
		TextView* tv = (TextView *)mFloatListView->getByIndex(index);
		if (tv) {
			mLabel->setText(tv->getText());
			selectedIndex = index;
		}
	}
	void setSelected(String name) {
		std::vector<View*> items = mFloatListView->getViews();
		for (int i = 0; i < items.size(); i++) {
			if (((TextView *)items[i])->getText() == name) {
				setSelected(i);
				break;
			}
		}
	}

	void draw(Canvas* canvas) {
		ViewGroup::draw(canvas);

		//resetProjection(canvas);
		//mFloatListView->setWidth(floatWidth);
		auto b = mFloatListView->getBounds();
		if (b.getRight() > Platform::getDisplayWidth()) {
			b.x = Platform::getDisplayWidth() - b.getWidth();
			mFloatListView->setBounds(b);
		}


		//canvas->rotateProjection(glm::radians(90.0f), Position(0, 0, 0));
		//canvas->drawTriangle(Position(), 20, 20, Color());


	}

	void addItem(String str) {
		TextView* text = new TextView(Rect(0,0,View::FILL, Metrics::px(20)));
		text->setLeftMargin(8);
		text->setRightMargin(8);
		text->setText(str);
		text->setClickBackground(Color("#ff0000"));
		text->setLabel("comboitem");

		//text->setOnTouchListener(this);

		mFloatListView->addItem(text);
	}
	void addItems(std::vector<String> items) {
		for (int i = 0; i < items.size(); i++) {
			addItem(items[i]);
		}
	}

	void clear() {
	
		mFloatListView->clear();
	}




	void close() {
		mFloatListView->setVisibility(View::HIDDEN);
	}
	void open() {
		if (mVisible) {
			mVisible->setVisibility(View::HIDDEN);
		}
		mVisible = mFloatListView;

		mFloatListView->setVisibility(View::VISIBLE);
	}


	void onClickListener(std::string label, View* view, TouchEvent& e) {
		if (view == this) {
			open();
		}else if(label == "comboitem"){
			String text = ((TextView *)view)->getText();
			setSelected(text);
			e.handled = true;
			close();
			if (mListener) {
				mListener->onChangeListener(this, text, selectedIndex, true);
			}
			
		}

	};

	bool onFailTouchEvent(TouchEvent& e) {
		if(mFloatListView->isVisible() && e.type == TouchEvent::DOWN && !mFloatListView->isInnerTouchEvent(e))
		close();


		return false;
	}


};

#endif
