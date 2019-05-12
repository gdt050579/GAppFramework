#include "GApp.h"

// moduri
// - normal - scriu pana la \n si apoi trec pe linia urmatoare
// - word wrap - scriu pana la \n sau am ajuns la size
// - justify
// FLAGS:
// 0-4	- Alignament Text
// 4-8	- tip x,y - Coordonates Type
// 8-10 - Char Mode
// 11-14- Font Size


#define CHAR_MODE_NORMAL		0
#define CHAR_MODE_WORD_WRAP		1
#define CHAR_MODE_JUSTIFY		2
#define TEXT_PAINT_MODE_DOCK	0x10000


#define CHAR_TYPE_SPACE			0x81
#define CHAR_TYPE_TAB			0x82
#define CHAR_TYPE_WORD			0x03
#define CHAR_TYPE_EOL			0x04
#define CHAR_TYPE_EOW			0x83
#define CHAR_COMMAND_STOP		0x80
#define CHAR_TYPE_MASK			0x7F

#define CHAR_INVALID_POSITION	0xFFFFFFFF

#define WORD_TYPE_SPACE			(CHAR_TYPE_SPACE & CHAR_TYPE_MASK)
#define WORD_TYPE_TAB			(CHAR_TYPE_TAB   & CHAR_TYPE_MASK)
#define WORD_TYPE_WORD			(CHAR_TYPE_WORD  & CHAR_TYPE_MASK)
#define WORD_TYPE_EOL			(CHAR_TYPE_EOL   & CHAR_TYPE_MASK)

struct GlyphWord
{
	unsigned int	start,end;
	int				type;
	float			width;
	float			height;
	bool			wordIsTruncated;

	// in
	float scale,charSpace,spaceWidth,wrapWidth;
	GlyphLocation* text;
	GApp::Resources::Font *font;
	unsigned int charactersCount;
};
struct GlyphLine
{
	unsigned int	start,end;
	unsigned int	startFirstWord,endLastWord;
	float			size;
};



int GetCharCodeType(int ch)
{
	if ((ch>='A') && (ch<='Z'))
		return CHAR_TYPE_WORD;
	if ((ch>='a') && (ch<='z'))
		return CHAR_TYPE_WORD;
	if ((ch>='0') && (ch<='9'))
		return CHAR_TYPE_WORD;
	if (ch==' ')
		return CHAR_TYPE_SPACE;
	if (ch=='\t')
		return CHAR_TYPE_TAB;
	if (ch=='\n')
		return CHAR_TYPE_EOL;
	if ((ch>32) && (ch<127))
		return CHAR_TYPE_EOW;
	return CHAR_TYPE_WORD;
}
void FindNextGlyphWord(unsigned int start,GlyphWord *w)
{
	GlyphInformation* glyph;
	GlyphLocation* p = w->text+start;
	int type = (GetCharCodeType(p->Code) & CHAR_TYPE_MASK);
	int ctype;
	float charWidth = 0,charHeight;

	w->start = w->end = start;
	w->width = 0;
	w->height = 0;
	w->type = type;
	w->wordIsTruncated = false;
	do
	{
		ctype = GetCharCodeType(p->Code);
		if ((ctype & CHAR_TYPE_MASK)!=type)
			break;
		glyph = (GlyphInformation*)w->font->GetGlyph(p->Code);
		if ((glyph != NULL) && (glyph->Image!=NULL))
		{
			charWidth = glyph->Image->Width * w->scale;
			charHeight = glyph->Image->Height * w->scale;
			p->Image = glyph->Image;
		} else {
			charWidth = w->spaceWidth;
			charHeight = 0;
			p->Image = NULL;
			if (ctype == CHAR_TYPE_TAB)
				charWidth*=4;
		}
		if (w->end>w->start) // prima litera
			p->x = w->width+w->charSpace;
		else
			p->x = w->width;

		if (charHeight>w->height)
			w->height = charHeight;
		if (w->wrapWidth>=0) // am wrap
		{
			if (p->x+charWidth>w->wrapWidth)
			{
				w->wordIsTruncated = true;
				break;
			}
		} 		
		// all ok
		w->width = p->x+charWidth;

		w->end++;		
		p++;
		if ((ctype & CHAR_COMMAND_STOP)!=0)
			break;
	} while (w->end<w->charactersCount);
	if (w->end==w->start)
	{
		w->width = p->x+charWidth;
		w->end++; // macar un caracter (asta se poate intampla daca am word wrap
	}
	if (w->type == WORD_TYPE_EOL)
		w->width = 0; // nu am size daca e \n - doar trec pe urmatoarea linie
}
void ClearGlyphLine(GlyphLine *l)
{
	l->start = l->end = CHAR_INVALID_POSITION;
	l->startFirstWord = l->endLastWord = CHAR_INVALID_POSITION;
	l->size = 0;
}
void AddGlyphWordToGlyphLine(GlyphLine *l,GlyphWord *w)
{
	if (l->start==CHAR_INVALID_POSITION)	
		l->start = w->start;
	else
		l->size+=w->charSpace;

	l->end = w->end;
	if (w->type == WORD_TYPE_WORD)
	{
		if (l->startFirstWord == CHAR_INVALID_POSITION)
			l->startFirstWord = w->start;
		l->endLastWord = w->end;
	}
	// recalculez pozitia fiecarui cuvant
	GlyphLocation* p = w->text+w->start;
	for (unsigned int tr=w->start;tr<w->end;tr++,p++)
		p->x+=l->size;
	l->size+=w->width;	
}
float GetLineSizeWithWord(GlyphLine *l,GlyphWord *w)
{
	if (l->start == CHAR_INVALID_POSITION)
		return w->width;
	else
		return l->size+w->charSpace+w->width;
}
float UpdateGlyphLinePosition(GlyphLine *l,GlyphWord *w,GAC_TYPE_ALIGNAMENT textAlign,unsigned int wordWrapMode,float y)
{
	if (l->startFirstWord == CHAR_INVALID_POSITION)
		return 0; // nu am nici un caracter in linie - nu am ce align sau repozitionare sa fac
	float lineWidth = l->size;
	float addX,addY,extraJustify;
	GlyphInformation* glyph;
	extraJustify = 0;
	if (wordWrapMode==CHAR_MODE_JUSTIFY) // JUSTIFY
	{
		lineWidth = w->text[l->endLastWord-1].x-w->text[l->startFirstWord].x;
		glyph = ((GlyphInformation*)w->font->GetGlyph(w->text[l->endLastWord-1].Code));
		if ((glyph!=NULL) && (glyph->Image!=NULL))
			lineWidth += glyph->Image->Width*w->scale;
		extraJustify = (w->wrapWidth-lineWidth)/(l->endLastWord-l->startFirstWord);
		lineWidth = w->wrapWidth;
		// SIGUR nu e bine - trebuie corectat sa evit spatiile de la inceput (va merge bine daca linia chiar incepe cu un cuvant);
	}
	if (wordWrapMode == CHAR_MODE_WORD_WRAP)
	{
		// scot extra spatiile
		lineWidth = w->text[l->endLastWord - 1].x - w->text[l->startFirstWord].x;
		glyph = ((GlyphInformation*)w->font->GetGlyph(w->text[l->endLastWord - 1].Code));
		if ((glyph != NULL) && (glyph->Image != NULL))
			lineWidth += glyph->Image->Width*w->scale;
	}
	ALIGN_COORD(addX,addY,0,0,lineWidth,0,textAlign);
	GlyphLocation* p = w->text+l->startFirstWord;
	for (unsigned int tr=l->startFirstWord;tr<l->endLastWord;tr++,p++)
	{
		p->x+=addX;
		addX += extraJustify;
		p->y = y;
	}
	return lineWidth;
}
void RecomputePositions(GApp::Graphics::TextPainter *tp,float forceScale = -1.0f)
{
	unsigned int cPoz,nrLines;
	float fontHeight,fontBaseline,usedCharsMaxHeight,tmpSize,lineWidth;
	unsigned int wordWrap;
	float charHeight,charDescend,addY;
	GlyphWord word;
	GlyphLine line;
	GlyphLocation* g;
	
	GAC_TYPE_ALIGNAMENT textAlign;
	GAC_TYPE_COORDINATES coordType;
	GAC_TYPE_FONTSIZETYPE fontSizeMethod;

	if (tp==NULL)
		return;
	if (tp->TextFont==NULL) 
		return;
	if ((word.charactersCount = (tp->Characters.GetSize()/sizeof(GlyphLocation)))==0)
		return;
	
	if ((word.text = (GlyphLocation*)tp->Characters.Data)==NULL)
		return;

	fontSizeMethod = GET_BITFIELD(tp->Flags,11,4);
	switch (fontSizeMethod)
	{
		case GAC_FONTSIZETYPE_SCALE:
			word.scale = tp->FontSizeValue;
			break;
		case GAC_FONTSIZETYPE_PIXELS:
			word.scale = tp->FontSizeValue/(tp->TextFont->Baseline+tp->TextFont->Descent);
			break;
		case GAC_FONTSIZETYPE_PERCENTAGEOFVIEWHEIGHT:
			word.scale = (tp->FontSizeValue*(tp->ViewBottom-tp->ViewTop))/(tp->TextFont->Baseline+tp->TextFont->Descent);
			break;
		case GAC_FONTSIZETYPE_SCALETOFITVIEWWIDTH:
			word.scale = 1.0f;
			break;
		case GAC_FONTSIZETYPE_SHRINKTOFITVIEWWIDTH:
			word.scale = tp->FontSizeValue;
			break;
		default:
			word.scale = 1.0f;
			break;
	}
	if (forceScale>0)
		word.scale = forceScale;

	word.font = tp->TextFont;	
	word.charSpace = tp->CharacterSpacing*word.scale;
	word.spaceWidth = tp->SpaceWidth*word.scale;
	word.wrapWidth = tp->ViewRight-tp->ViewLeft;
	
	tp->MaxTextWidth = 0;
	tp->MaxTextHeight = 0;
	
	fontHeight = (tp->TextFont->Baseline+tp->TextFont->Descent)*word.scale*tp->HeightPercentage;


	fontBaseline = tp->TextFont->Baseline * word.scale;
	textAlign = (GAC_TYPE_ALIGNAMENT)GET_BITFIELD(tp->Flags,0,4);
	coordType = (GAC_TYPE_COORDINATES)GET_BITFIELD(tp->Flags,4,4);
	wordWrap = GET_BITFIELD(tp->Flags,8,3);
	if ((tp->Flags & TEXT_PAINT_MODE_DOCK)!=0)
	{
		ALIGN_OBJECT_TO_RECT(tp->TextLeft,tp->TextTop,tp->ViewLeft,tp->ViewTop,tp->ViewRight,tp->ViewBottom,0,0,textAlign);
	} else {
		TRANSLATE_COORD_TO_PIXELS(tp->TextLeft,tp->TextTop,tp->X,tp->Y,tp->ViewRight-tp->ViewLeft,tp->ViewBottom-tp->ViewTop,coordType);
		tp->TextLeft+=tp->ViewLeft;
		tp->TextTop+=tp->ViewTop;
		wordWrap = CHAR_MODE_NORMAL; // fara ViewRect nu am word wrap
		word.wrapWidth = 0;
	}

	cPoz = 0;
	nrLines = 0;
	usedCharsMaxHeight = 0;
	
	ClearGlyphLine(&line);
	while (cPoz<word.charactersCount)
	{
		FindNextGlyphWord(cPoz,&word);
		usedCharsMaxHeight = GAC_MAX(usedCharsMaxHeight,word.height);
		cPoz = word.end;
		tmpSize = GetLineSizeWithWord(&line,&word);
		if (word.type == WORD_TYPE_EOL)
		{
			// update la linia curenta			
			lineWidth = UpdateGlyphLinePosition(&line,&word,textAlign,wordWrap,(float)nrLines);
			tp->MaxTextWidth = GAC_MAX(tp->MaxTextWidth,lineWidth);
			nrLines++;
			// clear linia curenta
			ClearGlyphLine(&line);
			// update			
			continue;
		}
		if ((tmpSize>word.wrapWidth) && (wordWrap!=CHAR_MODE_NORMAL)) // si daca am word wrap
		{
			// update linia curenta
			lineWidth = UpdateGlyphLinePosition(&line,&word,textAlign,wordWrap,(float)nrLines);
			tp->MaxTextWidth = GAC_MAX(tp->MaxTextWidth,lineWidth);
			nrLines++;
			// curat linia
			ClearGlyphLine(&line);
			// pun cuvantul in linia noua - stiu ca e spatiu pentru ca nu are cum sa fie cuvantul mai mare decat linia
			if (word.type == WORD_TYPE_WORD)
				AddGlyphWordToGlyphLine(&line,&word);
			continue;
		}
		// altfel adaug word la linie
		AddGlyphWordToGlyphLine(&line,&word);
	}
	if (line.start!=CHAR_INVALID_POSITION)
	{
		lineWidth = UpdateGlyphLinePosition(&line,&word,textAlign,wordWrap,(float)nrLines);
		tp->MaxTextWidth = GAC_MAX(tp->MaxTextWidth,lineWidth);
		nrLines++;
	}
	// in y am acuma numarul de linii
	tp->MaxTextHeight = 0;
	if (nrLines==1)
	{
		// am o singura linie - height-ul e dat de cel mai inalt caracter
		tp->MaxTextHeight = usedCharsMaxHeight;
		// calculez baseline-ul
		fontBaseline = 0;
		for (cPoz=0,g=word.text;cPoz<word.charactersCount;cPoz++,g++)
		{
			if (g->Image!=NULL)
			{
				charDescend = ((GlyphInformation*)word.font->GetGlyph(g->Code))->Descend;
				charHeight = g->Image->Height*word.scale;
				tmpSize = (1-charDescend)*charHeight;
				fontBaseline = GAC_MAX(tmpSize,fontBaseline);
			}
		}
	}
	if (nrLines>1)
	{
		// am mai multe 
		tp->MaxTextHeight = (nrLines-1)*fontHeight+(tp->TextFont->Baseline+tp->TextFont->Descent)*word.scale;
	}

		
	ALIGN_COORD(tmpSize,addY,0,tp->TextTop,0,tp->MaxTextHeight,textAlign);
	
	for (cPoz=0,g=word.text;cPoz<word.charactersCount;cPoz++,g++)
	{
		if (g->Image!=NULL)
		{
			charDescend = ((GlyphInformation*)word.font->GetGlyph(g->Code))->Descend;
			charHeight = g->Image->Height*word.scale;
			g->y = (g->y * fontHeight) +fontBaseline - charHeight*(1-charDescend)+addY;
			g->x += tp->TextLeft;
		}
		else {
			g->y = (g->y * fontHeight) + fontBaseline + addY;
			g->x += tp->TextLeft;
		}
	}

	// actualizez si TextLeft si TextUp
	ALIGN_COORD(tp->TextLeft,tp->TextTop,tp->TextLeft,tp->TextTop,tp->MaxTextWidth,tp->MaxTextHeight,textAlign);
	tp->PaintScale = word.scale;

	// recalculez daca am un anumit font width
	switch (fontSizeMethod)
	{
		case GAC_FONTSIZETYPE_SHRINKTOFITVIEWWIDTH:
			if (forceScale<0) // am rulat prima data
			{
				if ((tp->MaxTextWidth>(tp->ViewRight-tp->ViewLeft)) && (tp->ViewRight>tp->ViewLeft) && (tp->MaxTextWidth>0))
				{
					RecomputePositions(tp,(tp->ViewRight-tp->ViewLeft)/(tp->MaxTextWidth));
				}
			}
			break;
		case GAC_FONTSIZETYPE_SCALETOFITVIEWWIDTH:
			if (forceScale<0) // am rulat prima data
			{
				if ((tp->ViewRight>tp->ViewLeft) && (tp->MaxTextWidth>0))
				{
					RecomputePositions(tp,(tp->ViewRight-tp->ViewLeft)/(tp->MaxTextWidth));
				}
			}
			break;
	}
}

//====================================================================================
GApp::Graphics::TextPainter::TextPainter()
{
	TextFont = NULL;
	Characters.Create(32*sizeof(GlyphLocation)); 
	ViewLeft = ViewRight = ViewTop = ViewBottom = 0;
	FontSizeValue = 1.0f;
	PaintScale = 1.0f;
	HeightPercentage = 1.0f;
	X = Y = 0;
	S = NULL;
	MaxTextWidth = MaxTextHeight = 0;
	Flags = 0;
	ClearShaderParams();
	SET_BITFIELD(Flags,0,4,GAC_ALIGNAMENT_TOPLEFT);
	SET_BITFIELD(Flags,4,4,GAC_COORDINATES_PIXELS);	
	SET_BITFIELD(Flags,8,3,CHAR_MODE_NORMAL);	
	SET_BITFIELD(Flags,11,4,GAC_FONTSIZETYPE_SCALE);	
	//SET_BIT(Flags,TEXT_PAINT_USE_DESCENT);
	Blend.Mode = BLEND_IMAGE_MODE_NONE;
}
void GApp::Graphics::TextPainter::SetShader(GApp::Resources::Shader *s)
{
	S = s;
	Blend.Mode = BLEND_IMAGE_MODE_NONE;
	ClearShaderParams();
}
void GApp::Graphics::TextPainter::SetAlphaBlending(float alpha)
{
	S = NULL;
	Blend.Alpha = alpha;
	Blend.Mode = BLEND_IMAGE_MODE_ALPHA;
}
void GApp::Graphics::TextPainter::SetColorBlending(unsigned int color)
{
	S = NULL;
	Blend.Color = color;
	Blend.Mode = BLEND_IMAGE_MODE_COLOR;
}
bool GApp::Graphics::TextPainter::SetFont(GApp::Resources::Font *f)
{	
	CHECK(f!=NULL,false,"");
	if (f!=TextFont)
	{
		TextFont = f;
		SpaceWidth = TextFont->SpaceWidth;
		CharacterSpacing = TextFont->CharacterSpacing;
		RecomputePositions(this);
	}
	return true;
}
bool GApp::Graphics::TextPainter::SetText(const char *text,int textSize)
{
	unsigned int tr,len,realLen,currentLen;
	unsigned int code;
	const unsigned char *tmp;
	GlyphLocation* g;
	bool modified = false;
	CHECK(text!=NULL,false,"");
	
	if (textSize<0)
	{
		for (len=0,tmp=(unsigned char *)text;(*tmp)!=0;len++,tmp++);
	} else {
		len = (unsigned int)textSize;
	}
	currentLen = Characters.GetSize() / sizeof(GlyphLocation);
	if (len != currentLen)
	{
		CHECK(Characters.Resize(len*sizeof(GlyphLocation)),false,"Unable to resize internal character vector to %d",len);
	}
	// setez caracterele
	CHECK((g = (GlyphLocation*)Characters.Data)!=NULL,false,"Unable to get glyph vector pointer");

	realLen = 0;	
	for (tmp=(const unsigned char *)text,tr=0;tr<len;tmp++,g++,tr++,realLen++)
	{
		code = (*tmp);
		// daca e cod extins il calculez
		if ((code == '|') && (tr + 1<len) && (tmp[1] == '~'))
		{
			code = 0;
			tmp += 2; tr += 2;
			CHECK(((*tmp) >= 48) && ((*tmp) < 80), false, "Invalid extended code - should be between 32 and 90 (value is %d)",*tmp);
			code = (code<<5)+ ((*tmp)-48);
			tmp++; tr++;
			CHECK(((*tmp) >= 48) && ((*tmp) < 80), false, "Invalid extended code - should be between 32 and 90 (value is %d)", *tmp);
			code = (code<<5)+((*tmp)-48);
			tmp++; tr++;
			CHECK(((*tmp) >= 48) && ((*tmp) < 80), false, "Invalid extended code - should be between 32 and 90 (value is %d)", *tmp);
			code = (code<<5)+((*tmp)-48);
		}
		g->Visible = true;
		if (!modified)
		{
			if (g->Code!=code)
			{
				modified = true;
				g->Code = code;
				g->Image = NULL;
			}
		} else {
			g->Code = code;	
			g->Image = NULL;
		}
		
	}
	//LOG_INFO("StringSize: %d (%d) -> %s", realLen, len, text);
	//LOG_INFO("SetText(%s,%d) - Len = %d, RealLen=%d, modified = %d, cLen=%d", text, textSize, len, realLen, modified, currentLen);
	if (currentLen != realLen)
		modified = true;
	if (realLen != len)
	{
		CHECK(Characters.Resize(realLen*sizeof(GlyphLocation)), false, "Unable to resize internal character vector to %d", realLen);
	}
	if (modified)
	{		
		RecomputePositions(this);
	}
	return true;
}
bool GApp::Graphics::TextPainter::SetText(GApp::Utils::String &str)
{
	return SetText(str.GetText());
}
void GApp::Graphics::TextPainter::SetFontSize(GAC_TYPE_FONTSIZETYPE mode,float value)
{
	unsigned int fontSizeMethod = GET_BITFIELD(Flags,11,4);
	if ((FontSizeValue!=value) || (fontSizeMethod!=mode))
	{
		SET_BITFIELD(Flags,11,4,mode);
		FontSizeValue = value;
		RecomputePositions(this);
	}
}
void GApp::Graphics::TextPainter::SetCharacterSpacing(float value)
{
	if (CharacterSpacing!=value)
	{
		CharacterSpacing = value;
		RecomputePositions(this);
	}
}
void GApp::Graphics::TextPainter::SetSpaceWidth(float value)
{
	if (SpaceWidth!=value)
	{
		SpaceWidth = value;
		RecomputePositions(this);
	}
}
void GApp::Graphics::TextPainter::SetLineSpace(float value)
{
	if (HeightPercentage!=(value+1))
	{
		HeightPercentage = (value+1);
		RecomputePositions(this);
	}
}
void GApp::Graphics::TextPainter::SetViewRect(float left,float top,float right,float bottom)
{
	if ((left!=ViewLeft) || (top!=ViewTop) || (bottom!=ViewBottom) || (right!=ViewRight))
	{
		ViewTop = top;
		ViewBottom = bottom;
		ViewRight = right;
		ViewLeft = left;
		RecomputePositions(this);
	}
}
void GApp::Graphics::TextPainter::SetViewRectWH(float left, float top, float width, float height)
{
	SetViewRect(left, top, left + width, top + height);
}
void GApp::Graphics::TextPainter::SetViewRectWH(float x, float y, GAC_TYPE_ALIGNAMENT align, float Width, float Height)
{
	RectF tmpRect;
	tmpRect.Set(x, y, align, Width, Height);
	SetViewRect(tmpRect.Left, tmpRect.Top, tmpRect.Right, tmpRect.Bottom);
}
void GApp::Graphics::TextPainter::MoveViewRectTo(float x, float y, GAC_TYPE_ALIGNAMENT align)
{
	RectF tmpRect;
	tmpRect.Set(x, y, align, ViewRight - ViewLeft, ViewBottom - ViewTop);
	if ((tmpRect.Left != ViewLeft) || (tmpRect.Top != ViewTop))
	{
		float off_x = tmpRect.Left - ViewLeft;
		float off_y = tmpRect.Top - ViewTop;
		unsigned int tr, currentLen;
		GlyphLocation* g;		
		currentLen = Characters.GetSize() / sizeof(GlyphLocation);
		if (currentLen > 0)
		{
			g = (GlyphLocation*)Characters.Data;
			if (g != NULL)
			{
				for (tr = 0; tr < currentLen; tr++, g++)
				{
					g->x += off_x;
					g->y += off_y;
				}
			}
		}
		ViewLeft += off_x;
		ViewRight += off_x;
		ViewTop += off_y;
		ViewBottom += off_y;
	}
}
void GApp::Graphics::TextPainter::SetPosition(float x,float y,GAC_TYPE_COORDINATES coord,GAC_TYPE_ALIGNAMENT align)
{
	unsigned int flags = Flags;
	SET_BITFIELD(flags,0,4,align);
	SET_BITFIELD(flags,4,4,coord);
	REMOVE_BIT(flags,TEXT_PAINT_MODE_DOCK);

	if (Flags==flags)
	{
		if ((X!=x) || (Y!=y))
		{
			// nu mai fac recompute - doar translez
			/*
			float addX = x-X;
			float addY = y-Y;
			unsigned int count = Characters.GetSize();
			GlyphLocation *g = (GlyphLocation*)Characters.GetVector();
			if ((count>0) && (g!=NULL))
			{
				for (unsigned int tr=0;tr<count;tr++,g++)
				{
					g->x+=addX;
					g->y+=addY;
				}
				X = x;
				Y = y;				
			} else { */
				X = x;
				Y = y;
				RecomputePositions(this);
			//}
		}
	} else {
		if ((X!=x) || (Y!=y))
		{
			X = x;
			Y = y;
			Flags = flags;
			RecomputePositions(this);
		}
	}
}
void GApp::Graphics::TextPainter::SetDockPosition(GAC_TYPE_ALIGNAMENT dockAlignament)
{
	unsigned int flags = Flags;
	SET_BITFIELD(flags,0,4,dockAlignament);
	SET_BIT(flags,TEXT_PAINT_MODE_DOCK);

	if (Flags!=flags)
	{
		Flags = flags;
		RecomputePositions(this);
	}
}
void GApp::Graphics::TextPainter::SetWordWrap(bool value)
{
	unsigned int flags = Flags;
	if (value)
	{
		SET_BITFIELD(flags,8,3,CHAR_MODE_WORD_WRAP);
	} else {
		SET_BITFIELD(flags,8,3,CHAR_MODE_NORMAL);
	}
	if (flags!=Flags)
	{
		Flags = flags;
		RecomputePositions(this);
	}	
}
void GApp::Graphics::TextPainter::SetTextJustify(bool value)
{
	unsigned int flags = Flags;
	if (value)
	{
		SET_BITFIELD(flags,8,3,CHAR_MODE_JUSTIFY);
	} else {
		SET_BITFIELD(flags,8,3,CHAR_MODE_NORMAL);
	}
	if (flags!=Flags)
	{
		Flags = flags;
		RecomputePositions(this);
	}		
}
float GApp::Graphics::TextPainter::GetX()
{
	return X;
}
float GApp::Graphics::TextPainter::GetY()
{
	return Y;
}
float GApp::Graphics::TextPainter::GetTextWidth()
{
	return MaxTextWidth;
}
float GApp::Graphics::TextPainter::GetTextHeight()
{
	return MaxTextHeight;
}
void GApp::Graphics::TextPainter::GetTextRect(RectF *r)
{
	if (r!=NULL)
	{
		r->SetWH(TextLeft,TextTop,MaxTextWidth,MaxTextHeight);
	}
}
void GApp::Graphics::TextPainter::ClearShaderParams()
{
	ShaderParamInfo[0] = ShaderParamInfo[1] = ShaderParamInfo[2] = ShaderParamInfo[3] = 0;
}
bool GApp::Graphics::TextPainter::SetShaderUniformValue(const char * VariableName, unsigned int count, float v1, float v2, float v3, float v4)
{
	CHECK(S != NULL, false, "");
	int index = S->GetUniformIndex(VariableName);
	CHECK(index >= 0, false, "");
	CHECK((count >= 1) && (count <= 4), false, "");
	ShaderParamInfo[index] = count;
	ShaderParams[index * 4] = v1;
	ShaderParams[index * 4 + 1] = v2;
	ShaderParams[index * 4 + 2] = v3;
	ShaderParams[index * 4 + 3] = v4;
	return true;
}
bool GApp::Graphics::TextPainter::SetShaderUniformColor(const char * VariableName, unsigned int color)
{
	return SetShaderUniformValue(VariableName, 4, RED(color), GREEN(color), BLUE(color), ALPHA(color));
}
void GApp::Graphics::TextPainter::UpdateShaderParams()
{
	if (S == NULL)
		return;
	unsigned char *spi = &ShaderParamInfo[0];
	float *params = &ShaderParams[0];
	for (int tr = 0; tr < 4; tr++, spi++, params += 4)
	{
		if ((*spi) != 0)
		{
			switch (*spi)
			{
			case 1: S->SetVariableValue(tr, params[0], false); break;
			case 2: S->SetVariableValue(tr, params[0], params[1], false); break;
			case 3: S->SetVariableValue(tr, params[0], params[1], params[2], false); break;
			case 4: S->SetVariableValue(tr, params[0], params[1], params[2], params[3], false); break;
			}
		}
	}
}
unsigned int GApp::Graphics::TextPainter::GetCharactersCount()
{
	return Characters.GetSize() / sizeof(GlyphLocation);
}
void GApp::Graphics::TextPainter::SetCharactesVisibility(unsigned int startIndex, unsigned int endIndex, bool visibility)
{
	GlyphLocation* g = (GlyphLocation*)Characters.Data;
	if (g == NULL)
		return;
	unsigned int sz = GetCharactersCount();
	if (endIndex > sz)
		endIndex = sz;
	GlyphLocation* e;
	e = g + endIndex;
	g += startIndex;
	while (g < e)
	{
		g->Visible = visibility;
		g++;
	}
}
bool GApp::Graphics::TextPainter::CopyText(GApp::Utils::String * txt)
{
	CHECK(txt != NULL, false, "");
	CHECK(txt->Set(""), false, "");
	GlyphLocation * glph = (GlyphLocation *)Characters.GetBuffer();
	CHECK(glph != NULL, false, "");
	unsigned int sz = Characters.GetSize()/sizeof(GlyphLocation);
	for (unsigned int tr = 0; tr < sz; tr++, glph++)
	{
		CHECK(txt->AddChar(glph->Code), false, "");
	}
	return true;
}