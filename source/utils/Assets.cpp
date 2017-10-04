#include "Assets.hpp"


#include "../graphics/Bitmap.hpp"
#include "../ui/ViewGroup.hpp"

#include "../graphics/Font.hpp"

#include "../platform/Platform.hpp"
#include "../ui/layout/LayoutParser.hpp"
#include <fstream>

bool Asset::load(std::string name) {
		
    String fp(Platform::getAssetsDir() + name);

    std::wifstream wife(fp.c_str());
    wife.imbue(std::locale(std::locale::classic(), new std::codecvt_utf8<wchar_t>));
    if (wife.is_open()) {

        std::wstringstream wss;
        wss << wife.rdbuf();

        this->data.fromWide(wss.str());
        this->name = name;
        return true;
    }
    return false;



	
}


Bitmap* Assets::getBitmap(std::string file) {

		std::string ext;
		std::size_t ext_pos = file.rfind(".");
		ext.append(file.begin() + ext_pos - 2, file.begin() + ext_pos);
		if (ext == ".9") {
			//NinePath *np = new NinePath();
			//np->load(Platform::getAssetsDir() + file);
			//return np;
		}
		else {
			Bitmap *np = new Bitmap();
			np->load(Platform::getAssetsDir() + file);
			return np;
		}
    return nullptr;
	}
ViewGroup* Assets::getLayout(std::string name) {
	ViewGroup* vg = LayoutParser::create(name);
	return vg;

	}
