#ifndef _COMBOPREFERENCEX_H
#define _COMBOPREFERENCEX_H

#include "../ui/layout/VerticalLayout.hpp"
#include "../ui/pwidget/TextView.hpp"
#include "../ui/pwidget/ComboView.hpp"

#include "Preference.hpp"

class ComboPreference : public VerticalLayout, Preference, ComboView::IChangeListener {
private:
	TextView* mText;
	ComboView* mCombo;

public:
	ComboPreference(std::string name, std::string label, std::vector<String> items, Preference::IPrefListener* listener) : Preference(name) {
		setOnChangeListener(listener);

		setMargin(Spacing(Metrics::dpToPx(2)));
		setPadding(Metrics::dpToPx(10));
		setRightPadding(Metrics::dpToPx(20));

		setSize(View::FILL, Metrics::dpToPx(70));
		mText = new TextView(View::FILL, Metrics::dpToPx(20));
		mText->setText(label);
		addView(mText);

		mCombo = new ComboView(View::FILL, Metrics::dpToPx(30));
		mCombo->setBackground(Color("#00000033"));
		mCombo->setOnChangeListener(this);
		mCombo->addItems(items);
		mCombo->setSelected(getValue().toInt());

		addView(mCombo);
	
	}

	void setItems(std::vector<String> items) {
		mCombo->clear();
		mCombo->addItems(items);
	}



	~ComboPreference() {
		delete mText;
		delete mCombo;
	}

	void onChangeListener(ComboView* slider, String selected, int selectedindex, bool fromUser) {

		if (!setValue(selectedindex)) {
			mCombo->setSelected(getValue().toInt());
		}
	}

};
#endif
