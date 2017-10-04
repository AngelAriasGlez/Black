#ifndef _CHECKPREFERENCEX_H
#define _CHECKPREFERENCEX_H

#include "../ui/layout/HorizontalLayout.hpp"
#include "../ui/pwidget/TextView.hpp"
#include "../ui/pwidget/CheckView.hpp"

#include "Preference.hpp"


class CheckPreference : public HorizontalLayout, Preference, CheckView::IChangeListener {
private:
	TextView* mText;
	CheckView* mCheck;

public:
	CheckPreference(std::string name, std::string label, Preference::IPrefListener* listener) : Preference(name) {
		setOnChangeListener(listener);

		setMargin(Spacing(Metrics::dpToPx(2)));
		setPadding(Metrics::dpToPx(10));
		setRightPadding(Metrics::dpToPx(20));

		setSize(View::FILL, Metrics::dpToPx(40));
		mText = new TextView(View::FILL, View::FILL);
		mText->setText(label);
		addView(mText);

		mCheck = new CheckView(Metrics::dpToPx(20), View::FILL);
		mCheck->setBackground(Color("#00000033"));
		mCheck->setOnChangeListener(this);

		addView(mCheck);

		if (getValue().toBool()) {
			mCheck->setSelected(true);
		}
		else {
			mCheck->setSelected(false);
		}
	
	}



	~CheckPreference() {
		delete mText;
		delete mCheck;
	}

	void onChangeListener(CheckView* self, bool selected, bool fromUser){
		if (!fromUser) {
			return;
		}
		setValue(String(selected));

	}

};
#endif
