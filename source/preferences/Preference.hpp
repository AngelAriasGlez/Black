#ifndef _PREFERENCEX_H
#define _PREFERENCEX_H

#include "../core/String.hpp"
extern "C" {
	#include "../../libs/unqlite/unqlite.h"
}

class Preference{
public:
	class IPrefListener {
	public:
		virtual bool onChangeListener(Preference* pref, String name, String value) = 0;
	};
private:

	String mName;
	String mValue;

	IPrefListener* mListener;

	bool mEmpty;
public:
	Preference(String name) {
		mName = name;
		mEmpty = true;
        mListener = 0;
		load();
	}
	virtual ~Preference() {

	}

	static String get(String name) {
		Preference p(name);
		return p.getValue();
	}
	static String set(String name, std::string val) {
		Preference p(name);
		return p.setValue(val, true);
	}

	bool setValue(String val, bool force = false) {
		if (mListener && !mListener->onChangeListener(this, mName, val)) {
			if(!force) return false;
		}
		mValue = val;
		mEmpty = false;
		save();
		return true;
	}

	bool isEmpty() {
		return mEmpty;
	}

	String getValue() {
		return mValue;
	}

	void setOnChangeListener(IPrefListener* listener) {
		mListener = listener;
	}

	void save() {
		String dbdir = Platform::getCacheDir() + "preferences.db";

		unqlite *pDb;
		int rc = unqlite_open(&pDb, dbdir.c_str(), UNQLITE_OPEN_CREATE);
		if (rc != UNQLITE_OK) { 
			unqlite_close(pDb);
			return; 
		}

		rc = unqlite_kv_store(pDb, mName.c_str(), mName.size(), mValue.c_str(), mValue.size());


		if (rc != UNQLITE_OK) {
			LOGE("Error storing preference");
		}
		unqlite_close(pDb);
	}
	

	void load() {
		String dbdir = Platform::getCacheDir() + "preferences.db";

		unqlite *pDb;
		int rc = unqlite_open(&pDb, dbdir.c_str(), UNQLITE_OPEN_CREATE);
		if (rc != UNQLITE_OK) {
			unqlite_close(pDb);
			return; 
		}

		unqlite_int64 nBytes;  //Data length


		rc = unqlite_kv_fetch(pDb, mName.c_str(), mName.size(), NULL, &nBytes);
		if (rc != UNQLITE_OK || nBytes < 1) {
			unqlite_close(pDb);
			mEmpty = true;
			return;
		}
		mEmpty = false;
		mValue.resize(nBytes);

		unqlite_kv_fetch(pDb, mName.c_str(), mName.size(), (void*)mValue.data(), &nBytes);


		unqlite_close(pDb);
	}


};
#endif
