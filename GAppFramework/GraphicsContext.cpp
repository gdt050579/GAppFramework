#include "GApp.h"

#define TRANSLATE(x,y) { (x)+=TranslateX;(y)-=TranslateY; }
#define PREPARE_SHADER(shader,defaultShader) GApp::Resources::Shader *s; if (shader==NULL) { s = &defaultShader; } else { s = shader; }; if (s!=CurrentShader) ChangeShader(s);if (s!=NULL) s->UpdateVaribleValues(UseDerivedShaderVariable);
#define PREPARE_COORD(coord) { 	if (coord == GAC_COORDINATES_AUTO) coord = DefaultCoordinates;  }

#define CONVERT_COORD_TO_PIXELS(res_x,res_y,x,y,coord) {\
	switch (coord) { \
		case GAC_COORDINATES_PIXELS: res_x = (x); res_y = (y); break; \
		case GAC_COORDINATES_PERCENTAGE: res_x = (x)*Width; res_y = (y)*Height; break; \
		case GAC_COORDINATES_PERCENTAGEOFSCREENWIDTH: res_x = (x)*Width; res_y = (y)*Width; break; \
		case GAC_COORDINATES_PERCENTAGEOFSCREEHEIGHT: res_x = (x)*Height; res_y = (y)*Height; break; \
		case GAC_COORDINATES_DRAWINGBOUNDS: res_x = DrawingBounds.Left+((DrawingBounds.Right-DrawingBounds.Left)*(x));res_y = DrawingBounds.Top+((DrawingBounds.Bottom-DrawingBounds.Top)*(y)); break; \
		default: res_x = res_y = 0; \
	} \
	res_x += TranslateX; \
	res_y += TranslateY; \
	}

#define CONVERT_PIXELS_TO_OPENGL(x,y) { x = x * rapX - 1.0f;y = 1.0f - y * rapY; }

#define SET_ALPHA_TRANSPARENCY(alpha) {\
	if ((BlendImageMode != BLEND_IMAGE_MODE_ALPHA) || (LastColorImageAlpha!=(alpha))) { \
		if ((alpha)<0.0f) \
			ColorImage.SetVariableValue(0, 0.0f, 1.0f, 1.0f, 1.0f, false); \
		else if (alpha>1.0f) \
			ColorImage.SetVariableValue(0, 1.0f, 1.0f, 1.0f, 1.0f, false); \
		else \
			ColorImage.SetVariableValue(0, 1.0f, 1.0f, 1.0f, (alpha), false); \
			BlendImageMode = BLEND_IMAGE_MODE_ALPHA; \
		LastColorImageAlpha = alpha; \
	} \
}
//RED(color), GREEN(color), BLUE(color), ALPHA(color)
#define SET_COLOR_BLEND(color) {\
	if ((BlendImageMode != BLEND_IMAGE_MODE_COLOR) || (LastColorImageColor!=(color))) { \
		ColorImage.SetVariableValue(0, RED(color), GREEN(color), BLUE(color), ALPHA(color), false); \
		BlendImageMode = BLEND_IMAGE_MODE_COLOR; \
		LastColorImageColor = color; \
	} \
}


bool GApp::Graphics::GraphicsContext::Init(int width,int height)
{
	LOG_INFO("Initializing Graphic Context (%d x %d) ", width, height);
	LastError = 0;
	CHECK((width>0) && (height>0) && (width<10000) && (height<10000),false,"");
	Width = (float)width;
	Height = (float)height;
	rapX = 2.0f/Width;
	rapY = 2.0f/Height;
	TranslateX = TranslateY = 0;
	ClippingEnabled = StencilsEnabled = false;
	UseDerivedShaderVariable = true;
	BlendImageMode = BLEND_IMAGE_MODE_NONE;
	LastColorImageAlpha = 0.0f;
	LastColorImageColor = 0;

	CHECK_GLERROR(glViewport(0, 0, width, height));
	CHECK_GLERROR(glEnable(GL_BLEND));
	CHECK_GLERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	LOG_INFO("Creating default shaders ...");
	CHECK(Default.Create("attribute vec4 pos;void main() {gl_Position = pos;}",
						 "#ifdef GL_ES\nprecision lowp float;\n#endif\n uniform vec4 color; void main() { gl_FragColor = color; \n }",
						 "pos","color"),
						 false,"");
	CHECK(ImageNoAlpha.Create("attribute vec4 pos;varying vec2 v_texCoord;attribute vec2 a_texCoord;void main() {gl_Position = pos;v_texCoord = a_texCoord;}",
					   "#ifdef GL_ES\nprecision mediump float;\n#endif\nvarying vec2 v_texCoord;uniform sampler2D s_texture;void main() {vec4 resultColor = texture2D( s_texture, v_texCoord );if (resultColor.a==0.0) { discard; }; gl_FragColor = resultColor; }",
					   "pos,a_texCoord","s_texture"),
					   false,"");
	CHECK(Image.Create("attribute vec4 pos;varying vec2 v_texCoord;attribute vec2 a_texCoord;void main() {gl_Position = pos;v_texCoord = a_texCoord;}",
		"#ifdef GL_ES\nprecision mediump float;\n#endif\nvarying vec2 v_texCoord;uniform sampler2D s_texture;void main() {gl_FragColor = texture2D( s_texture, v_texCoord ); }",
		"pos,a_texCoord", "s_texture"),
		false, "");
	CHECK(ColorImage.Create("attribute vec4 pos;varying vec2 v_texCoord;attribute vec2 a_texCoord;void main() {gl_Position = pos;v_texCoord = a_texCoord;}", 
							"#ifdef GL_ES\nprecision mediump float;\n#endif\nuniform vec4 color;\nvarying vec2 v_texCoord;uniform sampler2D s_texture;void main() { gl_FragColor = texture2D( s_texture, v_texCoord )*color; }", 
							"pos,a_texCoord", "color,s_texture"), 
							false, "");
	LOG_INFO("Default shaders created ok !");
#ifdef PLATFORM_DEVELOP
	GLuint rboColor, rboDepth;

	// Color renderbuffer.
	CHECK_GLERROR(glGenRenderbuffers(1, &rboColor));
	CHECK_GLERROR(glBindRenderbuffer(GL_RENDERBUFFER, rboColor));
	// Set storage for currently bound renderbuffer.
	CHECK_GLERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height));

	// Depth renderbuffer
	CHECK_GLERROR(glGenRenderbuffers(1, &rboDepth));
	CHECK_GLERROR(glBindRenderbuffer(GL_RENDERBUFFER, rboDepth));
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	CHECK_GLERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));

	// stencil buffer
	//glGenRenderbuffers(1, &rboStencil);
	//glBindRenderbuffer(GL_RENDERBUFFER, rboStencil);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);

	// Framebuffer
	CHECK_GLERROR(glGenFramebuffers(1, &fbo));
	CHECK_GLERROR(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
	CHECK_GLERROR(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboColor));
	// Set renderbuffers for currently bound framebuffer
	CHECK_GLERROR(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth));
	// stencil buffer
	CHECK_GLERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth));
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboStencil);

	// Set to write to the framebuffer.
	CHECK_GLERROR(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

	// Tell glReadPixels where to read from.
	CHECK_GLERROR(glReadBuffer(GL_COLOR_ATTACHMENT0));
#endif


	return true;
}

void GApp::Graphics::GraphicsContext::Prepare()
{
	CurrentShader = NULL;
	CurrentTexture = NULL;

	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_STENCIL_TEST);
	ClippingEnabled = StencilsEnabled = false;
	glClearColor(0, 0, 0, 1.0f);	
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	
	ClearTranslate();
	DrawingBounds.Set(0,0,Width,Height);
	DefaultCoordinates = GAC_COORDINATES_PIXELS;
	UseDerivedShaderVariable = true;
	BlendImageMode = BLEND_IMAGE_MODE_NONE;
}
void GApp::Graphics::GraphicsContext::Finish()
{
	ClearClip();
	DisableAdvancedClipping();
}
bool GApp::Graphics::GraphicsContext::ChangeShader(GApp::Resources::Shader *s)
{
	CurrentShader = s;
	if (s!=NULL)
	{
		CHECK(s->Use(),false,"");
	}
	return true;
}
bool GApp::Graphics::GraphicsContext::DrawLine(float x1,float y1,float x2,float y2,unsigned int color, float penWidth,GApp::Resources::Shader *shader,GAC_TYPE_COORDINATES coord)
{
	if ((penWidth<=0) || ((color>>24)==0))
		return true;
	GLfloat v[4];
	PREPARE_COORD(coord);
	CONVERT_COORD_TO_PIXELS(v[0],v[1],x1,y1,coord);
	CONVERT_COORD_TO_PIXELS(v[2],v[3],x2,y2,coord);
	CONVERT_PIXELS_TO_OPENGL(v[0],v[1]);
	CONVERT_PIXELS_TO_OPENGL(v[2],v[3]);
	/*
	TRANSLATE_COORD(v[0],v[1],x1,y1,rapX,rapY,-1,1,1,-1,coord);
	TRANSLATE_COORD(v[2],v[3],x2,y2,rapX,rapY,-1,1,1,-1,coord);
	
	TRANSLATE(v[0],v[1]);
	TRANSLATE(v[2],v[3]);
	*/
	PREPARE_SHADER(shader,Default);
	CHECK(s->SetPosition(v),false,"");
	CHECK(s->SetUniformData(0,RED(color),GREEN(color),BLUE(color),ALPHA(color)),false,"");
	CHECK_GLERROR(glLineWidth(penWidth));
	CHECK_GLERROR(glDrawArrays(GL_LINES, 0, 2));
	return true;
}
bool GApp::Graphics::GraphicsContext::DrawLine(float x1,float y1,float x2,float y2,GApp::Resources::Shader *shader,float penWidth,GAC_TYPE_COORDINATES coord)
{
	if (penWidth<=0)
		return true;
	
	GLfloat v[4];
	PREPARE_COORD(coord);
	CONVERT_COORD_TO_PIXELS(v[0],v[1],x1,y1,coord);
	CONVERT_COORD_TO_PIXELS(v[2],v[3],x2,y2,coord);
	CONVERT_PIXELS_TO_OPENGL(v[0],v[1]);
	CONVERT_PIXELS_TO_OPENGL(v[2],v[3]);


	/*
	TRANSLATE_COORD(v[0],v[1],x1,y1,rapX,rapY,-1,1,1,-1,coord);
	TRANSLATE_COORD(v[2],v[3],x2,y2,rapX,rapY,-1,1,1,-1,coord);
	
	TRANSLATE(v[0],v[1]);
	TRANSLATE(v[2],v[3]);
	*/

	PREPARE_SHADER(shader,Default);

	CHECK(s->SetPosition(v),false,"");	
	CHECK_GLERROR(glLineWidth(penWidth));
	CHECK_GLERROR(glDrawArrays(GL_LINES, 0, 2));
	return true;
}

bool GApp::Graphics::GraphicsContext::DrawRect(float left,float top,float right,float bottom,unsigned int borderColor,unsigned int fillColor,float penWidth,GApp::Resources::Shader* shader,GAC_TYPE_COORDINATES coord)
{
	PREPARE_SHADER(shader,Default);

	GLfloat v[12];
	PREPARE_COORD(coord);
	CONVERT_COORD_TO_PIXELS(left,top,left,top,coord);
	CONVERT_COORD_TO_PIXELS(right,bottom,right,bottom,coord);
	CONVERT_PIXELS_TO_OPENGL(left,top);
	CONVERT_PIXELS_TO_OPENGL(right,bottom);

	/*
	TRANSLATE_COORD(left,top,left,top,rapX,rapY,-1,1,1,-1,coord);
	TRANSLATE_COORD(right,bottom,right,bottom,rapX,rapY,-1,1,1,-1,coord);	
	TRANSLATE(left,top);
	TRANSLATE(right,bottom);
	*/

	if (fillColor!=0)
	{
		v[0] = v[2] = v[6] = left;
		v[1] = v[7] = v[11] = bottom;
		v[3] = v[5] = v[9] = top;
		v[4] = v[8] = v[10] = right;
		
		CHECK(s->SetPosition(v),false,"");	
		CHECK(s->SetUniformData(0,RED(fillColor),GREEN(fillColor),BLUE(fillColor),ALPHA(fillColor)),false,"");
		CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6));
	}
	if ((borderColor!=0) && (penWidth>0))
	{
		v[0] = v[2] = left;
		v[1] = v[7] = top;
		v[3] = v[5] = bottom;
		v[4] = v[6] = right;

		CHECK(s->SetPosition(v),false,"");	
		//CHECK(s->SetUniformData(0,RED(fillColor),GREEN(fillColor),BLUE(fillColor),ALPHA(fillColor)));
		CHECK(s->SetUniformData(0,RED(borderColor),GREEN(borderColor),BLUE(borderColor),ALPHA(borderColor)),false,"");
		CHECK_GLERROR(glLineWidth(penWidth));
		CHECK_GLERROR(glDrawArrays(GL_LINE_LOOP, 0, 4));
	}

	return true;
}
bool GApp::Graphics::GraphicsContext::DrawRect(float left,float top,float right,float bottom,GApp::Resources::Shader* shader,GAC_TYPE_COORDINATES coord)
{
	PREPARE_SHADER(shader,Default);
	PREPARE_COORD(coord);
	CONVERT_COORD_TO_PIXELS(left,top,left,top,coord);
	CONVERT_COORD_TO_PIXELS(right,bottom,right,bottom,coord);
	CONVERT_PIXELS_TO_OPENGL(left,top);
	CONVERT_PIXELS_TO_OPENGL(right,bottom);

	/*
	TRANSLATE_COORD(left,top,left,top,rapX,rapY,-1,1,1,-1,coord);
	TRANSLATE_COORD(right,bottom,right,bottom,rapX,rapY,-1,1,1,-1,coord);

	TRANSLATE(left,top);
	TRANSLATE(right,bottom);
	*/

	GLfloat v[12];
	v[0] = v[2] = v[6] = left;
	v[1] = v[7] = v[11] = bottom;
	v[3] = v[5] = v[9] = top;
	v[4] = v[8] = v[10] = right;
		
	CHECK(s->SetPosition(v),false,"");		
	CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6));

	return true;
}
bool GApp::Graphics::GraphicsContext::DrawRectWH(float left, float top, float width, float height, unsigned int borderColor, unsigned int fillColor, float penWidth, GApp::Resources::Shader* shader, GAC_TYPE_COORDINATES coord)
{
	return DrawRect(left, top, left + width, top + height, borderColor, fillColor, penWidth, shader, coord);
}
bool GApp::Graphics::GraphicsContext::DrawRectF(RectF *rect, unsigned int borderColor, unsigned int fillColor, float penWidth, GApp::Resources::Shader* shader, GAC_TYPE_COORDINATES coord)
{
	CHECK(rect != NULL, false, "Expecting a valid RectF object !");
	return DrawRect(rect->Left, rect->Top, rect->Right, rect->Bottom, borderColor, fillColor, penWidth, shader, coord);
}


#define __DRAW_EXCLUSION_RECT__(valueLeft,valueTop,valueRight,valueBottom) {\
	l = valueLeft; t = valueTop; r = valueRight; b = valueBottom; \
	CONVERT_PIXELS_TO_OPENGL(l, t); \
	CONVERT_PIXELS_TO_OPENGL(r, b); \
	v[0] = v[2] = v[6] = l; v[1] = v[7] = v[11] = b; v[3] = v[5] = v[9] = t; v[4] = v[8] = v[10] = r; \
	CHECK(s->SetPosition(v), false, ""); \
	CHECK(s->SetUniformData(0, c_r, c_g, c_b, c_a), false, ""); \
	CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6)); \
}

bool GApp::Graphics::GraphicsContext::DrawExclusionRect(float left, float top, float right, float bottom, unsigned int fillColor, GAC_TYPE_COORDINATES coord)
{
	if (GET_ALPHA(fillColor) == 0)
		return true; // nu am alpha - degeaba desenez
	
	PREPARE_SHADER(NULL, Default);
	GLfloat v[12];

	PREPARE_COORD(coord);
	CONVERT_COORD_TO_PIXELS(left, top, left, top, coord);
	CONVERT_COORD_TO_PIXELS(right, bottom, right, bottom, coord);
	// rotunjesc la int
	left = (float)((int)left);
	right = (float)((int)right);
	top = (float)((int)top);
	bottom = (float)((int)bottom);

	float c_a = ALPHA(fillColor);
	float c_r = RED(fillColor);
	float c_g = GREEN(fillColor);
	float c_b = BLUE(fillColor);

	float l, r, t, b;

	// verific daca am un colt in ecran
	if ((left > this->Width) || (right<0) || (top>this->Height) || (bottom < 0))
	{
		// fac un fill la ecran si gata
		__DRAW_EXCLUSION_RECT__(0,0,this->Width,this->Height);
		return true;
	}
	// altfel am o zona comuna
	if (top<0)
		top = 0;
	if (left<0)
		left = 0;
	if (bottom>this->Height)
		bottom = this->Height;
	if (right>this->Width)
		right = this->Width;

	__DRAW_EXCLUSION_RECT__(0, 0, this->Width, top);
	__DRAW_EXCLUSION_RECT__(0, bottom, this->Width, this->Height);
	if (left > 0) {
		__DRAW_EXCLUSION_RECT__(0, top, left, bottom);
	}
	if (right < this->Width) {
		__DRAW_EXCLUSION_RECT__(right, top, this->Width, bottom);
	}
	return true;
}


bool GApp::Graphics::GraphicsContext::DrawImage(GApp::Resources::Bitmap *bmp, float x, float y, GApp::Resources::Shader* shader, GAC_TYPE_COORDINATES coord)
{
	PREPARE_SHADER(shader, Image);
	CHECK(bmp != NULL, false, "");

	GLfloat v[12];
	PREPARE_COORD(coord);
	CONVERT_COORD_TO_PIXELS(x, y, x, y, coord);
	CONVERT_PIXELS_TO_OPENGL(x, y);
	/*
	TRANSLATE_COORD(x,y,x,y,rapX,rapY,-1,1,1,-1,coord);
	TRANSLATE(x,y);
	*/

#if defined(DYNAMIC_IMAGE_RESIZE)
	float scale = 1.0f;
	scale *= ((float)bmp->Width);
	scale /= ((float)bmp->OriginalWidth);
	if (scale == 1.0f)
	{
		v[0] = v[2] = v[6] = x;
		v[1] = v[7] = v[11] = y - bmp->Coordonates[13];
		v[3] = v[5] = v[9] = y;
		v[4] = v[8] = v[10] = x + bmp->Coordonates[12];
		CHECK(s->SetPosition(v), false, "");
		CHECK(s->SetAttributeData(1, bmp->Coordonates, 2), false, "");
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

		//CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		//CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	}
	else {
		v[0] = v[2] = v[6] = x;
		v[1] = v[7] = v[11] = y - bmp->Coordonates[13] * scale;
		v[3] = v[5] = v[9] = y;
		v[4] = v[8] = v[10] = x + bmp->Coordonates[12] * scale;
		CHECK(s->SetPosition(v), false, "");
		CHECK(s->SetAttributeData(1, bmp->Coordonates, 2), false, "");
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		//CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		//CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	}

	CHECK_GLERROR(glBindTexture(GL_TEXTURE_2D, bmp->TextureID));
	CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6));
#elif defined(RESIZE_AT_STARTUP_IMAGE_RESIZE)
	v[0] = v[2] = v[6] = x;
	v[1] = v[7] = v[11] = y - bmp->Coordonates[13];
	v[3] = v[5] = v[9] = y;
	v[4] = v[8] = v[10] = x + bmp->Coordonates[12];
	CHECK(s->SetPosition(v), false, "");
	CHECK(s->SetAttributeData(1, bmp->Coordonates, 2), false, "");
	CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	
	//CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	//CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	CHECK_GLERROR(glBindTexture(GL_TEXTURE_2D, bmp->TextureID));
	CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6));
#else
	if ((bmp->Width != bmp->OriginalWidth) && ((bmp->Flags & GAC_RESOURCEFLAG_RESIZEMODE) == GAC_RESIZEMODE_DYNAMIC))
	{
		float scale = 1.0f;
		scale *= ((float)bmp->Width);
		scale /= ((float)bmp->OriginalWidth);
		if (scale == 1.0f)
		{
			v[0] = v[2] = v[6] = x;
			v[1] = v[7] = v[11] = y - bmp->Coordonates[13];
			v[3] = v[5] = v[9] = y;
			v[4] = v[8] = v[10] = x + bmp->Coordonates[12];
			CHECK(s->SetPosition(v), false, "");
			CHECK(s->SetAttributeData(1, bmp->Coordonates, 2), false, "");
			CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		}
		else {
			v[0] = v[2] = v[6] = x;
			v[1] = v[7] = v[11] = y - bmp->Coordonates[13] * scale;
			v[3] = v[5] = v[9] = y;
			v[4] = v[8] = v[10] = x + bmp->Coordonates[12] * scale;
			CHECK(s->SetPosition(v), false, "");
			CHECK(s->SetAttributeData(1, bmp->Coordonates, 2), false, "");
			CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

			//CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			//CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		}

		CHECK_GLERROR(glBindTexture(GL_TEXTURE_2D, bmp->TextureID));
		CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6));
	} else {
		v[0] = v[2] = v[6] = x;
		v[1] = v[7] = v[11] = y - bmp->Coordonates[13];
		v[3] = v[5] = v[9] = y;
		v[4] = v[8] = v[10] = x + bmp->Coordonates[12];
		CHECK(s->SetPosition(v), false, "");
		CHECK(s->SetAttributeData(1, bmp->Coordonates, 2), false, "");
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

		//CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		//CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		CHECK_GLERROR(glBindTexture(GL_TEXTURE_2D, bmp->TextureID));
		CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6));
	}
#endif



	return true;
}
bool GApp::Graphics::GraphicsContext::DrawImageWithAlphaBlending(GApp::Resources::Bitmap *bmp, float x, float y, float alpha, GAC_TYPE_COORDINATES coord)
{
	SET_ALPHA_TRANSPARENCY(alpha);
	return DrawImage(bmp, x, y, &ColorImage, coord);
}
bool GApp::Graphics::GraphicsContext::DrawImageWithColorBlending(GApp::Resources::Bitmap *bmp, float x, float y, unsigned int color, GAC_TYPE_COORDINATES coord)
{
	SET_COLOR_BLEND(color);
	return DrawImage(bmp, x, y, &ColorImage, coord);
}

bool GApp::Graphics::GraphicsContext::DrawImageScaled(GApp::Resources::Bitmap *bmp,float x,float y, float scale, GApp::Resources::Shader *shader)
{
	PREPARE_SHADER(shader,Image);
			
	GLfloat v[12];

	CONVERT_COORD_TO_PIXELS(x,y,x,y,GAC_COORDINATES_PIXELS);
	CONVERT_PIXELS_TO_OPENGL(x,y);

	/*
	TRANSLATE_COORD(x,y,x,y,rapX,rapY,-1,1,1,-1,GAC_COORDINATES_PIXELS);
	TRANSLATE(x,y);
	*/

#if defined(DYNAMIC_IMAGE_RESIZE)
	if (bmp->Width != bmp->OriginalWidth)
	{
		scale *= ((float)bmp->Width);
		scale /= ((float)bmp->OriginalWidth);
	}
#elif defined(RESIZE_AT_STARTUP_IMAGE_RESIZE)
	// nu facem nimik 
#else
	if ((bmp->Width != bmp->OriginalWidth) && ((bmp->Flags & GAC_RESOURCEFLAG_RESIZEMODE) == GAC_RESIZEMODE_DYNAMIC))
	{
		scale *= ((float)bmp->Width);
		scale /= ((float)bmp->OriginalWidth);
	}
#endif

	if (scale==1.0f)
	{
		v[0] = v[2] = v[6] = x;
		v[1] = v[7] = v[11] = y-bmp->Coordonates[13];
		v[3] = v[5] = v[9] = y;
		v[4] = v[8] = v[10] = x+bmp->Coordonates[12];
		CHECK(s->SetPosition(v),false,"");	
		CHECK(s->SetAttributeData(1,bmp->Coordonates,2),false,"");
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	} else {
		v[0] = v[2] = v[6] = x;
		v[1] = v[7] = v[11] = y-bmp->Coordonates[13]*scale;
		v[3] = v[5] = v[9] = y;
		v[4] = v[8] = v[10] = x+bmp->Coordonates[12]*scale;
		CHECK(s->SetPosition(v),false,"");	
		CHECK(s->SetAttributeData(1,bmp->Coordonates,2),false,"");
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	}
	
	CHECK_GLERROR(glBindTexture(GL_TEXTURE_2D,bmp->TextureID));
	CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6));

	return true;
}
bool GApp::Graphics::GraphicsContext::DrawImageScaledWithAlphaBlending(GApp::Resources::Bitmap *bmp, float x, float y, float scale, float alpha)
{
	SET_ALPHA_TRANSPARENCY(alpha);
	return DrawImageScaled(bmp, x, y, scale, &ColorImage);
}
bool GApp::Graphics::GraphicsContext::DrawImageScaledWithColorBlending(GApp::Resources::Bitmap *bmp, float x, float y, float scale, unsigned int color)
{
	SET_COLOR_BLEND(color);
	return DrawImageScaled(bmp, x, y, scale, &ColorImage);
}


bool GApp::Graphics::GraphicsContext::Draw9PatchImage(GApp::Resources::Bitmap *bmp, float x, float y, float w, float h, float pLeft, float pTop, float pRight, float pBottom, GApp::Resources::Shader *shader)
{
	// nu trebuie sa fie musai pe 4 dimensiun - poate sa fie si cu doar lungit  sau doar latit (de la caz la caz)
	RETURNERROR(false, "Not implemented yet !");
}

bool GApp::Graphics::GraphicsContext::DrawImageAligned(GApp::Resources::Bitmap *bmp,float x,float y, GAC_TYPE_ALIGNAMENT align, float scale, GApp::Resources::Shader* shader,GAC_TYPE_COORDINATES coord)
{
	PREPARE_SHADER(shader,Image);	
	float x2, y2, w, h;
	w = bmp->Width*scale;
	h = bmp->Height*scale;

#if defined(DYNAMIC_IMAGE_RESIZE)
	if (bmp->Width != bmp->OriginalWidth)
	{
		scale *= ((float)bmp->Width);
		scale /= ((float)bmp->OriginalWidth);
	}
#elif defined(RESIZE_AT_STARTUP_IMAGE_RESIZE)
	// nu facem nimik 
#else
	if ((bmp->Width != bmp->OriginalWidth) && ((bmp->Flags & GAC_RESOURCEFLAG_RESIZEMODE) == GAC_RESIZEMODE_DYNAMIC))
	{
		scale *= ((float)bmp->Width);
		scale /= ((float)bmp->OriginalWidth);
	}
#endif


	PREPARE_COORD(coord);
	CONVERT_COORD_TO_PIXELS(x,y,x,y,coord);
	ALIGN_COORD(x,y,x,y,w,h,align);
	x2 = x+w;
	y2 = y+h;
	CONVERT_PIXELS_TO_OPENGL(x,y);
	CONVERT_PIXELS_TO_OPENGL(x2,y2);
	
	
	/*
	TRANSLATE_COORD(x,y,x,y,rapX,rapY,-1,1,1,-1,coord);
	TRANSLATE_COORD(x2,y2,x2,y2,rapX,rapY,-1,1,1,-1,coord);
	TRANSLATE(x,y);
	TRANSLATE(x2,y2);
	*/
	GLfloat v[12];
	v[0] = v[2] = v[6] = x;
	v[1] = v[7] = v[11] = y2;
	v[3] = v[5] = v[9] = y;
	v[4] = v[8] = v[10] = x2;
	CHECK(s->SetPosition(v),false,"");	
	CHECK(s->SetAttributeData(1,bmp->Coordonates,2),false,"");
	if (scale==1.0f)
	{
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	} else {
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	}
	CHECK_GLERROR(glBindTexture(GL_TEXTURE_2D,bmp->TextureID));
	CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6));

	return true;	
}
bool GApp::Graphics::GraphicsContext::DrawImageAlignedWithAlphaBlending(GApp::Resources::Bitmap *bmp, float x, float y, GAC_TYPE_ALIGNAMENT align, float scale, float alpha, GAC_TYPE_COORDINATES coord)
{
	SET_ALPHA_TRANSPARENCY(alpha);
	return DrawImageAligned(bmp, x, y, align, scale, &ColorImage, coord);
}
bool GApp::Graphics::GraphicsContext::DrawImageAlignedWithColorBlending(GApp::Resources::Bitmap *bmp, float x, float y, GAC_TYPE_ALIGNAMENT align, float scale, unsigned int color, GAC_TYPE_COORDINATES coord)
{
	SET_COLOR_BLEND(color);
	return DrawImageAligned(bmp, x, y, align, scale, &ColorImage, coord);
}

bool GApp::Graphics::GraphicsContext::DrawImageResizedAndAligned(GApp::Resources::Bitmap *bmp, float x, float y, GAC_TYPE_ALIGNAMENT align, float scaleWidth, float scaleHeight, GApp::Resources::Shader* shader, GAC_TYPE_COORDINATES coord)
{
	PREPARE_SHADER(shader, Image);
	float x2, y2, w, h;
	w = bmp->Width*scaleWidth;
	h = bmp->Height*scaleHeight;

#if defined(DYNAMIC_IMAGE_RESIZE)
	if (bmp->Width != bmp->OriginalWidth)
	{
		float scale = ((float)bmp->Width) / ((float)bmp->OriginalWidth);
		scaleWidth *= scale;
		scaleHeight *= scale;
	}
#elif defined(RESIZE_AT_STARTUP_IMAGE_RESIZE)
	// nu facem nimik 
#else
	if ((bmp->Width != bmp->OriginalWidth) && ((bmp->Flags & GAC_RESOURCEFLAG_RESIZEMODE) == GAC_RESIZEMODE_DYNAMIC))
	{
		float scale = ((float)bmp->Width) / ((float)bmp->OriginalWidth);
		scale /= ((float)bmp->OriginalWidth);
		scaleWidth *= scale;
		scaleHeight *= scale;
	}
#endif


	PREPARE_COORD(coord);
	CONVERT_COORD_TO_PIXELS(x, y, x, y, coord);
	ALIGN_COORD(x, y, x, y, w, h, align);
	x2 = x + w;
	y2 = y + h;
	CONVERT_PIXELS_TO_OPENGL(x, y);
	CONVERT_PIXELS_TO_OPENGL(x2, y2);


	/*
	TRANSLATE_COORD(x,y,x,y,rapX,rapY,-1,1,1,-1,coord);
	TRANSLATE_COORD(x2,y2,x2,y2,rapX,rapY,-1,1,1,-1,coord);
	TRANSLATE(x,y);
	TRANSLATE(x2,y2);
	*/
	GLfloat v[12];
	v[0] = v[2] = v[6] = x;
	v[1] = v[7] = v[11] = y2;
	v[3] = v[5] = v[9] = y;
	v[4] = v[8] = v[10] = x2;
	CHECK(s->SetPosition(v), false, "");
	CHECK(s->SetAttributeData(1, bmp->Coordonates, 2), false, "");
	if ((scaleWidth == 1.0f) && (scaleHeight == 1.0f))
	{
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	}
	else {
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	}
	CHECK_GLERROR(glBindTexture(GL_TEXTURE_2D, bmp->TextureID));
	CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6));

	return true;
}
bool GApp::Graphics::GraphicsContext::DrawImageResizedAndAlignedWithColorBlending(GApp::Resources::Bitmap *bmp, float x, float y, GAC_TYPE_ALIGNAMENT align, float scaleWidth, float scaleHeight, unsigned int color, GAC_TYPE_COORDINATES coord)
{
	SET_COLOR_BLEND(color);
	return DrawImageResizedAndAligned(bmp, x, y, align, scaleWidth, scaleHeight, &ColorImage, coord);
}

bool GApp::Graphics::GraphicsContext::DrawImageResized(GApp::Resources::Bitmap *bmp,float x,float y, float w, float h, GApp::Resources::Shader *shader,GAC_TYPE_COORDINATES coord)
{
	PREPARE_SHADER(shader,Image);
	GLfloat v[12];
	float x2,y2;
	PREPARE_COORD(coord);
	CONVERT_COORD_TO_PIXELS(x2,y2,x+w,y+h,coord);
	CONVERT_COORD_TO_PIXELS(x,y,x,y,coord);
	CONVERT_PIXELS_TO_OPENGL(x,y);
	CONVERT_PIXELS_TO_OPENGL(x2,y2);
	/*
	TRANSLATE_COORD(x2,y2,x+w,y+h,rapX,rapY,-1,1,1,-1,coord);
	TRANSLATE_COORD(x,y,x,y,rapX,rapY,-1,1,1,-1,coord);	
	TRANSLATE(x,y);
	TRANSLATE(x2,y2);
	*/
	v[0] = v[2] = v[6] = x;
	v[1] = v[7] = v[11] = y2;
	v[3] = v[5] = v[9] = y;
	v[4] = v[8] = v[10] = x2;
	CHECK(s->SetPosition(v),false,"");	
	CHECK(s->SetAttributeData(1,bmp->Coordonates,2),false,"");
	CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	//CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 33071));
	//CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 33071));
	CHECK_GLERROR(glBindTexture(GL_TEXTURE_2D,bmp->TextureID));
	CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6));

	return true;
}
bool GApp::Graphics::GraphicsContext::DrawImageResizedWithAlphaBlending(GApp::Resources::Bitmap *bmp, float x, float y, float w, float h, float alpha, GAC_TYPE_COORDINATES coord)
{
	SET_ALPHA_TRANSPARENCY(alpha);
	return DrawImageResized(bmp, x, y, w, h, &ColorImage, coord);
}
bool GApp::Graphics::GraphicsContext::DrawImageResizedWithColorBlending(GApp::Resources::Bitmap *bmp, float x, float y, float w, float h, unsigned int color, GAC_TYPE_COORDINATES coord)
{
	SET_COLOR_BLEND(color);
	return DrawImageResized(bmp, x, y, w, h, &ColorImage, coord);
}


bool GApp::Graphics::GraphicsContext::DrawImageResized(GApp::Resources::Bitmap *bmp, float imageLeft, float imageTop, float imageWidth, float imageHeight, float x, float y, float w, float h, GApp::Resources::Shader* shader)
{
	PREPARE_SHADER(shader, Image);
	CHECK(bmp != NULL, false, "Invalid - null image");
	float x2 = x + w;
	float y2 = y + h;
	GLfloat v[12];
	float	Coordonates[12];
	CONVERT_PIXELS_TO_OPENGL(x, y);
	CONVERT_PIXELS_TO_OPENGL(x2, y2);
	v[0] = v[2] = v[6] = x;
	v[1] = v[7] = v[11] = y2;
	v[3] = v[5] = v[9] = y;
	v[4] = v[8] = v[10] = x2;

	//Coordonates[0] = Coordonates[2] = Coordonates[6] = ((float)left) / ((float)tex->Width);
	//Coordonates[1] = Coordonates[7] = Coordonates[11] = 1.0f - ((float)bottom) / ((float)tex->Height);
	//Coordonates[3] = Coordonates[5] = Coordonates[9] = 1.0f - ((float)top) / ((float)tex->Height);
	//Coordonates[4] = Coordonates[8] = Coordonates[10] = ((float)right) / ((float)tex->Width);

	float oglWidth = bmp->Coordonates[4] - bmp->Coordonates[0];
	float oglHeight = bmp->Coordonates[3] - bmp->Coordonates[1];
	Coordonates[0] = Coordonates[2] = Coordonates[6] = ((imageLeft*oglWidth) / bmp->Width) + bmp->Coordonates[0];
	Coordonates[4] = Coordonates[8] = Coordonates[10] = (((imageLeft + imageWidth)*oglWidth) / bmp->Width) + bmp->Coordonates[0];
	Coordonates[3] = Coordonates[5] = Coordonates[9] = bmp->Coordonates[3] - ((imageTop*oglHeight) / bmp->Height);
	Coordonates[1] = Coordonates[7] = Coordonates[11] = bmp->Coordonates[3] - (((imageTop + imageHeight)*oglHeight) / bmp->Height);
	//Coordonates[1] = Coordonates[7] = Coordonates[11] = ((imageTop*oglHeight) / bmp->Height) + bmp->Coordonates[1];
	//Coordonates[3] = Coordonates[5] = Coordonates[9] = (((imageTop+imageHeight)*oglHeight) / bmp->Height) + bmp->Coordonates[1];

	CHECK(s->SetPosition(v), false, "");
	CHECK(s->SetAttributeData(1, Coordonates, 2), false, "");
	CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	CHECK_GLERROR(glBindTexture(GL_TEXTURE_2D, bmp->TextureID));
	CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6));

	return true;
}
bool GApp::Graphics::GraphicsContext::DrawImageResizedWithAlphaBlending(GApp::Resources::Bitmap *bmp, float imageLeft, float imageTop, float imageWidth, float imageHeight, float x, float y, float w, float h, float alpha)
{
	SET_ALPHA_TRANSPARENCY(alpha);
	return DrawImageResized(bmp, imageLeft, imageTop, imageWidth, imageHeight, x, y, w, h, &ColorImage);
}
bool GApp::Graphics::GraphicsContext::DrawImageResizedWithColorBlending(GApp::Resources::Bitmap *bmp, float imageLeft, float imageTop, float imageWidth, float imageHeight, float x, float y, float w, float h, unsigned int color)
{
	SET_COLOR_BLEND(color);
	return DrawImageResized(bmp, imageLeft, imageTop, imageWidth, imageHeight, x, y, w, h, &ColorImage);
}

bool GApp::Graphics::GraphicsContext::DrawImageCentered(GApp::Resources::Bitmap *bmp,float x,float y,GApp::Resources::Shader* shader,GAC_TYPE_COORDINATES coord)
{
	return DrawImageAligned(bmp,x,y,GAC_ALIGNAMENT_CENTER,1.0f,shader,coord);
}
bool GApp::Graphics::GraphicsContext::DrawImageCenteredWithAlphaBlending(GApp::Resources::Bitmap *bmp, float x, float y, float alpha, GAC_TYPE_COORDINATES coord)
{
	SET_ALPHA_TRANSPARENCY(alpha);
	return DrawImageCentered(bmp, x, y, &ColorImage, coord);
}
bool GApp::Graphics::GraphicsContext::DrawImageCenteredWithColorBlending(GApp::Resources::Bitmap *bmp, float x, float y, unsigned int color, GAC_TYPE_COORDINATES coord)
{
	SET_COLOR_BLEND(color);
	return DrawImageCentered(bmp, x, y, &ColorImage, coord);
}


bool GApp::Graphics::GraphicsContext::FillScreen(unsigned int color)
{
	return DrawRect(0.0f,0.0f,1.0f,1.0f,0,color,0.0f,NULL,GAC_COORDINATES_PERCENTAGE);
}
bool GApp::Graphics::GraphicsContext::FillScreen(GApp::Resources::Bitmap *bmp,GApp::Resources::Shader *shader)
{
	return DrawImageResized(bmp,0.0f,0.0f,1.0f,1.0f,shader,GAC_COORDINATES_PERCENTAGE);
}
bool GApp::Graphics::GraphicsContext::FillScreenWithAlphaBlending(GApp::Resources::Bitmap *bmp, float alpha)
{
	SET_ALPHA_TRANSPARENCY(alpha);
	return FillScreen(bmp, &ColorImage);
}
bool GApp::Graphics::GraphicsContext::FillScreenWithColorBlending(GApp::Resources::Bitmap *bmp, unsigned int color)
{
	SET_COLOR_BLEND(color);
	return FillScreen(bmp, &ColorImage);
}
bool GApp::Graphics::GraphicsContext::FillScreen(GApp::Resources::Shader *shader)
{
	return DrawRect(0.0f,0.0f,1.0f,1.0f,shader,GAC_COORDINATES_PERCENTAGE);
}


bool GApp::Graphics::GraphicsContext::DrawTexture(unsigned int texID)
{
	/*
	CHECK(Image.Use2());
	float v[12];
	v[0] = v[2] = v[6] = 0;
	v[1] = v[7] = v[11] = 1;
	v[3] = v[5] = v[9] = 0;
	v[4] = v[8] = v[10] = 1;

	float Coordonates[12];

	Coordonates[0] = Coordonates[2] = Coordonates[6] = 0;
	Coordonates[1] = Coordonates[7] = Coordonates[11] = 1.0f;
	Coordonates[3] = Coordonates[5] = Coordonates[9] = 0;
	Coordonates[4] = Coordonates[8] = Coordonates[10] = 1.0f;

	CHECK(Image.SetPosition(v));	
	CHECK(Image.SetAttributeData(1,Coordonates,2));
	CHECK_GLERROR(glBindTexture(GL_TEXTURE_2D,texID));
	CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	CHECK_GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	CHECK_GLERROR(glDrawArrays(GL_TRIANGLES, 0, 6));
	*/
	return true;

}

bool GApp::Graphics::GraphicsContext::Draw(ImagePainter *ip)
{
	CHECK(ip!=NULL,false,"");
	if (ip->Image==NULL)
		return false;
	switch (ip->Flags & (PAINT_OBJECT_NO_RESIZE|PAINT_OBJECT_VALID))
	{
		case PAINT_OBJECT_VALID:
			if (ip->Image != NULL)
			{
				switch (ip->Blend.Mode)
				{
					case BLEND_IMAGE_MODE_ALPHA:
						return DrawImageResizedWithAlphaBlending(ip->Image, ip->Left, ip->Top, ip->Width, ip->Height, ip->Blend.Alpha);
					case BLEND_IMAGE_MODE_COLOR:
						return DrawImageResizedWithColorBlending(ip->Image, ip->Left, ip->Top, ip->Width, ip->Height, ip->Blend.Color);
					default:
						ip->UpdateShaderParams();
						return DrawImageResized(ip->Image, ip->Left, ip->Top, ip->Width, ip->Height, ip->S);
				}
			}
		case PAINT_OBJECT_VALID|PAINT_OBJECT_NO_RESIZE:
			if (ip->Image != NULL)
			{
				switch (ip->Blend.Mode)
				{
					case BLEND_IMAGE_MODE_ALPHA:
						return DrawImageWithAlphaBlending(ip->Image, ip->Left, ip->Top, ip->Blend.Alpha,(unsigned int)GAC_COORDINATES_AUTO);
					case BLEND_IMAGE_MODE_COLOR:
						return DrawImageWithColorBlending(ip->Image, ip->Left, ip->Top, ip->Blend.Color);
					default:
						ip->UpdateShaderParams();
						return DrawImage(ip->Image, ip->Left, ip->Top, ip->S);
				}
			}
		default:
			RETURNERROR(false,"Image is not set properly !");
	}
}
bool GApp::Graphics::GraphicsContext::DrawString(TextPainter *tp,float offsetX,float offsetY)
{
	CHECK(tp!=NULL,false,"");
	CHECK(tp->TextFont!=NULL,false,"");
	unsigned int size = tp->Characters.GetSize()/sizeof(GlyphLocation);
	if (size==0)
		return true;
	GlyphLocation *g = (GlyphLocation*)tp->Characters.Data;
	if (g==NULL)
		return false;
	GApp::Resources::Shader *shd;
	switch (tp->Blend.Mode)
	{
		case BLEND_IMAGE_MODE_ALPHA:
			shd = &ColorImage;
			SET_ALPHA_TRANSPARENCY(tp->Blend.Alpha);
			break;
		case BLEND_IMAGE_MODE_COLOR:
			shd = &ColorImage;
			SET_COLOR_BLEND(tp->Blend.Color);
			break;
		default:
			shd = tp->S;
			tp->UpdateShaderParams();
			break;
	}
	
	float fontHeight = 0;
	//bool extra = (g->Code == 'M') && (tp->PaintScale<1) && (size<12);
	if (tp->PaintScale==1)
	{
		while (size>0)
		{
			if (g->Image!=NULL)
			{
				if (g->Visible)
					DrawImage(g->Image, g->x + offsetX, g->y + offsetY, shd);
			}
			else {
				if ((g->Code != ' ') && (g->Code != '\t') && (g->Code != '\n'))
				{
					if (fontHeight == 0)
					{
						GlyphLocation *g2 = (GlyphLocation*)tp->Characters.Data;
						unsigned int sz2 = tp->Characters.GetSize() / sizeof(GlyphLocation);
						while (sz2 > 0)
						{
							if (g2->Image != NULL)
								fontHeight = GAC_MAX(g2->Image->Height, fontHeight);
							g2++;
							sz2--;
						}
					}
					LOG_ERROR("Unable to find character: %d (Height = %f, Image = %p)", g->Code,fontHeight, g->Image);
					if (g->Visible)
						DrawRect(g->x + offsetX, g->y + offsetY, g->x + tp->TextFont->SpaceWidth + offsetX, g->y - fontHeight + offsetY, 0xFFFFFF00, 0xFFFF0000);
				}
			}
			g++;
			size--;
		}
	} else {
		while (size>0)
		{
			//if (extra)
			//{
			//	LOG_INFO("Print character: %d (X=%f,Y=%f) Image=%p", g->Code, g->x, g->y, g->Image);
			//}
			if (g->Image!=NULL)
			{
				if (g->Visible)
					DrawImageScaled(g->Image, g->x + offsetX, g->y + offsetY, tp->PaintScale, shd);
			}
			else {
				if ((g->Code != ' ') && (g->Code != '\t') && (g->Code!='\n'))
				{
					if (fontHeight == 0)
					{
						GlyphLocation *g2 = (GlyphLocation*)tp->Characters.Data;
						unsigned int sz2 = tp->Characters.GetSize() / sizeof(GlyphLocation);
						while (sz2 > 0)
						{
							if (g2->Image != NULL)
								fontHeight = GAC_MAX(g2->Image->Height, fontHeight);
							g2++;
							sz2--;
						}
					}
					LOG_ERROR("Unable to find character: %d (Height = %f, Image = %p)", g->Code, fontHeight, g->Image);
					if (g->Visible)
						DrawRect(g->x + offsetX, g->y + offsetY, g->x + tp->TextFont->SpaceWidth*tp->PaintScale + offsetX, g->y - fontHeight*tp->PaintScale + offsetY, 0xFFFFFF00, 0xFFFF0000);
				}
			}
			g++;
			size--;
		}	
	}
	return true;
}
bool GApp::Graphics::GraphicsContext::Draw(TextImagePainter *tip)
{
	CHECK(tip!=NULL,false,"");
	Draw(&tip->Background);
	Draw(&tip->Face);
	if (tip->Text.TextFont!=NULL)
		this->DrawString(&tip->Text);
	return true;
}
void GApp::Graphics::GraphicsContext::SetTranslate(float x, float y)
{
	//TranslateX = x*rapX;
	//TranslateY = y*rapY;
	TranslateX = x;
	TranslateY = y;
}
void GApp::Graphics::GraphicsContext::ClearTranslate()
{
	TranslateX = TranslateY = 0;	
}
bool GApp::Graphics::GraphicsContext::SetClip(int left,int top,int right,int bottom)
{
	if (!ClippingEnabled)
	{
		CHECK_GLERROR(glEnable(GL_SCISSOR_TEST));
		ClippingEnabled = true;
	}
	CHECK_GLERROR(glScissor(left,top,right-left,bottom-top));
	ClipRect.Left = left;
	ClipRect.Top = top;
	ClipRect.Right = right;
	ClipRect.Bottom = bottom;
	return true;
}
bool GApp::Graphics::GraphicsContext::SetClip(Rect *rect)
{
	CHECK(rect!=NULL,false,"");
	return SetClip(rect->Left,rect->Top,rect->Right,rect->Bottom);
}
void GApp::Graphics::GraphicsContext::GetClipRect(Rect *rect)
{
	rect->Left = ClipRect.Left;
	rect->Top = ClipRect.Top;
	rect->Bottom = ClipRect.Bottom;
	rect->Right = ClipRect.Right;
}
bool GApp::Graphics::GraphicsContext::ClearClip()
{
	if (ClippingEnabled)
	{
		CHECK_GLERROR(glDisable(GL_SCISSOR_TEST));
		ClippingEnabled = false;
	}
	return true;
}
bool GApp::Graphics::GraphicsContext::SetCoordinatesType(GAC_TYPE_COORDINATES newCoordType)
{
	CHECK(newCoordType!=GAC_COORDINATES_AUTO,false,"");
	DefaultCoordinates = newCoordType;
	return true;
}
bool GApp::Graphics::GraphicsContext::SetDrawingBoundsRect(float pxLeft,float pxTop,float pxRight,float pxBottom)
{
	CHECK((pxLeft<=pxRight) && (pxTop<=pxBottom),false,"");
	DrawingBounds.Set(pxLeft,pxTop,pxRight,pxBottom);
	return true;
}
float GApp::Graphics::GraphicsContext::CoordinateToXPixel(float value,GAC_TYPE_COORDINATES coord)
{
	PREPARE_COORD(coord);
	switch (coord)
	{
		case GAC_COORDINATES_PIXELS: return value; 
		case GAC_COORDINATES_PERCENTAGE: return value*Width; 
		case GAC_COORDINATES_PERCENTAGEOFSCREENWIDTH: return value*Width;
		case GAC_COORDINATES_PERCENTAGEOFSCREEHEIGHT: return value*Height;
		case GAC_COORDINATES_DRAWINGBOUNDS: return DrawingBounds.Left+((DrawingBounds.Right-DrawingBounds.Left)*(value));
	}
	return 0.0f;
}
float GApp::Graphics::GraphicsContext::CoordinateToYPixel(float value,GAC_TYPE_COORDINATES coord)
{
	PREPARE_COORD(coord);
	switch (coord)
	{
		case GAC_COORDINATES_PIXELS: return value; 
		case GAC_COORDINATES_PERCENTAGE: return value*Height; 
		case GAC_COORDINATES_PERCENTAGEOFSCREENWIDTH: return value*Width;
		case GAC_COORDINATES_PERCENTAGEOFSCREEHEIGHT: return value*Height;
		case GAC_COORDINATES_DRAWINGBOUNDS: return DrawingBounds.Top+((DrawingBounds.Bottom-DrawingBounds.Top)*(value));
	}
	return 0.0f;
}


bool GApp::Graphics::GraphicsContext::SetClipRect(bool inside, float x1, float y1, float x2, float y2, GAC_TYPE_COORDINATES coord)
{
	if (inside)
	{
		//LOG_INFO("SetClipRect(true,%f,%f,%f,%f)", x1, y1, x2, y2);
		PREPARE_COORD(coord);
		CONVERT_COORD_TO_PIXELS(x1, y1, x1, y1, coord);
		CONVERT_COORD_TO_PIXELS(x2, y2, x2, y2, coord);
		y1 = Height - y1;
		y2 = Height - y2;
		//LOG_INFO("SetClipRect => AFTER (true,%f,%f,%f,%f)", x1, y1, x2, y2);
		// trebuie inversate si coordonatele (de la cel mai mic la cel mai mare)
		return SetClip((int)x1, (int)y2, (int)x2, (int)y1);
	}

	if (!StencilsEnabled)
	{
		CHECK_GLERROR(glEnable(GL_STENCIL_TEST));
		StencilsEnabled = true;
	}
	CHECK_GLERROR(glClearStencil(0));
	CHECK_GLERROR(glClear(GL_STENCIL_BUFFER_BIT));
	CHECK_GLERROR(glStencilFunc(GL_ALWAYS, 1, 1));
	CHECK_GLERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
	CHECK_GLERROR(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
	CHECK_GLERROR(glDepthMask(GL_FALSE));
	CHECK(DrawRect(x1, y1, x2, y2, 0, 0xFFFFFFFF, 0, NULL, coord), false, "");
	if (inside) {
		CHECK_GLERROR(glStencilFunc(GL_EQUAL, 1, 1));
	} else {
		CHECK_GLERROR(glStencilFunc(GL_NOTEQUAL, 1, 1));
	}	
	CHECK_GLERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP));
	CHECK_GLERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
	CHECK_GLERROR(glDepthMask(GL_TRUE));

	return true;
}
bool GApp::Graphics::GraphicsContext::SetClipImage(bool inside, GApp::Resources::Bitmap *bmp, float x, float y, GAC_TYPE_ALIGNAMENT align, float scale, GAC_TYPE_COORDINATES coord)
{
	if (!StencilsEnabled)
	{
		CHECK_GLERROR(glEnable(GL_STENCIL_TEST));
		StencilsEnabled = true;
	}
	CHECK_GLERROR(glClearStencil(0));
	CHECK_GLERROR(glClear(GL_STENCIL_BUFFER_BIT));
	CHECK_GLERROR(glStencilFunc(GL_ALWAYS, 1, 1));
	CHECK_GLERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
	CHECK_GLERROR(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
	CHECK_GLERROR(glDepthMask(GL_FALSE));
	CHECK(DrawImageAligned(bmp, x, y, align, scale, &ImageNoAlpha, coord), false, "");
	if (inside) {
		CHECK_GLERROR(glStencilFunc(GL_EQUAL, 1, 1));
	}
	else {
		CHECK_GLERROR(glStencilFunc(GL_NOTEQUAL, 1, 1));
	}
	CHECK_GLERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP));
	CHECK_GLERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
	CHECK_GLERROR(glDepthMask(GL_TRUE));

	return true;
}
bool GApp::Graphics::GraphicsContext::DisableAdvancedClipping()
{
	if (StencilsEnabled)
	{
		CHECK_GLERROR(glDisable(GL_STENCIL_TEST));
		StencilsEnabled = false;
	}
	ClearClip();
	return true;
}
