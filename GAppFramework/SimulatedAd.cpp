#include "DevelopMode.h"

using namespace GApp::DevelopMode;
struct LocalImages
{
	int x, y, w, h;
};
LocalImages ads_images[] = {
	{8,4,360,64},
	{8,72,180,100},
	{8,176,180,140},
	{8,400,280,100},
	{192,76,240,240},
	{300,320,100,180},
	{440,20,64,360},
};
LocalImages close_images[] = {
	{8,324,64,64},
	{ 128, 324, 64, 64 },
};


SimulatedAds::SimulatedAds()
{
	Count = 0;
	flashIndex = 0;
	addValue = 4;
	App = NULL; 
	selectedAd = -1;
	closeButtonSelected = false;
	EnableAdService = true;
}
bool SimulatedAds::Init(GApp::UI::Application *app)
{
	App = app;

	unsigned int pictureSize;
	const void * picture = Execution::GetSimulatedAdsImages(pictureSize);

	CHECK(this->bmpImages.Create(picture, pictureSize), false, "");
#if defined(ENABLE_INFO_LOGGING) || defined(ENABLE_ERROR_LOGGING)
	this->profile.Name = "Ads Images";
#endif
	CHECK(this->profile.Create(app->CoreContext, GAC_PROFILE_GLOBAL, 1), false, "");
	CHECK(this->profile.SetResource(0, &this->bmpImages), false, "");
	CHECK(this->profile.Load(true), false, "");

	return true;
}
bool SimulatedAds::Add(unsigned int deviceWidth, unsigned int deviceHeight, const char *Name, float x, float y, float width, float height, bool loadOnStartup, int adType, bool reloadAfterOpen, int maxAttemptsOnFail)
{
	CHECK(Count < MAX_SIMULATED_ADS, false, "Too many ads objctes. Max allowd is :%d", MAX_SIMULATED_ADS);

	int lenName = GApp::Utils::String::Len(Name);
	if (lenName >= (MAX_AD_NAME - 1))
		lenName = MAX_AD_NAME - 1;

	SimAd *ad = &Ads[Count];
	ad->Name[0] = 0;
	GApp::Utils::String::Set(ad->Name, Name, MAX_AD_NAME, lenName);
	ad->w = (int)(deviceWidth*width);
	ad->h = (int)(deviceHeight*height);
	ad->x = (int)(deviceWidth*x);
	ad->y = (int)(deviceHeight*y);
	ad->Visible = false; //Daca il incarcam la startup il consideram by default visible
	ad->Loaded = false;
	ad->TimeLeftUntilLoaded = 0;
	ad->adType = adType;
	ad->MaxAttemptsOnFail = maxAttemptsOnFail;
	ad->ReloadAfterOpen = reloadAfterOpen;
	ad->FailedAttempts = 0;

	int bestScore = -1;
	int bestIndex = -1;
	float bestScale = 1.0f;
	// caut cea mai buna imagine
	for (int tr = 0; tr < (sizeof(ads_images) / sizeof(ads_images[0])); tr++)
	{
		int scor = 0;
		float scale = 1.0f;
		if ((ads_images[tr].w <= ad->w) && (ads_images[tr].h <= ad->h))
		{
			// incape sigur - compar procentul de umplere (in unitati de 1000)
			scor = (ads_images[tr].h*ads_images[tr].w * 1000) / (ad->w*ad->h);
			scor += 100000;
		}
		else {
			float rap1 = (float)(ads_images[tr].w) / (float)ad->w;
			float rap2 = (float)(ads_images[tr].h) / (float)ad->h;
			if (rap2 > rap1)
				rap1 = rap2;
			scor = (int)((ads_images[tr].h*ads_images[tr].w * 1000) / (ad->w*ad->h*rap1*rap1));
			scale = rap1;
		}
		if (scor > bestScore)
		{
			bestScore = scor;
			bestIndex = tr;
			bestScale = scale;
		}
	}
	if (bestIndex >= 0)
	{
		ad->img_x = ad->x + ad->w / 2 - (ads_images[bestIndex].w/bestScale) / 2.0f;
		ad->img_y = ad->y + ad->h / 2 - (ads_images[bestIndex].h/bestScale) / 2.0f;
		ad->img_w = ads_images[bestIndex].w/bestScale;
		ad->img_h = ads_images[bestIndex].h/bestScale;
		ad->imageIndex = bestIndex;

		LOG_INFO("Ad[%d] = [x=%d,y=%d,w=%d,h=%d] - Image:%d (%d x %d) Scale: %f", Count, ad->x, ad->y, ad->w, ad->h, bestIndex, ads_images[bestIndex].w, ads_images[bestIndex].h, bestScale);
	}
	else {
		ad->img_x = ad->img_y = 0;
		ad->img_w = ad->img_h = 0;
		ad->imageIndex = -1;
		LOG_ERROR("Failed to find an image that fits tha surface of the ad for Ad[%d] = [x=%d,y=%d,w=%d,h=%d] ", Count, ad->x, ad->y, ad->w, ad->h);
	}
	
	Count++;
	if (loadOnStartup)
		Load(Count - 1);
	return true;
}
bool SimulatedAds::Load(int index)
{
	CHECK((index >= 0) && (index < (int)Count), false, "Invalid ad index(%d) - it should be between [0..%d)", index, Count);
	if (Ads[index].TimeLeftUntilLoaded <= 0)
	{
		Ads[index].TimeLeftUntilLoaded = rand() % ((50 + 1) - 10) + 10;
	}
	return true;
}
bool SimulatedAds::UnLoad(int index)
{
	CHECK((index >= 0) && (index < (int)Count), false, "Invalid ad index(%d) - it should be between [0..%d)", index, Count);
	if (Ads[index].Visible)
		App->ProcessSystemEvent(SYSEVENT_ADVERTISING, index, GAC_ADEVENT_HIDDEN, 0, 0);
	if (Ads[index].Loaded)
		App->ProcessSystemEvent(SYSEVENT_ADVERTISING, index, GAC_ADEVENT_UNLOAD, 0, 0);
	Ads[index].Visible = false;
	Ads[index].Loaded = false;
	Ads[index].TimeLeftUntilLoaded = 0;
	return true;
}
void SimulatedAds::Update()
{
	SimAd *ad = &Ads[0];
	for (unsigned int tr = 0; tr < Count; tr++, ad++)
	{
		if (ad->TimeLeftUntilLoaded>0)
		{
			ad->TimeLeftUntilLoaded--;
			if (ad->TimeLeftUntilLoaded == 0)
			{
				if (EnableAdService)
				{
					ad->Loaded = true;
					ad->FailedAttempts = 0;
					App->ProcessSystemEvent(SYSEVENT_ADVERTISING, tr, GAC_ADEVENT_LOADED, 0, 0);
				}
				else {
					LOG_ERROR("Ad service is unavailable - failed to load ad: '%s'", ad->Name);
					ad->FailedAttempts++;
					if (ad->FailedAttempts <= ad->MaxAttemptsOnFail)
					{
						LOG_INFO("Retry to load ad '%s': Retry no: %d from %d", ad->Name,ad->FailedAttempts,ad->MaxAttemptsOnFail);
						App->ProcessSystemEvent(SYSEVENT_ADVERTISING, tr, GAC_ADEVENT_FAILTOLOAD, 0, 0);
						Load(tr);
					}
				}
			}
		}
	}

	// flashes
	flashIndex += addValue;
	if (flashIndex>255) {
		flashIndex = 255;
		addValue = -addValue;
	}
	if (flashIndex < 0)
	{
		flashIndex = 0;
		addValue = -addValue;
	}
}
void SimulatedAds::Paint(GApp::Graphics::GraphicsContext &gContext)
{
	if (Count == 0)
		return;
	SimAd *ad = &Ads[0];
	for (unsigned int tr = 0; tr < Count; tr++, ad++)
	{
		if ((ad->Visible == false) || (ad->Loaded == false))
			continue;
		// desenez
		gContext.DrawRect(ad->x, ad->y, ad->x + ad->w, ad->y + ad->h, 0xFFFFFFFF, 0xFFFFFFFF, 1);
		// imaginea
		if (ad->imageIndex >= 0)
			gContext.DrawImageResized(&bmpImages, ads_images[ad->imageIndex].x, ads_images[ad->imageIndex].y, ads_images[ad->imageIndex].w, ads_images[ad->imageIndex].h, ad->img_x, ad->img_y, ad->img_w, ad->img_h);
		// chenar
		if (tr == selectedAd)
			gContext.DrawRect(ad->x, ad->y, ad->x + ad->w, ad->y + ad->h, COLOR_ARGB(0xFF, flashIndex, flashIndex, 0), 0, 8.0f);
		else
			gContext.DrawRect(ad->x, ad->y, ad->x + ad->w, ad->y + ad->h, COLOR_ARGB(flashIndex, 0, 0, 0), 0, 4.0f);
		// desen pentru X-ul de inchidere
		if (ad->adType == SIM_AD_TYPE_INTERSTITIAL)
		{
			if (closeButtonSelected == false)
				gContext.DrawImageResized(&bmpImages, close_images[1].x, close_images[1].y, close_images[1].w, close_images[1].h, ad->x + ad->w - 10 - close_images[1].w, ad->y + 10, close_images[1].w, close_images[1].h);
			else
				gContext.DrawImageResized(&bmpImages, close_images[0].x, close_images[0].y, close_images[0].w, close_images[0].h, ad->x + ad->w - 10 - close_images[0].w, ad->y + 10, close_images[0].w, close_images[0].h);
		}
			

	}
}
bool SimulatedAds::SetVisible(int index, bool value)
{
	CHECK((index >= 0) && (index < Count), false, "Invalid ad index(%d) - it should be between [0..%d)", index, Count);
	if ((Ads[index].Visible != value) && (App != NULL) && (Ads[index].Loaded))
	{
		Ads[index].Visible = value;
		switch (Ads[index].adType)
		{
			case SIM_AD_TYPE_INTERSTITIAL:
				if (value) {
					App->ProcessSystemEvent(SYSEVENT_PAUSE, 0, 0, 0, 0);
					App->ProcessSystemEvent(SYSEVENT_ADVERTISING, index, GAC_ADEVENT_VISIBLE, 0, 0);
				}
				break;
			case SIM_AD_TYPE_REWARDABLE:
				if (value == true) {
					App->ProcessSystemEvent(SYSEVENT_PAUSE, 0, 0, 0, 0);
					App->ProcessSystemEvent(SYSEVENT_ADVERTISING, index, GAC_ADEVENT_VISIBLE, 0, 0);
					int result = MessageBoxA(NULL, "Collect the reward ?", "Rewardable ad simulator", MB_YESNO);
					App->ProcessSystemEvent(SYSEVENT_RESUME, 0, 0, 0, 0);
					if (result == IDYES)
						App->ProcessSystemEvent(SYSEVENT_ADVERTISING, index, GAC_ADEVENT_REWARDED, 0, 0);
					UnLoad(index);
					if (Ads[index].ReloadAfterOpen)
						Load(index);
				}
				break;
			default:
				// banner sau nativ
				if (value)
					App->ProcessSystemEvent(SYSEVENT_ADVERTISING, index, GAC_ADEVENT_VISIBLE, 0, 0);
				else
					App->ProcessSystemEvent(SYSEVENT_ADVERTISING, index, GAC_ADEVENT_HIDDEN, 0, 0);
				break;
		}


	}
	return true;
}
bool SimulatedAds::IsVisible(int index)
{
	CHECK((index >= 0) && (index < Count), false, "Invalid ad index(%d) - it should be between [0..%d)", index, Count);
	return Ads[index].Visible;
}
bool SimulatedAds::IsLoaded(int index)
{
	CHECK((index >= 0) && (index < Count), false, "Invalid ad index(%d) - it should be between [0..%d)", index, Count);
	return Ads[index].Loaded;
}
bool SimulatedAds::OnTouchEvent(int eventType, int x, int y)
{
	//LOG_INFO("SimulatedAds::OnTouchEvent(%d, %d, %d)", eventType, x, y);
	if (eventType == DEVMODE_EVENT_TOUCH_DOWN)
	{
		for (int tr = 0; tr < Count; tr++)
		{
			if ((x >= Ads[tr].x) && (x <= Ads[tr].x + Ads[tr].w) && (y >= Ads[tr].y) && (y <= Ads[tr].y + Ads[tr].h) && (Ads[tr].Visible))
			{
				if (Ads[tr].adType == SIM_AD_TYPE_INTERSTITIAL)
				{
					closeButtonSelected = ((x >= Ads[tr].x + Ads[tr].w - 10 - close_images[0].w) && (x <= Ads[tr].x + Ads[tr].w - 10) && (y >= Ads[tr].y + 10) && (y <= Ads[tr].y + Ads[tr].h + close_images[0].h));
				}
				selectedAd = tr;
				return true;
			}
		}
		return false;
	}
	if ((eventType == DEVMODE_EVENT_TOUCH_UP) && (selectedAd >= 0))
	{
		if (closeButtonSelected==false)
			App->ProcessSystemEvent(SYSEVENT_ADVERTISING, selectedAd, GAC_ADEVENT_OPENAD, 0, 0);
		UnLoad(selectedAd);
		if (Ads[selectedAd].ReloadAfterOpen)
			Load(selectedAd);
		selectedAd = -1;
		closeButtonSelected = false;
		return true;
	}
	// daca e la mine - il procesez eu
	return (selectedAd >= 0);
}
void SimulatedAds::SetAdServiceStatus(bool value)
{
	EnableAdService = value;
	if (EnableAdService == false) {
		LOG_INFO("Ad services are turned off !");
	} 
	else {
		LOG_INFO("Ad services are turned on !");
	}
}