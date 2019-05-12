#include "GApp.h"

void InsertIndexSort(GApp::Resources::Font *f,unsigned short charStart,unsigned short charEnd,unsigned int index)
{
	int tr,gr;
	for (tr=0;tr<3;tr++)
	{
		if ((charEnd-charStart)>(f->CharEnd[tr]-f->CharStart[tr]))
			break;
	}
	if (tr==3)
		return;
	for (gr=1;gr>=tr;gr--)
	{
		f->CharStart[gr+1] = f->CharStart[gr];
		f->CharEnd[gr+1] = f->CharEnd[gr];
		f->CharIndex[gr+1]  = f->CharIndex[gr];
	}
	f->CharStart[tr] = charStart;
	f->CharEnd[tr] = charEnd;
	f->CharIndex[tr] = index;
}
GlyphInformation* BTreeSearch(GlyphInformation *gi,int start,int end,unsigned short caracter)
{
	if (start>end)
		return NULL;
	int mij = (start+end)/2;
	if (gi[mij].Character == caracter)
		return &gi[mij];
	if (gi[mij].Character < caracter)
		return BTreeSearch(gi,mij+1,end,caracter);
	else
		return BTreeSearch(gi,start,mij-1,caracter);
}
//============================================================================================================================
GApp::Resources::Font::Font()
{
	Glyphs = NULL;
	SpaceWidth = 0;
	Baseline = 0;
	Descent = 0;
	Count = 0;
	CharacterSpacing = 0;
}
bool GApp::Resources::Font::Create(unsigned int glyphsCount)
{
	CHECK(Glyphs==NULL,false,"Font was already created !");
	CHECK(glyphsCount>0,false,"");
	CHECK((Glyphs = new GlyphInformation[glyphsCount])!=NULL,false,"");

	GlyphInformation *gi = (GlyphInformation *)Glyphs;
	for (unsigned int tr=0;tr<glyphsCount;tr++)
	{
		gi[tr].Character = 0;
		gi[tr].Descend = 0;
		gi[tr].Image = NULL;
	}
	Count = glyphsCount;
	Baseline = 0;
	Descent = 0;
	SpaceWidth = 0;
	CharacterSpacing = 0;
	return true;
}
bool GApp::Resources::Font::Add(unsigned int Character, Bitmap *bmp, float descend)
{
	CHECK(Glyphs!=NULL,false,"Font was not created ");
	CHECK(Character>0,false,"");
	// caut prima locatie libera
	unsigned int poz;
	GlyphInformation *gi = (GlyphInformation *)Glyphs;

	for (poz = 0; (poz<Count) && (gi->Character>0) && (gi->Character != Character); poz++, gi++) {};
	CHECK(poz<Count,false,"Invalid index for glyph (%d). Should be smaller than %d",poz,Count-1);
	CHECK(gi->Character!=Character,false,"");

	gi->Character = (unsigned short)(Character & 0xFFFF);
	gi->Image = bmp;
	gi->Descend = descend;

	return true;
}

bool GApp::Resources::Font::Prepare(unsigned int widthReferenceCharacter,float spaceWidth,float characterSpace)
{
	CHECK(Glyphs!=NULL,false,"Font was not created ");
	CHECK(Count>0,false,"");
	CHECK(spaceWidth>0,false,"");
	CHECK(characterSpace>=0,false,"");
	
	GlyphInformation *gi = (GlyphInformation *)Glyphs;
	CHECK(gi[Count-1].Character>0,false,"");
	CHECK(gi[Count-1].Image!=NULL,false,"");

	// totul e ok - sortam vectorul
	unsigned int lastPoz,tr;
	unsigned int lastChange = Count-1;
	bool sorted = false;
	GlyphInformation aux;
	while (!sorted)
	{
		sorted = true;
		lastPoz = lastChange;
		for (tr=0;tr<lastPoz;tr++)
		{
			if (gi[tr].Character>gi[tr+1].Character)
			{
				aux = gi[tr];
				gi[tr] = gi[tr+1];
				gi[tr+1] = aux;
				sorted = false;
				lastChange = tr;
			}
		}
	}	
	// este sortat - caut secvente consecutive
	for (tr=0;tr<3;tr++)
	{
		CharStart[tr] = CharEnd[tr] = 0;
		CharIndex[tr] = 0;
	}

	unsigned int start;
	start = 0;
	for (tr=1;tr<Count;tr++)
	{
		if (gi[tr].Character!=(gi[tr-1].Character+1))
		{
			InsertIndexSort(this,gi[start].Character,gi[tr-1].Character,start);
			start = tr;
		}
	}
	InsertIndexSort(this,gi[start].Character,gi[Count-1].Character,start);
	// calculez si heigh-ul
	
	Baseline = 0;
	Descent = 0;
	for (tr=0;tr<Count;tr++,gi++)
	{
		Baseline = GAC_MAX(Baseline, gi->Image->Height*(1 - gi->Descend));
		Descent = GAC_MAX(Descent, gi->Descend*gi->Image->Height);
		if (gi->Character == widthReferenceCharacter)
		{
			SpaceWidth = gi->Image->Width*spaceWidth;
			CharacterSpacing = gi->Image->Width*characterSpace;
		}
	}	

	CHECK(SpaceWidth>0,false,"");
	CHECK(CharacterSpacing>=0,false,"");
	return true;
}
void* GApp::Resources::Font::GetGlyph(unsigned short CharacterCode)
{
	if (Glyphs==NULL)
		return NULL;
	
	if ((CharacterCode>=CharStart[0]) && (CharacterCode<=CharEnd[0]))
	{
		return &(((GlyphInformation *)Glyphs)[CharIndex[0]+CharacterCode-CharStart[0]]);
	}
	if ((CharacterCode>=CharStart[1]) && (CharacterCode<=CharEnd[1]))
	{
		return &(((GlyphInformation *)Glyphs)[CharIndex[1]+CharacterCode-CharStart[1]]);
	}
	if ((CharacterCode>=CharStart[2]) && (CharacterCode<=CharEnd[2]))
	{
		return &(((GlyphInformation *)Glyphs)[CharIndex[2]+CharacterCode-CharStart[2]]);
	}
	// BTree pentru celelalte
	return BTreeSearch((GlyphInformation *)Glyphs,0,Count-1,CharacterCode);
}