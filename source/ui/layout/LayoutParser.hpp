#ifndef _LAYOUTPARSER_H
#define _LAYOUTPARSER_H


#include <pugixml.hpp>

#include <string>
#include <sstream>
#include <locale>
#include "../../utils/Log.hpp"

#include "../ViewGroup.hpp"
#include "HorizontalLayout.hpp"
#include "VerticalLayout.hpp"
#include "AbsoluteLayout.hpp"
#include "../pwidget/ImageView.hpp"
#include "../pwidget/TextView.hpp"
#include "../pwidget/ButtonView.hpp"
#include "../pwidget/SliderView.hpp"
#include "../pwidget/TouchSliderView.hpp"
#include "../pwidget/ListView.hpp"

#include "../../graphics/Metrics.hpp"

#include "../ViewRegisterer.hpp"


class LayoutParser{
private:
	std::vector<View *> mNodes;


public:
	static ViewGroup* create(String name){

        
		String pat(Platform::getAssetsDir() + name);

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(pat.c_str());
		if(!result){
			LOGE("ERROR loading layout '%s'", pat.c_str());
			return 0;
		}
		LayoutParser lp;
		std::vector<View *> mNodes  = lp.parse(doc);
		if (mNodes.size() > 0){
			return (ViewGroup *)mNodes[0];
		}
		return 0;
	}
	/*static ViewGroup* create(Asset *as) {
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_buffer(as->data.c_str(), as->data.size());
		if (!result) {
			LOGE("ERROR loading layout '%s'", as->name.c_str());
			return 0;
		}
		LayoutParser lp;
		std::vector<View *> mNodes = lp.parse(doc);
		if (mNodes.size() > 0) {
			return (ViewGroup *)mNodes[0];
		}
		return 0;
	}*/

	std::vector<View *> parse(pugi::xml_node_iterator node){
		std::vector<View *> nodes;
		for (pugi::xml_node_iterator it = node->begin(); it != node->end(); ++it){
			View* view = instantiateView(it);
			if (view){
				if (ViewGroup* c = dynamic_cast<ViewGroup*> (view)){
					std::vector<View *> innodes = parse(it);
					for (int i = 0; i < innodes.size(); i++){
						c->addView(innodes[i]);
					}
				}

				nodes.push_back(view);
			}
		}
		return nodes;
	}

	View *instantiateView(pugi::xml_node_iterator node){
		String n(node->name());

		View * view = NULL;
		if (n == "AbsoluteLayout"){
			view = new  AbsoluteLayout();
		}
		else if (n == "VerticalLayout"){
			view = new VerticalLayout();
		}
		else if (n == "HorizontalLayout"){
			view = new HorizontalLayout();
		}
		else if (n == "TextView"){
			view = new TextView();
			parseTextView(node, (TextView*)view);
		}
		else if (n == "ButtonView"){
			view = new ButtonView();
			parseTextView(node, (TextView*)view);

			if (Bitmap *b = parseBitmapResource(node->attribute("bitmap").as_string())) {
				((ButtonView *)view)->setBitmap(b);
			}
			if (Bitmap *b = parseBitmapResource(node->attribute("activeBitmap").as_string())) {
				((ButtonView *)view)->setActiveBitmap(b);
			}
		}
		else if (n == "ImageView"){
			view = new ImageView();
			if (Bitmap *b = parseBitmapResource(node->attribute("bitmap").as_string())){
				((ImageView *)view)->setBitmap(b);
			}
			if (!node->attribute("rotate").empty())
				((ImageView *)view)->rotate(node->attribute("rotate").as_double());
		}
		else if (n == "SliderView") {
			view = new SliderView(Rect());
			if (!node->attribute("origin").empty()) {
				std::string t = node->attribute("origin").as_string();
				if (t == "center") {
					((SliderView *)view)->setOrigin(SliderView::CENTER);
				}
			}
			if (!node->attribute("type").empty()) {
				std::string t = node->attribute("type").as_string();
				if (t == "circular") {
					((SliderView *)view)->setType(SliderView::CIRCULAR);
				}else if (t == "linear_inf") {
					((SliderView *)view)->setType(SliderView::LINEAR_INF);
				}
			}
		

		}
		else if (n == "TouchSliderView") {
			view = new TouchSliderView(Rect());
		}
		else if (n == "ListView") {
			view = new ListView(Rect());

		}

		else{
			view = ViewRegisterer::getInstance(n);
		}

		if (view != NULL){
			setCommonViewAttributes(node, view);
		}else{
			LOGE("LayoutParser: %s not found", n.c_str());
		}
		return view;
		
	}
	void setCommonViewAttributes(pugi::xml_node_iterator node, View *view){

		for (pugi::xml_attribute_iterator ait = node->attributes_begin(); ait != node->attributes_end(); ++ait){
			//LOGE("attr %s %s", ait->name(), ait->value());
			String n(ait->name());
			String nc(n.c_str());
			if (n == "label"){
				view->setLabel(node->attribute("label").as_string());
			}
			else if (n == "width"){
				if(int v = parseMeasure(node->attribute("width"))){
					view->setWidth(v);
				}
				else if (std::string(node->attribute("width").as_string()) == "fill"){
					view->setWidth(View::FILL);
				}
				else if (std::string(node->attribute("width").as_string()) == "wrap"){
					view->setWidth(View::WRAP);
				}
			}
			else if (n == "height"){
				if (int v = parseMeasure(node->attribute("height"))){
					view->setHeight(v);
				}
				else if (std::string(node->attribute("height").as_string()) == "fill"){
					view->setHeight(View::FILL);
				}
				else if (std::string(node->attribute("height").as_string()) == "wrap"){
					view->setHeight(View::WRAP);
				}

			}
			else if (n == "weight"){
				view->setWeight(node->attribute("weight").as_double());
			}
			else if (n == "margin"){
				view->setMargin(parseMeasure(node->attribute(nc)));
			}
			else if (n == "leftMargin" || n == "marginLeft"){
				view->setLeftMargin(parseMeasure(node->attribute(nc)));
			}
			else if (n == "rightMargin" || n == "marginRight"){
				view->setRightMargin(parseMeasure(node->attribute(nc)));
			}
			else if (n == "topMargin" || n == "marginTop"){
				view->setTopMargin(parseMeasure(node->attribute(nc)));
			}
			else if (n == "bottomMargin" || n == "marginBottom"){
				view->setBottomMargin(parseMeasure(node->attribute(nc)));
			}
			else if (n == "padding"){
				view->setPadding(parseMeasure(node->attribute(nc)));
			}
			else if (n == "leftPadding" || n == "paddingLeft"){
				view->setLeftPadding(parseMeasure(node->attribute(nc)));
			}
			else if (n == "rightPadding" || n == "paddingRight"){
				view->setRightPadding(parseMeasure(node->attribute(nc)));
			}
			else if (n == "topPadding" || n == "paddingTop"){
				view->setTopPadding(parseMeasure(node->attribute(nc)));
			}
			else if (n == "bottomPadding" || n == "paddingBottom"){
				view->setBottomPadding(parseMeasure(node->attribute(nc)));
			}
			else if (n == "gravity"){
				View::Gravity g = parseGravity(node->attribute(nc));
				view->setGravity(g, g);
			}
			else if (n == "verticalGravity"){
				view->setVerticalGravity(parseGravity(node->attribute(nc)));
			}
			else if (n == "horizontalGravity"){
				view->setHorizontalGravity(parseGravity(node->attribute(nc)));
			}
			else if (n == "visibility"){
				view->setVisibility(parseVisibility(node->attribute(nc)));
			}else if (n == "background"){
				view->setBackground(parseBackgroundAttribute(ait->value()));
			}else if (n == "backgroundRadius") {
				auto b = view->getBackground();
				b.setRadius(parseMeasure(node->attribute(nc)));
				view->setBackground(b);
			}else if (n == "zindex") {
				//view->setZIndex(node->attribute(nc).as_double());
			}else if (n == "handleTouch")
				view->setHandleTouchEvents(node->attribute(nc).as_bool());
		}

	}
	View::Visibility parseVisibility(pugi::xml_attribute attr) {
		String str(attr.as_string());
		if (str == "hidden") {
			return View::Visibility::HIDDEN;
		}
		else if (str == "gone") {
			return View::Visibility::GONE;
        }else{
            return View::Visibility::VISIBLE;
        }
	}

	View::Gravity parseGravity(pugi::xml_attribute attr){
		String str (attr.as_string());
		if (str == "left"){
			return View::Gravity::LEFT;
		}
		else if (str == "right"){
			return View::Gravity::RIGHT;
		}
		else if (str == "top"){
			return View::Gravity::TOP;
		}
		else if (str == "bottom"){
			return View::Gravity::BOTTOM;
		}
		else if (str == "center"){
			return View::Gravity::CENTER;
		}
		return View::Gravity::LEFT;
	}


	Background parseBackgroundAttribute(String str){
		if (str.substr(0, 1) == "#"){
			return Color(str.substr(1, str.size()));
		}
		else if (Bitmap *b = parseBitmapResource(str)){
			return b;
        }else{
            return NULL;
        }
	}
	Bitmap *parseBitmapResource(std::string str){
		if (str.substr(0, 1) == "@"){
			return Assets::getBitmap(str.substr(1, str.size()));
		}
		return NULL;
	}

	bool checkStringIsNumber(std::string str){
		std::locale loc;
		std::string::const_iterator it = str.begin();
		while (it != str.end() && std::isdigit(*it, loc)) ++it;
		return !str.empty() && it == str.end();
	}


	int parseMeasure(pugi::xml_attribute attr){
		String str(attr.as_string());

		int out = 0;
		if (str.size() > 2){
			std::string t = str.substr(str.size() - 2, str.size());
			std::string v = str.substr(0, str.size() - 2);
			if (t == "dp" && checkStringIsNumber(v)){
				std::stringstream ss;
				ss << v.data();
				ss >> out;
				return Metrics::dpToPx(out);
			}
			else if (t == "px" && checkStringIsNumber(v)){
				std::stringstream ss;
				ss << v.data();
				ss >> out;
				return out;
			}
		}
		std::stringstream ss;
		ss << str.data();
		ss >> out;
		return out;
	}

	void parseTextView(pugi::xml_node_iterator node, TextView *view) {
		if (!node->attribute("color").empty())
			view->setColor(Color(node->attribute("color").as_string()));
		if (!node->attribute("clickColor").empty())
			view->setClickColor(Color(node->attribute("clickColor").as_string()));
		if (!node->attribute("activeColor").empty())
			view->setActiveColor(Color(node->attribute("activeColor").as_string()));
		if (!node->attribute("active").empty())
			view->setActive(node->attribute("active").as_bool());
		if (!node->attribute("clickBackground").empty())
			view->setClickBackground(parseBackgroundAttribute(node->attribute("clickBackground").as_string()));
		if (!node->attribute("activeBackground").empty())
			view->setActiveBackground(parseBackgroundAttribute(node->attribute("activeBackground").as_string()));
		if (!node->attribute("text").empty())
			view->setText(node->attribute("text").as_string());
		if (!node->attribute("font").empty()) {
			view->setFont(Font(node->attribute("font").as_string()));
		}
		if (!node->attribute("size").empty()) {
			Font f = view->getFont();
			f.setSize(parseMeasure(node->attribute("size")));
			view->setFont(f);
		}
	}


};


#endif
