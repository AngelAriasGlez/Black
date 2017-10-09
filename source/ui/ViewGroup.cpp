#include "ViewGroup.hpp"
#include "../utils/Log.hpp"
#include "../graphics/Canvas.hpp"
#include "View.hpp"

#include <functional>


ViewGroup::~ViewGroup(){
    for (int i = 0; i < mViews.size(); i++){
        if (ViewGroup* c = dynamic_cast<ViewGroup*> (mViews[i])) {
            delete c;
        }else if (mViews[i]){
            delete mViews[i];
        }
    }
}
std::string ViewGroup::toString() {
    return "ViewGroup::" + View::toString();
}
/*bool compByDepth(View* a, View* b) {
    return a->getFront() > b->getFront();
}
*/
void ViewGroup::addView(View *view){
    view->mParent = this;
    mViews.push_back(view);
      
}
void ViewGroup::addViews(std::vector<View*> views) {
    for (auto v : views) {
		addView(v);
    }
    
}

void ViewGroup::draw(Canvas *canvas){
    View::draw(canvas);

    for(int i=0;i<mViews.size();i++){
		mViews[i]->invokeDraw(canvas);
    }
}

void ViewGroup::update(Canvas *canvas){
    View::update(canvas);

	for(int i=0;i<mViews.size();i++){
		mViews[i]->update(canvas);
    }
}


std::vector<View *> ViewGroup::getViews(){
    return mViews;
}
void ViewGroup::setBounds(mt::Rect b) {
    //if (mBounds != b){
        View::setBounds(b);
    //}
}
/*bool ViewGroup::processTouchEvent(TouchEvent& e) {
	return View::processTouchEvent(e);
 }*/
int count = 0;
bool ViewGroup::dispatchTouchEvent(TouchEvent e) {

    /**std::vector<View*> childs;
    if (e.type == TouchEvent::DOWN || e.type == TouchEvent::CLK || e.type == TouchEvent::DBLCLK) {
        getAllVisibleChilds(childs);
    }else {
        getAllChilds(childs);
    }
    std::sort(childs.begin(), childs.end(), compByDepth);
    
    
    for (int i = 0; i < childs.size(); i++) {
        if (childs[i]->processTouchEvent(e)) {
            e.handled = true;
        }
    }*/
	for (std::vector<View*>::reverse_iterator i = mViews.rbegin();i != mViews.rend(); ++i) {

		if (ViewGroup* c = dynamic_cast<ViewGroup*> (*i)) {
			e.handled = c->dispatchTouchEvent(e)?true: e.handled;
		}
		e.handled = (*i)->processTouchEvent(e) ? true : e.handled;

		//LOGE("%d", count++);
    }    
	return e.handled;
}

void ViewGroup::getAllVisibleChilds(std::vector<View*>& out) {
    if(isVisible()){
        out.push_back(this);
        
        for(int i=0;i<mViews.size();i++){
            if (ViewGroup* c = dynamic_cast<ViewGroup*> (mViews[i])) {
                c->getAllVisibleChilds(out);
            }
            else if(mViews[i]->isVisible()){
                out.push_back(mViews[i]);
            }
        }
        
    }
}


void ViewGroup::onResize(int oldW, int newW, int oldH, int newH){
	View::onResize(oldW, newW, oldH, newH);
}

void ViewGroup::onMarginChanged() {
	View::onMarginChanged();
}


void ViewGroup::onGravityChanged(){
	View::onGravityChanged();
}




int ViewGroup::computeChildPositionLeft(View* view){
    int left = 0;
    
    if (getHorizontalGravity() == Gravity::CENTER){
        left = ((getWidth() / 2) - (view->getWidth() / 2)) + getLeft();
    }
    else if (getHorizontalGravity() == Gravity::RIGHT){
		int r = (getLeft() + getWidth() - getRightPadding());
        int vtw = view->getWidth() + view->getLeftMargin() + view->getRightMargin();
        left = r - vtw;
    }else{
		left = getLeft() + getLeftPadding();
    }
    
    return left;
}
int ViewGroup::computeChildPositionTop(View* view){
    
    int top = 0;
    
    if (getVerticalGravity() == Gravity::CENTER){
        top = ((getHeight() / 2) - (view->getHeight() / 2))  + getTop();
    }
    else if (getVerticalGravity() == Gravity::BOTTOM){
		auto c = getHeight();
		int r = (getTop() + getHeight() - getBottomPadding());
        int vtw = view->getHeight() + view->getTopMargin() + view->getBottomMargin();
        top = r - vtw;
    }else{
		top = getTop() + getTopPadding();
    }
    return top;
}


/*void ViewGroup::computeChildsDepth() {
    mBounds.rect.z = mBounds.getFront() + zIndex;
    for (int i = 0; i<mViews.size(); i++) {
        //mViews[i]->mBounds.position.z = floor(mBounds.getFront()) + 1.0 + (0.1 / (float)mViews.size() *(float)i) + zIndex;
        mViews[i]->mBounds.position.z = floor(mBounds.getFront()) + 1.0 + (0.1 / (float)mViews.size() *(float)i);
        
        if (ViewGroup* c = dynamic_cast<ViewGroup*> (mViews[i])) {
            c->computeChildsDepth();
        }
    }
}*/

void ViewGroup::computeChildWidth(View* view) {
	view->computeWidth(mBounds.getWidth() - (mPadding.getLeft() + mPadding.getRight()));
}
void ViewGroup::computeChildHeight(View* view) {
	view->computeHeight(mBounds.getHeight() - (mPadding.getTop() + mPadding.getBottom()));
}


void ViewGroup::performLayout(){
    for (auto v : mViews){
        

		computeChildWidth(v);
		computeChildHeight(v);


		v->mBounds.setX(computeChildPositionLeft(v));
		v->mBounds.setY(computeChildPositionTop(v));


        if (ViewGroup* c = dynamic_cast<ViewGroup*> (v)){
            c->performLayout();
        }
    }
    
    /////////////////!!!! here Stack overflow with animation
    /*if (getWidthMode() == View::WRAP){
        int w = 0;
        for (int i = 0; i < mViews.size(); i++){
            w += mViews[i]->getWidth();
        }
        setWidth(w);
    }
    if (getHeightMode() == View::WRAP){
        int h = 0;
        for (int i = 0; i < mViews.size(); i++){
            h += mViews[i]->getHeight();
        }
        setHeight(h);
    }*/
}
std::vector<View *>* ViewGroup::getAllChilds() {
	return &mViews;

}
void ViewGroup::getAllChilds(std::vector<View*>& out) {
    
    out.push_back(this);
    
    for (int i = 0; i<mViews.size(); i++) {
        if (ViewGroup* c = dynamic_cast<ViewGroup*> (mViews[i])) {
            c->getAllChilds(out);
        }else{
            out.push_back(mViews[i]);
        }
    }
    
    
}

int ViewGroup::childCount() {
    return mViews.size();
}
void ViewGroup::removeChilds() {
    for (int i = 0; i < mViews.size(); i++) {
        if (mViews[i]) {
            delete mViews[i];
        }
    }
    mViews.clear();
}
ViewGroup * ViewGroup::getParent(){
    return mParent;
}

void ViewGroup::setVisibility(Visibility visibility){
    
    if (mVisibility != visibility) {
        View::setVisibility(visibility);
        if (mParent){
            mParent->performLayout();
        }
		performLayout();
    }
}
View* ViewGroup::getByIndex(int index) {
    return mViews[index];
}



View * ViewGroup::getByLabel(std::string label){
    if (mLabel != label)
        for (int i = 0; i<mViews.size(); i++){
            if (mViews[i]->getLabel() == label){
                return mViews[i];
            }else if (ViewGroup* c = dynamic_cast<ViewGroup*> (mViews[i])){
                //LOGE("%s %s %d", mViews[i]->getLabel().c_str(), getLabel().c_str(), mViews.size());
                //LOGE("%s",typeid(*c).name());
                View *v = c->getByLabel(label);
                if (v != NULL){
                    
                    return v;
                }
            }
        }
    //LOGE("getByLabel(%s) not found", label.c_str());
    return NULL;
}



