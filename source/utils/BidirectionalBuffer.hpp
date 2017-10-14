#ifndef BIDIRECTIONALBUFFER_H
#define BIDIRECTIONALBUFFER_H

#include <algorithm>
#include <stdlib.h> 
#include "../utils/Log.hpp"

template <class T>
class BidirectionalBuffer {
private:
	T *mData;
	int mRight;
	int mLeft;
	int mReadPosition;

	int mSize;
public:
	explicit BidirectionalBuffer(int capacity = 100) :
		mData(new T[capacity]),
		mSize(capacity),
		mRight(0),
		mLeft(0),
		mReadPosition(0){
	}

	~BidirectionalBuffer() {
		delete[] mData;
	}
	void resize(int capacity){
		//realloc();
		delete[] mData;
		mData = new T[capacity];
		mSize = capacity;
		clear();
	}

	int getElements() {
		return mRight - mLeft;
	}
	int getLeft(){
		return mLeft;
	}
	int getRight(){
		return mRight;
	}
	int getSize(){
		return mSize;
	}
	void setReadPosition(int position){
		mReadPosition = std::max(position, mLeft);
		mReadPosition = std::min(position, mRight);
	}
	int getReadPosition(){
		return mReadPosition;
	}
	int pushRight(T* pointer, int elements) {
		int croped = 0;
		elements = std::min(mSize, elements);
		int aval = (mSize - mRight);
		if (aval < elements){
			int need = elements - aval;
			croped = moveLeft(need);
		}
		memcpy(&mData[mRight], pointer, elements*sizeof(T));

		mRight += elements;
		return croped;
	}
	int pushLeft(T* pointer, int elements) {
		int croped = 0;
		elements = std::min(mSize, elements);
		int aval = mLeft;
		if (aval < elements){
			int need = elements - aval;
			croped = moveRight(need);
		}
		memcpy(&mData[mLeft-elements], pointer, elements*sizeof(T));
		mLeft -= elements;
		return croped;
	}
	int moveRight(int elements){
		elements = std::min(mSize, elements);
		memmove(&mData[elements], &mData[0], (mSize - elements) * sizeof(T));
		int croped = 0;
		if (mRight + elements > mSize){
			croped = (mRight + elements) - mSize;
		}
		mRight = std::min(mRight + elements, mSize);
		mLeft = std::min(mLeft + elements, mSize);
		mReadPosition = std::min(mReadPosition + elements, mRight);
		return croped;
	}
	int moveLeft(int elements){
		elements = std::min(mSize, elements);
		memmove(&mData[0], &mData[elements], (mSize - elements) * sizeof(T));
		int croped = 0;
		if (mLeft - elements < 0){
			croped = abs(mLeft - elements);
		}
		mRight = std::max(mRight - elements, 0);
		mLeft = std::max(mLeft - elements, 0);
		mReadPosition = std::max(mReadPosition - elements, mLeft);
		return croped;
	}
	int avaliableForward(){
		return mRight - mReadPosition;
	}
	int copyForward(T* pointer, int elements){
		elements = std::min(avaliableForward(), elements);
		memcpy(pointer, &mData[mReadPosition], elements*sizeof(T));
		mReadPosition = std::min(mReadPosition + elements, mRight);
		return elements;
	}
	int readForward(T* pointer, int elements) {
		elements = std::min(avaliableForward(), elements);
		mReadPosition = std::min(mReadPosition + elements, mRight);
		pointer = &mData[mReadPosition];
		return elements;
	}
	int avaliableBackward(){
		return mReadPosition - mLeft;
	}
	int copyBackward(T* pointer, int elements){	
		elements = std::min(avaliableBackward(), elements);
		memcpy(pointer, &mData[mReadPosition - elements], elements*sizeof(T));
		mReadPosition = std::max(mReadPosition - elements, mLeft);
		return elements;
	}
	int readBackward(T* pointer, int elements) {
		elements = std::min(avaliableBackward(), elements);
		mReadPosition = std::max(mReadPosition - elements, mLeft);
		pointer = &mData[mReadPosition];
		return elements;
	}
	void clear(){
		mRight = 0;
		mLeft = 0;
		mReadPosition = 0;
		memset(mData, 0, mSize*sizeof(T));
	}

	T &operator[](int i)
	{
		return mData[mLeft + i];
	}



};


#endif
