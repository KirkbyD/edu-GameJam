#pragma once
#include "GLCommon.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "cError.hpp"

extern GLFWwindow* _text_window;
extern unsigned _text_program;
extern unsigned _uniform_tex;
extern unsigned _attribute_coord;
extern unsigned _uniform_color;
extern unsigned _dp_vbo;
extern int _window_width;
extern int _window_height;
extern const float MAX_HEIGHT;
extern FT_Library _ft;
extern FT_Face _face;
extern GLfloat _current_y_position;
extern GLfloat _y_offset;
extern char _text_buffer[512];


class cTextRenderer {
	cError errmngr;

	struct point {
		GLfloat x;
		GLfloat y;
		GLfloat s;
		GLfloat t;
	};

public:
	cTextRenderer() { };
	~cTextRenderer() { };

	bool InitFreetype();
	void RenderText(const char* text, unsigned textShaderProgID);
};
