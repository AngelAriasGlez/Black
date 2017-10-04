#ifndef _METRICS_H
#define _METRICS_H


class Metrics{
private:
	double mDensity;
	double mSize;
	
	static Metrics* mInstance;
public:
	static Metrics* getInstance();
	Metrics();

	void setDensity(double density);
	void setSize(double inches);
	static int dpToPx(double dp);
	static int px(double dp);
};


#endif