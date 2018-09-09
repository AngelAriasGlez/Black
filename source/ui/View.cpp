#include "View.hpp"

#include "ViewGroup.hpp"
#include "../graphics/Canvas.hpp"

#include "../utils/Utils.hpp"
#include "../platform/Application.hpp"

#include "../graphics/Metrics.hpp"
double View::mNextId = 0;

View::View(){
	initialize();
}
View::View(Rect r) {
	initialize();
	setWidth(r.getWidth());
	setHeight(r.getHeight());
}
View::~View(){


}
void View::initialize(){
	mParent = NULL;

	mRetained = false;

	mWidthMode = SizeMode::FIXED;
	mHeightMode = SizeMode::FIXED;

	mWeight = 1;


	mResizeListener = NULL;


	mHorizontalGravity = Gravity::LEFT;
	mVerticalGravity = Gravity::TOP;

	mVisibility = Visibility::VISIBLE;

	mInit = false;

	mId = mNextId++;

	zIndex = 0;

	mEnabled = true;

	mHandleTouchEvents = false;
    
    mTouchDown = false;

	mFramebuffer = nullptr;
	mInvalidated = true;
}

ViewGroup*  View::getParent(){
	return mParent;

}
std::string View::toString() {

	std::stringstream s;
	s << "View " << mId << mLabel.data() << " \n";


	return s.str();
}


double View::getId(){
	return mId;
}
void View::setLabel(std::string label){
		mLabel = label;
}
std::string View::getLabel(){
	return mLabel;
}
mt::Rect View::getBounds() {
	return mBounds;
}
void View::setBounds(mt::Rect b) {
	//if (mBounds != b) {
		//if (mBounds != b.rect) {
			onResize(mBounds.getWidth(), b.getWidth(), mBounds.getHeight(), b.getHeight());
			if (mResizeListener) {
				mResizeListener->onResizeListener(mLabel, this, mBounds.getWidth(), mBounds.getHeight());
			}
			onPositionChanged();
		//}
		mBounds = b;
	//}
}
void View::setMargin(Spacing margin){
	//if (margin != mMargin) {
		onMarginChanged();
		mMargin = margin;
	//}
}

void View::setPadding(Spacing padding){
	//if (padding != mPadding) {
		onPaddingChanged();
		mPadding = padding;
	//}
	
}
int View::getLeftMargin(){
		return mMargin.getLeft();
	}
int View::getTopMargin(){
		return mMargin.getTop();
	}
int View::getRightMargin(){
		return mMargin.getRight();
	}
int View::getBottomMargin(){
		return mMargin.getBottom();
	}
void View::setLeftMargin(int margin){
	if(mMargin.getLeft() != margin)
		onMarginChanged();
		mMargin.setLeft(margin);
	}
void View::setTopMargin(int margin){
	if (mMargin.getTop() != margin)
		onMarginChanged();
		mMargin.setTop(margin);
	}
void View::setRightMargin(int margin){
	if (mMargin.getRight() != margin)
		onMarginChanged();
		mMargin.setRight(margin);
	}
void View::setBottomMargin(int margin){
	if (mMargin.getBottom() != margin)
		onMarginChanged();
		mMargin.setBottom(margin);
	}

int View::getLeftPadding(){
	return mPadding.getLeft();
}
int View::getTopPadding(){
	return mPadding.getTop();
}
int View::getRightPadding(){
	return mPadding.getRight();
}
int View::getBottomPadding(){
	return mPadding.getBottom();
}
void View::setLeftPadding(int padding){
	if (mPadding.getLeft() != padding)
		onPaddingChanged();
	mPadding.setLeft(padding);
}
void View::setTopPadding(int padding){
	if (mPadding.getTop() != padding)
		onPaddingChanged();
	mPadding.setTop(padding);
}
void View::setRightPadding(int padding){
	if (mPadding.getRight() != padding)
		onPaddingChanged();
	mPadding.setRight(padding);
}
void View::setBottomPadding(int padding){
	if (mPadding.getBottom() != padding)
		onPaddingChanged();
	mPadding.setBottom(padding);
}
	
float View::getLeft(){
		return mBounds.getLeft() + mMargin.getLeft();
}
float View::getTop() {
		return mBounds.getTop() + mMargin.getTop();
	}
float View::getRight(){
		return mBounds.getRight() + mMargin.getLeft();
	}
float View::getBottom(){
		return mBounds.getBottom() + mMargin.getTop();
	}




int View::getLeftFromParent(){
	if (mParent){
		return getLeft() - mParent->getLeft();
	}else{
		return getLeft();
	}
}
int View::getTopFromParent(){
	if (mParent){
		return getTop() - mParent->getTop();
	}
	else{
		return getTop();
	}
}


/*void View::setPosition(Position pos) {
	mBounds.position = pos;

}*/






double View::getWidth(){
	if (mVisibility == Visibility::GONE){
		return 0;
	}
	return mBounds.getWidth();
	}
double View::getHeight(){
	if (mVisibility == Visibility::GONE) {
		return 0;
	}
	return mBounds.getHeight();
}
void View::setWidth(double width){
	double old = mBounds.getWidth();
	if (width == SizeMode::WRAP){
		mWidthMode = SizeMode::WRAP;
		mBounds.setWidth(0);
	}else if (width == SizeMode::FILL){
		mWidthMode = SizeMode::FILL;
		mBounds.setWidth(0);
	}else{
		//mWidthType = Size::NORMAL;
		mBounds.setWidth(std::max(width, 0.0));
	}
	if (old != mBounds.getWidth()) {
		onResize(old, mBounds.getWidth(), mBounds.getHeight(), mBounds.getHeight());
		if (mResizeListener) {
			mResizeListener->onResizeListener(mLabel, this, mBounds.getWidth(), mBounds.getHeight());
		}
	}
}
void View::setHeight(double height){
	double old = mBounds.getHeight();
	if (height == SizeMode::WRAP){
		mHeightMode = SizeMode::WRAP;
		mBounds.setHeight(0);
	}else if (height == SizeMode::FILL){
		mHeightMode = SizeMode::FILL;
		mBounds.setHeight(0);
	}else{
		//mHeightType = Size::NORMAL;
		mBounds.setHeight(std::max(height, 0.0));
	}
	if (old != mBounds.getHeight()) {
		onResize(mBounds.getWidth(), mBounds.getWidth(), old, mBounds.getHeight());
		if (mResizeListener) {
			mResizeListener->onResizeListener(mLabel, this, mBounds.getWidth(), mBounds.getHeight());
		}
	}
}

void View::setHeightMode(SizeMode mode) {
	mHeightMode = mode;
}
void View::setWidthMode(SizeMode mode) {
	mWidthMode = mode;
}

void View::onResize(int oldW, int newW, int oldH, int newH) {

}
void View::onMarginChanged() {

}
void View::onPaddingChanged() {

}
void View::onPositionChanged() {

}
void View::onGravityChanged() {

}



View::SizeMode View::getWidthMode(){
	if (mWidthMode == SizeMode::WRAP){
		return SizeMode::WRAP;
	}else if (mWidthMode == SizeMode::FILL){
		return SizeMode::FILL;
	}else{
		return SizeMode::FIXED;
	}
}
View::SizeMode View::getHeightMode(){
	if (mHeightMode == SizeMode::WRAP){
		return SizeMode::WRAP;
	}
	else if (mHeightMode == SizeMode::FILL){
		return SizeMode::FILL;
	}
	else{
		return SizeMode::FIXED;
	}
}

void View::setWeight(double weight){
	mWeight = weight;
}
double View::getWeight(){
	return mWeight;
}

void View::setSize(double width, double height){
		setWidth(width);
		setHeight(height);
	}

void View::onInitialize(){
	/*std::stringstream s;
	s << " ";
	if (mExtPosition.z == 0) {
		setBackground(Color(0, 0, 100 + (rand() % 155)));
	}

	for (int i = 0;i < mExtPosition.z;i++) {
		s << "-";
	}
	LOGE("%15s %s %f", mLabel.c_str(), s.str().c_str(), mExtPosition.z);*/
}

void View::setBackground(Background bg){
		mBackground = bg;
}


bool View::processTouchEvent(TouchEvent e) {
	if (!isVisible() || !isEnabled()) {
		return false;
	};

	int left = std::max(0.f, getLeft());
	int top = std::max(0.f, getTop());

	e.pos = mt::Point(e.rawPos.x - left, e.rawPos.y - top);


	if (e.type == TouchEvent::DOWN && isInnerPoint(e.rawPos) && !e.handled) {
		//LOGE("down");
		//LOGE("%s", toString().c_str());
		mTouchDown = true;
		return callTouchEventListeners(e);
	}else if (e.type == TouchEvent::UP && isTouchDown()) {
		//LOGE("up");
		mTouchDown = false;
		return callTouchEventListeners(e);
	}else if (e.type == TouchEvent::DBLCLK && isTouchDown()) {
		//LOGE("dbclick");
		return callTouchEventListeners(e);
	}else if (e.type == TouchEvent::CLK && isTouchDown()) {
		//LOGE("click");
		return callTouchEventListeners(e);
	}else if (e.type == TouchEvent::MOVE && isInnerPoint(e.rawPos)) {
		//LOGE("move");
		return callTouchEventListeners(e);
	}else if (e.type == TouchEvent::DRAG && isTouchDown()) {
		//LOGE("drag");
		return callTouchEventListeners(e);
	}else if (e.type == TouchEvent::LONG && isTouchDown()) {
		//LOGE("long");
		return callTouchEventListeners(e);
	}else if (e.type == TouchEvent::WHEEL && isInnerPoint(e.rawPos) && !e.handled) {
		//LOGE("wheel");
		//setBackground(Color::random());
		return callTouchEventListeners(e);
    }else if ((e.type == TouchEvent::PINCH || e.type == TouchEvent::PINCH_START) && isInnerPoint(e.gestureDown1) && isInnerPoint(e.gestureDown2) && !e.handled) {
        //LOGE("wheel");
        //setBackground(Color::random());
        return callTouchEventListeners(e);
    }else if (e.type == TouchEvent::PAN && isInnerPoint(e.gestureDown1) && isInnerPoint(e.gestureDown2) && !e.handled) {
        //LOGE("wheel");
        //setBackground(Color::random());
        return callTouchEventListeners(e);
    }
	//LOGE("%s", e.print().c_str());

	return false;
}
bool View::callTouchEventListeners(TouchEvent e) {
	bool res = false;
	for (auto l : mTouchListeners) {
		res = l(this, e)?true:res;
	}
	res = onTouchEvent(e) ? true : res;
	return res;
}

bool View::isTouchDown() {
	return mTouchDown;
}
void View::setHandleTouchEvents(bool val) {
	mHandleTouchEvents = val;
}

bool View::isInnerPoint(mt::Point e) {
	/*auto ll = ((mParent) ? mParent->getLeftPadding() : 0);

	auto l = getLeft();
	auto r = getRight();
	auto t = getTop();
	auto b = getBottom();

	auto bl = e.rawX >= l;
	auto br = e.rawX <= r;
	auto bt = e.rawY >= t;
	auto bb = e.rawY <= b;

	auto res = bl && br && bt && bb;
	if (res == true) {
		auto zz = 0;
	}
	return res;*/
	return (e.x >= getLeft() && e.x <= getRight() && e.y >= getTop() && e.y <= getBottom());
}

bool View::onTouchEvent(TouchEvent e){
	return mHandleTouchEvents;
}


void View::setOnTouchListener(std::function<bool(View*, TouchEvent)> callback) {
	mTouchListeners.clear();
	mTouchListeners.push_back(callback);
}
void View::addOnTouchListener(std::function<bool(View*, TouchEvent)> callback){
		mTouchListeners.push_back(callback);
}
void View::setOnResizeListener(IResizeListener *callback) {
	mResizeListener = callback;
}



void View::computeParent(){
	if (mParent){
		mParent->performLayout();
	}
}


void View::setVerticalGravity(Gravity gravity){
	if (gravity != mVerticalGravity)
		onGravityChanged();
	mVerticalGravity = gravity;
}
void View::setHorizontalGravity(Gravity gravity){
	if (gravity != mHorizontalGravity)
		onGravityChanged();
	mHorizontalGravity = gravity;
}
void View::setGravity(Gravity vertical, Gravity horizontal){
	if (horizontal != mHorizontalGravity ||vertical != mVerticalGravity)
		onGravityChanged();
	mHorizontalGravity = vertical;
	mVerticalGravity = horizontal;
}
View::Gravity View::getVerticalGravity(){
	return mVerticalGravity;
}
View::Gravity View::getHorizontalGravity(){
	return mHorizontalGravity;
}

void View::setVisibility(Visibility visibility){
	mVisibility = visibility;
}
View::Visibility View::getVisibility() {
	return mVisibility;
}
bool View::isVisible(){
	return mVisibility == Visibility::VISIBLE;
}

bool View::isEnabled() {
	return mEnabled;
}
void View::setEnabled(bool enable) {
	mEnabled = enable;
}
static int count = 0;


void View::update(Canvas *canvas) {
	if (!mInit) {
		onInitialize();
		mInit = true;
	}
}
void View::draw(Canvas * canvas) {
	auto vg = canvas->getNVGContext();

	if (!isVisible()) return;

	if (mBackground.isSolidSetted() || mBackground.isGradientSetted() || mBackground.getBorderWidth() > 1) {
		nvgBeginPath(vg);
		if (mBackground.getCornerRadius() > 0) {
			nvgRoundedRect(vg, 0, 0, getWidth(), getHeight(), mBackground.getCornerRadius());
		}
		else {
			nvgRect(vg, 0, 0, getWidth(), getHeight());
		}

		if (mBackground.isGradientSetted()) {
			int tmiddle = Utils::max(getHeight() / 0.5 * (mBackground.getGradientMiddle() - 0.5), 0);
			int bmiddle = Utils::min(getHeight() / 0.5 * (mBackground.getGradientMiddle() + 0.5), getHeight());
			auto paint = nvgLinearGradient(vg, 0, tmiddle, 0, bmiddle, mBackground.getGradientStartColor(), mBackground.getGradientEndColor());
			nvgFillPaint(vg, paint);
		}else if (mBackground.isSolidSetted()) {
			nvgFillColor(vg, mBackground.getSolidColor());
		}
		if (mBackground.getBorderWidth() > 1) {
			nvgStrokeWidth(vg, mBackground.getBorderWidth()*2);
			nvgStroke(vg);
		}
		nvgFill(vg);

		
	}
	if (!dynamic_cast<ViewGroup*> (this)) {
		canvas->translate(Point(getLeftPadding(), getTopPadding()));
		//canvas->setScissor(0, 0, getInnerWidth(), getInnerHeight());
	}else {
		//canvas->setScissor(getLeftPadding(), getTopPadding(), getInnerWidth(), getInnerHeight());
	}


}


void View::invokeDraw(Canvas * canvas) {
	if (!isVisible()) return;
	//if (!mRetained) {
		canvas->save();
		if (mParent) {
			canvas->translate(Point(getLeft() - mParent->getLeft(), getTop() - mParent->getTop()));
		}
		else {
			canvas->translate(Point(getLeft(), getTop()));
		}

		draw(canvas);
		canvas->restore();
		mInvalidated = false;
		return;
	//}

/*
	if (isVisible()) {
		if (mInvalidated && getWidth() > 0 && getHeight() > 0) {
			if (!mFramebuffer) mFramebuffer = canvas->createRenderFrame(getWidth(), getHeight());
			auto cfb = canvas->getCurrentRenderFrame();
			canvas->bindRenderFrame(mFramebuffer);
			//redraw(canvas);
			canvas->bindRenderFrame(cfb);
			mInvalidated = false;
			//canvas->begin(getWidth(), getHeight());
		}
		//canvas->drawRenderFrame(0, 0, mFramebuffer);
		if (mParent) {
			canvas->drawRenderFrame(getLeft()-mParent->getLeft(), getTop() - mParent->getTop(), mFramebuffer);
		}else {
			canvas->drawRenderFrame(getLeft(), getTop(), mFramebuffer);
		}
	}else {
		mInvalidated = true;
	}
	*/


}



void View::invalidate() {
	mInvalidated = true;
	if(mParent){
		mParent->invalidate();
	}

	//LOGE("Invalidate %d %d", (int)mId, count++);
}

/*
Rect View::preferedSize(Rect max) {
	auto rect = getRect();
	if (getWidthMode() == View::FILL) {
		rect.setWidth(max.getWidth());
	}else if (getWidthMode() == View::WRAP) {
		rect.setWidth(Utils::min(onMeasure(max).getWidth(), max.getWidth()));
	}else {
		rect.setWidth(Utils::min(rect.getWidth(), max.getWidth()));
	}
	if (getHeightMode() == View::FILL) {
		rect.setHeight(max.getHeight());
	}
	else if (getHeightMode() == View::WRAP) {
		rect.setHeight(Utils::min(onMeasure(max).getHeight(), max.getHeight()));
	}else {
		rect.setHeight(Utils::min(rect.getHeight(), max.getHeight()));
	}

	return rect;
}*/

void View::computeWidth(int max) {
	max -= (mMargin.getLeft() + mMargin.getRight());
	if (mWidthMode == SizeMode::FILL) {
		mBounds.setWidth(max);
	}
}
void View::computeHeight(int max) {
	max -= (mMargin.getTop() + mMargin.getBottom());
	if (mHeightMode == SizeMode::FILL) {
		mBounds.setHeight(max);
	}
}
