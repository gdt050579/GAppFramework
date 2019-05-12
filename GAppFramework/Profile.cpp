#include "GApp.h"
#ifdef PLATFORM_DEVELOP
	#include "DevelopMode.h"
#endif

#define INVALID_TEXTURE_ID	0xFFFFFFFF

#define PROFILE_STATE_NOT_LOADED		0
#define PROFILE_STATE_LOADED			1
#define PROFILE_STATE_DELAY_LOAD		2

#define CLEAR_TEXTURE_DATA	ClearTextureData(Textures,sizeof(Textures)/sizeof(Textures[0]));
/*
1->n	-> Globale
1		-> Scena
1		-> Local
*/

void ComputeBitmapSize(GApp::Resources::Bitmap *bmp, unsigned int &bmp_Width, unsigned int &bmp_Height)
{
	#if defined(DYNAMIC_IMAGE_RESIZE)
		bmp_Width = bmp->OriginalWidth;
		bmp_Height = bmp->OriginalHeight;
	#elif defined(RESIZE_AT_STARTUP_IMAGE_RESIZE)
		bmp_Width = bmp->Width;
		bmp_Height = bmp->Height;
	#else
		if ((bmp->Flags & GAC_RESOURCEFLAG_RESIZEMODE) == GAC_RESIZEMODE_DYNAMIC)
		{
			bmp_Width = bmp->OriginalWidth;
			bmp_Height = bmp->OriginalHeight;
		}
		else {
			bmp_Width = bmp->Width;
			bmp_Height = bmp->Height;
		}
	#endif
}
unsigned int Power2(unsigned int value)
{
	for (unsigned int tr = 0; tr<30; tr++)
	if ((((unsigned int)1) << tr) >= value)
		return 1 << tr;
	return 0xFFFFFFFF;
}

#define MAX_TEXTURE_POZ_COUNT		1000
#define HEIGHT_UNTIL_END			0xFFFFFFFF
#define ATLAS_SCORE_NOMATCH			0xFFFFFFFF
#define ATLAS_METHOD_FIRST			0
#define ATLAS_METHOD_BESTFIT		1
#define ATLAS_METHOD_BESTFITHEIGHT	2

struct TexturePoz
{
	unsigned int	X, Y, Height;
	TexturePoz*		Next;
	TexturePoz*		Prec;
};
class Atlas
{
	TexturePoz		Pozitions[MAX_TEXTURE_POZ_COUNT];
	TexturePoz*		Start;
	TexturePoz*		Last;
	unsigned int	Width, Height;
	unsigned int	MinHeight,MaxTextureSize;
	unsigned int	Count;
	bool			InternalError;
	unsigned int	ComputeFitScore(TexturePoz* tp, unsigned int imageWidth, unsigned int imageHeight);
public:
	void			Clear(unsigned int minHeight, unsigned int maxTextureSize);
	TexturePoz*		Add(unsigned int imageWidth, unsigned int imageHeight, unsigned int method = ATLAS_METHOD_FIRST);
	bool			Grow();
	void			Ungrow() { Width = Width / 2; }
	unsigned int	GetWidth() { return Width; }
	unsigned int	GetHeight() { return Height; }
	bool			HasError() { return InternalError; }
};
Atlas atlas;

bool Atlas::Grow()
{
	if (Width <= Height)
	{
		if ((Width * 2) <= MaxTextureSize)
		{
			Width = Width * 2;
			return true;
		}
	}
	else {
		if ((Height * 2) <= MaxTextureSize)
		{
			Height = Height * 2;
			return true;
		}
	}
	return false;
}

unsigned int Atlas::ComputeFitScore(TexturePoz* tp, unsigned int imageWidth, unsigned int imageHeight)
{
	unsigned int realHeight;
	if ((tp->X + imageWidth) > Width)
		return ATLAS_SCORE_NOMATCH;
	if (tp->Height == HEIGHT_UNTIL_END)
		realHeight = Height - tp->Y;
	else
		realHeight = tp->Height;
	return ((GAC_MIN(realHeight - imageHeight, 0xFFFF)) << 16) | (GAC_MAX(tp->X, 0xFFFF));
}
void Atlas::Clear(unsigned int minHeight,unsigned int maxTextureSize)
{
	for (int tr = 0; tr < MAX_TEXTURE_POZ_COUNT; tr++)
	{
		this->Pozitions[tr].X = 0;
		this->Pozitions[tr].Y = 0;
		this->Pozitions[tr].Height = HEIGHT_UNTIL_END;
		this->Pozitions[tr].Next = this->Pozitions[tr].Prec = NULL;
	}
	this->Start = this->Last = NULL;
	this->Count = 0;
	this->Width = this->Height = 0;
	this->MinHeight = minHeight;
	this->MaxTextureSize = maxTextureSize;
	this->InternalError = false;
}
TexturePoz* Atlas::Add(unsigned int imgW, unsigned int imgH, unsigned int method)
{
	TexturePoz* tp = NULL;
	TexturePoz* cursor =  NULL;
	unsigned int score,bestScore;
	// daca nu am nimik adaugat - atunci creez un spatiu nou
	imgW = (imgW + 4) & 0xFFFFFFFC;
	imgH = (imgH + 4) & 0xFFFFFFFC;
	//LOG_INFO("Adding image of size (%d,%d) to Atlas (W=%d,H=%d) with %d rectangles", imgW, imgH, Width, Height, Count);
	if (Count == 0)
	{
		Width = GAC_MAX(Power2(imgW), 16);
		Height = GAC_MAX(Power2(imgH), 16);
		//if (Height * 2 <= MaxTextureSize)
		//	Height *= 2;
		//if (Height * 2 <= MaxTextureSize)
		//	Height *= 2;
		if ((Width > MaxTextureSize) || (Height > MaxTextureSize))
		{
			this->InternalError = true;
			RETURNERROR(NULL,"Image is larger than the capacity of one texture. Image size is %d x %d (texture max capacity is %d)", imgW, imgH, MaxTextureSize);
		}
		Pozitions[0].X = Pozitions[0].Y = 0;
		Pozitions[0].Height = HEIGHT_UNTIL_END;
		Pozitions[0].Prec = Pozitions[0].Next = NULL;
		Count = 1;
		Start = Last = &Pozitions[0];
		tp = &Pozitions[0];
	}
	else 
	{
		// caut o pozitie in functie de methoda
		cursor = Start;
		switch (method)
		{
			case ATLAS_METHOD_FIRST:
				while (cursor != NULL)
				{
					score = ComputeFitScore(cursor, imgW, imgH);
					if (score != ATLAS_SCORE_NOMATCH)
					{
						tp = cursor;
						break;
					}
					cursor = cursor->Next;
				}
				break;
			case ATLAS_METHOD_BESTFIT:
				bestScore = ATLAS_SCORE_NOMATCH;
				while (cursor != NULL)
				{
					score = ComputeFitScore(cursor, imgW, imgH);
					int h = cursor->Height;
					if (h == HEIGHT_UNTIL_END)
						h = Height - cursor->Y;
					//LOG_INFO("  Image(%d,%d) in Square(X=%d,Y=%d,W=%d,H=%d) -> %d", imgW, imgH, cursor->X, cursor->Y,Width - cursor->X,h, score);
					if (score < bestScore)
					{
						tp = cursor;
						bestScore = score;
					}
					cursor = cursor->Next;
				}
				break;
			case ATLAS_METHOD_BESTFITHEIGHT:
				bestScore = ATLAS_SCORE_NOMATCH;
				while (cursor != NULL)
				{
					score = ComputeFitScore(cursor, imgW, imgH) >> 16;
					if (score < bestScore)
					{
						tp = cursor;
						bestScore = score;
						if (score == 0) // potrivire exacta
							break;
					}
					cursor = cursor->Next;
				}
				break;
		}
	}
	// am terminat cautarea - daca nu am nimik in tp - nu am gasit nici o pozitie, ar trebui sa extind
	if (tp == NULL)
		return NULL;
	// adaug cele doua dreptunghiuri nou create
	unsigned int nX, nY;
	nX = tp->X + imgW; // alinierea se face la 4 pixeli
	nY = tp->Y + imgH; // alinierea se face la 4 pixeli
	// dreptunghiul mare
	if ((Count < MAX_TEXTURE_POZ_COUNT) && (nX<=Width))
	{		
		Pozitions[Count].X = nX;
		Pozitions[Count].Y = tp->Y;
		Pozitions[Count].Height = imgH;
		Last->Next = &Pozitions[Count];
		Pozitions[Count].Prec = Last;
		Pozitions[Count].Next = NULL;
		Last = &Pozitions[Count];
		//LOG_INFO("Adding new rect (M1) ID=%5d: (%d,%d - Height=%d)", Count, Pozitions[Count].X, Pozitions[Count].Y, Pozitions[Count].Height);
		Count++;
	}
	// dreptunghiul mai mic
	//if ((Count < MAX_TEXTURE_POZ_COUNT) && (nY+MinHeight<=Height))
	if ((Count < MAX_TEXTURE_POZ_COUNT) && (nY<= Height))
	{
		Pozitions[Count].X = tp->X;
		Pozitions[Count].Y = nY;
		if (tp->Height == HEIGHT_UNTIL_END)
			Pozitions[Count].Height = HEIGHT_UNTIL_END;
		else
			Pozitions[Count].Height = (tp->Height + tp->Y) - nY;
		if (Pozitions[Count].Height >= MinHeight)
		{
			Last->Next = &Pozitions[Count];
			Pozitions[Count].Prec = Last;
			Pozitions[Count].Next = NULL;
			Last = &Pozitions[Count];
			//LOG_INFO("Adding new rect (M2) ID=%5d: (%d,%d - Height=%d) - TP:(%d,%d - Height=%d)", Count, Pozitions[Count].X, Pozitions[Count].Y, Pozitions[Count].Height,tp->X,tp->Y,tp->Height);
			Count++;
		}
	}
	// daca am ceva in tp - il sterg ca si not utilizat
	if (tp->Prec != NULL)
		tp->Prec->Next = tp->Next;
	if (tp->Next != NULL)
		tp->Next->Prec = tp->Prec;
	if (tp == Start)
		Start = tp->Next;
	if (tp == Last)
		Last = tp->Prec;
	return tp;
}

//=============================================================================================================================

void ClearTextureData(GApp::Resources::TextureData *td,unsigned int count)
{
	for (unsigned int tr=0;tr<count;tr++)
	{
		td[tr].ID = INVALID_TEXTURE_ID;
		td[tr].Width = td[tr].Height = 0;
	}
}
#ifdef SAVE_TEXTURES
static int last_texture_id = 0;
void SaveTexture(GApp::Graphics::Renderer *r)
{
	char temp[2048];
	memset(temp,0,2048);
	GetModuleFileName(NULL,temp,2000);
	int last = -1;
	for (int tr=0;temp[tr]!=0;tr++)
		if (temp[tr]=='\\')
			last = tr;
	last+=1;
	sprintf(&temp[last],"texture_%d.tex",last_texture_id);
	LOG_INFO("Saving texture to: %s",temp);
	last_texture_id++;
	HANDLE hFile = CreateFile(temp,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	DWORD nrWrite;
	unsigned int w,h;
	w = r->GetWidth();
	h = r->GetHeight();

	if (hFile!=INVALID_HANDLE_VALUE)
	{
		WriteFile(hFile,"GTEX",4,&nrWrite,NULL);
		WriteFile(hFile,&w,4,&nrWrite,NULL);
		WriteFile(hFile,&h,4,&nrWrite,NULL);
		WriteFile(hFile,r->GetPixelBuffer(),w*h*4,&nrWrite,NULL);
		CloseHandle(hFile);
	}
}
#endif
bool Bigger(GApp::Resources::GenericResource* r1,GApp::Resources::GenericResource* r2)
{
	// imaginile in fata
	if (((r1->Flags & GAC_RESOURCEFLAG_IMAGE)!=0) && ((r2->Flags & GAC_RESOURCEFLAG_IMAGE)==0))
		return false;
	if (((r1->Flags & GAC_RESOURCEFLAG_IMAGE)==0) && ((r2->Flags & GAC_RESOURCEFLAG_IMAGE)!=0))
		return true;
	if (((r1->Flags & GAC_RESOURCEFLAG_IMAGE)!=0) && ((r2->Flags & GAC_RESOURCEFLAG_IMAGE)!=0))
	{
		if ((r1->IsLoaded()==false) && (r2->IsLoaded()))
			return false;
		if ((r1->IsLoaded()) && (r2->IsLoaded()==false))
			return true;
		if (((GApp::Resources::Bitmap*)r1)->Height<((GApp::Resources::Bitmap*)r2)->Height)
			return true;
	}

	return false;
}
GApp::Resources::Profile::Profile()
{
	CoreContext = NULL;
	Resources = NULL;
	ResourcesCount = 0;
	State = PROFILE_STATE_NOT_LOADED;
	CLEAR_TEXTURE_DATA;
#if defined(ENABLE_INFO_LOGGING) || defined(ENABLE_ERROR_LOGGING)
	Name=NULL;
#endif
}

bool GApp::Resources::Profile::Create(CoreReference coreContext,unsigned char profileType,unsigned int nrResources)
{
	CLEAR_TEXTURE_DATA;
	CHECK(coreContext!=NULL,false,"Invalid (NULL) core object for profile %s",Name);
	CHECK(nrResources>0,false,"Profile %s should contain at least one resource !",Name);
	CHECK((Resources=new GenericResource*[nrResources])!=NULL,false,"Unable to allocate resources indexes (%d elements) for profile %s",nrResources,Name);
	ResourcesCount = nrResources;
	for (unsigned int tr=0;tr<ResourcesCount;tr++)
		Resources[tr]=NULL;
	
	CoreContext = coreContext;
	ProfileType = profileType;
	State = PROFILE_STATE_NOT_LOADED;
	return true;
}
bool GApp::Resources::Profile::CreateTexture(unsigned int ImageCount,bool RunsOnUIThread)
{
	if (ImageCount==0)
	{
		LOG_INFO("No images for this profile %s!",Name);
		return true; // nu am nici o imagine - all good
	}

	unsigned int tr,gr,texSize,texImages,bmp_Width,bmp_Height;
	GApp::Graphics::Renderer rImg,rTex;
	Bitmap *bmp;
	TexturePoz* tp;
	float *Coordonates;
	//unsigned int px = 0;
	//unsigned int py = 0;	
	unsigned int maxTextureSize = ((AppContextData*)API.Context)->MaxTextureSize;	
	unsigned int nrTextures = sizeof(Textures)/sizeof(Textures[0]);
	
	unsigned int minHeight = 0xFFFFFFFF;
	// calculez dimensiunea minima
	for (tr = 0; tr < ImageCount;tr++)
	{
		bmp = (Bitmap*)Resources[tr];
		ComputeBitmapSize(bmp,bmp_Width,bmp_Height);
		minHeight = GAC_MIN(bmp_Height, minHeight);
	}
	LOG_INFO("Create textures (Minim height for one image is %d pixels from %d Images)", minHeight,ImageCount);
#ifdef PLATFORM_DEVELOP
	if (((GApp::DevelopMode::DevelopModeOSContext *)(((AppContextData *)(API.Context))->OS))->SpecificData.MaxTextureSize >= 64)
	{
		LOG_INFO("Using texture size: %d", ((GApp::DevelopMode::DevelopModeOSContext *)(((AppContextData *)(API.Context))->OS))->SpecificData.MaxTextureSize);
		maxTextureSize = ((GApp::DevelopMode::DevelopModeOSContext *)(((AppContextData *)(API.Context))->OS))->SpecificData.MaxTextureSize;
	}
#endif	

	unsigned int currentTexture = 0;
	unsigned int LastWidth = 0;
	Textures[currentTexture].ID = INVALID_TEXTURE_ID;
	atlas.Clear(minHeight, maxTextureSize);

	while (true)
	{
		unsigned int missed = 0;
		for (tr = 0; tr < ImageCount; tr++)
		{
			bmp = (Bitmap*)Resources[tr];
			if (bmp->TextureID != INVALID_TEXTURE_ID)
				continue;
			ComputeBitmapSize(bmp, bmp_Width, bmp_Height);
			tp = atlas.Add(bmp_Width, bmp_Height, ATLAS_METHOD_BESTFIT);
			if (tp != NULL)
			{				
				bmp->TextureLeft = tp->X;
				bmp->TextureTop = tp->Y;
				bmp->TextureID = currentTexture;
				LastWidth = atlas.GetWidth();
				LOG_INFO("* Adding image %s to texture #%d [%dx%d] - ImageSize: %dx%d to position (%d,%d)", bmp->Name, currentTexture + 1, atlas.GetWidth(), atlas.GetHeight(), bmp_Width, bmp_Height, bmp->TextureLeft, bmp->TextureTop);
			}
			else {
				LOG_INFO("Unable to find a solution for Bitmap #%d (%s) (%d x %d)", tr, bmp->Name,bmp_Width, bmp_Height);
				CHECK(atlas.HasError() == false, false, "Unable to create atlas !");
				if (atlas.Grow())
				{
					tp = atlas.Add(bmp_Width, bmp_Height, ATLAS_METHOD_BESTFIT);
					if (tp != NULL)
					{
						bmp->TextureLeft = tp->X;
						bmp->TextureTop = tp->Y;
						bmp->TextureID = currentTexture;
						LastWidth = atlas.GetWidth();
						//LOG_INFO("* Adding image %s to texture #%d [%dx%d] - ImageSize: %dx%d to position (%d,%d)", bmp->Name, currentTexture + 1, atlas.GetWidth(), atlas.GetHeight(), bmp_Width, bmp_Height, bmp->TextureLeft, bmp->TextureTop);
					}
					else {
						atlas.Ungrow();
						missed++;
					}
				}
				else {
					missed++;
				}
			}
		}
		if (missed == 0)
		{
			if (LastWidth > 0)
			{
				Textures[currentTexture].Width = LastWidth;
				Textures[currentTexture].Height = atlas.GetHeight();
				Textures[currentTexture].ID = currentTexture;
				currentTexture++;
				LOG_INFO("Finishing current atlas (%d x %d)", LastWidth, atlas.GetHeight());
			}
			break;
		}
		// crestem atlasul
		LOG_INFO("Atlas should grow (%d images were not added)", missed);
		if (atlas.Grow() == false)
		{
			Textures[currentTexture].Width = LastWidth;
			Textures[currentTexture].Height = atlas.GetHeight();
			Textures[currentTexture].ID = currentTexture;
			LOG_INFO("Finishing current atlas (%d x %d)", LastWidth, atlas.GetHeight());
			currentTexture++;
			CHECK(currentTexture<nrTextures, false, "Too many textures - maximum expected is %d in profile %s (Only %d images were added from %d available for this profile)", nrTextures, Name, tr + 1, ImageCount);
			Textures[currentTexture].ID = INVALID_TEXTURE_ID;
			atlas.Clear(minHeight, maxTextureSize);
			LastWidth = 0;
		}
	}
	/*
	for (tr = 0; tr < ImageCount; tr++)
	{
		bmp = (Bitmap*)Resources[tr];
		ComputeBitmapSize(bmp,bmp_Width,bmp_Height);
		do
		{
			tp = atlas.Add(bmp_Width, bmp_Height, ATLAS_METHOD_BESTFIT);
			if (tp == NULL)
			{
				CHECK(atlas.HasError() == false, false, "Unable to create atlas !");
				if (atlas.Grow() == false)
				{
					LOG_INFO("Unable to find space for image (%s) - %d x %d", bmp->Name, bmp_Width, bmp_Height);
					Textures[currentTexture].Width = LastWidth;
					Textures[currentTexture].Height = atlas.GetHeight();
					Textures[currentTexture].ID = currentTexture;
					LOG_INFO("Finishing current atlas (%d x %d)", LastWidth, atlas.GetHeight());
					currentTexture++;
					CHECK(currentTexture<nrTextures, false, "Too many textures - maximum expected is %d in profile %s (Only %d images were added from %d available for this profile)", nrTextures, Name, tr + 1, ImageCount);
					Textures[currentTexture].ID = INVALID_TEXTURE_ID;
					atlas.Clear(minHeight, maxTextureSize);
					LastWidth = 0;
				}
			}
		} while (tp == NULL);
		bmp->TextureLeft = tp->X;
		bmp->TextureTop = tp->Y;
		bmp->TextureID = currentTexture;
		LastWidth = atlas.GetWidth();
		LOG_INFO("* Adding image %s to texture #%d [%dx%d] - ImageSize: %dx%d to position (%d,%d)", bmp->Name, currentTexture + 1, atlas.GetWidth(), atlas.GetHeight(), bmp_Width, bmp_Height, bmp->TextureLeft, bmp->TextureTop);
	}
	if (LastWidth > 0)
	{
		Textures[currentTexture].Width = LastWidth;
		Textures[currentTexture].Height = atlas.GetHeight();
		Textures[currentTexture].ID = currentTexture;
		currentTexture++;
		LOG_INFO("Finishing current atlas (%d x %d)", LastWidth, atlas.GetHeight());
	}
	*/

	/*
	tr = 0;
	while (tr<ImageCount)
	{
		bmp = (Bitmap*)Resources[tr];
		ComputeBitmapSize(bmp,bmp_Width,bmp_Height);
		if (Textures[currentTexture].ID == INVALID_TEXTURE_ID)
		{
			Textures[currentTexture].ID = currentTexture;
			Textures[currentTexture].Height = Power2(bmp_Height);
			Textures[currentTexture].Width = Power2(bmp_Width);
			CHECK((Textures[currentTexture].Width<=maxTextureSize)&&(Textures[currentTexture].Height<=maxTextureSize),false,"One image is too large (%dx%d) to fit in a texture of maximal size of %dx%d int profile %s!",bmp_Width,bmp_Height,maxTextureSize,maxTextureSize,Name);
			px = 0;
		}
		if (bmp_Width+px>Textures[currentTexture].Width)
		{
			// calculez noul width
			unsigned int newWidth = Power2(px+bmp_Width);
			if (newWidth>maxTextureSize)
			{
				// trec la o noua textura
				currentTexture++;
				CHECK(currentTexture<nrTextures,false,"Too many textures - maximum expected is %d in profile %s (Only %d images were added from %d available for this profile)",nrTextures,Name,tr+1,ImageCount);
				Textures[currentTexture].ID = INVALID_TEXTURE_ID;
				continue;
			} else
			{
				Textures[currentTexture].Width = newWidth;
			}			
		}
		bmp->TextureLeft = px;
		bmp->TextureTop = 0;
		bmp->TextureID = currentTexture;
		LOG_INFO("* Adding image %s to texture #%d [%dx%d] - ImageSize: %dx%d to position (%d,%d)", bmp->Name,currentTexture+1, Textures[currentTexture].Width, Textures[currentTexture].Height, bmp_Width, bmp_Height,bmp->TextureLeft,bmp->TextureTop);
		px+=bmp_Width+1;
		tr++;
	}
	currentTexture++;
	//*/
	// in currentTexture am numarul de texturi
	// ma apuc si le incarc pe toate
	LOG_INFO("Total textures in profile %s: %d ",Name,currentTexture);
	LOG_INFO("Total images   in profile %s: %d ",Name,ImageCount);
	for (gr=0;gr<currentTexture;gr++)
	{		
		CHECK(Core.TempBufferBig.Resize(Textures[gr].Width*Textures[gr].Height*4),false,"");
		CHECK(rImg.Create(&Core.TempBufferMedium),false,"");
		CHECK(rTex.Create(&Core.TempBufferBig,Textures[gr].Width,Textures[gr].Height),false,"");
		rTex.Clear();
		texSize = 0;
		texImages = 0;
		for (tr=0;tr<ImageCount;tr++)
		{
			bmp = (Bitmap*)Resources[tr];	
			if ((bmp->TextureID != gr) || (bmp->IsLoaded()))
				continue;
			ComputeBitmapSize(bmp, bmp_Width, bmp_Height);
			CHECK(bmp->CreateRenderer(this->CoreContext,&rImg,&Core.TempBufferSmall),false,"");
			CHECK(rTex.DrawImage(&rImg,bmp->TextureLeft,bmp->TextureTop,bmp_Width,bmp_Height),false,"");
			// leg si bitmapul
			Coordonates = bmp->Coordonates;

			Coordonates[0] = Coordonates[2] = Coordonates[6] = ((float)bmp->TextureLeft)/((float)Textures[gr].Width);
			Coordonates[1] = Coordonates[7] = Coordonates[11] = 1.0f-((float)(bmp->TextureTop+bmp_Height))/((float)Textures[gr].Height);
			Coordonates[3] = Coordonates[5] = Coordonates[9] = 1.0f-((float)bmp->TextureTop)/((float)Textures[gr].Height);
			Coordonates[4] = Coordonates[8] = Coordonates[10] = ((float)(bmp->TextureLeft+bmp_Width))/((float)Textures[gr].Width);

			Coordonates[12] = (bmp_Width)*2.0f/((float)(Core.Width));
			Coordonates[13] = (bmp_Height)*2.0f/((float)(Core.Height));	
			texImages++;
			texSize+=bmp_Width*bmp_Height;
			Core.ProgressValue++;
		}

		LOG_INFO("* Creating texture for profile %s: %d/%d [%d x %d], Images:%d, ImageSize=%d [%.2f%%]",Name,(gr+1),currentTexture,Textures[gr].Width,Textures[gr].Height,texImages,texSize,((float)texSize*100)/(Textures[gr].Width*Textures[gr].Height));
		// aloc si urc textura
		Core.ThreadRequest.Create();
		Core.ThreadRequest.Values[0].PtrValue = this;
		Core.ThreadRequest.Values[1].UInt32Value = gr;			
		Core.ThreadRequest.Values[2].UInt32Value = ImageCount;	
		Core.ThreadRequest.SendCommand(GAC_THREADCMD_PROFILE_LOADTEXTURES);
		LOG_INFO("Starting thread execution with command: %d", Core.ThreadRequest.Command);
		if (RunsOnUIThread)
		{
			LoadTextureToGraphicCard(Core.ThreadRequest);
		}
		Core.ThreadRequest.Wait();
		CHECK(Core.ThreadRequest.Result.BoolValue,false,"Loading texture %d failed in profile %s!",gr,Name);
	}
	LOG_INFO("All texture created in profile %s!",Name);
	// all good
	return true;
}
bool GApp::Resources::Profile::Load(bool RunsOnUIThread, bool onlyIfMarkedForDelayLoad)
{
	unsigned int tr,lastPoz,curentLastPoz;
	GenericResource* r;
	
	if (((onlyIfMarkedForDelayLoad) && (State != PROFILE_STATE_DELAY_LOAD)) || (State == PROFILE_STATE_LOADED))
	{
		RETURNERROR(true, "Profile %s will not be loaded (State=%d,onlyIfMarkedForDelayLoad=%d)", Name, State, onlyIfMarkedForDelayLoad);
	}


	LOG_INFO("----------------------------- Loading profile: %s (%d resources) -----------------------------",Name,ResourcesCount);

	CLEAR_TEXTURE_DATA;
	CHECK(CoreContext!=NULL,false,"Invalid (NULL) core object in profile %s",Name);
	CHECK(ResourcesCount>0,false,"At least one resource should be added to profile %s !",Name);
	for (tr=0;tr<ResourcesCount;tr++)
	{
		r = Resources[tr];
		CHECK(r!=NULL,false,"Resource %s was not set in profile %s",r->Name,Name);
		CHECK(r->Flags!=GAC_RESOURCEFLAG_NONE,false,"Resource %s was not defined in profile %s",r->Name,Name);
		if ((onlyIfMarkedForDelayLoad) && (r->ProfileContext == this))
			r->ProfileContext = NULL; // daca e doar pentru cele marcate, atunci curat contextul
	}

	LOG_INFO("Sorting resources ...");
	// sortez
	bool sorted = false;
	lastPoz = ResourcesCount-1;
	while (!sorted)
	{
		curentLastPoz = lastPoz;
		sorted = true;
		for (tr=0;tr<curentLastPoz;tr++)
		{
			if (Bigger(Resources[tr],Resources[tr+1]))			
			{
				r = Resources[tr];
				Resources[tr] = Resources[tr+1];
				Resources[tr+1] = r;
				lastPoz = tr;
				sorted = false;
			}
		}
	}
	// cel mai simplu encoding - una dupa alta
	// dupa sortare avem doar imaginile neincarcate in fata
	// numar cate imagini am
	unsigned int ImageCount = 0;
	for (; (ImageCount < ResourcesCount) && (Resources[ImageCount]->IsLoaded() == false) && ((Resources[ImageCount]->Flags & GAC_RESOURCEFLAG_IMAGE) != 0); ImageCount++) {};
	// in ImageCount am exact cate imagini trebuie sa pun si unde
	LOG_INFO("Images for texture: %d", ImageCount);

	CHECK(CreateTexture(ImageCount,RunsOnUIThread),false,"Unable to create textures for profile %s",Name);
	
	LOG_INFO("Loading non-graphical resources ...");
	// incarc si celelalte resurse
	for (tr=0;tr<ResourcesCount;tr++)
	{
		r = Resources[tr];
		if (r->IsLoaded())
			continue;
		if ((r->Flags & GAC_RESOURCEFLAG_IMAGE)!=0)
			continue;
		Core.ProgressValue++;
		// incarc resursa
		if ((r->Flags & GAC_RESOURCEFLAG_SHADER)!=0)
		{
			if (RunsOnUIThread)
			{
				CHECK(((Shader*)r)->Load(),false,"Unable to load shader %s in profile %s !",r->Name,Name);
			} else {
				Core.ThreadRequest.Create();
				Core.ThreadRequest.Values[0].PtrValue = r;
				LOG_INFO("Sending Shader pointer: %p (trq = %p) Context=%p,Pos=%d,Size=%d", r,&(Core.ThreadRequest),r->CoreContext,r->Position,r->Size);
				Core.ThreadRequest.SendCommand(GAC_THREADCMD_PROFILE_LOADSHADER);
				Core.ThreadRequest.Wait();				
				CHECK(Core.ThreadRequest.Result.BoolValue,false,"Loading shader %s failed in profile %s !",r->Name,Name);
			}
		}		
		if ((r->Flags & GAC_RESOURCEFLAG_SOUND)!=0)
		{
			if (RunsOnUIThread)
			{
				CHECK(((Sound*)r)->Load(), false, "Faild to load sound %s in profile %s ", r->Name, Name);
			}
			else {
				Core.ThreadRequest.Create();
				Core.ThreadRequest.Values[0].PtrValue = r;				
				Core.ThreadRequest.SendCommand(GAC_THREADCMD_PROFILE_LOADSOUND);
				Core.ThreadRequest.Wait();
				CHECK(Core.ThreadRequest.Result.BoolValue, false, "Loading sound %s failed in profile %s !", r->Name, Name);
			}			
		}
		if ((r->Flags & GAC_RESOURCEFLAG_ANIMATION)!=0)
		{
			CHECK(((Presentation*)r)->Load(), false, "Faild to load animation %s in profile %s ", r->Name, Name);
		}
		if ((r->Flags & GAC_RESOURCEFLAG_RAW)!=0)
		{
			CHECK(((RawResource*)r)->Load(),false,"Faild to load raw resource %s in profile %s ",r->Name,Name);
		}
		r->ProfileContext = this;
	}
	LOG_INFO("All ok - profile is loaded !");
	//
	#ifdef SAVE_TEXTURES
	SaveTexture(&rTex);
	#endif
	// textura a fost creata o urc
	State = PROFILE_STATE_LOADED;
#ifdef PLATFORM_DEVELOP
	unsigned int maxTexturesIDs = sizeof(Textures) / sizeof(Textures[0]);
	unsigned int countTextures = 0;
	for (tr = 0; tr < maxTexturesIDs; tr++)
	{
		if (Textures[tr].ID != INVALID_TEXTURE_ID)
			countTextures++;
	}
	DEBUGMSG("ProfileLoad|%s|%d|%d", Name,ResourcesCount,countTextures);
#endif
	return true;
}
bool GApp::Resources::Profile::LoadTextureToGraphicCard(GApp::Utils::ThreadSyncRequest &tsr)
{
	// 0 - this
	// 1 - id-ul texturii	
	// 2 - Imagini
	unsigned int nrTexturesIDs = sizeof(Textures)/sizeof(Textures[0]);	
	TextureData *td = NULL;
	GLuint textureID = INVALID_TEXTURE_ID;
	while (true)
	{
		unsigned int LastError = 0;		
		CHECKBK(tsr.Values[1].UInt32Value<nrTexturesIDs,"");
		td = &Textures[tsr.Values[1].UInt32Value];

		CHECK_GLERROR_BK(glGenTextures(1,&textureID));
		LOG_INFO("LoadToGraphicCard(this=%p,TextID=%d,OGL_ID=%d)", this, tsr.Values[1].UInt32Value, textureID);
		// ar trebuisa leg pe rand fiecare textura si apoi o adaug
		CHECK_GLERROR_BK(glActiveTexture(GL_TEXTURE0));
		CHECK_GLERROR_BK(glBindTexture(GL_TEXTURE_2D,textureID));
		
		CHECK_GLERROR_BK(glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,td->Width,td->Height,0,GL_RGBA,GL_UNSIGNED_BYTE,Core.TempBufferBig.GetBuffer()));
		// all good - leg obiectele de profile
		td->ID = textureID;
#ifdef PLATFORM_DEVELOP
		unsigned int imageCount = 0;
#endif
		for (unsigned int tr=0;tr<tsr.Values[2].UInt32Value;tr++)
		{
			if (Resources[tr]->IsLoaded())
				continue;
			if (((Bitmap *)Resources[tr])->TextureID == tsr.Values[1].UInt32Value)
			{
				Resources[tr]->ProfileContext = this;
				((Bitmap *)Resources[tr])->TextureID = textureID;
#ifdef PLATFORM_DEVELOP
				imageCount++;
#endif
			}
		}
		tsr.Result.BoolValue = true;
		tsr.Finish();
		DEBUGMSG("TextureLoad|%d|%d|%d|%d", td->ID, td->Width, td->Height,imageCount);
		LOG_INFO("Texture Loaded(ID=%d, Size=%d x %d), Address = %p, Result=%d", td->ID, td->Width, td->Height, &(tsr.Result.BoolValue), tsr.Result.BoolValue);
		return true;
	}
	if (textureID!=INVALID_TEXTURE_ID)
	{
		glDeleteTextures(1,&textureID);
	}
	if (td != NULL)
		td->ID = INVALID_TEXTURE_ID;
	tsr.Result.BoolValue = false;
	tsr.Finish();
	RETURNERROR(false, "Fail to load texture !!!");
}
bool GApp::Resources::Profile::MarkForDelayLoad()
{
	if (State == PROFILE_STATE_LOADED)
	{
		State = PROFILE_STATE_DELAY_LOAD;
		DEBUGMSG("ProfileDelayLoad|%s", Name);
		return true;
	}
	return false;
}
bool GApp::Resources::Profile::Unload(bool RunsOnUIThread, bool markForDelayLoad)
{	
	GenericResource* r;
	// daca nu e incarcat - nu are sens sa mai pierd timpul
	if (State != PROFILE_STATE_LOADED)
	{
		State = PROFILE_STATE_NOT_LOADED;
		DEBUGMSG("ProfileUnLoad|%s", Name);
		return true;
	}
		
	LOG_INFO("============================= Unload profile: %s (Delay=%d) =============================", Name, markForDelayLoad);
	CHECK(CoreContext!=NULL,false,"Invalid (NULL) core object in profile %s",Name);
	CHECK(ResourcesCount>0,false,"At least one resource should be added to this %s profile !",Name);
	for (unsigned int tr=0;tr<ResourcesCount;tr++)
	{
		r = Resources[tr];
		if (r->ProfileContext!=this)
			continue; // este in alt profil - nu o dau jos
		if ((r->Flags & GAC_RESOURCEFLAG_SHADER)!=0)
		{
			if (RunsOnUIThread)
			{
				CHECK(((Shader*)r)->Unload(),false,"Unable to unload shader %s from profile %s!",r->Name,Name);
			} else {
				Core.ThreadRequest.Create();
				Core.ThreadRequest.Values[0].PtrValue = r;
				Core.ThreadRequest.SendCommand(GAC_THREADCMD_PROFILE_UNLOADSHADER);
				Core.ThreadRequest.Wait();
				CHECK(Core.ThreadRequest.Result.BoolValue,false,"Unloading shader %s failed in profile %s !",r->Name,Name);
			}
		}
		if ((r->Flags & GAC_RESOURCEFLAG_SOUND)!=0)
		{
			if (RunsOnUIThread)
			{
				CHECK(((Sound*)r)->Unload(), false, "Faild to unload sound %s from profile %s", r->Name, Name);
			}
			else {
				Core.ThreadRequest.Create();
				Core.ThreadRequest.Values[0].PtrValue = r;
				Core.ThreadRequest.SendCommand(GAC_THREADCMD_PROFILE_UNLOADSSOUND);
				Core.ThreadRequest.Wait();
				CHECK(Core.ThreadRequest.Result.BoolValue, false, "Unloading Sound %s failed in profile %s !", r->Name, Name);
			}
		}
		if ((r->Flags & GAC_RESOURCEFLAG_ANIMATION)!=0)
		{
			CHECK(((Presentation*)r)->Unload(), false, "Faild to unload animation %s from profile %s", r->Name, Name);
		}
		if ((r->Flags & GAC_RESOURCEFLAG_RAW)!=0)
		{
			CHECK(((RawResource*)r)->Unload(),false,"Faild to unload raw resource %s from profile %s",r->Name,Name);
		}
		if ((r->Flags & GAC_RESOURCEFLAG_IMAGE) != 0)
		{
			((Bitmap*)r)->TextureID = INVALID_TEXTURE_ID;
		}
		LOG_INFO("Releasing resource: %s", r->Name);
		r->ProfileContext = NULL;		
	}
	// ar trebui sa dau jos si texturile alocate - trebuie facut algoritm pentru alocarea lor
	unsigned int maxTexturesIDs = sizeof(Textures)/sizeof(Textures[0]);	
	for (unsigned int tr = 0; tr<maxTexturesIDs; tr++)
	{
		if (Textures[tr].ID!=INVALID_TEXTURE_ID)
		{
			DEBUGMSG("TextureUnload|%d", Textures[tr].ID);
			if (RunsOnUIThread)
			{
				glDeleteTextures(1,&Textures[tr].ID);
			} else {
				Core.ThreadRequest.Create();
				Core.ThreadRequest.Values[0].UInt32Value = Textures[tr].ID;
				Core.ThreadRequest.SendCommand(GAC_THREADCMD_PROFILE_UNLOADTEXTURES);
				Core.ThreadRequest.Wait();
			}
		}
	}
	CLEAR_TEXTURE_DATA;
	if (markForDelayLoad)
	{
		State = PROFILE_STATE_DELAY_LOAD;
		DEBUGMSG("ProfileDelayLoad|%s", Name);
	}
	else
	{
		State = PROFILE_STATE_NOT_LOADED;
		DEBUGMSG("ProfileUnLoad|%s", Name);
	}
	LOG_INFO("State for %s is %d", Name, State);
	return true;
}
/*
bool GApp::Resources::Profile::Clear_()
{
	Unload();
	if (Resources!=NULL)
		delete Resources;
	Resources = NULL;
	ResourcesCount = 0;
	return true;
}
*/
bool GApp::Resources::Profile::IsLoaded()
{
	return (State == PROFILE_STATE_LOADED);
}
bool GApp::Resources::Profile::IsDelayedProfile()
{
	return (State == PROFILE_STATE_DELAY_LOAD);
}
bool GApp::Resources::Profile::SetResource(unsigned int index,GApp::Resources::GenericResource *res)
{
	CHECK(res!=NULL,false,"Expecting an valid resource (not a NULL object) in profile %s",Name);
	CHECK(index<ResourcesCount,false,"Invalid index (%d) for resource %s (should be less than %d) in profile %s",index,res->Name,ResourcesCount,Name);	
	Resources[index] = res;
	return true;
}
