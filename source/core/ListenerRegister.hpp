#ifndef _EVENTREGISTER_H
#define _EVENTREGISTER_H

#include <list>

template<typename T> class ListenerRegister{
private:
	std::list<T*> mListeners;
public:
	void addListener(T* event) {
		mListeners.push_back(event);
	}
	void removeListener(T* event) {
		mListeners.remove(event);
	}
	void setListener(T* event) {
		mListeners.clear();
		mListeners.push_back(event);
	}
	bool hasListeners() {
		return mListeners.size() > 0;
	}
protected:
	std::list<T*>& getListeners() {
		return mListeners;
	}
	typedef typename std::list<T*>::iterator ListenerIterator;
};

#endif
