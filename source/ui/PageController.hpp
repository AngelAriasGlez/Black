#ifndef _PAGECONTROLLER_H
#define _PAGECONTROLLER_H

#include "../core/Map.hpp"
#include "../core/String.hpp"
#include "Page.hpp"
#include "../platform/Platform.hpp"

#include <stack>

class PageController: Page::IPageResultListener {

private:
	std::map<std::string, Page*> mPages;
	std::map<std::string, IPageResultListener*> mPagesResult;
	std::stack<std::string> mPageStack;
	std::string mCurrentPage;
public:
	PageController(){


	}
	void set(Page* page) {
		mPages[page->getName()] = page;
		page->setSize(Platform::getDisplayWidth(), Platform::getDisplayHeight());
		page->performLayout();

	}
	Page* get(std::string name) {
        if(mPages.count(name)){
            return mPages[name];
        }else{
            return NULL;
        }
	}
	void show(std::string name) {
		auto p = get(mCurrentPage);
		if(p) p->setVisibility(View::HIDDEN);
		mCurrentPage = name;
		p = get(mCurrentPage);
		if (p) {
			p->setVisibility(View::VISIBLE);
			p->onResume(0);
		}
		mPageStack.push(name);
	}
	Page* getCurrentPage() {
        return get(mCurrentPage);
	}
	void result(std::string name, IPageResultListener* callbck) {
        if(Page* p = get(name)){
            mPagesResult[name] = callbck;
            p->setOnPageResultListener(this);
            show(name);
        }
	}
	void back() {
		mPageStack.pop();
		std::string prev = mPageStack.top();
		if (mPages[prev]) {
			show(prev);
		}
	}
    void resizeEvent(){
        typedef std::map<std::string, Page*>::iterator it_type;
        for(it_type iterator = mPages.begin(); iterator != mPages.end(); iterator++) {
            iterator->second->setSize(Platform::getDisplayWidth(), Platform::getDisplayHeight());
			iterator->second->performLayout();
        }
    }
private:
	void onPageResultListener(Page* page, void* dta) {
		mPagesResult[page->getName()]->onPageResultListener(page, dta);
		mPagesResult.erase(page->getName());
		mPages[page->getName()]->setOnPageResultListener(NULL);
		back();
	}


};
#endif
