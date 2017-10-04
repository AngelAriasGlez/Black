#ifndef _ADJ_AUTOPOINTER_H
#define _ADJ_AUTOPOINTER_H

#include <stdlib.h> 


	template <class T> class AutoPointer{
	public:
		AutoPointer();
		AutoPointer(const  AutoPointer<T> &cSource);
		~AutoPointer();
		void copy(const  AutoPointer<T> &cSource);
		T get(int index);
		void push(T value);
		void pop(int index);
		int size();

		void push(T* value, int elements);

		void reallocate(int elements);

		T &operator[](int index);
		AutoPointer<T>& operator=(const AutoPointer<T>& cSource);

		T* data();
	private:
		T* mBuffer;
		int mElements;

	};

	template <class T> AutoPointer<T>::AutoPointer(){
		mBuffer = NULL;
		mElements = 0;
	}
	
	template <class T> AutoPointer<T>::AutoPointer(const  AutoPointer<T> &cSource){
		copy(cSource);
	}

	template <class T> AutoPointer<T>::~AutoPointer(){
		free(mBuffer);
		mBuffer = NULL;
	}
	template <class T> void AutoPointer<T>::copy(const  AutoPointer<T> &cSource) {
		mBuffer = (T*)malloc(sizeof(T)*cSource.mElements);
		memcpy(mBuffer, cSource.mBuffer, sizeof(T)*cSource.mElements);
		mElements = cSource.mElements;
	}


	template <class T> T AutoPointer<T>::get(int index){
		return mBuffer[index];
	}
	template <class T> void AutoPointer<T>::push(T value){
		reallocate(mElements + 1);
		mBuffer[mElements-1] = value;
	}
	template<class T> void AutoPointer<T>::pop(int index){
		for (int i = index; i < (mElements - 1); i++){
			mBuffer[i] = mBuffer[i+1];
		}
		reallocate(mElements - 1);
	}

	template <class T> int AutoPointer<T>::size(){
		return mElements;
	}

	template <class T> void AutoPointer<T>::reallocate(int elements){
		if (mBuffer == NULL) {
			mBuffer = (T*)malloc(sizeof(T)*elements);
		}
		else {
			mBuffer = (T*)realloc((void *)mBuffer, sizeof(T)*elements);
		}
		mElements = elements;
	}

	template <class T> void AutoPointer<T>::push(T* data, int elements) {
		if (elements <= 0) return;
		reallocate(mElements + elements);
		memcpy(&mBuffer[mElements - elements], data, sizeof(T)*elements);
	}

	template <class T> T &AutoPointer<T>::operator[](int index)
	{
		return mBuffer[index];
	}

	template <class T> AutoPointer<T>& AutoPointer<T>::operator=(const AutoPointer<T>& cSource) {
		copy(cSource);
		return *this;
	}



	template <class T> T* AutoPointer<T>::data() {
		return mBuffer;
	}












#endif
