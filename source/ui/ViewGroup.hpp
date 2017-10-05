#ifndef _ADJ_VIEWGROUP_H
#define _ADJ_VIEWGROUP_H

#include "View.hpp"
class View;
class Canvas;

#include <vector>

class ViewGroup : public View{
protected:
    std::vector<View *> mViews;
  
    
public:
	ViewGroup() : View(){}
	ViewGroup(mt::Rect r) : View(r){}


	ViewGroup(const ViewGroup& other) : View(other) {
		for (auto v : other.mViews) {
			v->mParent = this;
			mViews.push_back(v->clone());
		}
	}
	virtual View *clone() const override { return new ViewGroup(*this); };

    virtual ~ViewGroup();


    virtual std::string toString();
    virtual void addView(View *view);
    virtual void addViews(std::vector<View*> views);
   
	virtual void update(Canvas *canvas);

    virtual void draw(Canvas *canvas);
    
    
    std::vector<View *> getViews();
    virtual bool dispatchTouchEvent(TouchEvent e);
    //virtual bool processTouchEvent(TouchEvent& e);
    virtual void getAllVisibleChilds(std::vector<View*>& out);
    
    virtual virtual void setBounds(Bounds b);
    
    void onResize(int oldW, int newW, int oldH, int newH);
    void onMarginChanged();
    void onGravityChanged();
    
	/*virtual Rect onMeasure(Rect max) { 
		for (auto v : mViews) {
			if (ViewGroup* c = dynamic_cast<ViewGroup*> (mViews[i])) {
				c->getAllChilds(out);
			}else {
				aut
			}
		}
	
	}*/

	virtual void computeChildWidth(View* view);
	virtual void computeChildHeight(View* view);
    
    virtual int computeChildPositionLeft(View* view);
    virtual int computeChildPositionTop(View* view);
    

    virtual void performLayout();
    
    void getAllChilds(std::vector<View*>& out);
	std::vector<View *>* getAllChilds();
    int childCount();
    void removeChilds();
    ViewGroup *getParent();
    
    void setVisibility(Visibility visibility);
    
	template<typename T>
	T getByLabel(std::string label) {
		return (T)getByLabel(label);
	}
    View* getByLabel(std::string label);
    View* getByIndex(int index);

	template<typename T>
	std::vector<T*> getByType() {
		std::vector<T*> out;
		for (auto v : mViews) {
			if (auto c = dynamic_cast<T*> (v)) {
				out.push_back(c);
			}
			if (auto c = dynamic_cast<ViewGroup*> (v)) {
				auto res = c->getByType<T>();
				out.insert(out.end(), res.begin(), res.end());
			}
		}
		return out;
	}

	virtual void onResume(void *args) override{
		for (auto v : mViews)
			if (auto c = dynamic_cast<ViewGroup*> (v)) v->onResume(args);
		
		View::onResume(args);

	}

};

#endif
