#ifndef _DJENGINE_SINGLETON
#define _DJENGINE_SINGLETON


class Singleton {
public:       
	static Singleton* getInstance();    
protected:       
	Singleton();       
	Singleton(const Singleton &);       
	Singleton &operator= (const Singleton &);    
private:      
	static Singleton* pinstance; 
};

#endif