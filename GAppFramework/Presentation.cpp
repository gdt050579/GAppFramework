#include "GApp.h"

GApp::Resources::Presentation::Presentation()
{
	CoreContext = NULL;
	ImagesCount = 0;
	FontsCount = 0;
	ShadersCount = 0;
	StringsCount = 0;
	StringValuesCount = 0;
	Images = NULL;
	Fonts = NULL;
	Shaders = NULL;
	Strings = NULL;
	StringsValues = NULL;
	Flags = GAC_RESOURCEFLAG_ANIMATION;
}
bool	GApp::Resources::Presentation::Create(CoreReference coreContext,unsigned int resPos,unsigned int resSize,unsigned int nrImages,unsigned int nrFonts,unsigned int nrShaders,unsigned int nrStrings, unsigned int nrStringsValue)
{
	CHECK(CoreContext==NULL,false,"Object was already created !"); // nu a fost creat pana acuma
	CHECK(coreContext!=NULL,false,"Invalid (null) coreContext parametere"); // are un context valid
	CHECK(resSize>0,false,"Resource size sould be bigger than 0");

	Position = resPos;
	Size = resSize;
	CoreContext = coreContext;
	ImagesCount = nrImages;
	FontsCount = nrFonts;
	ShadersCount = nrShaders;
	StringsCount = nrStrings;
	StringValuesCount = nrStringsValue;
	if (ImagesCount>0)
		Images = new BitmapHolder[ImagesCount];
	else
		Images = NULL;
	if (FontsCount>0)
		Fonts = new FontHolder[FontsCount];
	else
		Fonts = NULL;
	if (ShadersCount>0)
		Shaders = new ShaderHolder[ShadersCount];
	else
		Shaders = NULL;
	if (StringsCount>0)
		Strings = new StringHolder[StringsCount];
	else
		Strings = NULL;

	if (StringValuesCount>0)
		StringsValues = new StringValueHolder[StringValuesCount];
	else
		StringsValues = NULL;

	return true;
}
bool	GApp::Resources::Presentation::SetImage(unsigned int index,Bitmap *bmp,const char* Name)
{
	CHECK(index<ImagesCount,false,"Invalid index (%d) should be within [0..%d)",index,ImagesCount);
	CHECK(bmp!=NULL,false,"Invalid (NULL) bmp object (parameter)");
	Images[index].Object = bmp;
	Images[index].Name = Name;
	return true;
}
bool	GApp::Resources::Presentation::SetFont(unsigned int index, Font *font, const char* Name)
{
	CHECK(index<FontsCount,false,"Invalid index (%d) should be within [0..%d)",index,FontsCount);
	CHECK(font!=NULL,false,"Invalid (NULL) font object (parameter)");
	Fonts[index].Object = font;
	Fonts[index].Name = Name;
	return true;
}
bool	GApp::Resources::Presentation::SetShader(unsigned int index, Shader *shader, const char* Name)
{
	CHECK(index<ShadersCount,false,"Invalid index (%d) should be within [0..%d)",index,ShadersCount);
	CHECK(shader!=NULL,false,"Invalid (NULL) shader object (parameter)");
	Shaders[index].Object = shader;
	Shaders[index].Name = Name;
	return true;
}
bool	GApp::Resources::Presentation::SetString(unsigned int index, char* text, const char* Name)
{
	CHECK(index<StringsCount,false,"Invalid index (%d) should be within [0..%d)",index,StringsCount);
	CHECK(text!=NULL,false,"Invalid (NULL) string object (parameter)");
	Strings[index].Object = text;
	Strings[index].Name = Name;
	return true;
}
bool	GApp::Resources::Presentation::CreateStringValue(const char * keyName, unsigned int index)
{
	CHECK(keyName != NULL, false, "Invalid (NULL) keyName");
	CHECK(keyName[0] != 0, false, "Invalid (Empty) keyName");
	CHECK(index< StringValuesCount, false, "Invalid index (should be between 0 and %d)", StringValuesCount);
	StringsValues[index].Name = keyName;
	return true;
}

bool	GApp::Resources::Presentation::SetImageForKey(const char* keyName, Bitmap *bmp)
{
	CHECK(keyName != NULL, false, "Invalid (NULL) keyName");
	CHECK(keyName[0] != 0, false, "Invalid (Empty) keyName");
	CHECK(bmp != NULL, false, "Invalid (NULL) bitmap");
	for (int tr = 0; tr < ImagesCount; tr++)
	{
		if (Images[tr].Name == NULL)
			continue;
		if (GApp::Utils::String::Equals(keyName, Images[tr].Name, true))
		{
			Images[tr].Object = bmp;
			return true;
		}
	}
	RETURNERROR(false, "Unable to find image for key: '%s'", keyName);
}
bool	GApp::Resources::Presentation::SetFontForKey(const char* keyName, Font *font)
{
	CHECK(keyName != NULL, false, "Invalid (NULL) keyName");
	CHECK(keyName[0] != 0, false, "Invalid (Empty) keyName");
	CHECK(font != NULL, false, "Invalid (NULL) font");
	for (int tr = 0; tr < FontsCount; tr++)
	{
		if (Fonts[tr].Name == NULL)
			continue;
		if (GApp::Utils::String::Equals(keyName, Fonts[tr].Name, true))
		{
			Fonts[tr].Object = font;
			return true;
		}
	}
	RETURNERROR(false, "Unable to find font for key: '%s'", keyName);
}
bool	GApp::Resources::Presentation::SetShaderForKey(const char* keyName, Shader *shader)
{
	CHECK(keyName != NULL, false, "Invalid (NULL) keyName");
	CHECK(keyName[0] != 0, false, "Invalid (Empty) keyName");
	CHECK(shader != NULL, false, "Invalid (NULL) shader");
	for (int tr = 0; tr < ShadersCount; tr++)
	{
		if (Shaders[tr].Name == NULL)
			continue;
		if (GApp::Utils::String::Equals(keyName, Shaders[tr].Name, true))
		{
			Shaders[tr].Object = shader;
			return true;
		}
	}
	RETURNERROR(false, "Unable to find shader for key: '%s'", keyName);
}
bool	GApp::Resources::Presentation::SetStringForKey(const char* keyName, const char* text)
{
	CHECK(keyName != NULL, false, "Invalid (NULL) keyName");
	CHECK(keyName[0] != 0, false, "Invalid (Empty) keyName");
	CHECK(text != NULL, false, "Invalid (NULL) string");
	for (int tr = 0; tr < StringsCount; tr++)
	{
		if (Strings[tr].Name == NULL)
			continue;
		if (GApp::Utils::String::Equals(keyName, Strings[tr].Name, true))
		{
			Strings[tr].Object = text;
			return true;
		}
	}
	RETURNERROR(false, "Unable to find string for key: '%s'", keyName);
}
bool	GApp::Resources::Presentation::SetStringValue(const char* ValueName, const char* text)
{
	CHECK(ValueName != NULL, false, "Invalid (NULL) keyName");
	CHECK(ValueName[0] != 0, false, "Invalid (Empty) keyName");
	CHECK(text != NULL, false, "Invalid (NULL) string");
	for (int tr = 0; tr < StringValuesCount; tr++)
	{
		if (GApp::Utils::String::Equals(ValueName, StringsValues[tr].Name, true))
		{
			CHECK(StringsValues[tr].Value.Set(text), false, "Unable to set value '%s' !", text);
			return true;
		}
	}
	RETURNERROR(false, "Unable to find string for value name: '%s'", ValueName);
}
bool	GApp::Resources::Presentation::Load()
{
	if (Data.GetSize()==0)
	{
		CHECK(CoreContext!=NULL,false,"Object was not created !");
		CHECK(Size!=0,false,"Resource size should be bigger than 0 !");
		CHECK(API.ReadResource(Position,&Data,Size,GAC_ENCRYPTION_NONE,NULL),false,"Failed to read animation data from resources");
		return true;
	}
	return true;
}
bool	GApp::Resources::Presentation::Unload()
{
	Data.Free();
	ProfileContext=NULL;
	return true;
}