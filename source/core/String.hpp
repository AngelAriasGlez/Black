#ifndef _STRING_H
#define _STRING_H

#include <string>
#include <codecvt>
#include <iostream>

#include "StringStream.hpp"

#include <algorithm>

#if __WIN32
    #define NOMINMAX 1
    #include <Windows.h>
#endif
#include <vector>

class String{
private:
	std::string mString;
public:
	typedef typename  std::string::const_iterator Iterator;
	static int npos;


	String(){

	}
	String(const char* chars){
		mString.assign(chars);

	}

	String(std::string string){
		mString.assign(string);

	}
	String(const wchar_t* string) {
		fromWide(string);
	}

	String(std::wstring string) {
		fromWide(string);
	}

	void fromWide(std::wstring string) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
        mString = convert.to_bytes(string);
		/*int size_needed = WideCharToMultiByte(CP_UTF8, 0, &string[0], (int)string.size(), NULL, 0, NULL, NULL);
		mString.resize(size_needed);
		WideCharToMultiByte(CP_UTF8, 0, &string[0], (int)string.size(), &mString[0], size_needed, NULL, NULL);
         */
	}


	std::wstring toWide() {
		/*std::wstring out;
		int wchars_num = MultiByteToWideChar(CP_UTF8, 0, mString.c_str(), (int)mString.size(), NULL, 0);
		out.resize(wchars_num);
		MultiByteToWideChar(CP_UTF8, 0, mString.c_str(), (int)mString.size(), &out[0], wchars_num);
        */

		/*std::wstring_convert<std::codecvt_utf8<__int32>, __int32> conv;
		auto x = conv.from_bytes(mString);
		std::u32string out;
		out.assign(x.begin(), x.end());*/
		//return out;
        //return std_wstring();
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
        return convert.from_bytes(mString);
	}



	/*std::u32string toUtf32() {
		std::wstring_convert<std::codecvt_utf8<__int32>, __int32> conv;
		auto x = conv.from_bytes(mString);
		std::u32string out;
		out.assign(x.begin(), x.end());
		return out;
	}

	std::u16string toUtf16() {
		std::wstring_convert<std::codecvt_utf8_utf16<__int32>, __int32> conv;
		auto x = conv.from_bytes(mString);
		std::u16string out;
		out.assign(x.begin(), x.end());
		return out;
	}*/


	/*std::wstring toUtf32() {
		int wchars_num = MultiByteToWideChar(CP_UTF8, 0, mString.c_str(), -1, NULL, 0);
		wchar_t* wstr = new wchar_t[wchars_num];
		MultiByteToWideChar(CP_UTF8, 0, mString.c_str(), -1, wstr, wchars_num);
		std::wstring out(wstr);
		delete[] wstr;
		return out;
	}*/


	static std::wstring s2ws(const std::string& str)
	{
		std::wstring ws;
		ws.assign(str.begin(), str.end());
		return ws;
		/*typedef std::codecvt_utf8<wchar_t> convert_typeX;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(str.c_str());*/
	}

	static std::string ws2s(const std::wstring& wstr)
	{

		std::string s;
		s.assign(wstr.begin(), wstr.end());
		return s;
		/*typedef std::codecvt_utf8<wchar_t> convert_typeX;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);*/
	}


	String(bool b){
		StringStream ss;
		ss << b;
		mString.assign(ss.str());
	}
	String(int b){
		StringStream ss;
		ss << b;
		mString.assign(ss.str());
	}
	String(double b){
		StringStream ss;
		ss << b;
		mString.assign(ss.str());
	}
	String(float b){
		StringStream ss;
		ss << b;
		mString.assign(ss.str());
	}
    String(long b){
        StringStream ss;
        ss << b;
        mString.assign(ss.str());
    }

	int toInt() {
		return atoi(c_str());
	}
	bool toBool() {
		return (toInt() > 0);
	}
	float toFloat() {
		return (float)atof(c_str());
	}
	double toDouble() {
		return atof(c_str());
	}

	operator int() {
		return toInt();

	}
	operator const char* () {
		return c_str();
	}


	operator void* () {
		return (void *)data();
	}


	operator std::string() {
		return mString;
	}

	char &operator[](int i)
	{
		return mString[i];
	}


	friend String operator+(const String& a, const String& b);
	friend String operator+(const char* a, const String& b);
	friend String operator+(const String& a, const char* b);
	String& operator+=(const String& a);
	String& operator+=(const char* a);

	
	bool operator<(const String& string) const {
		return mString.size() < string.mString.size();
	}
	bool operator>(const String& string) const {
		return mString.size() > string.mString.size();
	}
	bool operator<(const int& rhs) const {
		return mString.size() < rhs;
	}
	bool operator>(const int& rhs) const {
		return mString.size() > rhs;
	}


	bool operator==(const String& string) const {
		return mString == string.mString;
	}
	bool operator==(const char* string) const {
		return mString == string;
	}
	bool operator==(const int& in) const {
		return mString.size() == in;
	}

	bool operator!=(const String& string) const {
		return mString != string.mString;
	}



	friend String operator+(const String& a, const String& b);

	//friend std::ostream& operator<<(std::ostream& ost, const String& ls);

	size_t size() {
		return mString.size();
	}
	const char* data() {
		return mString.data();
	}
	void resize(size_t newsize) {
		mString.resize(newsize);
	}




	/*std::string toUtf8() {
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;
		return converter.to_bytes(data());
	}*/

	const char* c_str() {
		return mString.c_str();
	}


	std::wstring std_wstring() {
		std::wstring ws;
		ws.assign(mString.begin(), mString.end());
		return ws;
	}
	std::string std_string() {
		return mString;
	}

	String substr(int off, int count = std::wstring::npos) {
		return mString.substr(off, count);
	}
	char at(int off) {
		return mString.at(off);
	}

	void append(String str) {
		mString.append(str.c_str());
	}
	void append(Iterator first, Iterator last) {
		mString.append(first, last);
	}
	void assign(Iterator first, Iterator last) {
		mString.assign(first, last);
	}
	bool empty() {
		return mString.empty();
	}
	void insert(int off, wchar_t charx) {
		mString.insert(off, 1, charx);
	}

	Iterator begin() {
		return mString.begin();
	}
	Iterator end() {
		return mString.end();
	}


	int length() {
		return mString.length();
	}


	int rfind(String s) {
		return mString.rfind(s.c_str());
	}
	int find(String s) {
		return mString.find(s.c_str());
	}
	int find(String s, size_t off) {
		return mString.find(s.c_str(), off);
	}
	int find_last_of(String s) {
		return mString.find_last_of(s.data());
	}



	void replace(char find, char replace) {
		std::replace(mString.begin(), mString.end(), find, replace);
	}


	std::vector<String> split(String delim)
	{
		std::vector<String> tokens;
		size_t prev = 0, pos = 0;
		do
		{
			pos = find(delim, prev);
			if (pos == std::string::npos) pos = length();
			String token = substr(prev, pos - prev);
			if (!token.empty()) tokens.push_back(token);
			prev = pos + delim.length();
		} while (pos < length() && prev < length());
		return tokens;
	}


	// trim from start (in place)
	/*String ltrim(char c = ' ') {
		mString.erase(mString.begin(), std::find_if(mString.begin(), mString.end(), [c](int ch) {
			return ch != c;
		}));
		return this;
	}

	// trim from end (in place)
	String rtrim(char c = ' ') {
		mString.erase(std::find_if(mString.rbegin(), mString.rend(), [c](int ch) {
			return ch != c;
		}).base(), mString.end());
		return this;
	}

	// trim from both ends (in place)
	String trim(String c = ' ') {
		ltrim(c);
		rtrim(c);
		return this;
	}*/
};







#endif
