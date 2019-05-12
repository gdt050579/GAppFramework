#include "GApp.h"

struct Frame {	
	unsigned int	ObjectsCount;
	unsigned int	Time;
	unsigned int	BookmarkID;
};

#define GANI_HEADER_MAGIC					0x494E4147

#define ANIMATION_FLAG_RUNNING				1
#define ANIMATION_FLAG_LOOP					2
#define ANIMATION_FLAG_AUTOGOTONEXTFRAME	4
#define ANIMATION_FLAG_LOADED				8

#define GANI_HEADER_SIZE					18
#define INVALID_FRAME_INDEX					0xFFFFFFFF


#define ANIMATION_OBJECT_LINE_COLOR							0
#define ANIMATION_OBJECT_LINE_COLOR_WIDTH					1
#define ANIMATION_OBJECT_LINE_FULL							2

#define ANIMATION_OBJECT_CLIP								5
#define ANIMATION_OBJECT_CLEARCLIP							6

#define ANIMATION_OBJECT_RECT_FULL							10
#define ANIMATION_OBJECT_RECT_FILL							11
#define ANIMATION_OBJECT_RECT_STROKE						12
#define ANIMATION_OBJECT_RECT_FILL_STROKE					13

#define ANIMATION_OBJECT_IMAGE_SIMPLE						20
#define ANIMATION_OBJECT_IMAGE_RESIZED						21
#define ANIMATION_OBJECT_IMAGE_SIMPLE_SHADER				22
#define ANIMATION_OBJECT_IMAGE_RESIZED_SHADER				23
#define ANIMATION_OBJECT_IMAGE_DOCKED						24
#define ANIMATION_OBJECT_IMAGE_DOCKED_SHADER				25

#define ANIMATION_OBJECT_TEXT_RESOURCES						30
#define ANIMATION_OBJECT_TEXT_WITH_STRING					31
#define ANIMATION_OBJECT_TEXT_RESOURCES_SHADER				32
#define ANIMATION_OBJECT_TEXT_WITH_STRING_SHADER			33
#define ANIMATION_OBJECT_TEXT_RESOURCES_COLOR_BLENDING		34
#define ANIMATION_OBJECT_TEXT_WITH_STRING_COLOR_BLENDING	35
#define ANIMATION_OBJECT_TEXT_RESOURCES_ALPHA_BLENDING		36
#define ANIMATION_OBJECT_TEXT_WITH_STRING_ALPHA_BLENDING	37

#define ANIMATION_OBJECT_TEXT_USER_VALUE					38
#define ANIMATION_OBJECT_TEXT_USER_VALUE_SHADER				39
#define ANIMATION_OBJECT_TEXT_USER_VALUE_COLOR_BLENDING		40
#define ANIMATION_OBJECT_TEXT_USER_VALUE_ALPHA_BLENDING		41

#define ANIMATION_OBJECT_IMAGE_SIMPLE_COLOR_BLENDING		60
#define ANIMATION_OBJECT_IMAGE_SIMPLE_ALPHA_BLENDING		61
#define ANIMATION_OBJECT_IMAGE_RESIZED_COLOR_BLENDING		62
#define ANIMATION_OBJECT_IMAGE_RESIZED_ALPHA_BLENDING		63
#define ANIMATION_OBJECT_IMAGE_DOCKED_COLOR_BLENDING		64
#define ANIMATION_OBJECT_IMAGE_DOCKED_ALPHA_BLENDING		65



#define PREPARE_SHADER_INDEX shIndex = Anim->Data.ReadUInt16FromStream(0xFFFF);\
if ((shIndex & 0xFF) >= Anim->ShadersCount)\
{\
	REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);\
	RETURNERROR(false, "Invalid shader index (%d) - max allowed is %d - disabling object !", shIndex & 0xFF, Anim->ShadersCount);\
}\
if ((shIndex >> 8) != 0)\
{\
	REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);\
	RETURNERROR(false, "Shader with index (%d) uses dynamic parameters - this is not implemented yet !", shIndex & 0xFF);\
}


#define PREPARE_ALPHA_VALUE { alpha = (float)Anim->Data.ReadInt8FromStream() / 100.0f; if (alpha < 0) { alpha = 0; }; if (alpha>1.0f) { alpha = 1.0f;}; }

bool GetFrameInfo(unsigned int index,GApp::Utils::Buffer *b,Frame *f)
{
	unsigned int offset;
	CHECK((offset = b->GetUInt24(GANI_HEADER_SIZE+index*3,0xFFFFFFFF))!=0xFFFFFFFF,false,"Invalid offset in Animation control for frame=%d",index);	
	CHECK(b->SetStreamOffset(offset),false,"Unable to set stream offset to %d",offset);
	CHECK((f->Time = b->ReadCompactUInt32FromStream())>0,false,"Invalid time (0) in frame=%d",index);
	f->BookmarkID = b->ReadCompactUInt32FromStream();
	CHECK((f->ObjectsCount = b->ReadCompactUInt32FromStream())>=0,false,"No objects defined in frame=%d",index);	
	return true;
}
//===============================================================================================================================
void GApp::Controls::PresentationControl::Dispose() 
{
	goto_frame = INVALID_FRAME_INDEX;
	Anim = NULL;
	CurrentFrame = CurrentBookmark = INVALID_FRAME_INDEX;
	ResFactor = AnimationSpeed = 1.0f;
	FrameObjectsBufferOffset = INVALID_FRAME_INDEX;
	ObjectBufferOffset = INVALID_FRAME_INDEX;
	Flags = 0;
	FrameObjectsCount = 0;
	nrFrames = nrBookmarks = 0;
}
GApp::Controls::PresentationControl::PresentationControl(void *scene,const char* layout):GApp::Controls::GenericControl(scene,layout)
{
	Dispose();
	GenericControl::Flags |=  GAC_CONTROL_FLAG_NO_TOUCH;
}
GApp::Controls::PresentationControl::PresentationControl(void *scene,const char* layout,GApp::Resources::Presentation *anim):GApp::Controls::GenericControl(scene,layout)
{
	Dispose();
	GenericControl::Flags |=  GAC_CONTROL_FLAG_NO_TOUCH;
	LoadPresentation(anim, true, false, false);
}
GApp::Controls::PresentationControl::PresentationControl(void *scene, const char* layout, GApp::Resources::Presentation *anim, bool autoGotoNextFrame, bool startAnimation, bool loop) :GApp::Controls::GenericControl(scene, layout)
{
	Dispose();
	GenericControl::Flags |= GAC_CONTROL_FLAG_NO_TOUCH;
	LoadPresentation(anim, autoGotoNextFrame, startAnimation, loop);
}
bool GApp::Controls::PresentationControl::LoadPresentation(GApp::Resources::Presentation *anim, bool autoGotoNextFrame, bool startAnimation, bool loop)
{
	CHECK(anim!=NULL,false,"Invalid (NULL) animation object (anim)");
	if (anim==Anim)
		return true;
	while (true)
	{
		if (Anim!=NULL)
		{
			CHECKBK(Stop(),"Stopping the animation failed!");
		}
		Dispose();
		Anim = anim;
		CHECKBK(Anim->Load(),"Failed to load the animation!");
		CHECKBK(Prepare(),"Failed to prepare the animation!");
		SetAutoGotoToNextFrame(autoGotoNextFrame);
		SetLoop(loop);
		if (startAnimation)
			Start();
		return true;
	}	
	Dispose();
	return false;
}
bool GApp::Controls::PresentationControl::Prepare()
{
	CHECK(Anim!=NULL,false,"Invalid (NULL) animation object (anim)");
	CHECK(Anim->Data.GetSize()>=20,false,"An animation object should have a size of at least 20 bytes (%d)",Anim->Data.GetSize());
    // headerul
	CHECK(Anim->Data.GetUInt32(0)==GANI_HEADER_MAGIC,false,"Invalid magic for the animation object");
    // versiunea
	CHECK(Anim->Data.GetUInt8(4)==6,false,"Invalid version (expecting version 6)");
	// dimensiunile
	UInt32 W = (UInt32)Anim->Data.GetUInt16(5);
	UInt32 H = (UInt32)Anim->Data.GetUInt16(7);
	UInt32 DW = (UInt32)Anim->Data.GetUInt16(9);
	UInt32 DH = (UInt32)Anim->Data.GetUInt16(11);

	CHECK((W>0) && (H>0) && (DW>0) && (DH>0),false,"Invalid dimensions for animation object: Width=%d,Height=%d,DesignWidth=%d,DesignHeight=%d",W,H,DW,DH);
    // calculez noul size
	float rapW = (float)Core.Width/(float)DW;
	float rapH = (float)Core.Height/(float)DH;
	if (rapH<rapW) rapW = rapH;
    ResFactor = rapW;
    Resize((float)W * rapW, (float)H* rapH);
    
	// frameuri
	CHECK((nrFrames = (unsigned int)Anim->Data.GetUInt16(13))>0,false,"Invalid (0) number of frames");
	CHECK((ObjectBufferOffset = Anim->Data.GetUInt24(15))>0,false,"");
	CHECK(ObjectBufferOffset<Anim->Data.GetSize(),false,"ObjectBufferOffset is outside the animation buffer (%d)",ObjectBufferOffset);
    
	// verific frame-urile
	Frame f;
	nrBookmarks = 0;
	unsigned int temp;
	for (unsigned int tr=0;tr<nrFrames;tr++)
	{				
		CHECK(GetFrameInfo(tr,&Anim->Data,&f),false,"Unable to read frame info for frame=%d",tr);
		nrBookmarks = GAC_MAX(nrBookmarks, f.BookmarkID);
		// verific daca pozitiile obiectelor sunt corecte -> streamul e pozitionat la primul obiect
		for (unsigned int gr=0;gr<f.ObjectsCount;gr++)
		{
			CHECK((temp = Anim->Data.ReadCompactUInt32FromStream(0xFFFFFFFF))!=0xFFFFFFFF,false,"");
			CHECK(temp<Anim->Data.GetSize(),false,"");
			CHECK(temp+ObjectBufferOffset<=Anim->Data.GetSize(),false,"");			
		}
	}
    // ar trebui sa fiu exact la bufferul de date
	CHECK(Anim->Data.GetStreamOffset()==ObjectBufferOffset,false,"");
	SET_BIT(Flags,ANIMATION_FLAG_LOADED);
    return true;
}
bool GApp::Controls::PresentationControl::PaintObject(int offset)
{
	CHECK(Anim->Data.SetStreamOffset(offset+ObjectBufferOffset),false,"Unable to set stream position to %d",offset+ObjectBufferOffset);
	unsigned char animationObject,tempUInt8;
	float v1,v2,v3,v4,alpha;
	unsigned int color,color2;
	unsigned int index,shIndex;
	GApp::Graphics::Rect tmpRect;

	CHECK((animationObject= Anim->Data.ReadUInt8FromStream(0xFF))!=0xFF,false,"Unable to read animation object id");

	switch (animationObject)
	{
		case ANIMATION_OBJECT_LINE_COLOR:
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v4 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			color = Anim->Data.ReadUInt32FromStream();
			if (color==0)
			{
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid color value (TRANSPARENT) when reading from stream - disabling object !");
				break;
			}
			G.DrawLine(v1,v2,v3,v4,color);
			break;
		case ANIMATION_OBJECT_LINE_COLOR_WIDTH:
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v4 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			tempUInt8 = Anim->Data.ReadUInt8FromStream();
			color = Anim->Data.ReadUInt32FromStream();			
			if (color==0)
			{
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid color value (TRANSPARENT) when reading from stream - disabling object !");
			}
			if (tempUInt8==0)
			{
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid pen width value (0) when reading from stream - disabling object !");
			}
			//LOG_ERROR("Line: %d,%d  -> %d,%d - %d,%d", (int)G.TranslateX, (int)G.TranslateY, (int)v1, (int)v2, (int)v3, (int)v4);
			G.DrawLine(v1,v2,v3,v4,color,tempUInt8*this->ResFactor);
			break;
		case ANIMATION_OBJECT_RECT_FILL:
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v4 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			color = Anim->Data.ReadUInt32FromStream();
			if (color==0)
			{
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid color value (TRANSPARENT) when reading from stream - disabling object !");
			}
			G.DrawRect(v1,v2,v3,v4,0,color,0);
			break;
		case ANIMATION_OBJECT_RECT_STROKE:
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v4 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			color = Anim->Data.ReadUInt32FromStream();
			tempUInt8 = Anim->Data.ReadUInt8FromStream();
						
			if (color==0)
			{
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid color value (TRANSPARENT) when reading from stream - disabling object !");				
			}
			if (tempUInt8==0)
			{
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid pen width value (0) when reading from stream - disabling object !");
			}
			G.DrawRect(v1,v2,v3,v4,color,0,tempUInt8*this->ResFactor);
			break;
		case ANIMATION_OBJECT_RECT_FILL_STROKE:
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v4 = Anim->Data.ReadInt16FromStream()*this->ResFactor;			
			color = Anim->Data.ReadUInt32FromStream();			
			color2 = Anim->Data.ReadUInt32FromStream();
			tempUInt8 = Anim->Data.ReadUInt8FromStream();
			if ((color==0) || (color2==0))
			{
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid color value (TRANSPARENT) when reading from stream - disabling object !");
			}
			if (tempUInt8==0)
			{
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid pen width value (0) when reading from stream - disabling object !");
			}
			G.DrawRect(v1,v2,v3,v4,color2,color,tempUInt8*this->ResFactor);
			break;
		case ANIMATION_OBJECT_IMAGE_SIMPLE:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			if (index>=Anim->ImagesCount)
			{
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid image index (%d) - max allowed is %d - disabling object !",index,Anim->ImagesCount);
			}
			G.DrawImage(Anim->Images[index].Object,v1,v2);            
			break;
		case ANIMATION_OBJECT_IMAGE_SIMPLE_SHADER:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			if (index >= Anim->ImagesCount)
			{
				REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);
				RETURNERROR(false, "Invalid image index (%d) - max allowed is %d - disabling object !", index, Anim->ImagesCount);
			}
			PREPARE_SHADER_INDEX;
			G.DrawImage(Anim->Images[index].Object, v1, v2,Anim->Shaders[shIndex & 0xFF].Object);
			break;
		case ANIMATION_OBJECT_IMAGE_SIMPLE_COLOR_BLENDING:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			if (index >= Anim->ImagesCount)
			{
				REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);
				RETURNERROR(false, "Invalid image index (%d) - max allowed is %d - disabling object !", index, Anim->ImagesCount);
			}
			color = Anim->Data.ReadUInt32FromStream();
			G.DrawImageWithColorBlending(Anim->Images[index].Object, v1, v2, color);
			break;
		case ANIMATION_OBJECT_IMAGE_SIMPLE_ALPHA_BLENDING:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			if (index >= Anim->ImagesCount)
			{
				REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);
				RETURNERROR(false, "Invalid image index (%d) - max allowed is %d - disabling object !", index, Anim->ImagesCount);
			}
			PREPARE_ALPHA_VALUE;
			G.DrawImageWithAlphaBlending(Anim->Images[index].Object, v1, v2, alpha);
			break;
		case ANIMATION_OBJECT_IMAGE_RESIZED:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v4 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			if (index>=Anim->ImagesCount)
			{				
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid image index (%d) - max allowed is %d - disabling object !",index,Anim->ImagesCount);
			}
			G.DrawImageResized(Anim->Images[index].Object,v1,v2,v3,v4);            
			break;
		case ANIMATION_OBJECT_IMAGE_RESIZED_SHADER:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v4 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			if (index >= Anim->ImagesCount)
			{
				REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);
				RETURNERROR(false, "Invalid image index (%d) - max allowed is %d - disabling object !", index, Anim->ImagesCount);
			}
			PREPARE_SHADER_INDEX;
			G.DrawImageResized(Anim->Images[index].Object, v1, v2, v3, v4, Anim->Shaders[shIndex & 0xFF].Object);
			break;
		case ANIMATION_OBJECT_IMAGE_RESIZED_COLOR_BLENDING:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v4 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			if (index >= Anim->ImagesCount)
			{
				REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);
				RETURNERROR(false, "Invalid image index (%d) - max allowed is %d - disabling object !", index, Anim->ImagesCount);
			}
			color = Anim->Data.ReadUInt32FromStream();
			G.DrawImageResizedWithColorBlending(Anim->Images[index].Object, v1, v2, v3, v4,color);
			break;
		case ANIMATION_OBJECT_IMAGE_RESIZED_ALPHA_BLENDING:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v4 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			if (index >= Anim->ImagesCount)
			{
				REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);
				RETURNERROR(false, "Invalid image index (%d) - max allowed is %d - disabling object !", index, Anim->ImagesCount);
			}
			PREPARE_ALPHA_VALUE;
			G.DrawImageResizedWithAlphaBlending(Anim->Images[index].Object, v1, v2, v3, v4, alpha);
			break;
		case ANIMATION_OBJECT_IMAGE_DOCKED:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			tempUInt8 = Anim->Data.ReadUInt8FromStream();
			if (index >= Anim->ImagesCount)
			{
				REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);
				RETURNERROR(false, "Invalid image index (%d) - max allowed is %d - disabling object !", index, Anim->ImagesCount);
			}
			G.DrawImageAligned(Anim->Images[index].Object, v1, v2, tempUInt8, 1.0f);
			break;
		case ANIMATION_OBJECT_IMAGE_DOCKED_SHADER:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			tempUInt8 = Anim->Data.ReadUInt8FromStream();
			if (index >= Anim->ImagesCount)
			{
				REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);
				RETURNERROR(false, "Invalid image index (%d) - max allowed is %d - disabling object !", index, Anim->ImagesCount);
			}
			PREPARE_SHADER_INDEX;
			G.DrawImageAligned(Anim->Images[index].Object, v1, v2, tempUInt8, 1.0f, Anim->Shaders[shIndex & 0xFF].Object);
			break;
		case ANIMATION_OBJECT_IMAGE_DOCKED_COLOR_BLENDING:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			tempUInt8 = Anim->Data.ReadUInt8FromStream();
			if (index >= Anim->ImagesCount)
			{
				REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);
				RETURNERROR(false, "Invalid image index (%d) - max allowed is %d - disabling object !", index, Anim->ImagesCount);
			}
			color = Anim->Data.ReadUInt32FromStream();
			G.DrawImageAlignedWithColorBlending(Anim->Images[index].Object, v1, v2, tempUInt8, 1.0f, color);
			break;
		case ANIMATION_OBJECT_IMAGE_DOCKED_ALPHA_BLENDING:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			tempUInt8 = Anim->Data.ReadUInt8FromStream();
			if (index >= Anim->ImagesCount)
			{
				REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);
				RETURNERROR(false, "Invalid image index (%d) - max allowed is %d - disabling object !", index, Anim->ImagesCount);
			}
			PREPARE_ALPHA_VALUE;
			G.DrawImageAlignedWithAlphaBlending(Anim->Images[index].Object, v1, v2, tempUInt8, 1.0f, alpha);
			break;
		case ANIMATION_OBJECT_TEXT_RESOURCES:
		case ANIMATION_OBJECT_TEXT_RESOURCES_SHADER:
		case ANIMATION_OBJECT_TEXT_RESOURCES_COLOR_BLENDING:
		case ANIMATION_OBJECT_TEXT_RESOURCES_ALPHA_BLENDING:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			if (index>=Anim->StringsCount)
			{				
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid string index (%d) - max allowed is %d - disabling object !",index,Anim->StringsCount);
			}
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v4 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			tp.SetViewRect(v1,v2,v3,v4);
			tp.SetText(Anim->Strings[index].Object);
			index = Anim->Data.ReadUInt8FromStream();
			tp.SetDockPosition(index & 0x0F);
			if ((index & 64)!=0)
				tp.SetWordWrap(true);
			else if ((index & 32)!=0)
				tp.SetTextJustify(true);
			else
				tp.SetWordWrap(false);

			index = Anim->Data.ReadUInt8FromStream(0xFF);
			if (index>=Anim->FontsCount)
			{				
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid font index (%d) - max allowed is %d - disabling object !",index,Anim->FontsCount);
			}
			tp.SetFont(Anim->Fonts[index].Object);
			index = Anim->Data.ReadUInt8FromStream(0xFF);
			v1 = (float)Anim->Data.ReadUInt16FromStream();
			v2 = (float)Anim->Data.ReadInt16FromStream();
			tp.SetFontSize(index,v1/100.0f);
			tp.SetLineSpace(v2/100.0f);
			switch (animationObject)
			{
				case ANIMATION_OBJECT_TEXT_RESOURCES_SHADER:
					PREPARE_SHADER_INDEX;
					tp.SetShader(Anim->Shaders[shIndex & 0xFF].Object);
					break;
				case ANIMATION_OBJECT_TEXT_RESOURCES_COLOR_BLENDING:
					color = Anim->Data.ReadUInt32FromStream();
					tp.SetColorBlending(color);
					break;
				case ANIMATION_OBJECT_TEXT_RESOURCES_ALPHA_BLENDING:
					v1 = (float)Anim->Data.ReadInt8FromStream()/100.0f;
					if (v1 < 0)
						v1 = 0;
					if (v1>1.0f)
						v1 = 1.0f;
					tp.SetAlphaBlending(v1);
					break;
				default:
					tp.SetShader(NULL);
					break;
			}
			G.DrawString(&tp);
			break;

		case ANIMATION_OBJECT_TEXT_USER_VALUE:
		case ANIMATION_OBJECT_TEXT_USER_VALUE_SHADER:
		case ANIMATION_OBJECT_TEXT_USER_VALUE_COLOR_BLENDING:
		case ANIMATION_OBJECT_TEXT_USER_VALUE_ALPHA_BLENDING:
			index = Anim->Data.ReadUInt16FromStream(0xFFFF);
			if (index >= Anim->StringValuesCount)
			{
				REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);
				RETURNERROR(false, "Invalid string index (%d) - max allowed is %d - disabling object !", index, Anim->StringValuesCount);
			}
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v4 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			tp.SetViewRect(v1, v2, v3, v4);
			tp.SetText(Anim->StringsValues[index].Value.GetText());
			index = Anim->Data.ReadUInt8FromStream();
			tp.SetDockPosition(index & 0x0F);
			if ((index & 64) != 0)
				tp.SetWordWrap(true);
			else if ((index & 32) != 0)
				tp.SetTextJustify(true);
			else
				tp.SetWordWrap(false);

			index = Anim->Data.ReadUInt8FromStream(0xFF);
			if (index >= Anim->FontsCount)
			{
				REMOVE_BIT(Flags, ANIMATION_FLAG_LOADED);
				RETURNERROR(false, "Invalid font index (%d) - max allowed is %d - disabling object !", index, Anim->FontsCount);
			}
			tp.SetFont(Anim->Fonts[index].Object);
			index = Anim->Data.ReadUInt8FromStream(0xFF);
			v1 = (float)Anim->Data.ReadUInt16FromStream();
			v2 = (float)Anim->Data.ReadInt16FromStream();
			tp.SetFontSize(index, v1 / 100.0f);
			tp.SetLineSpace(v2 / 100.0f);
			switch (animationObject)
			{
			case ANIMATION_OBJECT_TEXT_USER_VALUE_SHADER:
				PREPARE_SHADER_INDEX;
				tp.SetShader(Anim->Shaders[shIndex & 0xFF].Object);
				break;
			case ANIMATION_OBJECT_TEXT_USER_VALUE_COLOR_BLENDING:
				color = Anim->Data.ReadUInt32FromStream();
				tp.SetColorBlending(color);
				break;
			case ANIMATION_OBJECT_TEXT_USER_VALUE_ALPHA_BLENDING:
				v1 = (float)Anim->Data.ReadInt8FromStream() / 100.0f;
				if (v1 < 0)
					v1 = 0;
				if (v1>1.0f)
					v1 = 1.0f;
				tp.SetAlphaBlending(v1);
				break;
			default:
				tp.SetShader(NULL);
				break;
			}
			G.DrawString(&tp);
			break;

		case ANIMATION_OBJECT_TEXT_WITH_STRING:
		case ANIMATION_OBJECT_TEXT_WITH_STRING_SHADER:
		case ANIMATION_OBJECT_TEXT_WITH_STRING_COLOR_BLENDING:
		case ANIMATION_OBJECT_TEXT_WITH_STRING_ALPHA_BLENDING:
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v2 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			v4 = Anim->Data.ReadInt16FromStream()*this->ResFactor;
			tp.SetViewRect(v1,v2,v3,v4);			
			index = Anim->Data.ReadUInt8FromStream();
			tp.SetDockPosition(index & 0x0F);
			if ((index & 64)!=0)
				tp.SetWordWrap(true);
			else if ((index & 32)!=0)
				tp.SetTextJustify(true);
			else
				tp.SetWordWrap(false);
			index = Anim->Data.ReadUInt8FromStream(0xFF);
			if (index>=Anim->FontsCount)
			{				
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid font index (%d) - max allowed is %d - disabling object !",index,Anim->FontsCount);
			}
			tp.SetFont(Anim->Fonts[index].Object);
			index = Anim->Data.ReadUInt8FromStream(0xFF);
			v1 = (float)Anim->Data.ReadUInt16FromStream();
			v2 = (float)Anim->Data.ReadInt16FromStream();
			tp.SetFontSize(index,v1/100.0f);
			tp.SetLineSpace(v2/100.0f);
			index = Anim->Data.ReadUInt8FromStream(0xFF);
			if (index==0xFF)
			{				
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Invalid string size (%d) - disabling object !",index);
			}
			tp.SetText((char *)&(Anim->Data.GetBuffer()[Anim->Data.GetStreamOffset()]),index);
			Anim->Data.SetStreamOffset(Anim->Data.GetStreamOffset() + index);
			switch (animationObject)
			{
			case ANIMATION_OBJECT_TEXT_WITH_STRING_SHADER:
				PREPARE_SHADER_INDEX;
				tp.SetShader(Anim->Shaders[shIndex & 0xFF].Object);
				break;
			case ANIMATION_OBJECT_TEXT_WITH_STRING_COLOR_BLENDING:
				color = Anim->Data.ReadUInt32FromStream();
				tp.SetColorBlending(color);
				break;
			case ANIMATION_OBJECT_TEXT_WITH_STRING_ALPHA_BLENDING:
				v1 = (float)Anim->Data.ReadInt8FromStream() / 100.0f;
				if (v1 < 0)
					v1 = 0;
				if (v1>1.0f)
					v1 = 1.0f;
				tp.SetAlphaBlending(v1);
				break;
			default:
				tp.SetShader(NULL);
				break;
			}
			G.DrawString(&tp);
			break;
		case ANIMATION_OBJECT_CLIP:
			v1 = Anim->Data.ReadInt16FromStream()*this->ResFactor + G.TranslateX;
			v4 = Core.Height - (Anim->Data.ReadInt16FromStream()*this->ResFactor + G.TranslateY);
			v3 = Anim->Data.ReadInt16FromStream()*this->ResFactor + G.TranslateX;
			v2 = Core.Height - (Anim->Data.ReadInt16FromStream()*this->ResFactor + G.TranslateY);

			//LOG_ERROR("Clip: %d,%d  -> %d,%d - %d,%d", (int)G.TranslateX, (int)G.TranslateY, this->Layout.ViewLeft, this->Layout.ViewTop, this->Layout.ViewRight, this->Layout.ViewBottom);
			//LOG_ERROR("Before : %d,%d  -> %d,%d - %d,%d", (int)G.TranslateX, (int)G.TranslateY, (int)v1,(int)v2,(int)v3,(int)v4);
			v1 = GAC_MAX(v1, this->Layout.ViewLeft);
			v2 = GAC_MAX(v2, this->Layout.ViewTop);
			v3 = GAC_MIN(v3, this->Layout.ViewRight);
			v4 = GAC_MIN(v4, this->Layout.ViewBottom);
			// daca sunt invers - nu ar trebui sa se mai vada nimik
			if ((v1 > v3) || (v2 > v4)) { v1 = v2 = v3 = v4 = 0; }
			//LOG_ERROR("After : %d,%d  -> %d,%d - %d,%d", (int)G.TranslateX, (int)G.TranslateY, (int)v1, (int)v2, (int)v3, (int)v4);
			//((GApp::UI::CoreSystem *)control->CoreContext)->Graphics.SetClip(control->Layout.ViewLeft, control->Layout.ViewTop, control->Layout.ViewRight, control->Layout.ViewBottom);
			//LOG_ERROR("Clip: %d,%d  -> %d,%d - %d,%d", (int)G.TranslateX, (int)G.TranslateY, this->Layout.ViewLeft, this->Layout.ViewTop, this->Layout.ViewRight, this->Layout.ViewBottom);
			//LOG_ERROR("Req : %d,%d  -> %d,%d - %d,%d", (int)G.TranslateX, (int)G.TranslateY, (int)v1,(int)v2,(int)v3,(int)v4);
			//G.DrawRect(v1 - G.TranslateX, v2 - G.TranslateY, v3 - G.TranslateX, v4 - G.TranslateY, 0xFFFFFFFF, 0, 8);
			G.SetClip((int)v1, (int)v2, (int)v3, (int)v4);
			//G.DrawRect(0, 0, 800, 800, 0, 0xFF00FF00, 0);
			break;
		case ANIMATION_OBJECT_CLEARCLIP:
			G.SetClip(this->Layout.ViewLeft, this->Layout.ViewTop, this->Layout.ViewRight, this->Layout.ViewBottom);
			break;
		default:
			REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
			RETURNERROR(false,"Unknown animation object type: %d - disabling object",animationObject);			
			break;
	}
	return true;
	/*
    Anim.Data.SetStreamPosition(offset);
    int tip = Anim.Data.GetByte();
    int left, right, top, bottom, c1, c2, pw,crx,cry;
    FontSizeType fst;
    float a;
    switch (tip)
    {
        case 0:
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            c1 = Anim.Data.GetInt();
            G.DrawEllipse(left, top, right, bottom, Color.Transparent, c1, 1);
            break;
        case 1:
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            c1 = Anim.Data.GetInt();
            pw = Anim.Data.GetByte();
            G.DrawEllipse(left, top, right, bottom, c1,Color.Transparent, pw);
            break;
        case 2:
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            c1 = Anim.Data.GetInt();
            c2 = Anim.Data.GetInt();
            pw = Anim.Data.GetByte();
            G.DrawEllipse(left, top, right, bottom, c2, c1, pw);
            break;
        case 3:
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            c1 = Anim.Data.GetInt();
            c2 = Anim.Data.GetInt();
            pw = Anim.Data.GetByte();
            crx = (int)(Anim.Data.GetShort() * ResFactor);
            cry = (int)(Anim.Data.GetShort() * ResFactor);
            a = Anim.Data.GetShort() / 100.0f;
            G.SaveState();
            G.Rotate(a, crx, cry);
            G.DrawEllipse(left, top, right, bottom, c2, c1, pw);
            G.RestoreState();
            break;

        case 10:
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            c1 = Anim.Data.GetInt();
            G.DrawRect(left, top, right, bottom, Color.Transparent, c1, 1);
            break;
        case 11:
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            c1 = Anim.Data.GetInt();
            pw = Anim.Data.GetByte();
            G.DrawRect(left, top, right, bottom, c1, Color.Transparent, pw);
            break;
        case 12:
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            c1 = Anim.Data.GetInt();
            c2 = Anim.Data.GetInt();
            pw = Anim.Data.GetByte();
            G.DrawRect(left, top, right, bottom, c2, c1, pw);
            break;
        case 13:
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            c1 = Anim.Data.GetInt();
            c2 = Anim.Data.GetInt();
            pw = Anim.Data.GetByte();
            crx = (int)(Anim.Data.GetShort() * ResFactor);
            cry = (int)(Anim.Data.GetShort() * ResFactor);
            a = Anim.Data.GetShort() / 100.0f;
            G.SaveState();
            G.Rotate(a, crx, cry);
            G.DrawRect(left, top, right, bottom, c2, c1, pw);
            G.RestoreState();
            break;

        case 20:
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            pw = Anim.Data.GetByte();
            c1 = Anim.Data.GetInt();
            G.DrawLine(left, top, right, bottom, c1, pw);
            break;
        case 21:
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            pw = Anim.Data.GetByte();
            c1 = Anim.Data.GetInt();
            crx = (int)(Anim.Data.GetShort() * ResFactor);
            cry = (int)(Anim.Data.GetShort() * ResFactor);
            a = Anim.Data.GetShort() / 100.0f;
            G.SaveState();
            G.Rotate(a, crx, cry);
            G.DrawLine(left, top, right, bottom, c1, pw);
            G.RestoreState();
            break;

        case 30:
            c1 = Anim.Data.GetWord();
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            G.DrawImage(Anim.Images[c1], left, top);
            break;
        case 31:
            c1 = Anim.Data.GetWord();
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            G.DrawImage(Anim.Images[c1], left, top,right,bottom);
            break;
        case 32:
            c1 = Anim.Data.GetWord();
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            crx = (int)(Anim.Data.GetShort() * ResFactor);
            cry = (int)(Anim.Data.GetShort() * ResFactor);
            a = Anim.Data.GetShort() / 100.0f;
            G.SaveState();
            G.Rotate(a, crx, cry);
            G.DrawImage(Anim.Images[c1], left, top, right, bottom);
            G.RestoreState();
            break;

        case 40:
            c1 = Anim.Data.GetWord();
            tp.SetText(Anim.Strings[c1]);
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            tp.SetDrawingRect(left, top, right, bottom);
            c1 = Anim.Data.GetByte();
            tp.SetTextPosition(IntToAlig[c1 & 0x0F]);
            tp.SetWordWrap((c1 & 64) != 0);
            tp.SetUseDescendInFontHeight((c1 & 32) != 0);
            c1 = Anim.Data.GetByte(); // fontul
            tp.SetFont(Anim.Fonts[c1]);
            c2 = Anim.Data.GetByte();
            c1 = Anim.Data.GetWord();
            fst= IntToFontSizeType[c2];
            if ((fst == FontSizeType.FitPercentageOfDrawingRectHeight) || (fst == FontSizeType.FitPercentageOfDrawingRectWidth))
                tp.SetFontSize(fst,c1/100.0f);
            else
                tp.SetFontSize(fst, c1 * ResFactor / 100.0f);
            c1 = Anim.Data.GetWord();
            tp.SetHeightPercentage(c1 / 100.0f);
            c1 = Anim.Data.GetInt();
            tp.SetColor(c1);
            G.DrawString(tp);
            break;
        case 50:                    
            tp.SetText(Anim.Data.GetStringWithSize());
            left = (int)(Anim.Data.GetShort() * ResFactor);
            top = (int)(Anim.Data.GetShort() * ResFactor);
            right = (int)(Anim.Data.GetShort() * ResFactor);
            bottom = (int)(Anim.Data.GetShort() * ResFactor);
            tp.SetDrawingRect(left, top, right, bottom);
            c1 = Anim.Data.GetByte();
            tp.SetTextPosition(IntToAlig[c1 & 0x0F]);
            tp.SetWordWrap((c1 & 64) != 0);
            tp.SetUseDescendInFontHeight((c1 & 32) != 0);
            c1 = Anim.Data.GetByte(); // fontul
            tp.SetFont(Anim.Fonts[c1]);
            c2 = Anim.Data.GetByte();
            c1 = Anim.Data.GetWord();
            fst= IntToFontSizeType[c2];
            if ((fst == FontSizeType.FitPercentageOfDrawingRectHeight) || (fst == FontSizeType.FitPercentageOfDrawingRectWidth))
                tp.SetFontSize(fst,c1/100.0f);
            else
                tp.SetFontSize(fst, c1 * ResFactor / 100.0f);
            c1 = Anim.Data.GetWord();
            tp.SetHeightPercentage(c1 / 100.0f);
            c1 = Anim.Data.GetInt();
            tp.SetColor(c1);
            G.DrawString(tp);
            break;
    }
	*/
}
bool GApp::Controls::PresentationControl::PaintFrame()
{
	if (goto_frame!=INVALID_FRAME_INDEX)
	{
		if (goto_frame<nrFrames)
		{
			Frame f;
			if (GetFrameInfo(goto_frame,&Anim->Data,&f)==false)
			{
				REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
				RETURNERROR(false,"Internal error - GetFrameInfo failed in OnPaint() !");
			}
			RemainingTicks = (unsigned int)(f.Time/AnimationSpeed);
			if (RemainingTicks==0)
				RemainingTicks = 1;			
			FrameObjectsBufferOffset = Anim->Data.GetStreamOffset();
			FrameObjectsCount = f.ObjectsCount;
			CurrentFrame = goto_frame;
			goto_frame = INVALID_FRAME_INDEX;
			TriggerEvent(GAC_EVENTTYPE_PRESENTATIONCURENTFRAMECHANGED);
			if (f.BookmarkID>0)
			{
				CurrentBookmark = f.BookmarkID-1;
				TriggerEvent(GAC_EVENTTYPE_PRESENTATIONBOOKMARKCHANGED);
			}
		} else {
			REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
			RETURNERROR(false,"Internal error - invalid goto_frame id = %d (Total frames = %d)!",goto_frame,nrFrames);
		}
	}
	// desenam obiectele
	unsigned int tmpOffset = FrameObjectsBufferOffset;
	unsigned int offset;
	for (unsigned int tr=0;tr<FrameObjectsCount;tr++)
	{
		if (Anim->Data.SetStreamOffset(tmpOffset)==false)
		{
			REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
			RETURNERROR(false,"Internal error - unable to set stream offset to %d",tmpOffset);
		}
		if ((offset = Anim->Data.ReadCompactUInt32FromStream(0xFFFFFFFF))==0xFFFFFFFF)
		{
			REMOVE_BIT(Flags,ANIMATION_FLAG_LOADED);
			RETURNERROR(false,"Internal error - invalid value for frame object offset");
		}
		tmpOffset = Anim->Data.GetStreamOffset();
		PaintObject(offset);
	}
        
	if ((Flags & ANIMATION_FLAG_AUTOGOTONEXTFRAME) != 0)
    {
        RemainingTicks--;
        if (RemainingTicks == 0)
        {
            CurrentFrame++;
            if (CurrentFrame >= nrFrames)
            {
                if ((Flags & ANIMATION_FLAG_LOOP)!=0)
                {
                    GoToFrame(0);
                } else {
					REMOVE_BIT(Flags,ANIMATION_FLAG_RUNNING);                    
                    TriggerEvent(GAC_EVENTTYPE_PRESENTATIONENDED);
                }
            }
            else
            {                
                goto_frame = CurrentFrame;
            }
        }
    }
	return true;
}
void GApp::Controls::PresentationControl::OnPaint()
{
	if (((Flags & (ANIMATION_FLAG_RUNNING|ANIMATION_FLAG_LOADED))!=(ANIMATION_FLAG_RUNNING|ANIMATION_FLAG_LOADED)) || (nrFrames==0))
		return;
	PaintFrame();
}


bool GApp::Controls::PresentationControl::Stop()
{
	CHECK(Anim!=NULL,false,"Invalid (NULL) animation object (anim)");
	REMOVE_BIT(Flags,ANIMATION_FLAG_RUNNING);
	goto_frame = INVALID_FRAME_INDEX;
	return true;
}
bool GApp::Controls::PresentationControl::Start()
{
	CHECK(Anim!=NULL,false,"Invalid (NULL) animation object (anim)");
	CHECK((Flags & ANIMATION_FLAG_LOADED)!=0,false,"Animation is not loaded !");
	if ((Flags & ANIMATION_FLAG_RUNNING)!=0)
		return true;
    goto_frame = 0;
    CurrentBookmark = INVALID_FRAME_INDEX;
    SET_BIT(Flags,ANIMATION_FLAG_RUNNING);
    TriggerEvent(GAC_EVENTTYPE_PRESENTATIONSTARTED);
	return true;
}
bool GApp::Controls::PresentationControl::IsPresentationLoaded()
{
	return ((Flags & ANIMATION_FLAG_LOADED)!=0);
}
bool GApp::Controls::PresentationControl::IsRunning()
{
	return ((Flags & ANIMATION_FLAG_RUNNING)!=0);
}
bool GApp::Controls::PresentationControl::HasBookmarks()
{
    return (nrBookmarks>0);
}

void GApp::Controls::PresentationControl::SetLoop(bool value)
{
	if (value) {
		SET_BIT(Flags,ANIMATION_FLAG_LOOP);
	} else {
		REMOVE_BIT(Flags,ANIMATION_FLAG_LOOP);    
	}
}
void GApp::Controls::PresentationControl::SetAutoGotoToNextFrame(bool value)
{
	if (value) {
		SET_BIT(Flags, ANIMATION_FLAG_AUTOGOTONEXTFRAME);
	} else {
		REMOVE_BIT(Flags, ANIMATION_FLAG_AUTOGOTONEXTFRAME);
	}
}

unsigned int GApp::Controls::PresentationControl::GetFramesCount()
{
	return nrFrames;
}
unsigned int GApp::Controls::PresentationControl::GetCurrentFrame()
{
    return CurrentFrame;
}
bool GApp::Controls::PresentationControl::GoToFrame(unsigned int index)
{
    if (index >= nrFrames)
	{
		goto_frame = INVALID_FRAME_INDEX;
		RETURNERROR(false,"Invalid request for GoToFrame - %d (Total Frames=%d)",index,nrFrames);
	}
    else
	{
        goto_frame = index;
		return true;
	}
}
bool GApp::Controls::PresentationControl::GoToNextFrame()
{
    return GoToFrame(CurrentFrame + 1);
}
bool GApp::Controls::PresentationControl::GoToPreviousFrame()
{
	if (CurrentFrame==0)
		return false;
    return GoToFrame(CurrentFrame - 1);
}

unsigned int GApp::Controls::PresentationControl::GetBookmarksCount()
{
	return nrBookmarks;
}
unsigned int GApp::Controls::PresentationControl::GetCurrentBookmark()
{
    return CurrentBookmark;
}
bool GApp::Controls::PresentationControl::GoToBookmark(unsigned int index)
{
	CHECK(index<nrBookmarks,false,"Invalid index for bookmark (%d) - allowed is [0..%d)",index,nrBookmarks);
	Frame f;
	index++;
	for (unsigned int tr=0;tr<nrFrames;tr++)
	{
		CHECK(GetFrameInfo(tr,&Anim->Data,&f),false,"Unable to read frame informations for frame-%d",index);
		if (f.BookmarkID == index)
			return GoToFrame(tr);
	}
	return false;    
}
bool GApp::Controls::PresentationControl::GoToNextBookmark()
{
    return GoToBookmark(CurrentBookmark + 1);
}
bool GApp::Controls::PresentationControl::GoToPreviousBookmark()
{	
	CHECK(CurrentBookmark>0,false,"You can not go to the previous bookmark from the first one");
    return GoToBookmark(CurrentBookmark - 1);
}
bool GApp::Controls::PresentationControl::SetPresentationSpeed(float value)
{
	CHECK((value>0) && (value<=1),false,"Animation speed shoul be beween 0 and 1 (value = %f)",value);
	AnimationSpeed = value;
	return true;
}