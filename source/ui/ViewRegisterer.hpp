#ifndef _CUSTOMVIEWSR_H
#define _CUSTOMVIEWSR_H

#include <map>
#include "View.hpp"

#ifdef __GNUG__
	#include <cxxabi.h>
#endif

#include "../utils/Log.hpp"

template<typename T> View* createInstance() { return new T(); }
typedef std::map<std::string, class View * (*)()> RegMap;

class ViewRegisterer{
	private:
		static RegMap mReg;
	public:
		ViewRegisterer(){
		}
		template<typename T> static void add(){
			std::string name = typeid(T).name();
			#ifdef __ANDROID__
				//hack
	        	name.erase(name.begin(), std::find_if(name.begin(), name.end(), std::not1(std::ptr_fun<int, int>(std::isdigit))));
	        	//not defined yet
				//std::string name = abi::__cxa_demangle(typeid(object).name(), 0, 0, 0);
			#elif _WIN32
				name = name.substr(6, name.size());
            #elif __APPLE__
                int status = -4;
                name = abi::__cxa_demangle(typeid(T).name(), NULL, NULL, &status);
			#endif

			//LOGE("name to load '%s'", name.c_str());
			add<T>(name);
		}
		template<typename T> static void add(std::string name){
			if (mReg.find(name) == mReg.end()) {
				mReg[name] = &createInstance<T>;
			}
		}
		static View* getInstance(std::string className){
			if (mReg.find(className) == mReg.end()) {
				return NULL;
			}
			else {
				return mReg[className]();
			}
		}


};

#endif
