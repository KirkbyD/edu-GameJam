#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cTextRenderer.hpp"

#include <iostream>


bool cTextRenderer::InitFreetype() {
	if (FT_Init_FreeType(&_ft)) {
		errmngr.DisplayError("cTextManager", __func__, "Init Failure", "Unable to Initialize FreeType");
		return false;
	}

	if (FT_New_Face(_ft, "data/fonts/Verdana.ttf", 0, &_face)) {
		errmngr.DisplayError("cTextManager", __func__, "Text Failure", "Unable to load FreeType face true type file");
		return false;
	}

	// Font Size
	FT_Set_Pixel_Sizes(_face, 0, 26);

	if (FT_Load_Char(_face, 'X', FT_LOAD_RENDER)) {
		errmngr.DisplayError("cTextManager", __func__, "Character Failure", "Unable to load characters from true type file");
		return false;
	}

	return true;
}

void cTextRenderer::RenderText(const char* text, unsigned textShaderProgID) {
	//RenderText(const char *text, float x, float y, float sx, float sy)
	float sx = 1.0f / _window_width;
	float sy = 1.0f / _window_height;

	GLfloat xoffset = 8 * sx;
	float x = -1 + xoffset;
	float y = 1 - _current_y_position * sy;

	glUseProgram(textShaderProgID);
	GLfloat white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(_uniform_color, 1, white);
	const char* p = nullptr;
	FT_GlyphSlot g = _face->glyph;

	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(_uniform_tex, 0);

	// We require 1 byte alignment when uploading texture data 
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Clamping to edges is important to prevent artifacts when scaling 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Linear filtering usually looks best for text 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set up the VBO for our vertex data
	glEnableVertexAttribArray(_attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, _dp_vbo);
	glVertexAttribPointer(_attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// Loop through all characters 
	for (p = text; *p; ++p) {
		// Try to load and render the character 
		if (FT_Load_Char(_face, *p, FT_LOAD_RENDER))
			continue;


		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			g->bitmap.width,
			g->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			g->bitmap.buffer
		);

		// Calculate the vertex and texture coordinates 
		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		point box[4] = {
				{ x2, -y2, 0, 0 },
			{ x2 + w, -y2, 1, 0 },
			{ x2, -y2 - h, 0, 1 },
		{ x2 + w, -y2 - h, 1, 1 },
		};


		// Draw the character on the screen
		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Advance the cursor to the start of the next character
		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}

	glDisableVertexAttribArray(_attribute_coord);
	glDeleteTextures(1, &tex);
	glEnableVertexAttribArray(0);

	_current_y_position = (_current_y_position > MAX_HEIGHT) ? 30.0f : (_current_y_position + _y_offset);
}
