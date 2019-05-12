#include "GApp.h"


#define RASTER_IMAGE_MAGIC		0x474D4952

GApp::Resources::Bitmap::Bitmap()
{
	Width = Height = OriginalWidth = OriginalHeight = 0;
	Size = 0;
	Position = 0;
	TextureID = -1;	
	TextureTop = TextureLeft = 0;
	for (int tr=0;tr<14;tr++)
		Coordonates[tr] = 0;
	Flags = GAC_RESOURCEFLAG_IMAGE;
#ifdef PLATFORM_DEVELOP
	localImageBuffer = NULL;
#endif
}
#ifdef PLATFORM_DEVELOP
bool GApp::Resources::Bitmap::Create(const void *picture, unsigned int size)
{
	unsigned char *p = (unsigned char *)picture;
	CHECK(picture != NULL, false, "");
	CHECK((*(unsigned int*)picture) == RASTER_IMAGE_MAGIC, false, "Invalid magic for image (found: 0x%08X - expecting: 0x%08X)", (*(unsigned int*)picture), RASTER_IMAGE_MAGIC);

	unsigned int w, h;
	//unsigned char type;
	w = *(unsigned short *)(p + 4);
	h = *(unsigned short *)(p + 6);

	CHECK((w>0) && (h>0), false, "");

	localImageBuffer = new GApp::Utils::Buffer(size);
	localImageBuffer->Resize(size);
	memcpy(localImageBuffer->GetBuffer(), picture, size);

	Width = w;
	Height = h;
	Position = 0;
	Size = 0;

	TextureID = -1;

	return true;
}
#endif

bool GApp::Resources::Bitmap::Create(/*CoreReference CoreContext, */ unsigned int currentScreenWidth, unsigned int currentScreenHeight, unsigned int *data, unsigned int count)
{
	// formatul este:
	// * rezolutie - w,h
	// * dimensiune imagine - w,h
	// * de unde din resurser - offset,[size,tip_incarcare] - size = primii 24 biti, tip incarcare restul 8
	//CHECK(CoreContext != NULL, false, "");
	CHECK(count > 0, false, "");
	CHECK(data != NULL, false, "");
	CHECK(currentScreenWidth>0, false, "");
	CHECK(currentScreenHeight>0, false, "");

	unsigned int designScreenWidth, designScreenHeight;
	float minScale = 0, scale = 0, minRap = 0;
	unsigned int minID = 0;
	for (unsigned int tr = 0; tr<count; tr++)
	{
		designScreenWidth = data[tr * 6];
		designScreenHeight = data[tr * 6 + 1];
		CHECK(designScreenHeight>0, false, "");
		CHECK(designScreenWidth>0, false, "");

		float rap1 = ((float)currentScreenWidth) / ((float)designScreenWidth);
		float rap2 = ((float)currentScreenHeight) / ((float)designScreenHeight);
		rap1 = GAC_MIN(rap1, rap2);
		scale = rap1 - 1.0f;
		if (scale < 0)
			scale = -scale;

		if (tr == 0)
		{
			minScale = scale;
			minID = 0;
			minRap = rap1;				 
		}
		else {
			if (scale < minScale)
			{
				minScale = scale;
				minID = tr;
				minRap = rap1;
			}
		}
	}
	
	if (minRap == 1.0f)
	{
		Width = OriginalWidth = data[minID * 6 + 2];
		Height = OriginalHeight = data[minID * 6 + 3];
	}
	else {
		Width = (unsigned int)(((float)data[minID * 6 + 2])*minRap);
		Height = (unsigned int)(((float)data[minID * 6 + 3])*minRap);
		OriginalWidth = data[minID * 6 + 2];
		OriginalHeight = data[minID * 6 + 3];
		Width = GAC_MAX(Width, 1);
		Height = GAC_MAX(Height, 1);
	}

	LOG_INFO("Resizing Bitmap (%s) for resolution %d x %d : Using %d forms, selected resolution: %d x %d, scale factor: %f Original:(%dx%d) -> Current:(%dx%d): ImageRawSize = %d", Name, currentScreenWidth, currentScreenHeight, count, data[minID * 6], data[minID * 6 + 1], minRap, OriginalWidth, OriginalHeight, Width, Height, data[minID * 6 + 5] & 0xFFFFFF);

	Position = data[minID * 6 + 4];
	Size = (data[minID * 6 + 5] & 0xFFFFFF);
	Flags |= (data[minID * 6 + 5] & GAC_RESOURCEFLAG_RESIZEMODE);
	CHECK(Size > 0, false, "");
	TextureID = -1;

	return true;
}

bool GApp::Resources::Bitmap::Create(CoreReference CoreContext,unsigned int imageWidth,unsigned int imageHeight,unsigned int designScreenWidth,unsigned int designScreenHeight,unsigned int currentScreenWidth, unsigned int currentScreenHeight, unsigned int Flags,unsigned int resFilePos,unsigned int resourceBufferSize)
{
	CHECK(CoreContext!=NULL,false,"");
	CHECK(imageWidth>0,false,"");
	CHECK(imageHeight>0,false,"");
	CHECK(resourceBufferSize>0,false,"");

	OriginalWidth = imageWidth;
	OriginalHeight = imageHeight;
	if ((designScreenWidth!=currentScreenWidth) || (designScreenHeight!=currentScreenHeight))
	{
		CHECK(currentScreenWidth>0,false,"");
		CHECK(currentScreenHeight>0,false,"");
		CHECK(designScreenHeight>0,false,"");
		CHECK(designScreenWidth>0,false,"");

		float rap1 = ((float)currentScreenWidth)/((float)designScreenWidth);
		float rap2 = ((float)currentScreenHeight)/((float)designScreenHeight);
		rap1 = GAC_MIN(rap1, rap2);

		// in functie de flaguri - se face in mai multe forme resize-ul sau deloc
		imageWidth = (unsigned int)(((float)imageWidth)*rap1);
		imageHeight = (unsigned int)(((float)imageHeight)*rap1);

		imageWidth = GAC_MAX(imageWidth, 1);
		imageHeight = GAC_MAX(imageHeight, 1);

	}

	Width = imageWidth;
	Height = imageHeight;
	Position = resFilePos;
	Size = resourceBufferSize;

	TextureID = -1;

	return true;
}

bool GApp::Resources::Bitmap::CreateRenderer(CoreReference CoreContext,GApp::Graphics::Renderer *renderer,GApp::Utils::Buffer *buffer)
{
	CHECK(CoreContext!=NULL,false,"");
	CHECK(renderer!=NULL,false,"");
	CHECK(buffer!=NULL,false,"");
	CHECK(Width>0,false,"");
	CHECK(Height>0,false,"");	
#ifdef PLATFORM_DEVELOP
	if (localImageBuffer != NULL)
	{
		CHECK(renderer->LoadImage(localImageBuffer),false,"");
	} else {
		CHECK(API.ReadResource(Position,buffer,Size,GAC_ENCRYPTION_NONE,NULL),false,"");
		CHECK(renderer->LoadImage(buffer), false, "");
	}
#else
	CHECK(API.ReadResource(Position,buffer,Size,GAC_ENCRYPTION_NONE,NULL),false,"");
	CHECK(renderer->LoadImage(buffer),false,"");
#endif
	return true;
}
/*
bool GApp::Resources::Bitmap::Create(GApp::Graphics::Texture *tex,unsigned int left,unsigned int top,unsigned int right,unsigned int bottom,unsigned int transparencyColor)
{
	CHECK(tex!=NULL);
	Tex = tex;

	Coordonates[0] = Coordonates[2] = Coordonates[6] = ((float)left)/((float)tex->Width);
	Coordonates[1] = Coordonates[7] = Coordonates[11] = 1.0f-((float)bottom)/((float)tex->Height);
	Coordonates[3] = Coordonates[5] = Coordonates[9] = 1.0f-((float)top)/((float)tex->Height);
	Coordonates[4] = Coordonates[8] = Coordonates[10] = ((float)right)/((float)tex->Width);

	Coordonates[12] = ((float)(right-left))*2.0f/((float)((GApp::UI::CoreSystem *)Tex->Core)->Width);
	Coordonates[13] = ((float)(bottom-top))*2.0f/((float)((GApp::UI::CoreSystem *)Tex->Core)->Height);

	Width = (float)(right-left);
	Height = (float)(bottom-top);

	return true;
}
*/