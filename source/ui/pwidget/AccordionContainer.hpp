#ifndef _ACCORDIONCONTAINER_H
#define _ACCORDIONCONTAINER_H

#include "HorizontalLayout.h"


class AccordionContainer : public HorizontalLayout{
private:

public:
	AccordionContainer(int width, int height) : HorizontalLayout(width, height){

	}
	void draw(Canvas *canvas){
		View::draw(canvas);



	}

};
#endif