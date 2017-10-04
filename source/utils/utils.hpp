#ifndef _ADJ_UTILS_H
#define _ADJ_UTILS_H

#ifdef _WIN32
	#include <direct.h>
	#define NOMINMAX
	#include <Windows.h>
	#include "Shlwapi.h"
#else
	#include <sys/stat.h>
	#include <dirent.h>
#endif

#include <vector>
#include <string>
#include <algorithm>
#include <locale>
#include <cmath>
#include <sstream>

#include "Log.hpp"

#include <map>

#include "../core/String.hpp"

class Utils{
public:



	inline static int pow2roundup (int x){
		if (x < 0)
			return 0;
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x+1;
	}
	inline static int makedir(const char *path){
		#ifdef WIN32	
			return _mkdir(path);
		#else
			return mkdir(path, 0777);
		#endif	
	}
	inline static double round(double val){
			return floor(val + 0.5);
	}
	static double degreesToRadians(double angleDegrees) { 
		return angleDegrees * 3.14159265359 / 180.0;
	}

	static double radiansToDegrees(double angleRadians) {
		return angleRadians * 180.0 / 3.14159265359;
	}

	static std::string formatPath(std::string path){
		std::replace(path.begin(), path.end(), '\\', '/');
		if ((path[path.length() - 1] != '/'))
			path = path + "/";

		return path;
	}


	static double clamp(double val, double min, double max){
		if (max < val)
			return max;
		else if (min > val)
			return min;
		else
			return (val);

	}
	static double max(double val1, double val2){
		if (val1 > val2)
			return val1;
		else
			return val2;
	}
	static double min(double val1, double val2){
		if (val1 < val2)
			return val1;
		else
			return val2;
	}
	static double getCurrentMs() {
		return ((double)clock() / (double)CLOCKS_PER_SEC) * 1000.;
	}
	
    static std::multimap<int, String> getDir(String dir){
        std::multimap<int, String> list;
		//std::string_convert<std::codecvt_utf8<wchar_t>> conv;
		#ifdef WIN32


			/*FILE* pipe = NULL;
			std::string pCmd = "dir /b " + dir;
			char buf[256];

			if (NULL != (pipe = _popen(pCmd.c_str(), "rt"))){


				while (!feof(pipe)){
					if (fgets(buf, 256, pipe) != NULL){
						std::string file = std::string(buf);
						file.substr(0, std::max(0, (int)file.size() - 2));
						list.push_back(file);
					}

				}
			}

			_pclose(pipe);*/


			dir.append("*");

			/*int len;
			int slength = (int)dir.length() + 1;
			len = MultiByteToWideChar(CP_ACP, 0, dir.c_str(), slength, 0, 0);
			wchar_t* buf = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, dir.c_str(), slength, buf, len);
			std::string ws(buf);
			delete[] buf;*/


			//PathCombine((LPWSTR)ws.c_str(), (LPWSTR)ws.c_str(), "*");
			WIN32_FIND_DATA FindFileData;
			HANDLE hFind = FindFirstFile(dir.std_wstring().c_str(), &FindFileData);
			while (hFind != INVALID_HANDLE_VALUE){

				String ws;
				ws.fromWide(FindFileData.cFileName);
				

				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT 
					|| FindFileData.dwFileAttributes &  FILE_ATTRIBUTE_HIDDEN
					|| FindFileData.dwFileAttributes &  FILE_ATTRIBUTE_SYSTEM){
				}else if (FindFileData.dwFileAttributes &  FILE_ATTRIBUTE_DIRECTORY){
					list.insert(std::pair<int, String>(0, ws));
				}else{
					list.insert(std::pair<int, String>(1, ws));
				}
				if (!FindNextFile(hFind, &FindFileData)){
					FindClose(hFind);
					hFind = INVALID_HANDLE_VALUE;
				}

			}

		#else
			DIR *d;
			struct dirent *ent;
			struct stat sbuf;

			if ((d = opendir(dir.c_str())) != NULL) {
				while ((ent = readdir(d)) != NULL) {
                    std::string tp = dir + "/" + std::string(ent->d_name);
					stat(tp.c_str(), &sbuf );

					if(S_ISDIR(sbuf.st_mode)){
						list.insert(std::pair<int, std::string>(0, std::string(ent->d_name)));
					}else{
						list.insert(std::pair<int, std::string>(1, std::string(ent->d_name)));
					}
				}
				closedir(d);
			}
			else {
				LOGE("Coudn't open dir %s", dir.c_str());
			}
		#endif
			return list;
	}

};

#endif
