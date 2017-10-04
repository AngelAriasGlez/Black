#ifndef _ADJ_DYNAMICPOINTER_H
#define _ADJ_DYNAMICPOINTER_H

#include <stdlib.h> 


	template <class T> class DynamicPointer{
	public:
		DynamicPointer();
		DynamicPointer(int elements);
		~DynamicPointer();
		T get(int index);
		void set(int index, T value);
		void copy(T* pointer, int elements);
		T* getPointer();
		int getAllocatedElements();

		void resize(int elements);
		void reserve(int elements);
		void reallocate(int elements);
	private:
		void* mBuffer;
		int mElements;

	};

	template <class T> DynamicPointer<T>::DynamicPointer(){
		mBuffer = NULL;
		mElements = 0;
	}
	template <class T> DynamicPointer<T>::DynamicPointer(int elements){
		mBuffer = calloc(elements, sizeof(T));
		mElements = elements;
	}
	template <class T> DynamicPointer<T>::~DynamicPointer(){
		free(mBuffer);
	}
	template <class T> T DynamicPointer<T>::get(int index){
		return ((T*)mBuffer)[index];
	}
	template <class T> void DynamicPointer<T>::set(int index, T value){
		((T*)mBuffer)[index] = value;
	}
	template <class T> void DynamicPointer<T>::copy(T* pointer, int elements){
		reserve(elements);
		memcpy(mBuffer, pointer, sizeof(T)*elements);
	}
	template <class T> T* DynamicPointer<T>::getPointer(){
		return ((T*)mBuffer);
	}
	template <class T> int DynamicPointer<T>::getAllocatedElements(){
		return mElements;
	}

	template <class T> void DynamicPointer<T>::reallocate(int elements){
			mBuffer = realloc(mBuffer, sizeof(T)*elements);
			mElements = elements;
	}
	template <class T> void DynamicPointer<T>::reserve(int elements){
		if(mElements < elements){
			reallocate(elements);
		}
	}
	template <class T> void DynamicPointer<T>::resize(int elements){
		reserve(elements);
		mElements = elements;
	}




















#endif
