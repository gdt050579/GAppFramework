#include "DevelopMode.h"

using namespace GApp::DevelopMode;

static LANG_REFS LanguageList[] = {
	{ "AFRIKAANS", GAC_LANGUAGE_AFRIKAANS },
	{ "ALBANIAN", GAC_LANGUAGE_ALBANIAN },
	{ "ARABIC", GAC_LANGUAGE_ARABIC },
	{ "ARMENIAN", GAC_LANGUAGE_ARMENIAN },
	{ "AZERBAIJANI", GAC_LANGUAGE_AZERBAIJANI },
	{ "BASQUE", GAC_LANGUAGE_BASQUE },
	{ "BELARUSIAN", GAC_LANGUAGE_BELARUSIAN },
	{ "BENGALI", GAC_LANGUAGE_BENGALI },
	{ "BOSNIAN", GAC_LANGUAGE_BOSNIAN },
	{ "BULGARIAN", GAC_LANGUAGE_BULGARIAN },
	{ "CATALAN", GAC_LANGUAGE_CATALAN },
	{ "CEBUANTO", GAC_LANGUAGE_CEBUANTO },
	{ "CHINESE", GAC_LANGUAGE_CHINESE },
	{ "CROATIAN", GAC_LANGUAGE_CROATIAN },
	{ "CZECH", GAC_LANGUAGE_CZECH },
	{ "DANISH", GAC_LANGUAGE_DANISH },
	{ "DUTCH", GAC_LANGUAGE_DUTCH },
	{ "ENGLISH", GAC_LANGUAGE_ENGLISH },
	{ "ESPERANTO", GAC_LANGUAGE_ESPERANTO },
	{ "ESTONIAN", GAC_LANGUAGE_ESTONIAN },
	{ "FILIPINO", GAC_LANGUAGE_FILIPINO },
	{ "FINNISH", GAC_LANGUAGE_FINNISH },
	{ "FRENCH", GAC_LANGUAGE_FRENCH },
	{ "GALICIAN", GAC_LANGUAGE_GALICIAN },
	{ "GEORGIAN", GAC_LANGUAGE_GEORGIAN },
	{ "GERMAN", GAC_LANGUAGE_GERMAN },
	{ "GREEK", GAC_LANGUAGE_GREEK },
	{ "GUJARATI", GAC_LANGUAGE_GUJARATI },
	{ "HAITIANCREOLE", GAC_LANGUAGE_HAITIANCREOLE },
	{ "HEBREW", GAC_LANGUAGE_HEBREW },
	{ "HINDI", GAC_LANGUAGE_HINDI },
	{ "HMONG", GAC_LANGUAGE_HMONG },
	{ "HUNGARIAN", GAC_LANGUAGE_HUNGARIAN },
	{ "ICELANDIC", GAC_LANGUAGE_ICELANDIC },
	{ "INDONESIAN", GAC_LANGUAGE_INDONESIAN },
	{ "IRISH", GAC_LANGUAGE_IRISH },
	{ "ITALIAN", GAC_LANGUAGE_ITALIAN },
	{ "JAPANESE", GAC_LANGUAGE_JAPANESE },
	{ "JAVANESE", GAC_LANGUAGE_JAVANESE },
	{ "KANNADA", GAC_LANGUAGE_KANNADA },
	{ "KHMER", GAC_LANGUAGE_KHMER },
	{ "LAO", GAC_LANGUAGE_LAO },
	{ "LATIN", GAC_LANGUAGE_LATIN },
	{ "LATVIAN", GAC_LANGUAGE_LATVIAN },
	{ "LITHUANIAN", GAC_LANGUAGE_LITHUANIAN },
	{ "MECEDONIAN", GAC_LANGUAGE_MECEDONIAN },
	{ "MALAY", GAC_LANGUAGE_MALAY },
	{ "MALTESE", GAC_LANGUAGE_MALTESE },
	{ "MARATHI", GAC_LANGUAGE_MARATHI },
	{ "NORWEGIAN", GAC_LANGUAGE_NORWEGIAN },
	{ "PERSIAN", GAC_LANGUAGE_PERSIAN },
	{ "POLISH", GAC_LANGUAGE_POLISH },
	{ "PORTUGUESE", GAC_LANGUAGE_PORTUGUESE },
	{ "ROMANIAN", GAC_LANGUAGE_ROMANIAN },
	{ "RUSSIAN", GAC_LANGUAGE_RUSSIAN },
	{ "SERBIAN", GAC_LANGUAGE_SERBIAN },
	{ "SLOVAK", GAC_LANGUAGE_SLOVAK },
	{ "SLOVENIAN", GAC_LANGUAGE_SLOVENIAN },
	{ "SPANISH", GAC_LANGUAGE_SPANISH },
	{ "SWAHILI", GAC_LANGUAGE_SWAHILI },
	{ "SWEDISH", GAC_LANGUAGE_SWEDISH },
	{ "TAMIL", GAC_LANGUAGE_TAMIL },
	{ "TELEGU", GAC_LANGUAGE_TELEGU },
	{ "THAI", GAC_LANGUAGE_THAI },
	{ "TURKISH", GAC_LANGUAGE_TURKISH },
	{ "UKRAINIAN", GAC_LANGUAGE_UKRAINIAN },
	{ "URDU", GAC_LANGUAGE_URDU },
	{ "VIETNAMESE", GAC_LANGUAGE_VIETNAMESE },
	{ "WELSH", GAC_LANGUAGE_WELSH },
	{ "YIDDISH", GAC_LANGUAGE_YIDDISH },
};



CRITICAL_SECTION							logCS;
GApp::UI::Application						*mainApp = NULL;
GApp::DevelopMode::DevelopModeOSContext		devOSContext;


unsigned int startRenderTime;
unsigned int countFPS;
unsigned int imageScale = 1;
unsigned int requestedFPs = 80;
unsigned char* snapshootPixelBuffer = NULL;
bool shouldTakePicture = false;
bool shouldGetSettings = false;



int requestTextureId = -1;

// functiile pentru OpenGL
OPENGL_FUNCTIONS_INIT(OPENGLNOTEXTERN)

typedef PROC(WINAPI *TwglGetProcAddress)(const char* functionName);
#define LOAD_FUNCTION(name) { *((PROC*)&name) = GetOpenGLFunction(hMod,wglGetProcAddress,#name); if (name==NULL) {RETURNERROR(false,"Unable to load OpenGL function: %s",#name); }; }
#define LOAD_WINOPENGL_FUNCTION(name) { *((PROC*)&OpenGL_##name) = GetProcAddress(hMod,#name); if (OpenGL_##name==NULL) return false; }

PROC GetOpenGLFunction(HMODULE openGLModule, TwglGetProcAddress wglGetProcAddress, const char* functionName)
{
	PROC p = NULL;
	*(PROC*)&p = GetProcAddress(openGLModule, functionName);
	if (p == NULL)
	{
		p = wglGetProcAddress(functionName);
		size_t addr = (size_t)p;
		if (addr < 10)
			return NULL;
		// verific ca P sa fie ok
	}
	return p;
}
bool InitializeOpenGLContext()
{
	HMODULE hMod = LoadLibraryA("opengl32.dll");
	if (hMod == NULL) return false;
	// specifice Windows
	LOAD_WINOPENGL_FUNCTION(wglMakeCurrent);
	LOAD_WINOPENGL_FUNCTION(wglCreateContext);
	LOAD_WINOPENGL_FUNCTION(wglDeleteContext);

	return true;
}
bool InitializeOpenGLFunctions()
{
	HMODULE hMod = LoadLibraryA("opengl32.dll");
	if (hMod == NULL) return false;
	TwglGetProcAddress wglGetProcAddress = (TwglGetProcAddress)GetProcAddress(hMod, "wglGetProcAddress");
	if (wglGetProcAddress == NULL) return false;

	// open GL exista si este incarcat
	LOAD_FUNCTION(glShadeModel);
	LOAD_FUNCTION(glBindFramebuffer);
	LOAD_FUNCTION(glDrawBuffer);
	LOAD_FUNCTION(glReadBuffer);
	LOAD_FUNCTION(glBlitFramebuffer);
	LOAD_FUNCTION(glClearColor);
	LOAD_FUNCTION(glClearDepth);
	LOAD_FUNCTION(glEnable);
	LOAD_FUNCTION(glDisable);
	LOAD_FUNCTION(glClear);
	LOAD_FUNCTION(glDepthFunc);
	LOAD_FUNCTION(glHint);
	LOAD_FUNCTION(glViewport);
	LOAD_FUNCTION(glBlendFunc);
	LOAD_FUNCTION(glGenRenderbuffers);
	LOAD_FUNCTION(glBindRenderbuffer);
	LOAD_FUNCTION(glRenderbufferStorage);
	LOAD_FUNCTION(glGenFramebuffers);
	LOAD_FUNCTION(glFramebufferRenderbuffer);
	LOAD_FUNCTION(glGetError);
	LOAD_FUNCTION(glDrawArrays);
	LOAD_FUNCTION(glTexParameteri);
	LOAD_FUNCTION(glBindTexture);
	LOAD_FUNCTION(glDeleteTextures);
	LOAD_FUNCTION(glGetTexLevelParameteriv);
	LOAD_FUNCTION(glGetIntegerv);
	LOAD_FUNCTION(glUniform1f);
	LOAD_FUNCTION(glUniform2f);
	LOAD_FUNCTION(glUniform3f);
	LOAD_FUNCTION(glUniform4f);
	LOAD_FUNCTION(glActiveTexture);
	LOAD_FUNCTION(glLineWidth);
	LOAD_FUNCTION(glCreateShader);
	LOAD_FUNCTION(glAttachShader);
	LOAD_FUNCTION(glCompileShader);
	LOAD_FUNCTION(glDeleteShader);
	LOAD_FUNCTION(glCreateProgram);
	LOAD_FUNCTION(glDeleteProgram);
	LOAD_FUNCTION(glEnableVertexAttribArray);
	LOAD_FUNCTION(glGenTextures);
	LOAD_FUNCTION(glGetAttribLocation);
	LOAD_FUNCTION(glGetProgramInfoLog);
	LOAD_FUNCTION(glGetProgramiv);
	LOAD_FUNCTION(glGetShaderiv);
	LOAD_FUNCTION(glGetShaderInfoLog);
	LOAD_FUNCTION(glTexImage2D);
	LOAD_FUNCTION(glVertexAttribPointer);
	LOAD_FUNCTION(glGetTexImage);
	LOAD_FUNCTION(glGetUniformLocation);
	LOAD_FUNCTION(glLinkProgram);
	LOAD_FUNCTION(glUseProgram);
	LOAD_FUNCTION(glScissor);
	LOAD_FUNCTION(glShaderSource);
	LOAD_FUNCTION(glReadPixels);
	LOAD_FUNCTION(glStencilFunc);
	LOAD_FUNCTION(glStencilOp);
	LOAD_FUNCTION(glStencilMask);
	LOAD_FUNCTION(glColorMask);
	LOAD_FUNCTION(glDepthMask);
	LOAD_FUNCTION(glClearStencil);
	return true;
}

// 
HANDLE GetResourceAssetHandle()
{
	char temp[2048];
	memset(temp, 0, 2048);
	GetModuleFileNameA(NULL, temp, 2000);
	int last = -1;
	for (int tr = 0; temp[tr] != 0; tr++)
	if (temp[tr] == '\\')
		last = tr;
	last += 1;
	memcpy(&temp[last], "resources.res", 14);
	return CreateFileA(temp, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
}
// MESSAGE LOOP

void ClientResize(HWND hWnd, int nWidth, int nHeight)
{
	RECT rcClient, rcWindow;
	POINT ptDiff;

	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWindow);
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	MoveWindow(hWnd, rcWindow.left, rcWindow.top, nWidth + ptDiff.x, nHeight + ptDiff.y, TRUE);
}
bool SendDataToDebugInterface(void* buffer, unsigned int size)
{
	char temp[2048];
	DWORD nrRead;
	memset(temp, 0, 2048);

	GetModuleFileNameA(NULL, temp, 2000);
	CHECK(GApp::Utils::String::Add(temp, ".data", 2000), false, "Unable to create debug file exchange !");
	HANDLE hFile = CreateFileA(temp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	CHECK(hFile != INVALID_HANDLE_VALUE, false, "Unable to open file: %s -> Error: %d", temp, GetLastError());
	CHECK((WriteFile(hFile, buffer, size, &nrRead, NULL)) && (nrRead == size), false, "Unable to write debug file exchange data to: %s", temp);
	CloseHandle(hFile);

	return true;
}
void TakePicture()
{
	int LastError;
	LOG_INFO("Taking a picture ...");
	while (true)
	{
		CHECK_GLERROR_BK(glReadPixels(0, 0, mainApp->CoreContext->Width, mainApp->CoreContext->Height, GL_RGB, GL_UNSIGNED_BYTE, snapshootPixelBuffer));
		CHECKBK(SendDataToDebugInterface(snapshootPixelBuffer, mainApp->CoreContext->Width * mainApp->CoreContext->Height * 3), "");
		DEBUGMSG("TakePictureOk|%d|%d", mainApp->CoreContext->Width, mainApp->CoreContext->Height);
		shouldTakePicture = false;
		return;
	}
	DEBUGMSG("TakePictureFailed|%d|%d", mainApp->CoreContext->Width, mainApp->CoreContext->Height);
	shouldTakePicture = false;
	return;
}
void GetTexture(int textureID)
{
	int format;
	glBindTexture(GL_TEXTURE_2D, textureID);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
	int w;
	int h;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
	if ((w <= 0) || (h <= 0))
	{
		DEBUGMSG("GetTextureFailed|Failed to receive texture dimensions !");
		requestTextureId = -1;
		return;
	}
	if (format != GL_RGBA)
	{
		DEBUGMSG("GetTextureFailed|Invalid format: %d", format);
		requestTextureId = -1;
		return;
	}
	unsigned int size = w *h *sizeof(unsigned int);
	unsigned char *pixels = new unsigned char[size];
	glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, &pixels[0]);
	if (SendDataToDebugInterface(pixels, size) == false)
	{
		delete[] pixels;
		DEBUGMSG("GetTextureFailed|Unable to send data to debug interface !");
	}
	else {
		delete[] pixels;
		DEBUGMSG("GetTextureOk|%d|%d", w, h);
	}

	requestTextureId = -1;
}
void GetSettings(GApp::Utils::KeyDB &db)
{
	GApp::Utils::Buffer b;
	while (true)
	{
		CHECKBK(b.Create(0x10000), "Unable to create Settings buffer !");
		CHECKBK(b.Resize(0), "Unable to clear Settings Map buffer !");
		unsigned int sz = db.Len();
		LOG_INFO("Settings count = %d", sz);
		for (unsigned int tr = 0; tr < sz; tr++)
		{
			unsigned int szBuf = b.GetSize();
			if (db.CopyKeyInfo(tr, &b) == false)
			{
				LOG_ERROR("Unable to copy key with index: %d to settings map !", tr);
				b.Resize(szBuf);
			}
		}
		CHECKBK(SendDataToDebugInterface(b.GetBuffer(), b.GetSize()), "Unable to save settings map !");
		DEBUGMSG("GetSettingsOk");
		shouldGetSettings = false;
		return;
	}
	shouldGetSettings = false;
	DEBUGMSG("GetSettingsFailed");
}
bool ConvertLanguageToID(char *value, unsigned int &lang)
{
	for (int tr = 0; tr < sizeof(LanguageList) / sizeof(LanguageList[0]); tr++)
	{
		if (GApp::Utils::String::Equals(LanguageList[tr].Name, value, true))
		{
			LOG_INFO("Set language to %s (%d)", value, LanguageList[tr].ID);
			lang = LanguageList[tr].ID;
			return true;
		}
	}
	RETURNERROR(false, "Unable to find ID for language: %s", value);
}
bool CreateSnapshot(unsigned int ID)
{
	char fname[32];
	GApp::Utils::String str;
	CHECK(str.Create(fname, 128, true), false, "");
	CHECK(str.Format("%08X.snapshot", ID), false, "");
	while (true)
	{
		CHECKBK(SaveSnapshotState(*mainApp, *((AppContextData*)mainApp->Context), false, fname), "Unable to create %s", fname);
		((AppContextData*)mainApp->Context)->CoreObject.SnapshotSaved = false; // resetez ca sa pot sa salvez si la OnTerminate
		return true;
	}
	RETURNERROR(false, "Failed to create snapshot with ID: %d", ID)
}
bool LoadSettingsFile(GApp::DevelopMode::DevelopModeOSContext *OS, bool updateOnlyRunTimeValues)
{
	char temp[1024];
	char key[32];
	char value[512];
	bool onKey = true;
	unsigned int procent;
	memset(temp, 0, 1023);
	GetModuleFileNameA(NULL, temp, 1020);
	int last = -1;
	for (int tr = 0; temp[tr] != 0; tr++)
	if (temp[tr] == '\\')
		last = tr;
	last += 1;
	memcpy(&temp[last], "settings.ini", 13);

	HANDLE hFile = CreateFileA(temp, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	CHECK(hFile != INVALID_HANDLE_VALUE, false, "");
	while (true)
	{
		DWORD nrRead;
		CHECKBK(ReadFile(hFile, temp, 1020, &nrRead, NULL) != FALSE, "");
		temp[nrRead] = '\n';
		nrRead++;
		temp[nrRead] = 0;
		CloseHandle(hFile);
		int poz = 0;
		key[0] = value[0] = 0;
		for (unsigned int tr = 0; tr < nrRead; tr++)
		{
			if ((temp[tr] == '\n') || (temp[tr] == '\r'))
			{
				if (key[0] != 0)
				{
					LOG_INFO("* NewKey: %s = %s", key, value);
					if (updateOnlyRunTimeValues == false)
					{
						if (GApp::Utils::String::Equals(key, "width", true))
						{
							CHECK(GApp::Utils::String::ConvertToUInt32(value, OS->Data.Width), false, "");
						}
						if (GApp::Utils::String::Equals(key, "height", true))
						{
							CHECK(GApp::Utils::String::ConvertToUInt32(value, OS->Data.Height), false, "");
						}
						if (GApp::Utils::String::Equals(key, "X", true))
						{
							CHECK(GApp::Utils::String::ConvertToInt32(value, OS->SpecificData.X), false, "");
						}
						if (GApp::Utils::String::Equals(key, "Y", true))
						{
							CHECK(GApp::Utils::String::ConvertToInt32(value, OS->SpecificData.Y), false, "");
						}
						if (GApp::Utils::String::Equals(key, "maxtexturesize", true))
						{
							CHECK(GApp::Utils::String::ConvertToUInt32(value, OS->SpecificData.MaxTextureSize), false, "");
						}					
						if (GApp::Utils::String::Equals(key, "language", true))
							CHECK(ConvertLanguageToID(value, OS->Data.Lang), false, "");
					}
					if (GApp::Utils::String::Equals(key, "enableadservice", true))
						OS->SpecificData.Ads.SetAdServiceStatus((value[0] ^ 0x20) == 't');
					if (GApp::Utils::String::Equals(key, "billingservice", true))
						OS->SpecificData.BillingServiceAvailable = ((value[0] ^ 0x20) == 't');
					if (GApp::Utils::String::Equals(key, "allowpurchases", true))
						OS->SpecificData.AllowPurchases = ((value[0] ^ 0x20) == 't');
					if (GApp::Utils::String::Equals(key, "showcontrols", true))
						OS->SpecificData.ShowControls = ((value[0] ^ 0x20) == 't');
					if (GApp::Utils::String::Equals(key, "loadsettings", true))
					{
						CHECK(GApp::Utils::String::ConvertToInt32(value, OS->SpecificData.SettingsSnapshotID), false, "");
					}
					if (GApp::Utils::String::Equals(key, "enablesounds", true))
						OS->SpecificData.EnableSounds = ((value[0] ^ 0x20) == 't');
					if (GApp::Utils::String::Equals(key, "enablesecondarytouch", true))
						OS->SpecificData.EnableSecondaryTouch = ((value[0] ^ 0x20) == 't');
					if (GApp::Utils::String::Equals(key, "showboudaryrectangle", true))
						OS->SpecificData.ShowBoudaryRectangle = ((value[0] ^ 0x20) == 't');
					if (GApp::Utils::String::Equals(key, "inappitems", true))
						OS->SpecificData.InAppItems.Set(value);
					if (GApp::Utils::String::Equals(key, "currency", true))
						OS->SpecificData.Currency.Set(value);

					if (GApp::Utils::String::Equals(key, "speed", true))
					{
						if (GApp::Utils::String::ConvertToUInt32(value, procent) == false)
						{
							requestedFPs = 80;
						}
						else {
							if (procent < 10)
								procent = 10;
							if (procent>100)
								procent = 100;
							requestedFPs = (80 * procent) / 100;
						}
						if (devOSContext.SpecificData.hWnd !=  NULL)
						{
							LOG_INFO("Changing speed of the game (divede parts for FPS = %d)", requestedFPs);
							CHECK(SetTimer(devOSContext.SpecificData.hWnd, 1234, 1000 / requestedFPs, NULL) != 0, false, "");
							countFPS = 0;
							startRenderTime = GetTickCount();							
						} 

					}

					if (GApp::Utils::String::Equals(key, "FaceBookHandlerAvailable", true))
						OS->SpecificData.FaceBookHandlerAvailable = ((value[0] ^ 0x20) == 't');
					if (GApp::Utils::String::Equals(key, "YouTubeHandlerAvailable", true))
						OS->SpecificData.YouTubeHandlerAvailable = ((value[0] ^ 0x20) == 't');
					if (GApp::Utils::String::Equals(key, "InstagramHandlerAvailable", true))
						OS->SpecificData.InstagramHandlerAvailable = ((value[0] ^ 0x20) == 't');
					if (GApp::Utils::String::Equals(key, "TwitterHandlerAvailable", true))
						OS->SpecificData.TwitterHandlerAvailable = ((value[0] ^ 0x20) == 't');

					if (GApp::Utils::String::Equals(key, "snapshottype", true))
					{
						if (GApp::Utils::String::Equals(value, "full", true))
							OS->Data.SnapshotType = GAC_SNAPSHOT_TYPE_FULL;
					}
				}
				key[0] = value[0] = 0;
				poz = 0;
				onKey = true;
				continue;
			}
			if (temp[tr] == '=')
			{
				if (onKey)
				{
					onKey = false;
					poz = 0;
					value[0] = 0;
				}
				continue;
			}
			if (onKey)
			{
				if (poz < 30) {
					key[poz] = temp[tr];
					poz++;
					key[poz] = 0;
					continue;
				}
			}
			else {
				if (poz < 510)
				{
					value[poz] = temp[tr];
					poz++;
					value[poz] = 0;
					continue;
				}
			}
		}
		if (updateOnlyRunTimeValues)
		{
			DEBUGMSG("SettingsLoadedOK");
		}
		return true;
	}
	CloseHandle(hFile);
	return false;

}
void ProcessDebugMessages(unsigned int message, unsigned int value)
{
	if (mainApp->Context == NULL) return;
	switch (message)
	{
	case PROCESS_DEBUG_MSG_TERMINATE:
		mainApp->ProcessSystemEvent(SYSEVENT_TERMINATE, 0, 0, 0, 0);
		PostQuitMessage(0);
		break;
	case PROCESS_DEBUG_MSG_PAUSE:
		mainApp->ProcessSystemEvent(SYSEVENT_PAUSE, 0, 0, 0.0f, 0.0f);
		break;
	case PROCESS_DEBUG_MSG_RESUME:
		mainApp->ProcessSystemEvent(SYSEVENT_RESUME, 0, 0, 0.0f, 0.0f);
		break;
	case PROCESS_DEBUG_MSG_SCALE:
		ClientResize(devOSContext.SpecificData.hWnd, mainApp->CoreContext->Width / value, mainApp->CoreContext->Height / value);
		imageScale = value;
		break;
	case PROCESS_DEBUG_MSG_TAKE_PICTURE:
		shouldTakePicture = true;
		LOG_INFO("Preparint to take a snapshot ...");
		break;
	case PROCESS_DEBUG_MSG_GET_SETTINGS:
		shouldGetSettings = true;
		LOG_INFO("Retrieving settings ... ");
		break;
	case PROCESS_DEBUG_MSG_SUSPEND:
		mainApp->ProcessSystemEvent(SYSEVENT_SAVE_STATE, 0, 0, 0, 0);
		mainApp->ProcessSystemEvent(SYSEVENT_TERMINATE, 0, 0, 0, 0);
		PostQuitMessage(0);
		break;
	case PROCESS_DEBUG_MSG_CHANGE_SCENE:
		mainApp->CoreContext->Api.ChangeScene(value);
		break;
	case PROCESS_DEBUG_MSG_GET_TEXTURE:
		requestTextureId = value;
		break;
	case PROCESS_DEBUG_MSG_RELOAD_SETTINGS:
		LoadSettingsFile(&devOSContext, true);
		break;
	case PROCESS_DEBUG_MSG_CREATE_SNAPSHOT:
		if (CreateSnapshot(value))
		{
			DEBUGMSG("SnapshotCreatedOK");
		}
		break;
	case PROCESS_DEBUG_MSG_ENABLE_COUNTER:		
		if (value < mainApp->CoreContext->Resources->GetCountersCount())
			mainApp->CoreContext->Resources->GetCountersList()[value].Enable();
		break;
	case PROCESS_DEBUG_MSG_DISABLE_COUNTER:
		if (value<mainApp->CoreContext->Resources->GetCountersCount())
			mainApp->CoreContext->Resources->GetCountersList()[value].Disable();
		break;
	case PROCESS_DEBUG_MSG_RECHECK_ALARMS:
		((AppContextData*)mainApp->Context)->AlarmCheckTimer = 1; // la prima decrementare se recitesc datele
		break;
	case PROCESS_DEBUG_MSG_UPDATE_TIME_DELTA:
		devOSContext.SpecificData.TimeDelta = *(int *)(&value);
		LOG_INFO("New time delta set: %d seconds", devOSContext.SpecificData.TimeDelta);
		((AppContextData*)mainApp->Context)->AlarmCheckTimer = 1; // la prima decrementare se recitesc datele
		break;
	default:
		LOG_ERROR("Unknown message (%d) - parameter: %d", message, value);
		break;
	}
}
void ProcessDebugCommand(unsigned int commandID, unsigned int u1, unsigned int u2)
{
	LOG_INFO("Debug command received: ID=%d, u1=%08X, u2=%08X", commandID, u1, u2);
	GApp::UI::Scene *s = ((AppContextData *)mainApp->Context)->CurrentScene;
	UInt64 data = u2;
	data = data << 32;
	data = data | u1;
	mainApp->OnDebugCommand(commandID, data);
	if (s != NULL)
		s->OnDebugCommand(commandID, data);
}
void FlashFBOToScreen(GLuint fbo, unsigned int scale, unsigned int Width, unsigned int Height)
{
	//LOG_INFO("FlashFBOToScreen(FBO = %d,scale=%d,Width=%d,Height=%d)", fbo, scale, Width, Height);
	/* We are going to blit into the window (default framebuffer)                     */
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);              /* Use backbuffer as color dst.         */

	/* Read from your FBO */
	//*
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT0); /* Use Color Attachment 0 as color src. */

	/* Copy the color and depth buffer from your FBO to the default framebuffer       */
	//*
	glBlitFramebuffer(0, 0, (int)Width, (int)Height,
		0, 0, (int)Width / scale, (int)Height / scale,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//*/
}
long __stdcall WindowProcedure(HWND window, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
	unsigned int tmp;
	if ((mainApp != NULL) && ((AppContextData*)mainApp->Context != NULL) && (devOSContext.SpecificData.hWnd == window))
	{
		switch (msg)
		{
		case WM_DESTROY:
			LOG_INFO("[SYSEVENT] - WM_DESTROY !");
			mainApp->ProcessSystemEvent(SYSEVENT_TERMINATE, 0, 0, 0, 0);
			PostQuitMessage(0);
			return 0L;
		case WM_QUIT:
			LOG_INFO("[SYSEVENT] - WM_QUIT !");
			return 0L;
		case WM_ERASEBKGND:
			return 0L;

		case WM_LBUTTONDOWN:
			mainApp->ProcessSystemEvent(SYSEVENT_TOUCH_DOWN, 0, GetTickCount(), LOSHORT(lParam)*imageScale, HISHORT(lParam)*imageScale);
			SetCapture(devOSContext.SpecificData.hWnd);
			return 0L;
		case WM_LBUTTONUP:
			mainApp->ProcessSystemEvent(SYSEVENT_TOUCH_UP, 0, GetTickCount(), LOSHORT(lParam)*imageScale, HISHORT(lParam)*imageScale);
			ReleaseCapture();
			return 0L;
		case WM_MOUSEMOVE:
			if ((wParam & MK_LBUTTON) != 0)
				mainApp->ProcessSystemEvent(SYSEVENT_TOUCH_MOVE, 0, GetTickCount(), LOSHORT(lParam)*imageScale, HISHORT(lParam)*imageScale);
			return 0L;
		case WM_ACTIVATE:
			/*
			LOG_INFO("----------------------------------------------------------------- Activate: %08X", wParam);
			if ((wParam & 0xFFFF) == 0)
			mainApp->ProcessSystemEvent(SYSEVENT_PAUSE, 0, 0, 0.0f, 0.0f);
			else
			mainApp->ProcessSystemEvent(SYSEVENT_RESUME, 0, 0, 0.0f, 0.0f);
			*/
			return 0L;
		case WM_KEYDOWN:
			tmp = wParam & 0xFFFF;
			if (tmp == 27) tmp = GAC_KEYBOARD_CODE_BACK;
			mainApp->ProcessSystemEvent(SYSEVENT_KEY_DOWN, tmp, 0, 0.0f, 0.0f);
			if (devOSContext.SpecificData.EnableSecondaryTouch)
				devOSContext.ProcessSecondaryTouchKeys(tmp, true);
			return 0L;
		case WM_KEYUP:
			tmp = wParam & 0xFFFF;
			if (tmp == 27) tmp = GAC_KEYBOARD_CODE_BACK;
			mainApp->ProcessSystemEvent(SYSEVENT_KEY_UP, tmp, 0, 0.0f, 0.0f);
			//LOG_INFO("KeyCode: %d", tmp);
			if (tmp == 'S')
				ProcessDebugMessages(PROCESS_DEBUG_MSG_SUSPEND, 0);
			if (tmp == 'P')
				shouldTakePicture = true;
			if (devOSContext.SpecificData.EnableSecondaryTouch)
				devOSContext.ProcessSecondaryTouchKeys(tmp, false);

			return 0L;
		case WM_PAINT:
			if (mainApp->Context != NULL)
				return DefWindowProcA(devOSContext.SpecificData.hWnd, msg, wParam, lParam);
			return 0L;
		case WM_TIMER:
			if (mainApp->CoreContext != NULL) {
				mainApp->LoopExecute();
				FlashFBOToScreen(mainApp->CoreContext->Graphics.fbo, imageScale, mainApp->CoreContext->Width, mainApp->CoreContext->Height);
				//LOG_INFO("SwapBuffer(AppContextData=%p,OS=%p,hDC=%p", ((AppContextData*)(mainApp->Context)), ((AppContextData*)(mainApp->Context))->OS, ((AppContextData*)(mainApp->Context))->OS->hDC);
				SwapBuffers(devOSContext.SpecificData.hDC);
				//LOG_INFO("SwapBuffer - OK");
				if (shouldTakePicture)
					TakePicture();
				if (shouldGetSettings)
					GetSettings(mainApp->CoreContext->SettingsDB);
				if (requestTextureId >= 0)
					GetTexture(requestTextureId);
				countFPS++;
				if (startRenderTime != 0xFFFFFFFF)
				{
					unsigned int t = GetTickCount();
					//printf("FPS: %d - %d\n", t - startRenderTime, countFPS);
					if ((t - startRenderTime) > 10000)
					{
						//printf("FPS: closing !\n");
						printf("\nFPS: %f\n", 1000.0f*countFPS / ((float)(t - startRenderTime)));
						startRenderTime = 0xFFFFFFFF;
					}
				}
			}
			//LOG("Painting: %d\n",Core.Api.GetSystemTicksCount());
			return 0L;
			//DEBUG CONTROL MESSAGES
		case 0xB000:
			ProcessDebugMessages(wParam, lParam);
			return 0L;
		case 0xB001:
		case 0xB002:
		case 0xB003:
		case 0xB004:
		case 0xB005:
		case 0xB006:
		case 0xB007:
		case 0xB008:
		case 0xB009:
		case 0xB00A:
		case 0xB00B:
		case 0xB00C:
		case 0xB00D:
		case 0xB00E:
		case 0xB00F:
		case 0xB010:
		case 0xB011:
		case 0xB012:
		case 0xB013:
		case 0xB014:
		case 0xB015:
		case 0xB016:
		case 0xB017:
		case 0xB018:
		case 0xB019:
		case 0xB01A:
		case 0xB01B:
		case 0xB01C:
		case 0xB01D:
		case 0xB01E:
		case 0xB01F:
		case 0xB020:
		case 0xB021:
		case 0xB022:
		case 0xB023:
		case 0xB024:
		case 0xB025:
		case 0xB026:
		case 0xB027:
		case 0xB028:
		case 0xB029:
		case 0xB02A:
		case 0xB02B:
		case 0xB02C:
		case 0xB02D:
		case 0xB02E:
		case 0xB02F:
			ProcessDebugCommand(msg - 0xB001, wParam, lParam);
			return 0L;
		default:
			//LOG_INFO("Message: %d [%d,%d]\n",msg,wParam,lParam);
			return DefWindowProcA(devOSContext.SpecificData.hWnd, msg, wParam, lParam);
		}
	}

	return DefWindowProcA(window, msg, wParam, lParam);
}
void DestroyOpenGLWindow()
{
	if (devOSContext.SpecificData.hRC)
	{
		if (!OpenGL_wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
			LOG_ERROR("Release Of DC And RC Failed.");
		if (!OpenGL_wglDeleteContext(devOSContext.SpecificData.hRC))						// Are We Able To Delete The RC?
			LOG_ERROR("Release Of DC And RC Failed.");
		devOSContext.SpecificData.hRC = NULL;										// Set RC To NULL
	}

	if (devOSContext.SpecificData.hDC && !ReleaseDC(devOSContext.SpecificData.hWnd, devOSContext.SpecificData.hDC))					// Are We Able To Release The DC
	{
		LOG_ERROR("Release Device Context Failed.");
		devOSContext.SpecificData.hDC = NULL;										// Set DC To NULL
	}

	if (devOSContext.SpecificData.hWnd && !DestroyWindow(devOSContext.SpecificData.hWnd))					// Are We Able To Destroy The Window?
	{
		LOG_ERROR("Could Not Release hWnd.");
		devOSContext.SpecificData.hWnd = NULL;										// Set hWnd To NULL
	}
	devOSContext.SpecificData.hDC = NULL;
	devOSContext.SpecificData.hRC = NULL;
	devOSContext.SpecificData.hWnd = NULL;
}

void GApp::DevelopMode::Execution::Paint(GApp::Graphics::GraphicsContext &gContext, AppContextData *ctx)
{
	devOSContext.Paint(gContext);
}
void GApp::DevelopMode::Execution::CreateAd(unsigned int deviceWidth, unsigned int deviceHeight, const char *Name, float x, float y, float width, float height, bool loadOnStartup, int adType, bool reloadAfterOpen, int maxAttemptsOnFail)
{
	devOSContext.SpecificData.Ads.Add(deviceWidth, deviceHeight, Name, x, y, width, height, loadOnStartup, adType, reloadAfterOpen, maxAttemptsOnFail);
}
void GApp::DevelopMode::Execution::OnAppInit()
{
	srand(GetTickCount());
	devOSContext.SpecificData.Ads.Init(mainApp);
}
void GApp::DevelopMode::Execution::InitDevelopMode(InitializationData &initData)
{
	InitializeCriticalSection(&logCS);
	devOSContext.Init(initData);
}
bool GApp::DevelopMode::Execution::OnTouchEvent(int eventType, int x, int y)
{
	return devOSContext.SpecificData.Ads.OnTouchEvent(eventType, x, y);
}
bool GApp::DevelopMode::Execution::StartApplication(GApp::UI::Application &App)
{
	AppContextData *AppCTX = new AppContextData();
	CHECK((App.Context = AppCTX) != NULL, false, "");
	LOG_INFO("Starting execution ....");
	LOG_INFO("");
	CHECK(InitializeOpenGLContext(), false, "Failed to initialize OpenGL Context !");	

	LoadSettingsFile(&devOSContext, false);
	if (devOSContext.SpecificData.EnableSounds) {
		CHECK(BassModeWrapper::Init(), false, "Failed to init bass library");
	}
	AppCTX->OS = &devOSContext;

	WNDCLASSEXA wndclass = { sizeof(WNDCLASSEXA), 0, WindowProcedure,
		0, 0, GetModuleHandle(0), LoadIcon(0, IDI_APPLICATION),
		LoadCursor(0, IDC_ARROW), HBRUSH(COLOR_WINDOW + 1),
		0, devOSContext.Data.Name, LoadIcon(0, IDI_APPLICATION) };

	CHECK(RegisterClassExA(&wndclass), false, "");

	RECT r, r2, desktopWorkingArea, desktopRect;
	r.left = 0;
	r.right = devOSContext.Data.Width;
	r.top = 0;
	r.bottom = devOSContext.Data.Height;

	DWORD winExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	CHECK(SystemParametersInfo(SPI_GETWORKAREA, 0, &desktopWorkingArea, 0), false, "");
	DWORD heightMax = desktopWorkingArea.bottom - desktopWorkingArea.top;
	DWORD widthMax = desktopWorkingArea.right - desktopWorkingArea.left;
	CHECK(GetClientRect(GetDesktopWindow(), &desktopRect), false, "Unable to read desktop size: Error = %d", GetLastError());
	DWORD desktopWidth = desktopRect.right - desktopRect.left;
	DWORD desktopHeight = desktopRect.bottom - desktopRect.top;
	int ScreenCX, ScreenCY;

	ScreenCX = (desktopWorkingArea.left + desktopWorkingArea.right) / 2;
	ScreenCY = (desktopWorkingArea.top + desktopWorkingArea.bottom) / 2;

	if (devOSContext.Data.Width == RESOLUTION_MODE_BESTFIT)
	{
		unsigned int bestSuprafata = 0, bestWidth = 0, bestHeight = 0;
		float bestRap = -1, cRap;
		CHECK(devOSContext.SpecificData.AvailableResolutionsCount / 2 >= 1, false, "No available resolutions supplied !");
		for (unsigned int tr = 0; tr < devOSContext.SpecificData.AvailableResolutionsCount; tr += 2)
		{
			unsigned int w = devOSContext.SpecificData.AvailableResolutions[tr];
			unsigned int h = devOSContext.SpecificData.AvailableResolutions[tr + 1];
			if ((w <= (desktopWorkingArea.right - desktopWorkingArea.left)) && (h <= (desktopWorkingArea.bottom - desktopWorkingArea.top)))
			{
				LOG_INFO("Found possible resolotuion: %d x %d", w, h);
				if ((w*h)>bestSuprafata)
				{
					bestSuprafata = w*h;
					devOSContext.Data.Width = w;
					devOSContext.Data.Height = h;
				}
			}
			else {
				cRap = GAC_MAX(((float)(desktopWorkingArea.right - desktopWorkingArea.left)) / w, ((float)(desktopWorkingArea.bottom - desktopWorkingArea.top)) / h);
				if (cRap >= 1.0f)
					cRap -= 1.0f;
				else
					cRap = 1.0f - cRap;
				if (bestRap<0)
				{
					bestRap = cRap;
					bestWidth = w;
					bestHeight = h;
				}
				else {
					if (cRap<bestRap)
					{
						bestRap = cRap;
						bestWidth = w;
						bestHeight = h;
					}
				}
			}
		}
		if (bestSuprafata == 0)
		{
			CHECK(bestRap >= 0, false, "Invalid resolutions suplied (nothing can be fit) !");
			LOG_INFO("Selecting resolution: %d x %d with factor: %lf", bestWidth, bestHeight, bestRap);
			devOSContext.Data.Width = bestWidth;
			devOSContext.Data.Height = bestHeight;
		}
		r.right = devOSContext.Data.Width;
		r.bottom = devOSContext.Data.Height;
	}

	if (devOSContext.Data.Width == RESOLUTION_MODE_FULLSCREEN)
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		CHECK(EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmScreenSettings), false, "Unable to read display settings !");
		LOG_INFO("Display settings: %d x %d with %d pixels", dmScreenSettings.dmPelsWidth, dmScreenSettings.dmPelsHeight, dmScreenSettings.dmBitsPerPel);
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		LONG result = ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		CHECK(result == DISP_CHANGE_SUCCESSFUL, false, "Unable to switch to full screen mode (result is 0x%08X)!", result);
		r.left = r.top = 0;
		r.right = devOSContext.Data.Width = desktopWidth;
		r.bottom = devOSContext.Data.Height = desktopHeight;
		ScreenCX = desktopWidth / 2;
		ScreenCY = desktopHeight / 2;
		winExStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;
		winStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}

	if (devOSContext.Data.Width == RESOLUTION_MODE_MAXIMIZEDWINDOW)
	{
		r2.left = r2.top = 0;
		r2.right = r2.bottom = 100;
		AdjustWindowRectEx(&r2, winStyle, FALSE, winExStyle);
		r.right = devOSContext.Data.Width = (widthMax - 2 - (r2.right - r2.left - 100)) & 0xFFFFFFFE; // par
		r.bottom = devOSContext.Data.Height = (heightMax - 2 - (r2.bottom - r2.top - 100)) & 0xFFFFFFFE; // par
	}
	AdjustWindowRectEx(&r, winStyle, FALSE, winExStyle);
	int pozX = ScreenCX - ((int)(r.right - r.left)) / 2;
	int pozY = ScreenCY - ((int)(r.bottom - r.top)) / 2;
	if ((devOSContext.SpecificData.X >= 0) && (devOSContext.SpecificData.Y >= 0))
	{
		pozX = devOSContext.SpecificData.X;
		pozY = devOSContext.SpecificData.Y;
	}

	LOG_INFO("Desktop: %d x %d", desktopWidth, desktopHeight);
	LOG_INFO("Desktop Working Area = (%x, %x - %d, %d)", desktopWorkingArea.left, desktopWorkingArea.top, desktopWorkingArea.right, desktopWorkingArea.bottom);
	LOG_INFO("Window Size : %d x %d", (r.right - r.left), (r.bottom - r.top));
	LOG_INFO("Application : X=%d,Y=%d Size:%d x %d", pozX, pozY, devOSContext.Data.Width, devOSContext.Data.Height);
	LOG_INFO("Desktop Working Area = (%x, %x - %d, %d)", desktopWorkingArea.left, desktopWorkingArea.top, desktopWorkingArea.right, desktopWorkingArea.bottom);
	CHECK((devOSContext.SpecificData.hWnd = CreateWindowExA(winExStyle, devOSContext.Data.Name, devOSContext.Data.Name, winStyle, pozX, pozY, r.right - r.left, r.bottom - r.top, 0, 0, GetModuleHandle(0), 0)) != NULL, false, "Unable to create Window: Error = %d", GetLastError());
	CHECK((devOSContext.SpecificData.hDC = GetDC(devOSContext.SpecificData.hWnd)) != NULL, false, "");


	float width_inch = (float)(GetDeviceCaps(devOSContext.SpecificData.hDC, HORZSIZE) * 0.0393700787);
	float width_px = GetDeviceCaps(devOSContext.SpecificData.hDC, HORZRES);
	devOSContext.Data.WidthInInch = devOSContext.Data.Width * width_inch / width_px;

	float height_inch = (float)(GetDeviceCaps(devOSContext.SpecificData.hDC, VERTSIZE) * 0.0393700787);
	float height_px = GetDeviceCaps(devOSContext.SpecificData.hDC, VERTRES);
	devOSContext.Data.HeightInInch = devOSContext.Data.Height * height_inch / height_px;

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  /* size */
		1,                              /* version */
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER,               /* support double-buffering */
		PFD_TYPE_RGBA,                  /* color type */
		32,                             /* prefered color depth */
		0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
		0,                              /* no alpha buffer */
		0,                              /* alpha bits (ignored) */
		0,                              /* no accumulation buffer */
		0, 0, 0, 0,                     /* accum bits (ignored) */
		16,                             /* depth buffer */
		0,                              /* no stencil buffer */
		0,                              /* no auxiliary buffers */
		PFD_MAIN_PLANE,                 /* main layer */
		0,                              /* reserved */
		0, 0, 0,                        /* no layer, visible, damage masks */
	};
	int pixelFormat;
	LOG_INFO("HWND: %p, HDC: %p", devOSContext.SpecificData.hWnd, devOSContext.SpecificData.hDC);
	CHECK((pixelFormat = ChoosePixelFormat(devOSContext.SpecificData.hDC, &pfd)) != 0, false, "");
	CHECK(SetPixelFormat(devOSContext.SpecificData.hDC, pixelFormat, &pfd), false, "");
	LOG_INFO("PixelFormat:%d", pixelFormat);


	CHECK((devOSContext.SpecificData.hRC = OpenGL_wglCreateContext(devOSContext.SpecificData.hDC)) != NULL, false, "");
	CHECK(OpenGL_wglMakeCurrent(devOSContext.SpecificData.hDC, devOSContext.SpecificData.hRC), false, "");
	LOG_INFO("hRC:%p", devOSContext.SpecificData.hRC);

	CHECK(InitializeOpenGLFunctions(), false, "Failed to initialize OpenGL Functions !");

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	LOG_INFO("Showing window -------------------------------------------");

	ShowWindow(devOSContext.SpecificData.hWnd, SW_SHOWDEFAULT);
	// set the timer to 60 FPS
	CHECK(SetTimer(devOSContext.SpecificData.hWnd, 1234, 1000 / requestedFPs, NULL) != 0, false, "");

	//glewInit(); // part of the old mode
	devOSContext.SpecificData.ResourceAssetHandle = GetResourceAssetHandle();
	// important sa fie inainte de App.Create(...)
	mainApp = &App;
	devOSContext.SpecificData.mainApp = mainApp;

	CHECK(App.Create(devOSContext), false, "Unable to create Application object");

	snapshootPixelBuffer = new unsigned char[devOSContext.Data.Width*devOSContext.Data.Height * 3];

	MSG msg;
	startRenderTime = GetTickCount();
	countFPS = 0;

	//while( GetMessage( &msg, 0, 0, 0 ) ) DispatchMessage(&msg) ;
	LOG_INFO("Starting message loop -------------------------------------------");
	while (GetMessage(&msg, NULL, 0, 0) == TRUE) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyOpenGLWindow();
	if (devOSContext.SpecificData.EnableSounds) {
		BassModeWrapper::Dispose();
	}
	return true;
}

void LockCS()
{
	EnterCriticalSection(&logCS);
}
void UnLockCS()
{
	LeaveCriticalSection(&logCS);
}

