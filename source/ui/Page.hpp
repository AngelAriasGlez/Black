#ifndef _PAGE_H
#define _PAGE_H

#include "ViewGroup.hpp"
#include "../core/String.hpp"
#include "../utils/Assets.hpp"


class Page : public  ViewGroup {


public:
	class IPageResultListener {
	public:
		virtual void onPageResultListener(Page*, void*) = 0;
	};

    Page(std::string name) : ViewGroup(mt::Rect(0,0,View::FILL, View::FILL)) {
		mPageResultListener = NULL;
		mName = name;
	}
    Page(std::string name, std::string layout) : ViewGroup(mt::Rect(0,0,View::FILL, View::FILL)){
		mPageResultListener = NULL;
		mName = name;
		loadLayout(layout);
	}
	void loadLayout(std::string layout) {
		View* vg = Assets::getLayout(layout);
		if (vg) {
			addView(vg);
		}
	}
	void setOnPageResultListener(IPageResultListener* listener) {
		mPageResultListener = listener;
	}

	/*void setName(std::string name) {
		mName = name;
	}*/
	std::string getName() {
		return mName;
	}

	void result(void* data) {
		if (mPageResultListener) {
			mPageResultListener->onPageResultListener(this, data);
		}
	}
    
    virtual void onResult(void* data){
        
    }
private:
	IPageResultListener* mPageResultListener;
	std::string mName;


};
#endif
