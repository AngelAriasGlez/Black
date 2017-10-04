#include "Metrics.hpp"
#include "../utils/Log.hpp"
#include "../platform/Platform.hpp"

Metrics* Metrics::mInstance = 0;
Metrics* Metrics::getInstance(){
	if (!Metrics::mInstance){
		Metrics::mInstance = new Metrics();
	}
	return Metrics::mInstance;
}
Metrics::Metrics(){
	mDensity = 108.79;
	mSize = 5;
}

void Metrics::setDensity(double density){
	mDensity = density;
}
void Metrics::setSize(double inches){
	mSize = inches;
}

int Metrics::dpToPx(double dp){
    return  (dp * Platform::getDisplayScale());
}
int Metrics::px(double dp) {
	return  dpToPx(dp);
}
