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
	s << "View " << mId << mLabel.data() << " ";


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
Bounds View::getBounds() {
	return mBounds;
}
void View::setBounds(Bounds b) {
	//if (mBounds != b) {
		//if (mBounds.rect != b.rect) {
			onResize(mBounds.rect.getWidth(), b.rect.getWidth(), mBounds.rect.getHeight(), b.rect.getHeight());
			if (mResizeListener) {
				mResizeListener->onResizeListener(mLabel, this, mBounds.rect.getWidth(), mBounds.rect.getHeight());
			}
			onPositionChanged();
		//}
		mBounds = b;
	//}
}
void View::setMargin(Spacing margin){
	//if (margin != mBounds.margin) {
		onMarginChanged();
		mBounds.margin = margin;
	//}
}

void View::setPadding(Spacing padding){
	//if (padding != mBounds.padding) {
		onPaddingChanged();
		mBounds.padding = padding;
	//}
	
}
int View::getLeftMargin(){
		return mBounds.margin.getLeft();
	}
int View::getTopMargin(){
		return mBounds.margin.getTop();
	}
int View::getRightMargin(){
		return mBounds.margin.getRight();
	}
int View::getBottomMargin(){
		return mBounds.margin.getBottom();
	}
void View::setLeftMargin(int margin){
	if(mBounds.margin.getLeft() != margin)
		onMarginChanged();
		mBounds.margin.setLeft(margin);
	}
void View::setTopMargin(int margin){
	if (mBounds.margin.getTop() != margin)
		onMarginChanged();
		mBounds.margin.setTop(margin);
	}
void View::setRightMargin(int margin){
	if (mBounds.margin.getRight() != margin)
		onMarginChanged();
		mBounds.margin.setRight(margin);
	}
void View::setBottomMargin(int margin){
	if (mBounds.margin.getBottom() != margin)
		onMarginChanged();
		mBounds.margin.setBottom(margin);
	}

int View::getLeftPadding(){
	return mBounds.padding.getLeft();
}
int View::getTopPadding(){
	return mBounds.padding.getTop();
}
int View::getRightPadding(){
	return mBounds.padding.getRight();
}
int View::getBottomPadding(){
	return mBounds.padding.getBottom();
}
void View::setLeftPadding(int padding){
	if (mBounds.padding.getLeft() != padding)
		onPaddingChanged();
	mBounds.padding.setLeft(padding);
}
void View::setTopPadding(int padding){
	if (mBounds.padding.getTop() != padding)
		onPaddingChanged();
	mBounds.padding.setTop(padding);
}
void View::setRightPadding(int padding){
	if (mBounds.padding.getRight() != padding)
		onPaddingChanged();
	mBounds.padding.setRight(padding);
}
void View::setBottomPadding(int padding){
	if (mBounds.padding.getBottom() != padding)
		onPaddingChanged();
	mBounds.padding.setBottom(padding);
}
	
float View::getLeft(){
	return mBounds.getLeft();
	}
float View::getTop(){
		return mBounds.getTop();
	}
float View::getRight(){
		return mBounds.getRight();
	}
float View::getBottom(){
		return mBounds.getBottom();
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

Rect View::getRect() {
	return mBounds.rect;

}

/*void View::setPosition(Position pos) {
	mBounds.position = pos;

}*/






double View::getWidth(){
	if (mVisibility == Visibility::GONE){
		return 0;
	}
	return mBounds.rect.getWidth();
	}
double View::getHeight(){
	if (mVisibility == Visibility::GONE) {
		return 0;
	}
	return mBounds.rect.getHeight();
}
void View::setWidth(double width){
	double old = mBounds.rect.getWidth();
	if (width == SizeMode::WRAP){
		mWidthMode = SizeMode::WRAP;
		mBounds.rect.setWidth(0);
	}else if (width == SizeMode::FILL){
		mWidthMode = SizeMode::FILL;
		mBounds.rect.setWidth(0);
	}else{
		//mWidthType = Size::NORMAL;
		mBounds.rect.setWidth(std::max(width, 0.0));
	}
	if (old != mBounds.rect.getWidth()) {
		onResize(old, mBounds.rect.getWidth(), mBounds.rect.getHeight(), mBounds.rect.getHeight());
		if (mResizeListener) {
			mResizeListener->onResizeListener(mLabel, this, mBounds.rect.getWidth(), mBounds.rect.getHeight());
		}
	}
}
void View::setHeight(double height){
	double old = mBounds.rect.getHeight();
	if (height == SizeMode::WRAP){
		mHeightMode = SizeMode::WRAP;
		mBounds.rect.setHeight(0);
	}else if (height == SizeMode::FILL){
		mHeightMode = SizeMode::FILL;
		mBounds.rect.setHeight(0);
	}else{
		//mHeightType = Size::NORMAL;
		mBounds.rect.setHeight(std::max(height, 0.0));
	}
	if (old != mBounds.rect.getHeight()) {
		onResize(mBounds.rect.getWidth(), mBounds.rect.getWidth(), old, mBounds.rect.getHeight());
		if (mResizeListener) {
			mResizeListener->onResizeListener(mLabel, this, mBounds.rect.getWidth(), mBounds.rect.getHeight());
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
	e.x = e.rawX - left;
	e.y = e.rawY - top;

	

	if (e.type == TouchEvent::DOWN && isInnerTouchEvent(e) && !e.handled) {
		//LOGE("down");
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
	}else if (e.type == TouchEvent::MOVE && isTouchDown()) {
		//LOGE("move");
		return callTouchEventListeners(e);
	}else if (e.type == TouchEvent::DRAG && isTouchDown()) {
		//LOGE("drag");
		return callTouchEventListeners(e);
	}else if (e.type == TouchEvent::LONG && isTouchDown()) {
		//LOGE("long");
		return callTouchEventListeners(e);
	}else if (e.type == TouchEvent::WHEEL && isInnerTouchEvent(e) && !e.handled) {
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

bool View::isInnerTouchEvent(TouchEvent e) {
	return (e.rawX >= mBounds.getLeft() && e.rawX <= mBounds.getRight() && e.rawY >= mBounds.getTop() && e.rawY <= mBounds.getBottom());
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

		if (!isVisible()) return;
		//canvas->reset();
		//canvas->translate(mBounds.position);
	if (mBackground.isBitmap()) {
		//canvas->drawBitmap(mBackground.getBitmap(), Point(), mBounds.rect, 0);
	}
	else if (mBackground.isSolid()) {
		if (mBackground.getRadius() > 0) {
			canvas->drawRoundQuad(Rect(0, 0, mBounds.rect.getWidth(), mBounds.rect.getHeight()), mBackground.getRadius(), 0, mBackground.getColor());
		}else {
			canvas->drawQuad(Rect(0, 0, mBounds.rect.getWidth(), mBounds.rect.getHeight()), 0, mBackground.getColor());
		}
		//canvas->drawFont(Position(0, 0), toString(), Font(10), Color::GREEN);
	}
	canvas->translate(Point(getLeftPadding(), getTopPadding()));
	canvas->setScissor(0, 0, getInnerWidth(), getInnerHeight());
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
	max -= (mBounds.margin.getLeft() + mBounds.margin.getRight());
	if (mWidthMode == SizeMode::FILL) {
		mBounds.rect.setWidth(max);
	}
}
void View::computeHeight(int max) {
	max -= (mBounds.margin.getTop() + mBounds.margin.getBottom());
	if (mHeightMode == SizeMode::FILL) {
		mBounds.rect.setHeight(max);
	}
}
