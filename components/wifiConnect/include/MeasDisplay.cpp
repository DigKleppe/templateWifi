/*
 * measDisplay.cpp
 *
 *  Created on: Apr 6, 2022
 *      Author: dig
 */

#include "MeasDisplay.h"


#define PADDING 18
#define ITEMHEIGHT	98
#define ITEMWIDTH	200
#define FIRSTY 		CLOCKDISPLAYHEIGHT

MeasDisplay::MeasDisplay( lv_obj_t * parent, int y, const char *name, const char * unit, const char * formatStr)
{
	_parent = parent;
	strncpy ( format, formatStr, sizeof (format) -1 );

	if ( name != NULL) {
		nameLabel = lv_label_create(_parent);
		lv_obj_set_pos(nameLabel, 10,y);
		lv_obj_add_style(nameLabel, &styleMeasName, 0);
		lv_label_set_text(nameLabel, name);
		lv_obj_set_style_text_align(nameLabel, LV_TEXT_ALIGN_RIGHT, 0);
	}
	valueLabel = lv_label_create(_parent);

	lv_obj_set_size(valueLabel, 180, ITEMHEIGHT - PADDING);
	lv_obj_add_style(valueLabel, &styleValue, 0);
	lv_label_set_text(valueLabel, "----");
	lv_obj_set_style_text_align(valueLabel, LV_TEXT_ALIGN_RIGHT, 0);

	if ( name != NULL) {
		lv_area_t c;
		lv_obj_get_coords(nameLabel, &c);
		lv_obj_set_pos(valueLabel, 10,c.y2 + PADDING + y);
	}
	else
		lv_obj_set_pos(valueLabel, 10,y);
	if ( unit != NULL) {
		unitLabel = lv_label_create(_parent);
		lv_obj_set_size(unitLabel, 150, ITEMHEIGHT - PADDING);
		lv_obj_add_style(unitLabel, &styleMeasUnit, 0);
		lv_label_set_text(unitLabel, unit);
		lv_obj_align_to(unitLabel, valueLabel, LV_ALIGN_TOP_RIGHT, 200, 0);
	}
}

void MeasDisplay::setText (char * text)
{
	lv_label_set_text(valueLabel, text);
}

void MeasDisplay::setValue (int value)
{	char str[20];
	sprintf( str, format, value );
	setText( str);
}

void MeasDisplay::setValue (float value)
{
	char str[20];
	if ( value < 100)
		sprintf( str, format, value );
	else
		sprintf( str, "%3.0f", value );
	setText( str);
}



MeasDisplay::~MeasDisplay() {
	// TODO Auto-generated destructor stub
}

