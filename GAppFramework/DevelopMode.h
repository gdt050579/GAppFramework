#ifndef __DEVELOP_MODE_HEADER__
#define __DEVELOP_MODE_HEADER__

#include "GApp.h"
#include "bass.h"


#define MAX_AD_NAME							64
#define MAX_SIMULATED_ADS					16
#define MAX_SOUND_OBJECTS					256

#define PROCESS_DEBUG_MSG_TERMINATE			0
#define PROCESS_DEBUG_MSG_PAUSE				1
#define PROCESS_DEBUG_MSG_RESUME			2
#define PROCESS_DEBUG_MSG_SCALE				3
#define PROCESS_DEBUG_MSG_TAKE_PICTURE		4
#define PROCESS_DEBUG_MSG_GET_SETTINGS		5
#define PROCESS_DEBUG_MSG_SUSPEND			6
#define PROCESS_DEBUG_MSG_CHANGE_SCENE		7
#define PROCESS_DEBUG_MSG_GET_TEXTURE		8
#define PROCESS_DEBUG_MSG_RELOAD_SETTINGS	9
#define PROCESS_DEBUG_MSG_CREATE_SNAPSHOT	10
#define PROCESS_DEBUG_MSG_ENABLE_COUNTER	11
#define PROCESS_DEBUG_MSG_DISABLE_COUNTER	12
#define PROCESS_DEBUG_MSG_RECHECK_ALARMS	13
#define PROCESS_DEBUG_MSG_UPDATE_TIME_DELTA	14

#define SETTINGS_SNAPSHOT_ID_NONE			-1
#define SETTINGS_SNAPSHOT_ID_LAST			0

#define SIM_AD_TYPE_BANNER					0
#define SIM_AD_TYPE_INTERSTITIAL			1
#define SIM_AD_TYPE_REWARDABLE				2
#define SIM_AD_TYPE_NATIVE					3

#define DEVMODE_EVENT_TOUCH_DOWN			1
#define DEVMODE_EVENT_TOUCH_UP				2
#define DEVMODE_EVENT_TOUCH_MOVE			3

#define LOSHORT(x)	(float)((int)((short)((x) & 0xFFFF)))
#define HISHORT(x)	(float)((int)((short)(((x)>>16) & 0xFFFF)))


#define PRINT_GLOBAL_COUNTER(gc) {\
	if (gc->Hash!=0) { \
		DEBUGMSG("GLOBALCOUNTER|%d|%s|%s|%d|%d|%d|%d|%d|1|%d", gc->Index, gc->Name, gc->Group, gc->Value, gc->Interval, gc->Times, gc->MaxTimes, gc->Enabled, gc->ActionsCount); \
	} else { \
		DEBUGMSG("GLOBALCOUNTER|%d|%s|%s|%d|%d|%d|%d|%d|0|%d", gc->Index, gc->Name, gc->Group, gc->Value, gc->Interval, gc->Times, gc->MaxTimes, gc->Enabled, gc->ActionsCount); \
	} \
}


#define PRINT_GLOBAL_COUNTER_GROUP(gc) { DEBUGMSG("GLOBALCOUNTERGROUP|%d|%s|%d|%d|%d|%d", gc->Index, gc->Name, gc->CountTillEnabled, gc->StartTime, gc->MinTimeLimit,  gc->CumulativeTime); /*LOG_INFO("GLOBALCOUNTERGROUP  %d  %s  %d  %d  %d  %d", gc->Index, gc->Name, gc->CountTillEnabled, gc->StartTime, gc->MinTimeLimit,  gc->CumulativeTime);*/ }

namespace GApp
{
	namespace DevelopMode
	{
		struct InitializationData;
		class BassModeWrapper
		{
			HSTREAM			hSound;
			QWORD			Length;
			unsigned char*	Buffer;
		public:
			BassModeWrapper();
			~BassModeWrapper();

			bool		Create(const void* Buffer, unsigned int size);
			void		Close();
			bool		Play();
			bool		Pause();
			bool		SetLoop(bool loop);
			bool		SetVolume(float vol);

			static bool	Init();
			static void Dispose();
		};
		struct SimAd
		{
			char	Name[MAX_AD_NAME];
			int		x, y, w, h;
			int		img_x, img_y, img_w, img_h;
			int		imageIndex;
			int		adType;
			bool	Visible, Loaded, ReloadAfterOpen;
			int		FailedAttempts,MaxAttemptsOnFail;
			int		TimeLeftUntilLoaded;
		};
		class SimulatedAds
		{
			SimAd						Ads[MAX_SIMULATED_ADS];
			unsigned int				Count;
			int							selectedAd;
			bool						closeButtonSelected;
			int							flashIndex, addValue;
			GApp::UI::Application		*App;
			GApp::Resources::Bitmap		bmpImages;
			GApp::Resources::Profile	profile;
			bool						EnableAdService;
		public:
			SimulatedAds();
			bool			Init(GApp::UI::Application *app);
			bool			Add(unsigned int deviceWidth,unsigned int deviceHeight, const char *Name, float x, float y, float width, float height, bool loadOnStartup, int adType, bool reloadAfterOpen, int maxAttemptsOnFail);
			void			Paint(GApp::Graphics::GraphicsContext &gContext);
			void			Update();
			bool			SetVisible(int index, bool value);
			bool			IsVisible(int index);
			bool			IsLoaded(int index);
			bool			Load(int index);
			bool			UnLoad(int index);
			bool			OnTouchEvent(int eventType, int x, int y);
			void			SetAdServiceStatus(bool value);
		};
		struct LANG_REFS
		{
			const char			*Name;
			GAC_TYPE_LANGUAGE	ID;
		};
		struct InitializationData
		{
			// MUST be in SYNK with the one from main.cpp (develop mode)
			unsigned int					Width, Height, DesignWidth, DesignHeight;
			GApp::Resources::ResourceData*	Resources;
			GApp::UI::GlobalDataContainer*	GlobalData;
			const char*						Name;
			unsigned int					AppVersion;
			unsigned int					SplashScreenResourceStart, SplashScreenResourceSize;
			unsigned int					SplashScreenLogoWidth, SplashScreenLogoHeight;
			float							SplashScreenViewWidth, SplashScreenViewHeight;
			unsigned int					SplashScreenMethod, SplashScreenMinimalWaitTime;
			unsigned int					RateInterval, RateMaxTimes;
			unsigned int					ComercialInterval, ComercialMaxTimes;
			unsigned int*					AvailableResolutions;
			unsigned int					AvailableResolutionsCount;
			int								AlarmCheckUpdateTicks;
		};
		namespace Execution
		{
			void		EXPORT InitDevelopMode(InitializationData &initData);
			bool		EXPORT StartApplication(GApp::UI::Application &App);
			void		EXPORT CreateAd(unsigned int deviceWidth, unsigned int deviceHeight, const char *Name, float x, float y, float width, float height, bool loadOnStartup, int adType, bool reloadAfterOpen, int maxAttemptsOnFail);
			void		OnAppInit();
			void		Paint(GApp::Graphics::GraphicsContext &gContext, AppContextData *ctx);
			const void*	GetSimulatedAdsImages(unsigned int &size);
			bool		OnTouchEvent(int eventType, int x, int y);
		}
		struct OSSpecificData
		{
			HDC								hDC;
			HGLRC							hRC;
			HWND							hWnd;
			HANDLE							ResourceAssetHandle;
			unsigned int					MaxTextureSize;
			bool							BillingServiceAvailable;
			bool							AllowPurchases;
			bool							ShowControls;
			bool							ShowBoudaryRectangle;
			bool							GooglePlayServicesSignedIn;
			int								SettingsSnapshotID;
			unsigned int*					AvailableResolutions;
			unsigned int					AvailableResolutionsCount;
			int								X, Y;
			HANDLE							hThread;
			bool							EnableSecondaryTouch;
			float							SecondaryTouchX;
			float							SecondaryTouchY;
			bool							SecondaryTouchPresed;
			int								SecondaryTouchColor, SecondaryTouchAdd;
			SimulatedAds					Ads;
			bool							FaceBookHandlerAvailable;
			bool							YouTubeHandlerAvailable;
			bool							InstagramHandlerAvailable;
			bool							TwitterHandlerAvailable;
			bool							EnableSounds;
			GApp::Utils::String				InAppItems, Currency;
			BassModeWrapper					BassSound[MAX_SOUND_OBJECTS];
			GApp::UI::Application			*mainApp;
			int								TimeDelta;
		};
		class DevelopModeOSContext : public GApp::UI::OSContext
		{
		public:
			OSSpecificData					SpecificData;
			DevelopModeOSContext();

			void							ShowSecondaryTouch(GApp::Graphics::GraphicsContext &gContext, float x, float y, bool touch);
			void							ProcessSecondaryTouchKeys(unsigned int Key, bool down);
			void							Paint(GApp::Graphics::GraphicsContext &gContext);
			void							Init(InitializationData &initData);

			virtual int						fnGetSystemTicksCount();
			virtual bool					fnGetSystemDateTime(GApp::Utils::DateTime *);
			virtual void					fnClose();
			virtual bool					fnReadBufferFromResources(unsigned int pos, unsigned char *Buffer, unsigned int size);
			virtual void					fnOpenBrowser(const char *url);

			virtual void					fnOpenAppHandler(unsigned int handlerID, const char * data);
			virtual bool					fnIsAppHandlerAvailable(unsigned int handlerID);

			virtual void					fnRateApp();
			virtual bool					fnIsAppInstalled(const char * appOrPackageName);
			virtual void					fnInstallApp(const char * appOrPackageName);

			virtual void					fnOpenApplicationMarketPage();
			virtual void					fnOpenDeveloperMarketPage();
			virtual bool					fnSaveBufferToLocalStorage(const char* fileName, const void *Buffer, unsigned int BufferSize);
			virtual bool					fnReadBufferFromLocalStorage(const char* fileName, GApp::Utils::Buffer *buffer);
			virtual void					fnPlaySound(int soundID, int volume, bool loop);
			virtual void					fnStopSound(int soundID);
			virtual void					fnSetSoundState(int soundID, int volume, bool loop);
			virtual bool					fnLoadSound(int soundID, unsigned int pos, unsigned int size);
			virtual void					fnUnLoadSound(int soundID);
			virtual void					fnThreadWait(unsigned int milliseconds);
			virtual bool					fnThreadRun(void(*runFunction)(void *threadObject), void*  threadObject);

			virtual void					fnQueryNewAd(int adID);
			virtual void					fnSetAdVisibility(int adID, bool visible);
			virtual bool					fnIsAdVisible(int adID);
			virtual bool					fnIsAdLoaded(int adID);

			virtual bool					fnSendBuyRequestForItem(const char *item);
			virtual bool					fnIsBillingSystemAvailable();
			virtual bool					fnGetItemPrice(const char* item, GApp::Utils::String &result);

			virtual void					fnSendEmail(const char *To, const char *Subject, const char *Content);

			virtual void					fnExecuteOsCommand(const char* command, GApp::Utils::String *result);
			virtual void					fnApplicationInitializeCompleted();

			virtual bool					fnSendAnalyticsEvent(const char* eventName, const void *buffer, GAC_ANALYTICS_EVENT_TYPE type);
			virtual void					fnShare(const char *text);
			virtual void					fnSendSMS(const char *message);

			// Google play services specific
			virtual bool					fnGooglePlayServices_IsSignedIn();
			virtual void					fnGooglePlayServices_SignIn();
			virtual void					fnGooglePlayServices_SignOut();
			virtual const char*				fnGooglePlayServices_GetCurrentPlayerName();
			virtual void					fnGooglePlayServices_SubmitScore(int leaderBoardID, int score);
			virtual void					fnGooglePlayServices_ShowLeaderBoard(int leaderBoardID);
			virtual void					fnGooglePlayServices_RequestCurrentPlayerScore(int leaderBoardID);
			virtual int						fnGooglePlayServices_GetCurrentPlayerScore(int leaderBoardID);
			virtual int						fnGooglePlayServices_GetCurrentPlayerRank(int leaderBoardID);
		};	
	}
}

bool SaveSnapshotState(GApp::UI::Application &app, AppContextData &acd, bool appIsTerminating, const char* fileName);


#endif