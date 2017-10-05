#ifndef _ADJ_VIEWX_H
#define _ADJ_VIEWX_H

#include "../graphics/Bitmap.hpp"

class ViewGroup;
class Canvas;
struct NVGLUframebuffer;

#include <algorithm>


#include "../graphics/Background.hpp"

#include "../core/TouchEvent.hpp"

#include <vector>

#include "../graphics/Point.hpp"
#include "../graphics/Bounds.hpp"
#include "../graphics/Spacing.hpp"
#include "../graphics/Rect.hpp"

#include "../utils/Utils.hpp"
#include <functional>

class View{
public:
	friend class ViewGroup;

	/*class ITouchListener{
	public:
		virtual void onTouchListener(std::string, View*, TouchEvent&) {};
		virtual void onClickListener(std::string, View*, TouchEvent&) {};
		virtual void onDoubleClickListener(std::string, View*, TouchEvent&) {};
		virtual void onLongClickListener(std::string, View*, TouchEvent&) {};
	};*/

	class IResizeListener {
	public:
		virtual void onResizeListener(std::string, View*, int, int) = 0;
	};



	enum Gravity{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
		CENTER
	};
	enum SizeMode { FIXED = -1, WRAP = -2, FILL = -3 };
	enum Visibility {GONE = -1, HIDDEN = 0, VISIBLE = 1};
protected:
	//int mRight;
	//int mBottom;

	bool mRetained;

	SizeMode mWidthMode;
	SizeMode mHeightMode;

	std::vector<std::function<bool(View*, TouchEvent&)>> mTouchListeners;

	IResizeListener *mResizeListener;
	

	Gravity mVerticalGravity;
	Gravity mHorizontalGravity;

	bool mInit;

	mt::Point mDrawOffset;
	Bounds mBounds;

	Visibility mVisibility;

	Background mBackground;

	ViewGroup* mParent;

	std::string mLabel;
	double mId;
	static double mNextId;

	double mWeight;

	double zIndex;
	bool mTouchDown;

	bool mEnabled;



	bool mHandleTouchEvents;



	bool mInvalidated;
	NVGLUframebuffer* mFramebuffer;
public:

	View();
	View(mt::Rect d);

	virtual View *clone() const = 0;

	void initialize();

	virtual ~View();
	virtual std::string toString();
	double getId();
	void setLabel(std::string label);
	std::string getLabel();


	ViewGroup*  getParent();


	void setMargin(Spacing margin);
	void setPadding(Spacing padding);

	int getLeftPadding();
	int getTopPadding();
	int getRightPadding();
	int getBottomPadding();
	void setLeftPadding(int padding);
	void setTopPadding(int padding);
	void setRightPadding(int padding);
	void setBottomPadding(int padding);

	int getLeftMargin();
	int getTopMargin();
	int getRightMargin();
	int getBottomMargin();
	void setLeftMargin(int margin);
	void setTopMargin(int margin);
	void setRightMargin(int margin);
	void setBottomMargin(int margin);

	virtual void onMarginChanged();
	virtual void onPaddingChanged();
	virtual void onPositionChanged();
	virtual void onResize(int oldW, int newW, int oldH, int newH);
	virtual void onGravityChanged();
	
	float getLeft();
	float getTop();
	float getRight();
	float getBottom();

	Bounds getBounds();
	virtual void setBounds(Bounds l);

	mt::Rect getRect();
	//void setPosition(Position pos);
	
	int getLeftFromParent();
	int getTopFromParent();

	double getWidth();
	double getHeight();



	SizeMode getWidthMode();
	SizeMode getHeightMode();

	virtual void setWidth(double width);
	virtual void setHeight(double height);
	virtual void setSize(double width, double height);

	void setHeightMode(SizeMode type);
	void setWidthMode(SizeMode type);

	virtual void onInitialize();


	virtual void invokeDraw(Canvas * canvas);

	virtual void setBackground(Background bg);
	Background getBackground() {
		return mBackground;
	}

	virtual bool processTouchEvent(TouchEvent e);
	bool isInnerTouchEvent(TouchEvent e);
	virtual bool onTouchEvent(TouchEvent e);

	bool callTouchEventListeners(TouchEvent e);

	bool isTouchDown();


	void setOnTouchListener(std::function<bool(View*, TouchEvent)> callback);
	void addOnTouchListener(std::function<bool(View*, TouchEvent)> callback);

	void setOnResizeListener(IResizeListener *callback);


	virtual void setVerticalGravity(Gravity gravity);
	virtual void setHorizontalGravity(Gravity gravity);
	void setGravity(Gravity horizontal, Gravity vertical);
	Gravity getVerticalGravity();
	Gravity getHorizontalGravity();

	virtual void setVisibility(Visibility visibility);
	Visibility getVisibility();
	bool isVisible();


	void computeParent();

	/*double getFront();

	void setZIndex(double zindex);
	double getZIndex();*/

	void setWeight(double weight);
	double getWeight();


	//bool operator<(const View& in) { return (mBounds.rect.z + zIndex) <  (in.mBounds.position.z + in.zIndex); }
	//bool operator>(const View& in) { return (mBounds.position.z + zIndex) >  (in.mBounds.position.z + in.zIndex); }

	bool isEnabled();
	void setEnabled(bool enable);


	void setHandleTouchEvents(bool val);


	void invalidate();
	virtual void update(Canvas *canvas);
	virtual void draw(Canvas* canvas);

	
    virtual mt::Rect onMeasure(mt::Rect max) { return mt::Rect(); }
	virtual void computeWidth(int max);
	virtual void computeHeight(int max);

	double getInnerHeight() {
		return getHeight() - getTopPadding() - getBottomPadding();
	}
	double getInnerWidth() {
		return getWidth() - getLeftPadding() - getRightPadding();
	}


	virtual void onResume(void* args) {

	}
	/*void measureDimension(int maxWidth, int maxHeight) {
		int desiredWidth = mBounds.dimension.width;
		int desiredHeight = mBounds.dimension.height;

		int wrapWidth = 10;
		int wrapHeight = 10;

		int width;
		if (mWidthMode == SizeMode::WRAP) {
			width = Utils::min(wrapWidth, maxWidth);
		}else if (mWidthMode == SizeMode::FILL) {
			width = maxWidth;
		}else {
			width = desiredWidth;
		}

		int height;
		if (mHeightMode == SizeMode::WRAP) {
			height = Utils::min(desiredHeight, maxHeight);
		}
		else if (mHeightMode == SizeMode::FILL) {
			height = maxHeight;
		}
		else {
			height = desiredHeight;
		}


		mBounds.dimension = Dimension(width, height);
	}*/

};

#endif
