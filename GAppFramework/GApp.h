#ifndef __G_APP_FRAMEWORK__
#define __G_APP_FRAMEWORK__

//[REMOVE-IN-DEVELOP-LIB:START]
//#define DYNAMIC_IMAGE_RESIZE
//#define RESIZE_AT_STARTUP_IMAGE_RESIZE
//[REMOVE-IN-DEVELOP-LIB:STOP]

#define TOUCH_FLAG_CONTROL_SENDEVENT_UP_AND_CLICK	0x00000001
#define TOUCH_FLAG_CONTROL_SENDEVENT_CLICK_AND_UP	0x00000002
#define TOUCH_FLAG_CONTROL_SENDEVENT_ONLY_UP		0x00000004
#define TOUCH_FLAG_CONTROL_SENDEVENT_UP_OR_CLICK	0x00000008
#define TOUCH_FLAG_CONTROL_SENDEVENT_MASK			0x0000000F

#define TOUCH_FLAG_SCENE_SENDEVENT_UP_AND_CLICK		0x00000010
#define TOUCH_FLAG_SCENE_SENDEVENT_CLICK_AND_UP		0x00000020
#define TOUCH_FLAG_SCENE_SENDEVENT_ONLY_UP			0x00000040
#define TOUCH_FLAG_SCENE_SENDEVENT_UP_OR_CLICK		0x00000080
#define TOUCH_FLAG_SCENE_SENDEVENT_MASK				0x000000F0

#define TOUCH_FLAG_ENABLE_ZOOM_EVENT				0x00000100



#define G						(((GApp::UI::CoreSystem *)CoreContext)->Graphics)
#define API						(((GApp::UI::CoreSystem *)CoreContext)->Api)
#define Res						(*((Resources*)(((GApp::UI::CoreSystem *)CoreContext)->Resources)))
#define I						((*((Resources*)(((GApp::UI::CoreSystem *)CoreContext)->Resources))).Images)
#define GD						(*((Global*)(((GApp::UI::CoreSystem *)CoreContext)->GlobalData)))
#define Core					(*((GApp::UI::CoreSystem *)CoreContext))
#define Settings				(((GApp::UI::CoreSystem *)CoreContext)->SettingsDB)
#define Audio					(((GApp::UI::CoreSystem *)CoreContext)->AudioInterface)
#define Analytics				(((GApp::UI::CoreSystem *)CoreContext)->AnalyticsObject)
#define ADS						((*((Resources*)(((GApp::UI::CoreSystem *)CoreContext)->Resources))).Ads)

#define CoreReference			GApp::UI::CoreSystem*
#define NULLREF(type)           (*((type *)NULL))
#define IS_NULLREF(obj)         ((&obj)==(NULL))


//[REMOVE-IN-DEVELOP-LIB:START]
#define PROTECTED_INTERNAL		public
#define PRIVATE_INTERNAL		public

#define ALPHA(x)				(((float)(x>>24))/255.0f)
#define RED(x)					(((float)((x>>16) & 0xFF))/255.0f)
#define GREEN(x)				(((float)((x>>8) & 0xFF))/255.0f)
#define BLUE(x)					(((float)(x & 0xFF))/255.0f)
//[REMOVE-IN-DEVELOP-LIB:STOP]

#define COLOR_RGB(r,g,b)		((((unsigned int)((r) & 0xFF))<<16)|(((unsigned int)((g) & 0xFF))<<8)|((unsigned int)((b) & 0xFF))|0xFF000000)
#define COLOR_ARGB(a,r,g,b)		((((unsigned int)((a) & 0xFF))<<24)|(((unsigned int)((r) & 0xFF))<<16)|(((unsigned int)((g) & 0xFF))<<8)|((unsigned int)((b) & 0xFF)))
#define COLOR_ALPHA(a,rgb)		((((unsigned int)((a) & 0xFF))<<24)|((unsigned int)((rgb)&0xFFFFFF)))

#define GET_ALPHA(x)			(((x)>>24) & 0xFF)
#define GET_RED(x)				(((x)>>16) & 0xFF)
#define GET_GREEN(x)			(((x)>>8) & 0xFF)
#define GET_BLUE(x)				(((x)) & 0xFF)


#define COLOR_Transparent		0
#define COLOR_Red				0xFFFF0000
#define COLOR_DarkRed			0xFF800000
#define COLOR_Green				0xFF00FF00
#define COLOR_DarkGreen			0xFF008000
#define COLOR_Blue				0xFF0000FF
#define COLOR_DarkBlue			0xFF000080
#define COLOR_White				0xFFFFFFFF
#define COLOR_Black				0xFF000000
#define COLOR_Gray				0xFF808080
#define COLOR_DarkGray			0xFF404040
#define COLOR_LightGray			0xFFC0C0C0
#define COLOR_Yellow			0xFFFFFF00
#define COLOR_Brown				0xFF808000
#define COLOR_Pink				0xFFFF00FF
#define COLOR_Mangenta			0xFF800080
#define COLOR_Azure				0xFF00FFFF
#define COLOR_Tan				0xFF008080
#define COLOR_Gray10			0xFF191919
#define COLOR_Gray20			0xFF323232
#define COLOR_Gray30			0xFF4B4B4B
#define COLOR_Gray40			0xFF646464
#define COLOR_Gray50			0xFF808080
#define COLOR_Gray60			0xFF999999
#define COLOR_Gray70			0xFFB2B2B2
#define COLOR_Gray80			0xFFCCCCCC
#define COLOR_Gray90			0xFFE5E5E5
#define COLOR_Gray100			0xFFFFFFFF

#define GAC_TYPE_MATRIXFLOAT	float

#define GAC_VERSION_GET_MAJOR(v)	(((v)>>24)&0xFF)
#define GAC_VERSION_GET_MINOR(v)	(((v)>>16)&0xFF)
#define GAC_VERSION_GET_BUILD(v)	(((v))&0xFFFF)
#define GAC_CREATE_VERSION(mj,mi,b)	((((mj)&0xFF)<<24)|(((mi)&0xFF)<<16)|((b)&0xFFFF))

#define MAX_SCENE_TIMERS				16
#define RESOLUTION_MODE_MAXIMIZEDWINDOW	100000
#define RESOLUTION_MODE_FULLSCREEN		100001
#define RESOLUTION_MODE_BESTFIT			100002

//[REMOVE-IN-DEVELOP-LIB:START]
#define SYSEVENT_PAUSE					0
#define SYSEVENT_RESUME					1
#define SYSEVENT_TERMINATE				2
#define SYSEVENT_SAVE_STATE				3
#define SYSEVENT_TOUCH_UP				4
#define SYSEVENT_TOUCH_DOWN				5
#define SYSEVENT_TOUCH_MOVE				6
#define SYSEVENT_KEY_DOWN				7
#define SYSEVENT_KEY_UP					8
#define SYSEVENT_BILLING				9
#define SYSEVENT_ADVERTISING			10
#define SYSEVENT_SERVICES				11
//[REMOVE-IN-DEVELOP-LIB:STOP]

#define GAC_MIN(x,y)					((x)<(y)?(x):(y))
#define GAC_MAX(x,y)					((x)>(y)?(x):(y))
#define GAC_ABS(x)						((x)>=0?(x):(-(x)))
#define SET_BIT(value,mask)				{ value |= (mask);     }
#define REMOVE_BIT(value,mask)			{ value &= (~(mask)) ; }
#define DISTANCE(x1,y1,x2,y2)			((float)(sqrt((float)(((x1)-(x2))*((x1)-(x2)) + ((y1)-(y2))*((y1)-(y2))))))
#define SQUAREDDIST(x1,y1,x2,y2)		(((x1)-(x2))*((x1)-(x2)) + ((y1)-(y2))*((y1)-(y2)))
#define FLOATEQAPROX(x,y,aprox)			((bool)(fabs((x)-(y))<=aprox))
#define FLOAT32EQ(x,y)					FLOATEQAPROX((x),(y),0.000001f)
#define FLOAT64EQ(x,y)					FLOATEQAPROX((x),(y),0.0000000001)
#define ADJUST_TO_INTERVAL(x,min,max)	((x)<(min)?(min):((x)>(max)?(max):(x)))

#define TEST_BUY_ITEM			"<default>"

//[REMOVE-IN-DEVELOP-LIB:START]
#define SET_BITFIELD(value,startBit,bitsCount,newValue) { value = ((value) & (~(((1<<(bitsCount))-1)<<(startBit)))) | (((newValue) & ((1<<(bitsCount))-1))<<(startBit)); }
#define GET_BITFIELD(value,startBit,bitsCount)  (((value)>>(startBit)) & ((1<<(bitsCount))-1)) 

#define PAINT_OBJECT_VALID		0x80000000
#define PAINT_OBJECT_NO_RESIZE	0x40000000
#define PAINT_OBJECT_MASK		0x0FFFFFFF
//[REMOVE-IN-DEVELOP-LIB:STOP]
// ====================================================================================================================================================== COORDINATES =================
#define GAC_COORDINATES_PIXELS						0
#define GAC_COORDINATES_PERCENTAGE					1
#define GAC_COORDINATES_AUTO						2
#define GAC_COORDINATES_PERCENTAGEOFSCREENWIDTH		3
#define GAC_COORDINATES_PERCENTAGEOFSCREEHEIGHT		4
#define GAC_COORDINATES_DRAWINGBOUNDS				5

#define GAC_TYPE_COORDINATES						unsigned int
//[REMOVE-IN-DEVELOP-LIB:START]
#define TRANSLATE_COORD_2(res_x,res_y,x,y,scale_width,scale_height,parent_left,parent_top,parent_right,parent_bottom,type) {\
	switch (type) { \
		case GAC_COORDINATES_PIXELS: res_x=((x)*(scale_width))+parent_left;res_y=parent_top-((y)*(scale_height)); break; \
		case GAC_COORDINATES_PERCENTAGE: res_x = (x)*((parent_right)-(parent_left))+(parent_left); res_y=(y)*((parent_top)-(parent_bottom))+(parent_bottom); break; \
		default: res_x = x;res_y = y; break; \
		}; \
	}

#define TRANSLATE_COORD_TO_PIXELS(res_x,res_y,x,y,parent_width_pixels,parent_height_pixels,type) {\
	switch (type) { \
		case GAC_COORDINATES_PERCENTAGE: res_x = (x)*(parent_width_pixels); res_y = (y)*(parent_height_pixels); break; \
		default: res_x = (x); res_y = (y); break; \
	}; \
}
#define IS_CONTROL_AUTOSIZED (((GAC_TYPE_COORDINATES)((Layout.Flags >>8) & 0x0F))==GAC_COORDINATES_AUTO)
//[REMOVE-IN-DEVELOP-LIB:STOP]
// ====================================================================================================================================================== ALIGNAMENT ==================

#define GAC_ALIGNAMENT_TOPLEFT			0
#define GAC_ALIGNAMENT_TOPCENTER		1
#define GAC_ALIGNAMENT_TOPRIGHT			2
#define GAC_ALIGNAMENT_RIGHTCENTER		3
#define GAC_ALIGNAMENT_BOTTOMRIGHT		4
#define GAC_ALIGNAMENT_BOTTOMCENTER		5
#define GAC_ALIGNAMENT_BOTTOMLEFT		6
#define GAC_ALIGNAMENT_LEFTCENTER		7
#define GAC_ALIGNAMENT_CENTER			8
#define GAC_TYPE_ALIGNAMENT				unsigned int

//[REMOVE-IN-DEVELOP-LIB:START]
#define ALIGN_COORD(res_x,res_y,x,y,width,height,align_type) {\
	switch (align_type) { \
		case GAC_ALIGNAMENT_TOPLEFT: res_x = (x); res_y = (y); break; \
		case GAC_ALIGNAMENT_TOPCENTER: res_x = (x) - (width)/2.0f; res_y = (y); break; \
		case GAC_ALIGNAMENT_TOPRIGHT: res_x = (x) - (width); res_y = (y); break; \
		case GAC_ALIGNAMENT_RIGHTCENTER: res_x = (x) - (width); res_y = (y) - (height)/2.0f; break; \
		case GAC_ALIGNAMENT_BOTTOMRIGHT: res_x = (x) - (width); res_y = (y) - (height); break; \
		case GAC_ALIGNAMENT_BOTTOMCENTER: res_x = (x) - (width)/2.0f;res_y = (y) - (height); break; \
		case GAC_ALIGNAMENT_BOTTOMLEFT: res_x = (x); res_y = (y) - (height); break; \
		case GAC_ALIGNAMENT_LEFTCENTER: res_x = (x); res_y = (y) - (height)/2.0f; break; \
		case GAC_ALIGNAMENT_CENTER: res_x = (x) - (width)/2.0f; res_y = (y) - (height)/2.0f; break; \
		default: res_x = (x); res_y = (y); break; \
		}; \
	}

#define ALIGN_OBJECT_TO_RECT(res_x,res_y,left,top,right,bottom,object_width,object_height,align_type) {\
	switch (align_type) { \
		case GAC_ALIGNAMENT_TOPLEFT: res_x = (left); res_y = (top); break; \
		case GAC_ALIGNAMENT_TOPCENTER: res_x = ((left)+(right)-(object_width))/2.0f; res_y = (top); break; \
		case GAC_ALIGNAMENT_TOPRIGHT: res_x = (right) - (object_width); res_y = (top); break; \
		case GAC_ALIGNAMENT_RIGHTCENTER: res_x = (right) - (object_width); res_y = ((top)+(bottom)-(object_height))/2.0f; break; \
		case GAC_ALIGNAMENT_BOTTOMRIGHT: res_x = (right) - (object_width); res_y = (bottom) - (object_height); break; \
		case GAC_ALIGNAMENT_BOTTOMCENTER: res_x = ((left)+(right)-(object_width))/2.0f;  res_y = (bottom) - (object_height); break; \
		case GAC_ALIGNAMENT_BOTTOMLEFT: res_x = (left); res_y = (bottom) - (object_height); break; \
		case GAC_ALIGNAMENT_LEFTCENTER: res_x = (left); res_y = ((top)+(bottom)-(object_height))/2.0f; break; \
		case GAC_ALIGNAMENT_CENTER: res_x = ((left)+(right)-(object_width))/2.0f; res_y = ((top)+(bottom)-(object_height))/2.0f; break; \
		default: res_x = (left); res_y = (top); break; \
		}; \
	}
//[REMOVE-IN-DEVELOP-LIB:STOP]
// ====================================================================================================================================================== TouchEventType ==================
#define GAC_TOUCHEVENTTYPE_NONE			0
#define GAC_TOUCHEVENTTYPE_DOWN			1
#define GAC_TOUCHEVENTTYPE_MOVE			2
#define GAC_TOUCHEVENTTYPE_UP			3
#define GAC_TOUCHEVENTTYPE_CLICK		4		
#define GAC_TOUCHEVENTTYPE_ZOOM			5
#define GAC_TOUCHEVENTTYPE_STARTZOOM	6
#define GAC_TOUCHEVENTTYPE_ENDZOOM		7
#define GAC_TYPE_TOUCHEVENTTYPE			unsigned int

// ====================================================================================================================================================== Language ==================
#define GAC_LANGUAGE_AFRIKAANS		0
#define GAC_LANGUAGE_ALBANIAN		1
#define GAC_LANGUAGE_ARABIC			2
#define GAC_LANGUAGE_ARMENIAN		3
#define GAC_LANGUAGE_AZERBAIJANI	4
#define GAC_LANGUAGE_BASQUE			5
#define GAC_LANGUAGE_BELARUSIAN		6
#define GAC_LANGUAGE_BENGALI		7
#define GAC_LANGUAGE_BOSNIAN		8
#define GAC_LANGUAGE_BULGARIAN		9
#define GAC_LANGUAGE_CATALAN		10
#define GAC_LANGUAGE_CEBUANTO		11
#define GAC_LANGUAGE_CHINESE		12
#define GAC_LANGUAGE_CROATIAN		13
#define GAC_LANGUAGE_CZECH			14
#define GAC_LANGUAGE_DANISH			15
#define GAC_LANGUAGE_DUTCH			16
#define GAC_LANGUAGE_ENGLISH		17
#define GAC_LANGUAGE_ESPERANTO		18
#define GAC_LANGUAGE_ESTONIAN		19
#define GAC_LANGUAGE_FILIPINO		20
#define GAC_LANGUAGE_FINNISH		21
#define GAC_LANGUAGE_FRENCH			22
#define GAC_LANGUAGE_GALICIAN		23
#define GAC_LANGUAGE_GEORGIAN		24
#define GAC_LANGUAGE_GERMAN			25
#define GAC_LANGUAGE_GREEK			26
#define GAC_LANGUAGE_GUJARATI		27
#define GAC_LANGUAGE_HAITIANCREOLE	28
#define GAC_LANGUAGE_HEBREW			29
#define GAC_LANGUAGE_HINDI			30
#define GAC_LANGUAGE_HMONG			31
#define GAC_LANGUAGE_HUNGARIAN		32
#define GAC_LANGUAGE_ICELANDIC		33
#define GAC_LANGUAGE_INDONESIAN		34
#define GAC_LANGUAGE_IRISH			35
#define GAC_LANGUAGE_ITALIAN		36
#define GAC_LANGUAGE_JAPANESE		37
#define GAC_LANGUAGE_JAVANESE		38
#define GAC_LANGUAGE_KANNADA		39
#define GAC_LANGUAGE_KHMER			40
#define GAC_LANGUAGE_LAO			41
#define GAC_LANGUAGE_LATIN			42
#define GAC_LANGUAGE_LATVIAN		43
#define GAC_LANGUAGE_LITHUANIAN		44
#define GAC_LANGUAGE_MECEDONIAN		45
#define GAC_LANGUAGE_MALAY			46
#define GAC_LANGUAGE_MALTESE		47
#define GAC_LANGUAGE_MARATHI		48
#define GAC_LANGUAGE_NORWEGIAN		49
#define GAC_LANGUAGE_PERSIAN		50
#define GAC_LANGUAGE_POLISH			51
#define GAC_LANGUAGE_PORTUGUESE		52
#define GAC_LANGUAGE_ROMANIAN		53
#define GAC_LANGUAGE_RUSSIAN		54
#define GAC_LANGUAGE_SERBIAN		55
#define GAC_LANGUAGE_SLOVAK			56
#define GAC_LANGUAGE_SLOVENIAN		57
#define GAC_LANGUAGE_SPANISH		58
#define GAC_LANGUAGE_SWAHILI		59
#define GAC_LANGUAGE_SWEDISH		60
#define GAC_LANGUAGE_TAMIL			61
#define GAC_LANGUAGE_TELEGU			62
#define GAC_LANGUAGE_THAI			63
#define GAC_LANGUAGE_TURKISH		64
#define GAC_LANGUAGE_UKRAINIAN		65
#define GAC_LANGUAGE_URDU			66
#define GAC_LANGUAGE_VIETNAMESE		67
#define GAC_LANGUAGE_WELSH			68
#define GAC_LANGUAGE_YIDDISH		69
#define GAC_TYPE_LANGUAGE			unsigned int

// ====================================================================================================================================================== EventType ==================
#define GAC_EVENTTYPE_CLICK								0
#define GAC_EVENTTYPE_VALUECHANGED						1
#define GAC_EVENTTYPE_PAGECHANGED						2
#define GAC_EVENTTYPE_PRESENTATIONBOOKMARKCHANGED		3
#define GAC_EVENTTYPE_PRESENTATIONSTARTED				4
#define GAC_EVENTTYPE_PRESENTATIONENDED					5
#define GAC_EVENTTYPE_PRESENTATIONCURENTFRAMECHANGED	6
#define GAC_EVENTTYPE_SLIDERCELLCHANGED					7
#define GAC_EVENTTYPE_SLIDERSCROLLUPDATED				8
#define GAC_EVENTTYPE_MODALFRAMECLOSED					9
#define GAC_EVENTTYPE_MODALFRAME_BEFORE_CLOSE			10
#define GAC_EVENTTYPE_POPUPCLICK						11
#define GAC_EVENTTYPE_ANIMATIONSTARTED					12
#define GAC_EVENTTYPE_ANIMATIONENDED					13
#define GAC_EVENTTYPE_ANIMATIONPAUSED					14
#define GAC_EVENTTYPE_ANIMATIONRESUMED					15
#define GAC_TYPE_EVENTTYPE								unsigned int

// ====================================================================================================================================================== Resample Method ==================
#define GAC_RESAMPLEMETHOD_NEARESTNEIGHBOR		0
#define GAC_RESAMPLEMETHOD_BILINEAR				1
#define GAC_RESAMPLEMETHOD_BICUBIC				2
#define GAC_RESAMPLEMETHOD_GAUSSIAN				3
#define GAC_RESAMPLEMETHOD_LANCZOS				4
#define GAC_RESAMPLEMETHOD_AVERAGE_SLOW			5
#define GAC_RESAMPLEMETHOD_AVERAGE_FAST			6
#define GAC_TYPE_RESAMPLEMETHOD					unsigned int

// ====================================================================================================================================================== ColorBlend ==================
#define GAC_COLORBLEND_NONE						0
#define GAC_COLORBLEND_XOR						1
#define GAC_COLORBLEND_ALPHA					2
#define GAC_COLORBLEND_TRANSPARENTCOLOR			3
#define GAC_COLORBLEND_CALLBACK					4
#define GAC_COLORBLEND_GRAYSCALE				5
#define GAC_TYPE_COLORBLENDTYPE					unsigned int

// ====================================================================================================================================================== ImageResizeMdoe ==================
#define GAC_IMAGERESIZEMODE_NONE		0
#define GAC_IMAGERESIZEMODE_DOCK		1
#define GAC_IMAGERESIZEMODE_FILL		2
#define GAC_IMAGERESIZEMODE_FIT			3
#define GAC_IMAGERESIZEMODE_SHRINK		4			
#define GAC_TYPE_IMAGERESIZEMODE		unsigned int

// ====================================================================================================================================================== EncryptionType ==================
#define GAC_ENCRYPTION_NONE				0		
#define GAC_TYPE_ENCRYPTION				unsigned int
// ====================================================================================================================================================== Change Audio Settings ===========
#define GAC_CHANGEAUDIOSETTINGSMETHOD_ONOFF			0
#define GAC_CHANGEAUDIOSETTINGSMETHOD_ONSFXOFF		1
#define GAC_CHANGEAUDIOSETTINGSMETHOD_SFX_ONOFF		2
#define GAC_CHANGEAUDIOSETTINGSMETHOD_MUSIC_ONOFF	3
#define GAC_TYPE_CHANGEAUDIOSETTINGSMETHOD		unsigned int
// ====================================================================================================================================================== Font Type Size ===========
#define GAC_FONTSIZETYPE_SCALE					0
#define GAC_FONTSIZETYPE_PIXELS					1
#define GAC_FONTSIZETYPE_PERCENTAGEOFVIEWHEIGHT	2
#define GAC_FONTSIZETYPE_SHRINKTOFITVIEWWIDTH	3
#define GAC_FONTSIZETYPE_SCALETOFITVIEWWIDTH	4
#define GAC_TYPE_FONTSIZETYPE					unsigned int
// ====================================================================================================================================================== ImageTextRelations===============
#define GAC_IMAGETEXTRELATION_NONE								0
#define GAC_IMAGETEXTRELATION_IMAGEONLEFTOFTHETEXT				1
#define GAC_IMAGETEXTRELATION_IMAGEONRIGHTOFTHETEXT				2
#define GAC_IMAGETEXTRELATION_IMAGEONTOPOFTHETEXT				3
#define GAC_IMAGETEXTRELATION_IMAGEONBOTTOMOFTHETEXT			4
#define GAC_TYPE_IMAGETEXTRELATION								unsigned int
// ====================================================================================================================================================== Scenes ==========================
#define MAX_SCENES					64

#define GAC_SCENES_MAIN				1
#define GAC_SCENES_SELECTSEASON		2
#define GAC_SCENES_SELECTLEVEL		3
#define GAC_SCENES_RATE				4
#define GAC_SCENES_FINAL			5
#define GAC_SCENES_GAME				6
#define GAC_SCENES_TUTORIAL			7
#define GAC_SCENES_CREDENTIALS		8
#define GAC_SCENES_WHATSNEW			9
#define GAC_SCENES_OPTIONS			10
#define GAC_SCENES_COMERCIAL		11
#define GAC_SCENES_FIRSTTIMERUN		12
#define GAC_SCENES_APP_ERROR		13

//#define SCENE_FLAGS_RESTORE_STATE	0x000001
//#define SCENE_FLAGS_RATE			0x000002
//#define SCENE_FLAGS_COMERCIAL		0x000004
// ====================================================================================================================================================== Keyboard ==========================
#define GAC_KEYBOARD_CODE_BACK		0x0010000
#define GAC_KEYBOARD_FLAG_DOWN		0x01000000
#define GAC_KEYBOARD_FLAG_ALT		0x02000000
#define GAC_KEYBOARD_FLAG_CTRL		0x04000000
#define GAC_KEYBOARD_FLAG_SHIFT		0x08000000
// ====================================================================================================================================================== Profile =======================
#define GAC_PROFILE_GLOBAL				0
#define GAC_PROFILE_SCENE				1
#define GAC_PROFILE_LOCAL				2
#define GAC_PROFILETYPE_COUNT			3
// ====================================================================================================================================================== Resource Flags =======================
#define GAC_RESOURCEFLAG_NONE			(0x00000000)
#define GAC_RESOURCEFLAG_IMAGE			(0x00000001)
#define GAC_RESOURCEFLAG_SOUND			(0x00000002)
#define GAC_RESOURCEFLAG_FONT			(0x00000004)
#define GAC_RESOURCEFLAG_RAW			(0x00000008)
#define GAC_RESOURCEFLAG_ANIMATION		(0x00000010)
#define GAC_RESOURCEFLAG_SHADER			(0x00000020)
#define GAC_RESOURCEFLAG_LOCALCOPY		(0x00100000)
#define GAC_RESOURCEFLAG_RESIZEMODE		(0xFF000000)

#define GAC_RESIZEMODE_DYNAMIC			(0x00000000)
#define GAC_RESIZEMODE_BILINEAR			(0x01000000)
// ====================================================================================================================================================== LOGO =======================
#define GAC_SPLASHSCREEN_NORESIZE		0
#define GAC_SPLASHSCREEN_FILL			1
#define GAC_SPLASHSCREEN_FIT			2
// ====================================================================================================================================================== Button State =======================
#define GAC_BUTTON_STATE_ALL				0
#define GAC_BUTTON_STATE_NORMAL				1
#define GAC_BUTTON_STATE_PRESSED			2
#define GAC_BUTTON_STATE_INACTIVE			3
#define GAC_BUTTON_STATE_NORMAL_PRESSED		4

#define GAC_BUTTON_STATE_TYPE				unsigned int


// ====================================================================================================================================================== CheckBox State =======================
#define GAC_CHECKBOX_STATE_ALL						0
#define GAC_CHECKBOX_STATE_NORMAL_AND_CHECKED		1
#define GAC_CHECKBOX_STATE_NORMAL_AND_UNCHECKED		2
#define GAC_CHECKBOX_STATE_PRESSED_AND_CHECKED		3
#define GAC_CHECKBOX_STATE_PRESSED_AND_UNCHECKED	4
#define GAC_CHECKBOX_STATE_INACTIVE_AND_CHECKED		5
#define GAC_CHECKBOX_STATE_INACTIVE_AND_UNCHECKED	6
#define GAC_CHECKBOX_STATE_CHECKED					7
#define GAC_CHECKBOX_STATE_UNCHECKED				8
#define GAC_CHECKBOX_STATE_CHECKED_NORMAL_PRESSED	9
#define GAC_CHECKBOX_STATE_UNCHECKED_NORMAL_PRESSED	10
#define GAC_CHECKBOX_STATE_NORMAL					11
#define GAC_CHECKBOX_STATE_PRESSED					12
#define GAC_CHECKBOX_STATE_INACTIVE					13
#define GAC_CHECKBOX_STATE_NORMAL_PRESSED			14

#define GAC_CHECKBOX_STATE_TYPE						unsigned int

// ====================================================================================================================================================== Scene Counters =======================
#define GAC_SCENE_COUNTER_RATE				0
#define GAC_SCENE_COUNTER_COMERCIAL			1
#define GAC_MAX_SCENE_COUNTERS				2

// ====================================================================================================================================================== BILLING State =======================
#define GAC_PURCHASE_STATUS_COMPLETED			0
#define GAC_PURCHASE_STATUS_CANCELED			1
#define GAC_PURCHASE_STATUS_ERROR				2

#define GAC_PURCHASE_STATUS_TYPE				unsigned int

// ====================================================================================================================================================== Ad Event Type =======================
#define GAC_ADEVENT_LOADED						0
#define GAC_ADEVENT_REWARDED					1
#define GAC_ADEVENT_OPENAD						2
#define GAC_ADEVENT_UNLOAD						3
#define GAC_ADEVENT_FAILTOLOAD					4
#define GAC_ADEVENT_VISIBLE						5
#define GAC_ADEVENT_HIDDEN						6

#define GAC_ADEVENT_TYPE						unsigned int

// ====================================================================================================================================================== Ad Event Type =======================
#define GAC_ADTYPE_BANNER						0
#define GAC_ADTYPE_INTERSTITIAL					1
#define GAC_ADTYPE_REWARDABLE					2
#define GAC_ADTYPE_NATIVE						3
#define GAC_ADTYPE_INPLAY						4

// ====================================================================================================================================================== Analytics Type =======================
#define GAC_ANALYTICS_EVENT_SIMPLE				0
#define GAC_ANALYTICS_EVENT_DOUBLE				1
#define GAC_ANALYTICS_EVENT_INT64				2
#define GAC_ANALYTICS_EVENT_INT32				3
#define GAC_ANALYTICS_EVENT_BOOL				4

#define GAC_ANALYTICS_EVENT_TYPE				unsigned int

#define GAC_ANALYTICS_STD_EVENT_FB_BUTTON					0
#define GAC_ANALYTICS_STD_EVENT_FB_REWARD_YES				1
#define GAC_ANALYTICS_STD_EVENT_FB_REWARD_NO				2
#define GAC_ANALYTICS_STD_EVENT_FB_POPUP_YES				3
#define GAC_ANALYTICS_STD_EVENT_FB_POPUP_NO					4
#define GAC_ANALYTICS_STD_EVENT_SHARE_BUTTON				5
#define GAC_ANALYTICS_STD_EVENT_SHARE_REWARD_YES			6
#define GAC_ANALYTICS_STD_EVENT_SHARE_REWARD_NO				7
#define GAC_ANALYTICS_STD_EVENT_SHARE_POPUP_YES				8
#define GAC_ANALYTICS_STD_EVENT_SHARE_POPUP_NO				9
#define GAC_ANALYTICS_STD_EVENT_RATE_BUTTON					10
#define GAC_ANALYTICS_STD_EVENT_RATE_REWARD_YES				11
#define GAC_ANALYTICS_STD_EVENT_RATE_REWARD_NO				12
#define GAC_ANALYTICS_STD_EVENT_RATE_POPUP_YES				13
#define GAC_ANALYTICS_STD_EVENT_RATE_POPUP_NO				14
//#define GAC_ANALYTICS_STD_EVENT_ACCEPT_REWARD				10
//#define GAC_ANALYTICS_STD_EVENT_DENY_REWARD					11
//#define GAC_ANALYTICS_STD_EVENT_TIMEOUT_REWARD				12




#define GAC_ANALYTICS_STD_EVENT_TYPE			unsigned int


// ====================================================================================================================================================== Services Type =======================
#define GAC_SERVICE_GOOGLE_PLAY_SERVICE					0

#define GAC_SERVICE_TYPE								unsigned int
// ====================================================================================================================================================== Services Event Type =======================
#define GAC_SERVICES_EVENT_CONNECTED					0
#define GAC_SERVICES_EVENT_DISCONNECTED					1
#define GAC_SERVICES_EVENT_CURRENT_PLAYER_SCORE			2

#define GAC_SERVICES_EVENT_TYPE							unsigned int

// ====================================================================================================================================================== Counter method =======================
#define GAC_COUNTER_NONE						0
#define GAC_COUNTER_CIRCULAR					1
#define GAC_COUNTER_MIN_MAX						2
#define GAC_COUNTER_MIN							3
#define GAC_COUNTER_MAX							4
#define GAC_COUNTER_KEEP_VALUE_CONST			5
#define GAC_COUNTER_TYPE						unsigned int
#define GAC_CUSTOM_COUNTER_MAXITEMS				32

#define GAC_COUNTER_FLAG_ASCENDENT							0x00000100
#define GAC_COUNTER_FLAG_DESCENDENT							0x00000200
#define GAC_COUNTER_FLAG_START_FROM_MAX						0x00000400
#define GAC_COUNTER_FLAG_START_FROM_MIN						0x00000800
#define GAC_COUNTER_FLAG_IGNORE_MAX_MARGIN_NOTIFICATION		0x00001000
#define GAC_COUNTER_FLAG_IGNORE_MIN_MARGIN_NOTIFICATION		0x00002000

// ====================================================================================================================================================== Blending modes ===========================
#define BLEND_IMAGE_MODE_NONE		0
#define BLEND_IMAGE_MODE_ALPHA		1
#define BLEND_IMAGE_MODE_COLOR		2

// ====================================================================================================================================================== Snapshot type ===========================
#define GAC_SNAPSHOT_TYPE_NONE				0
#define GAC_SNAPSHOT_TYPE_ONLYGLOBALDATA	1
#define GAC_SNAPSHOT_TYPE_FULL				2

// ====================================================================================================================================================== APP_HAMDLER ===========================
#define GAC_APP_HANDLER_FACEBOOK_PAGE		0
#define GAC_APP_HANDLER_TWITTER_PAGE		1
#define GAC_APP_HANDLER_INSTAGRAM_PAGE		2
#define GAC_APP_HANDLER_YOUTUBE_CHANNAL		3

// ====================================================================================================================================================== GROUP_COUNTER_STARTTIMER_MEETHOD ===========================
#define COUNTER_GROUP_STARTTIME_METHOD_NONE				0
#define COUNTER_GROUP_STARTTIME_METHOD_ON_APP_STARTS	1
#define COUNTER_GROUP_STARTTIME_METHOD_ON_EVERY_SCENE	2
#define COUNTER_GROUP_STARTTIME_METHOD_ON_SCENE			3

// ====================================================================================================================================================== AUTO_ENABLE_STATES ===========================
#define AUTO_ENABLE_STATE_FB_APP_EXISTS		0
#define AUTO_ENABLE_STATE_ACTIONS_BIGGER	1
#define AUTO_ENABLE_STATE_ACTIONS_LESS		2
#define AUTO_ENABLE_STATE_AD_IS_LOADED		3
#define AUTO_ENABLE_STATE_ADS_ARE_ENABLED	4
#define AUTO_ENABLE_STATE_ADS_ARE_DISABLED	5

#define MAX_AUTO_ENABLE_STATE_CONDITIONS	5


// ====================================================================================================================================================== Alarm state ===========
#define GAC_ALARMSTATE_NONE						0
#define GAC_ALARMSTATE_TRIGGERED				1
#define GAC_ALARMSTATE_CLOSED					2
#define GAC_ALARMSTATE_CLOSED_AND_TRIGGER		3
#define GAC_TYPE_ALARMSTATE						unsigned int

#define GAC_ALARM_NOT_TRIGGERED					0

//[REMOVE-IN-DEVELOP-LIB:START]
#ifdef PLATFORM_DEVELOP
//[REMOVE-IN-DEVELOP-LIB:STOP]

	//#define ENABLE_ERROR_LOGGING
	//#define ENABLE_INFO_LOGGING
	//#define ENABLE_EVENT_LOGGING
	//#define SAVE_TEXTURES

	#define RESIZE_AT_STARTUP_IMAGE_RESIZE

	//#include "glew.h"
	//#include <gl\gl.h>			// Header File For The OpenGL32 Library
	//#include <gl\glu.h>			// Header File For The GLu32 Library
	#include "stdio.h"
	#include <stdlib.h>
	#include <math.h>
	#include <Windows.h>

	// tipuri de date OpenGL
	typedef unsigned int GLenum;
	typedef unsigned int GLbitfield;
	typedef unsigned int GLuint;
	typedef int GLint;
	typedef int GLsizei;
	typedef unsigned char GLboolean;
	typedef signed char GLbyte;
	typedef short GLshort;
	typedef unsigned char GLubyte;
	typedef unsigned short GLushort;
	typedef unsigned long GLulong;
	typedef float GLfloat;
	typedef float GLclampf;
	typedef double GLdouble;
	typedef double GLclampd;
	typedef void GLvoid;

//[REMOVE-IN-DEVELOP-LIB:START]
	#include "WinOpenGL.h"
//[REMOVE-IN-DEVELOP-LIB:STOP]

	#define Int8		char
	#define UInt8		unsigned char
	#define Int16		short
	#define UInt16		unsigned short
	#define Int32		int
	#define UInt32		unsigned int
	#define Int64		__int64
	#define UInt64		unsigned __int64
	#define CHARACTER	char
	#define Float32		float
	#define Float64		double
	#define Bool		bool
	
	#define FUNCTION_CALL_TYPE		__stdcall	
	#define SNPRINTF				_snprintf
	#define PRINT					printf
	
	#define	FLUSH_OUTPUT			{ printf("\n"); fflush(stdout); }

	#define CONSOLE_COLOR(color)	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	void _declspec(dllexport) LockCS();
	void _declspec(dllexport) UnLockCS();
//[REMOVE-IN-DEVELOP-LIB:START]	
	#define EXPORT				_declspec(dllexport)	
//[REMOVE-IN-DEVELOP-LIB:STOP]	
//[REMOVE-IN-DEVELOP-LIB:START]
#ifdef ENABLE_ERROR_LOGGING
//[REMOVE-IN-DEVELOP-LIB:STOP]
#define LOG_ERROR(...)	{ LockCS();CONSOLE_COLOR(12);printf("[LOG-ERROR] ");printf(__VA_ARGS__); FLUSH_OUTPUT;UnLockCS(); }
//[REMOVE-IN-DEVELOP-LIB:START]
#else
	#define LOG_ERROR(...)
#endif

#ifdef ENABLE_INFO_LOGGING
//[REMOVE-IN-DEVELOP-LIB:STOP]
#define LOG_INFO(...)	{ LockCS();CONSOLE_COLOR(7);printf("[LOG-INFO ] ");printf(__VA_ARGS__); FLUSH_OUTPUT;UnLockCS(); }
//[REMOVE-IN-DEVELOP-LIB:START]
#else
	#define LOG_INFO(...)
#endif

#ifdef ENABLE_EVENT_LOGGING
#define LOG_EVENT(...)	{ LockCS();CONSOLE_COLOR(15);printf("[  EVENT  ] ");printf(__VA_ARGS__); FLUSH_OUTPUT;UnLockCS(); }
#else
	#define LOG_EVENT(...)
#endif
//[REMOVE-IN-DEVELOP-LIB:STOP]

#define DEBUGMSG(...)	{ LockCS();CONSOLE_COLOR(10);printf("##DEBUG##:");printf(__VA_ARGS__); FLUSH_OUTPUT;UnLockCS(); }

//[REMOVE-IN-DEVELOP-LIB:START]	
#endif
//[REMOVE-IN-DEVELOP-LIB:STOP]	

//[REMOVE-IN-DEVELOP-LIB:START]
#ifdef PLATFORM_WINDOWS_DESKTOP
	#include "stdio.h"
	#include <stdlib.h>
	#include <math.h>
	#include <Windows.h>

	// tipuri de date OpenGL
	typedef unsigned int GLenum;
	typedef unsigned int GLbitfield;
	typedef unsigned int GLuint;
	typedef int GLint;
	typedef int GLsizei;
	typedef unsigned char GLboolean;
	typedef signed char GLbyte;
	typedef short GLshort;
	typedef unsigned char GLubyte;
	typedef unsigned short GLushort;
	typedef unsigned long GLulong;
	typedef float GLfloat;
	typedef float GLclampf;
	typedef double GLdouble;
	typedef double GLclampd;
	typedef void GLvoid;

	#include "WinOpenGL.h"

	#define Int8		char
	#define UInt8		unsigned char
	#define Int16		short
	#define UInt16		unsigned short
	#define Int32		int
	#define UInt32		unsigned int
	#define Int64		__int64
	#define UInt64		unsigned __int64
	#define CHARACTER	char
	#define Float32		float
	#define Float64		double
	#define Bool		bool

	#define FUNCTION_CALL_TYPE		__stdcall	
	#define SNPRINTF				_snprintf
	#define PRINT					DBGPRINT
	#define EXPORT				

	bool DBGPRINT_ERR(const char *format, ...);
	bool DBGPRINT_INF(const char *format, ...);
	bool DBGPRINT_EVN(const char *format, ...);

	#ifdef ENABLE_ERROR_LOGGING
		#define LOG_ERROR(...)	{ DBGPRINT_ERR(__VA_ARGS__);  }
	#else
		#define LOG_ERROR(...)
	#endif

	#ifdef ENABLE_INFO_LOGGING
		#define LOG_INFO(...)	{ DBGPRINT_INF(__VA_ARGS__);  }
	#else
		#define LOG_INFO(...)
	#endif

	#ifdef ENABLE_EVENT_LOGGING
		#define LOG_EVENT(...)	{ DBGPRINT_EVN(__VA_ARGS__);  }
	#else
		#define LOG_EVENT(...)
	#endif
	#define DEBUGMSG(...)
#endif
//[REMOVE-IN-DEVELOP-LIB:STOP]

//[REMOVE-IN-DEVELOP-LIB:START]
#ifdef PLATFORM_ANDROID
	#include <sys/types.h>
	#include <jni.h>
	#include <android/log.h>
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#include <android/asset_manager.h>
	#include <android/asset_manager_jni.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <math.h>
	#include <time.h>
	#include <fcntl.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <errno.h>
	#include <pthread.h>


	#define Int8		signed char
	#define UInt8		unsigned char
	#define Int16		signed short
	#define UInt16		unsigned short
	#define Int32		int
	#define UInt32		unsigned int
	#define Int64		long long
	#define UInt64		unsigned long long
	#define CHARACTER	char
	#define Float32		float
	#define Float64		double
	#define Bool		bool


	#define FUNCTION_CALL_TYPE			
	#define CONSOLE_COLOR(color)
	#define SNPRINTF				snprintf
	#define EXPORT
	#define MAX_APP_PATH			512
	

	#ifdef ENABLE_ERROR_LOGGING
		bool DBGPRINT_ERR(const char *format, ...);
		#define LOG_ERROR(...)	{ DBGPRINT_ERR(__VA_ARGS__);  }
		//#define LOG_ERROR(...)	{ __android_log_print(ANDROID_LOG_ERROR,"GAPP","[C++ ERROR] " __VA_ARGS__);	}
	#else
		#define LOG_ERROR(...)
	#endif

	#ifdef ENABLE_INFO_LOGGING
		bool DBGPRINT_INF(const char *format, ...);
		#define LOG_INFO(...)	{ DBGPRINT_INF(__VA_ARGS__);  }
		//#define LOG_INFO(...)	{ __android_log_print(ANDROID_LOG_INFO,"GAPP","[C++  INFO] " __VA_ARGS__);	}
	#else
		#define LOG_INFO(...)
	#endif

	#ifdef ENABLE_EVENT_LOGGING
		bool DBGPRINT_EVN(const char *format, ...);
		#define LOG_EVENT(...)	{ DBGPRINT_EVN(__VA_ARGS__);  }
		//#define LOG_EVENT(...)	{ __android_log_print(ANDROID_LOG_INFO,"GAPP","[C++ EVENT] " __VA_ARGS__);	}
	#else
		#define LOG_EVENT(...)
	#endif
	#define DEBUGMSG(...)
#endif

#if defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
#ifdef PLATFORM_IOS
	#include <OpenGLES/ES2/gl.h>
#else
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
#endif
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <math.h>
	#include <time.h>
	#include <fcntl.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <errno.h>
	#include <pthread.h>
	#include <string.h>
#if defined(ENABLE_ERROR_LOGGING) || defined(ENABLE_INFO_LOGGING) || defined(ENABLE_EVENT_LOGGING)
	#include <syslog.h>
#endif


	#define Int8		signed char
	#define UInt8		unsigned char
	#define Int16		signed short
	#define UInt16		unsigned short
	#define Int32		int
	#define UInt32		unsigned int
	#define Int64		long long
	#define UInt64		unsigned long long
// should be signed char ????
	#define CHARACTER	char
	#define Float32		float
	#define Float64		double
	#define Bool		bool

	#define FUNCTION_CALL_TYPE			
	#define CONSOLE_COLOR(color)
	#define SNPRINTF				snprintf
	#define EXPORT
	#define MAX_APP_PATH			512

	#ifdef ENABLE_ERROR_LOGGING
		#define LOG_ERROR(...)	{ printf(__VA_ARGS__);printf("\n");	}
	#else
		#define LOG_ERROR(...)
	#endif

	#ifdef ENABLE_INFO_LOGGING
		#define LOG_INFO(...)	{ printf(__VA_ARGS__);printf("\n");	}
	#else
		#define LOG_INFO(...)
	#endif

	#ifdef ENABLE_EVENT_LOGGING
		#define LOG_EVENT(...)	{ printf (__VA_ARGS__);printf("\n");	} //syslog (LOG_NOTICE,__VA_ARGS__);
	#else
		#define LOG_EVENT(...)
	#endif
	#define DEBUGMSG(...)
#endif
//[REMOVE-IN-DEVELOP-LIB:STOP]

#define GAC_MAX_UINT8_VALUE		((UInt8)0xFF)
#define GAC_MAX_UINT16_VALUE	((UInt16)0xFFFF)
#define GAC_MAX_UINT32_VALUE	((UInt32)0xFFFFFFFF)
#define GAC_MAX_UINT63_VALUE	((UInt64)0xFFFFFFFFFFFFFFFF)
#define GAC_MAX_INT8_VALUE		((Int8)127)
#define GAC_MIN_INT8_VALUE		((Int8)(-128))
#define GAC_MAX_INT16_VALUE		((Int16)32767)
#define GAC_MIN_INT16_VALUE		((Int16)(-32768))
#define GAC_MAX_INT32_VALUE		((Int32)2147483647)
#define GAC_MIN_INT32_VALUE		((Int32)(-2147483648))
#define GAC_MAX_INT64_VALUE		((Int64)9223372036854775807)
#define GAC_MIN_INT64_VALUE		((Int64)(-9223372036854775808))
#define GAC_MAX_FLOAT_VALUE		(float)(3.402823466E+38)
#define GAC_MIN_FLOAT_VALUE		(float)(1.175494351E–38)
#define GAC_MAX_DOUBLE_VALUE	(double)(1.7976931348623158E+308)
#define GAC_MIN_DOUBLE_VALUE	(double)(2.2250738585072014E–308)



//[REMOVE-IN-DEVELOP-LIB:START]
#ifdef ENABLE_ERROR_LOGGING
//[REMOVE-IN-DEVELOP-LIB:STOP]

	#ifdef PLATFORM_DEVELOP
	#define CHECK(c,returnValue,format,...) { \
		if (!(c)) { \
			LockCS(); \
			CONSOLE_COLOR(12); \
			printf("[  ERROR  ]_|_%s:%d_|_%s_|_%s_|_"format,__FILE__,__LINE__,__FUNCTION__,#c, ##__VA_ARGS__); \
			FLUSH_OUTPUT; \
			UnLockCS(); \
			return (returnValue); \
		} \
	}
	#define CHECKBK(c,format,...) { \
		if (!(c)) { \
			LockCS(); \
			CONSOLE_COLOR(12); \
			printf("[  ERROR  ]_|_%s:%d_|_%s_|_%s_|_"format,__FILE__,__LINE__,__FUNCTION__,#c, ##__VA_ARGS__); \
			FLUSH_OUTPUT; \
			UnLockCS(); \
			break; \
		} \
	}
	#define RETURNERROR(returnValue,format,...) { \
			LockCS(); \
			CONSOLE_COLOR(12); \
			printf("[  ERROR  ]_|_%s:%d_|_%s_|_%s_|_"format,__FILE__,__LINE__,__FUNCTION__,#returnValue, ##__VA_ARGS__); \
			FLUSH_OUTPUT; \
			UnLockCS(); \
			return (returnValue); \
	}
	#endif

	#ifdef PLATFORM_WINDOWS_DESKTOP
	#define CHECK(c,returnValue,format,...) { \
		if (!(c)) { \
			DBGPRINT_ERR("%s:%d(%s) => %s"format,__FILE__,__LINE__,__FUNCTION__,#c, ##__VA_ARGS__); \
			return (returnValue); \
		} \
	}
	#define CHECKBK(c,format,...) { \
		if (!(c)) { \
			DBGPRINT_ERR("%s:%d(%s) => %s"format,__FILE__,__LINE__,__FUNCTION__,#c, ##__VA_ARGS__); \
			break; \
		} \
	}
	#define RETURNERROR(returnValue,format,...) { \
			DBGPRINT_ERR("%s:%d(%s) => %s"format,__FILE__,__LINE__,__FUNCTION__,#returnValue, ##__VA_ARGS__); \
			return (returnValue); \
	}
	#endif

	#ifdef PLATFORM_ANDROID
	#define CHECK(c,returnValue,format,...) { \
		if (!(c)) { \
			DBGPRINT_ERR("%s:%d (%s) -> Condition:'%s' -> "format,__FILE__,__LINE__,__FUNCTION__,#c, ##__VA_ARGS__); \
			return (returnValue); \
		} \
	}
	#define CHECKBK(c,format,...) { \
		if (!(c)) { \
			DBGPRINT_ERR("%s:%d (%s) -> Condition:'%s' -> "format,__FILE__,__LINE__,__FUNCTION__,#c, ##__VA_ARGS__); \
			break; \
		} \
	}
	#define RETURNERROR(returnValue,format,...) { \
			DBGPRINT_ERR("%s:%d (%s) -> Returns: '%s' "format,__FILE__,__LINE__,__FUNCTION__,#returnValue, ##__VA_ARGS__); \
			return (returnValue); \
	}
	#endif

	#if defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
	#define CHECK(c,returnValue,format,...) { \
		if (!(c)) { \
            printf("[C++ ERROR] %s:%d (%s) -> Condition:'%s' -> " format, __FILE__, __LINE__, __FUNCTION__, #c, ##__VA_ARGS__);printf("\n"); \
			return (returnValue); \
		} \
	}
	#define CHECKBK(c,format,...) { \
		if (!(c)) { \
			printf("[C++ ERROR] %s:%d (%s) -> Condition:'%s' -> " format,__FILE__,__LINE__,__FUNCTION__,#c, ##__VA_ARGS__);printf("\n"); \
			break; \
		} \
	}
	#define RETURNERROR(returnValue,format,...) { \
		printf("[C++ ERROR] %s:%d (%s) -> Returns:'%s' -> " format, __FILE__, __LINE__, __FUNCTION__, #returnValue, ##__VA_ARGS__);printf("\n"); \
		return (returnValue); \
	}
	#endif

	#define CHECK_GLERROR(x)	{ {x;}; LastError = glGetError(); if (LastError!=0) { LOG_ERROR("(Line:%d): OpenGL failed with code: %d in function '%s' at '%s'",__LINE__,LastError,__FUNCTION__,#x); return (false);} }
	#define CHECK_GLERROR_BK(x)	{ {x;}; LastError = glGetError(); if (LastError!=0) { LOG_ERROR("(Line:%d): OpenGL failed with code: %d in function '%s' at '%s'",__LINE__,LastError,__FUNCTION__,#x); break; } }	
//[REMOVE-IN-DEVELOP-LIB:START]
#else
	#define CHECK(c,returnValue,format,...)		{ if (!(c)) { return (returnValue); } }
	#define CHECKBK(c,format,...)				{ if (!(c)) break; }
	#define RETURNERROR(returnValue,format,...)	{ return (returnValue); }
	#define CHECK_GLERROR(x)					{ {x;}; LastError = glGetError(); if (LastError!=0) { return (false);} }
	#define CHECK_GLERROR_BK(x)					{ {x;}; LastError = glGetError(); if (LastError!=0) { break; } }
#endif
//[REMOVE-IN-DEVELOP-LIB:STOP]

#if defined(ENABLE_INFO_LOGGING) || defined(ENABLE_ERROR_LOGGING)
#define REFLECTION_OBJECT_NAME
static const char* ReflectionSceneNames[MAX_SCENES] = { NULL, 
	"Main", "SelectSeaon", "SelectLevel", "Rate", "Final", "Game", "Tutorial", "Credentials", "WhatsNew", "Options", "Comercial",
	"FirstTimeRun", "ApplicationError", "Scene 14", "Scene 15", "Scene 16", "Scene 17", "Scene 18", "Scene 19",
	"Scene 20", "Scene 21", "Scene 22", "Scene 23", "Scene 24", "Scene 25", "Scene 26", "Scene 27", "Scene 28", "Scene 29",
	"Scene 30", "Scene 31", "Scene 32", "Scene 33", "Scene 34", "Scene 35", "Scene 36", "Scene 37", "Scene 38", "Scene 39",
	"Scene 40", "Scene 41", "Scene 42", "Scene 43", "Scene 44", "Scene 45", "Scene 46", "Scene 47", "Scene 48", "Scene 49",
	"Scene 50", "Scene 51", "Scene 52", "Scene 53", "Scene 54", "Scene 55", "Scene 56", "Scene 57", "Scene 58", "Scene 59",
	"Scene 60", "Scene 61", "Scene 62", "Scene 63"
};

#endif


typedef int				(*_BinarySearchCompFunction)(void *element1,void *element2);
typedef int				(*_ContextCompFunction)(void *element1,void *element2,void *context);
typedef void			(*PixelPositionCallback)(int x,int y,int &resultX,int &resultY,void* Context);
typedef unsigned int	(*PixelColorBlendCallback)(int x,int y,unsigned int currentColor,unsigned int appliedColor,void* Context); 
typedef CHARACTER* UnicodeString;

#define BufferTypeFunctions(tip,name,defValue) \
			tip				Get##name				(unsigned int poz,tip defaultValue=defValue); \
			tip				Read##name##FromStream	(tip defaultValue=defValue); \
			bool			Copy##name				(unsigned int poz,tip &value); \
			bool			Set##name				(unsigned int poz,tip value,bool resizeIfBigger=true); \
			bool			Insert##name			(unsigned int poz,tip value); \
			bool			Push##name				(tip value); \
			bool			TryPop##name			(tip &value); \
			tip				Pop##name				(tip defaultValue=defValue); \
			bool			Set##name##Vector		(unsigned int poz,tip vector[],unsigned int count,bool resizeIfBigger=true); \
			bool			Insert##name##Vector	(unsigned int poz,tip vector[],unsigned int count); \
			bool			Push##name##Vector		(tip vector[],unsigned int count);

#define KeyDBTypeFunctions(tip,name,defValue) \
			bool			Set##name			(const char *key,tip value); \
			bool			Set##name			(String *key,tip value); \
			bool			Set##name##Vector	(const char *key,tip vector[],unsigned int count); \
			bool			Set##name##Vector	(String *key,tip vector[],unsigned int count); \
			tip				Get##name			(const char *key,tip defaultValue=defValue); \
			tip				Get##name			(String *key,tip defaultValue=defValue); \
			bool			Copy##name			(const char *key,tip &value); \
			bool			Copy##name			(String *key,tip &value); \
			bool			Copy##name##Vector	(const char *key,tip vector[],unsigned int maxElements,unsigned int &elementsCopied=NULLREF(unsigned int)); \
			bool			Copy##name##Vector	(String *key,tip vector[],unsigned int maxElements,unsigned int &elementsCopied=NULLREF(unsigned int));

namespace GApp
{
	namespace Graphics
	{
		class EXPORT Renderer;
		class EXPORT Rect;
		class EXPORT RectF;
	};
	namespace UI
	{
		class EXPORT CoreSystem;
		class EXPORT Scene;
		class EXPORT Application;
		class EXPORT FrameworkObject;
		class EXPORT Counter;
		class EXPORT CountersGroup;
		class EXPORT Alarm;
	};
	namespace Animations
	{
		class EXPORT AnimationObject;
	};
	namespace Utils
	{
		/*
		class EXPORT Vector
		{
		private:
			unsigned int	NrElemente;
			unsigned int	ElementSize;
			unsigned int	AlocatedElements;
			unsigned char	*Data;

			bool			Grow(unsigned int newSize=0xFFFFFFFF);
		public:
			Vector(void);
			~Vector(void);
	
			bool			Create(unsigned int ElemSize=sizeof(void *),unsigned int initialAllocatedElements=32); 
			void			Free();
			bool			Exists() { return (Data!=NULL); }

			unsigned int	GetSize() { return NrElemente; }
			unsigned int	GetElementSize() { return ElementSize; }
			void*			GetVector() { return Data; }
			void*			Get(unsigned int index);

			int				BinarySearch(void *Element,_BinarySearchCompFunction cmpFunc);
			void			Sort(_BinarySearchCompFunction cmpFunc,bool ascendet);
			void			Sort(_ContextCompFunction cmpFunc,bool ascendet,void *Context);

			bool			Push(void *Element);
			bool			Insert(void *Element,unsigned int index);
			bool			Insert(void *Element,_BinarySearchCompFunction cmpFunc,bool ascendent);
			bool			CopyElement(unsigned int index,void *addr);
			bool			Delete(unsigned int index);
			bool			DeleteAll();
			bool			Resize(unsigned int newSize);			
		};
		*/
		class EXPORT ArrayList
		{
			void**			Elements;
			unsigned int	Size;
			unsigned int	Capacity;
		public:
			ArrayList();
			~ArrayList();

			bool			Create(unsigned int initialCapacity,bool createArray=false);
			bool			Dispose();
			unsigned int	GetSize();
			void**			GetArrayList();
			void*			Get(unsigned int index);
			bool			Set(unsigned int index,void* element);
			bool			Push(void *element);
			bool			Insert(void *Element,unsigned int index);
			bool			Delete(unsigned int index);
			bool			Clear();
		};
		class EXPORT String
		{
			CHARACTER	*Text;
			int			Size;
			int			MaxSize;
			UInt8		Flags;
		public:
			bool	Grow(int newSize);
		public:
			String(const CHARACTER *initString,bool createConstantString=false);
			String(void);
			String(const String &s);
			~String(void);

		// functii statice
			static int		Len(const CHARACTER *string);			
			static bool		Add(CHARACTER *destination,const CHARACTER *text,int maxDestinationSize,int destinationTextSize = -1,int textSize = -1);	
			static bool		Set(CHARACTER *destination,const CHARACTER *text,int maxDestinationSize,int textSize = -1);
			static bool		Equals(const CHARACTER *sir1,const CHARACTER *sir2,bool ignoreCase=false);
			static int		Compare(const CHARACTER *sir1,const CHARACTER *sir2,bool ignoreCase=false);
			static bool		StartsWith(const CHARACTER *sir,const CHARACTER *text,bool ignoreCase=false);					
			static bool		EndsWith(const CHARACTER *sir,const CHARACTER *text,bool ignoreCase=false,int sirTextSize = -1,int textSize = -1);
			static int		Find(const CHARACTER *source,const CHARACTER *text,bool ignoreCase=false,int startPoz=0,bool searchForward=true,int sourceSize = -1);
			static bool		Delete(CHARACTER *source,int start,int end,int sourceSize=-1);
			static bool		Insert(CHARACTER *source,const CHARACTER *text,int pos,int maxSourceSize,int sourceSize=-1,int textSize=-1);
			static bool		ReplaceOnPos(CHARACTER *source,const CHARACTER *text,int start,int end,int maxSourceSize,int sourceSize=-1,int textSize=-1);
			static bool		Replace(CHARACTER *source,const CHARACTER *pattern,const CHARACTER *newText,int maxSourceSize,bool ignoreCase=false,int sourceSize=-1,int patternSize=-1,int newTextSize=-1);
			static bool		ConvertToUInt8(const CHARACTER *text,UInt8 &value,unsigned int base=0,int textSize=-1);
			static bool		ConvertToUInt16(const CHARACTER *text,UInt16 &value,unsigned int base=0,int textSize=-1);
			static bool		ConvertToUInt32(const CHARACTER *text,UInt32 &value,unsigned int base=0,int textSize=-1);
			static bool		ConvertToUInt64(const CHARACTER *text,UInt64 &value,unsigned int base=0,int textSize=-1);
			static bool		ConvertToInt8(const CHARACTER *text,Int8 &value,unsigned int base=0,int textSize=-1);
			static bool		ConvertToInt16(const CHARACTER *text,Int16 &value,unsigned int base=0,int textSize=-1);
			static bool		ConvertToInt32(const CHARACTER *text,Int32 &value,unsigned int base=0,int textSize=-1);
			static bool		ConvertToInt64(const CHARACTER *text,Int64 &value,unsigned int base=0,int textSize=-1);
			static bool		ConvertToFloat64(const CHARACTER *text,Float64 &value,int textSize=-1);
			static bool		ConvertToFloat32(const CHARACTER *text,Float32 &value,int textSize=-1);
			static bool		ConvertToBool(const CHARACTER *text,Bool &value);

			static bool		UInt8ToString(UInt8 value,CHARACTER *text,UInt32 textSize);
			static bool		UInt16ToString(UInt16 value,CHARACTER *text,UInt32 textSize);
			static bool		UInt32ToString(UInt32 value,CHARACTER *text,UInt32 textSize);
			static bool		UInt64ToString(UInt64 value,CHARACTER *text,UInt32 textSize);
			static bool		Int8ToString(Int8 value,CHARACTER *text,UInt32 textSize);
			static bool		Int16ToString(Int16 value,CHARACTER *text,UInt32 textSize);
			static bool		Int32ToString(Int32 value,CHARACTER *text,UInt32 textSize);
			static bool		Int64ToString(Int64 value,CHARACTER *text,UInt32 textSize);
			static bool		BoolToString(bool value,CHARACTER *text,UInt32 textSize);
			static bool		CharToString(char value,CHARACTER *text,UInt32 textSize);
			static bool		Float32ToString(Float32 value,CHARACTER *text,UInt32 textSize);
			static bool		Float64ToString(Float64 value,CHARACTER *text,UInt32 textSize);
			static bool		Contains(const CHARACTER *text, const CHARACTER *textToFind,bool ignoreCase=false);
			static int		GetChar(const CHARACTER *text,int index, int len, int defaultValue=0);

		// functii de setare
			bool			Create(int initialSize=64);
			bool			Create(const CHARACTER* text);
			bool			Create(CHARACTER* buffer,int bufferSize,bool emptyString=false);

		// functii obiect
			const CHARACTER*		GetText() const { return Text; }
			int				GetSize() { return Size; }
			int				Len() { return Size; }
			int				UpdateSize();
			int				GetAllocatedSize() { return MaxSize; }

			int				GetChar(int index);
			bool			SetChar(int index,CHARACTER value);

			bool			Add(const CHARACTER *ss,int size=-1);
			bool			AddChar(CHARACTER ch);
			bool			AddChars(CHARACTER ch,int count);
			bool			Add(String *ss,int size=-1);

			bool			Set(const CHARACTER *ss,int size=-1);
			bool			Set(String *ss,int size=-1);

			bool			SetFormat(const CHARACTER *format, ...);
			CHARACTER*		Format(const CHARACTER *format, ...);

			//bool			SetFormatedEx(const CHARACTER *format,...);
			bool			AddFormatedEx(const CHARACTER *format,...);
			
			bool			Realloc(int newSize);
			void			Distroy();
			bool			Truncate(int newSize);
			bool			Clear();

			int				Compare(const CHARACTER *ss,bool ignoreCase=false) const;
			int				Compare(const String *ss,bool ignoreCase=false) const;
			int				Compare(const String &ss,bool ignoreCase=false) const;

			bool			StartsWith(const CHARACTER *ss,bool ignoreCase=false);
			bool			StartsWith(String *ss,bool ignoreCase=false);
			bool			EndsWith(const CHARACTER *ss,bool ignoreCase=false);
			bool			EndsWith(String *ss,bool ignoreCase=false);
			bool			Contains(const CHARACTER *ss,bool ignoreCase=false);
			bool			Contains(String *ss,bool ignoreCase=false);
			bool			Equals(const CHARACTER *ss,bool ignoreCase=false);
			bool			Equals(String *ss,bool ignoreCase=false);

			int				Find(const CHARACTER *ss,bool ignoreCase=false,int startPoz=0,bool searchForward=true);
			int				Find(String *ss,bool ignoreCase=false,int startPoz=0,bool searchForward=true);
			int				FindLast(const CHARACTER *ss,bool ignoreCase=false);
			int				FindLast(String *ss,bool ignoreCase=false);

			bool			Delete(int start,int end);
			bool			DeleteChar(int pos);
			bool			Insert(const CHARACTER *ss,int pos);
			bool			InsertChar(CHARACTER ch,int pos);
			bool			InsertChars(CHARACTER ch,int pos,int count);
			bool			Insert(String *ss,int pos);
			bool			ReplaceOnPos(int start,int end,const CHARACTER *newText,int newTextSize=-1);
			bool			ReplaceOnPos(int start,int end,String *ss);
			bool			Replace(const CHARACTER *pattern,const CHARACTER *newText,bool ignoreCase=false,int patternSize = -1,int newTextSize = -1);
			bool			Replace(String *pattern,String *newText,bool ignoreCase=false);
			bool			Strip(const CHARACTER *charList=NULL,bool stripFromLeft=true,bool stripFromRight=true);

			bool			Split(const CHARACTER *separator,String arrayList[],int arrayListCount,int *elements=NULL,int separatorSize=-1);
			bool			Split(String *separator,String arrayList[],int arrayListCount,int *elements=NULL);

			bool			SplitInTwo(const CHARACTER *separator,String *leftPart,String *rightPart,int separatorIndex = 1,bool ignoreCase=false);
			bool			SplitInTwo(String *separator,String *leftPart,String *rightPart,int separatorIndex = 1,bool ignoreCase=false);

			bool			SplitInThree(const CHARACTER *separatorLeft,const CHARACTER *separatorRight,String *leftPart,String *middlePart,String *rightPart,int separatorLeftIndex = 1,int separatorRightIndex = 1,bool ignoreCase=false);

			bool			SplitFromLeft(const CHARACTER *separator,String *leftPart,String *rightPart,bool ignoreCase=false);
			bool			SplitFromLeft(String *separator,String *leftPart,String *rightPart,bool ignoreCase=false);
			bool			SplitFromRight(const CHARACTER *separator,String *leftPart,String *rightPart,bool ignoreCase=false);
			bool			SplitFromRight(String *separator,String *leftPart,String *rightPart,bool ignoreCase=false);

			bool			CopyNextLine(String *line,int& position);
			bool			CopyNext(String *token,const CHARACTER *separator,int& position,bool ignoreCase=false);
			bool			CopyFromLeft(String *text,unsigned int nrChars);
			bool			CopyFromRight(String *text,unsigned int nrChars);
			bool			CopySubString(String *text,unsigned int start,unsigned int size);

	
			bool			CopyPathName(String *path);
			bool			CopyFileName(String *path);
			bool			PathJoinName(const CHARACTER *name,CHARACTER separator = '\\');
			bool			PathJoinName(String *name,CHARACTER separator = '\\');

			bool			MatchSimple(const CHARACTER *mask,bool ignoreCase=false);
			bool			MatchSimple(String *mask,bool ignoreCase=false);

			bool			ConvertToInt8(Int8 &value,unsigned int base=0);
			bool			ConvertToInt16(Int16 &value,unsigned int base=0);
			bool			ConvertToInt32(Int32 &value,unsigned int base=0);
			bool			ConvertToInt64(Int64 &value,unsigned int base=0);

			bool			ConvertToUInt8(UInt8 &value,unsigned int base=0);
			bool			ConvertToUInt16(UInt16 &value,unsigned int base=0);
			bool			ConvertToUInt32(UInt32 &value,unsigned int base=0);
			bool			ConvertToUInt64(UInt64 &value,unsigned int base=0);

			bool			ConvertToFloat64(double &value);
			bool			ConvertToFloat32(float &value);

			bool			ConvertToLower();
			bool			ConvertToUpper();

			CHARACTER*		UInt8ToString(UInt8 value);
			CHARACTER*		UInt16ToString(UInt16 value);
			CHARACTER*		UInt32ToString(UInt32 value);
			CHARACTER*		UInt64ToString(UInt64 value);
			CHARACTER*		Int8ToString(Int8 value);
			CHARACTER*		Int16ToString(Int16 value);
			CHARACTER*		Int32ToString(Int32 value);
			CHARACTER*		Int64ToString(Int64 value);
			CHARACTER*		BoolToString(bool value);
			CHARACTER*		CharToString(char value);
			CHARACTER*		Float32ToString(Float32 value);
			CHARACTER*		Float64ToString(Float64 value);


		// Operatori
			String&			operator=  (const String &s);
			bool			operator>  (const String &s) const;
			bool			operator<  (const String &s) const;
			bool			operator== (const String &s) const;
			bool			operator!= (const String &s) const;
			bool			operator>= (const String &s) const;
			bool			operator<= (const String &s) const;
			CHARACTER&		operator[] (int poz);
			operator		CHARACTER *() { return Text; }
		};
		class EXPORT Buffer
		{
		PRIVATE_INTERNAL:
			unsigned char*	Data;
			unsigned int	Size;
			unsigned int	StreamOffset;
			unsigned int	Allocated;
		public:
			Buffer();
			Buffer(unsigned int initialAllocatedSize);
			~Buffer();
			bool			Create(unsigned int initialAllocatedSize);
			bool			Resize(unsigned int newSize);
			void			Free();
			unsigned int	GetSize();
			unsigned char*	GetBuffer();

			void			Clear(unsigned char value=0);
			void			Clear(unsigned int start,unsigned int size,unsigned char value=0);
			bool			Delete(unsigned int poz,unsigned int size);
			
			bool			SetData(unsigned int poz,const void* Buffer,unsigned int size,bool resizeIfBigger=true);			
			bool			CopyData(unsigned int poz,void* Destination,unsigned int DestinationSize);
			bool			InsertData(unsigned int poz,const void* Buffer,unsigned int size);
			bool			InsertBuffer(unsigned int poz,Buffer *b);			
			bool			PushData(const void* Buffer,unsigned int size);
			bool			PushBuffer(Buffer *b);

			BufferTypeFunctions(bool,Bool,false)
			BufferTypeFunctions(float,Float,0.0f)
			BufferTypeFunctions(double,Double,0.0)
			BufferTypeFunctions(Int8,Int8,0)
			BufferTypeFunctions(Int16,Int16,0)
			BufferTypeFunctions(Int32,Int32,0)
			BufferTypeFunctions(Int64,Int64,0)
			BufferTypeFunctions(UInt8,UInt8,0)
			BufferTypeFunctions(UInt16,UInt16,0)
			BufferTypeFunctions(UInt32,UInt32,0)
			BufferTypeFunctions(UInt64,UInt64,0)
			
			unsigned int	GetUInt24(unsigned int poz,unsigned int defaultValue=0);
			unsigned int	ReadCompactUInt32FromStream(unsigned int defaultValue=0);

			bool			SetString(unsigned int poz,const char *text,bool resizeIfBigger=true);
			bool			SetString(unsigned int poz,String *text,bool resizeIfBigger=true);
			//bool			SetVector(unsigned int poz,Vector *v,bool resizeIfBigger=true);
			
			bool			InsertString(unsigned int poz,const char *text);
			bool			InsertString(unsigned int poz,String *text);
			//bool			InsertVector(unsigned int poz,Vector *v);
			
			bool			PushString(const char *text);
			bool			PushString(String *text);
			//bool			PushVector(Vector *v);			

			bool			SetStreamOffset(unsigned int ofs);
			unsigned int	GetStreamOffset();
			bool			IsStreamAtEnd();

			//[REMOVE-IN-DEVELOP-LIB:START]
#ifdef PLATFORM_DEVELOP
			//[REMOVE-IN-DEVELOP-LIB:STOP]
			bool			AddDebugKeyDaya(char *Name,unsigned int dataType,void *Buffer,unsigned int BufferSize);
			//[REMOVE-IN-DEVELOP-LIB:START]
#endif
			//[REMOVE-IN-DEVELOP-LIB:STOP]
		};

		class EXPORT KeyDB
		{
		PRIVATE_INTERNAL:
			Buffer			Indexes;
			Buffer			Data;
			bool			WriteEnabled;
		public:
			KeyDB();

			bool			SetData(const char* key,const void *Buffer,unsigned int BufferSize,unsigned int DataType);
			bool			SetData(String *key, const void *Buffer, unsigned int BufferSize, unsigned int DataType);
			bool			CopyData(const char *key, void* Buffer, unsigned int BufferSize,unsigned int DataType);
			bool			HasKey(const char *key);			
			bool			CheckIntegrity();
			void			Clear();
			unsigned int	Len();
			unsigned int	GetSize();


			KeyDBTypeFunctions(bool,Bool,false)
			KeyDBTypeFunctions(float,Float,0.0f)
			KeyDBTypeFunctions(double,Double,0.0)
			KeyDBTypeFunctions(Int8,Int8,0)
			KeyDBTypeFunctions(Int16,Int16,0)
			KeyDBTypeFunctions(Int32,Int32,0)
			KeyDBTypeFunctions(Int64,Int64,0)
			KeyDBTypeFunctions(UInt8,UInt8,0)
			KeyDBTypeFunctions(UInt16,UInt16,0)
			KeyDBTypeFunctions(UInt32,UInt32,0)
			KeyDBTypeFunctions(UInt64,UInt64,0)


			bool			SetString(const char *key,const char *Text);
			bool			SetVector(const char *key,const void *Vector,unsigned int ElementsCount,unsigned int dataType);

			bool			CopyString(const char *key,String *text);
			bool			CopyString(const char *key,char *destination,unsigned int destinationSize);
			char*			GetString(const char *key,const char *defaultValue="");

			bool			CopyVector(const char *key,void *Vector,unsigned int ElementSize,unsigned int maxElements,unsigned int &copiedElements=NULLREF(unsigned int));	
//[REMOVE-IN-DEVELOP-LIB:START]
#ifdef ENABLE_INFO_LOGGING
//[REMOVE-IN-DEVELOP-LIB:STOP]
			bool			GetKeyInfo(unsigned int index, GApp::Utils::String *str);
//[REMOVE-IN-DEVELOP-LIB:START]
#endif
//[REMOVE-IN-DEVELOP-LIB:STOP]
//[REMOVE-IN-DEVELOP-LIB:START]
#ifdef PLATFORM_DEVELOP
//[REMOVE-IN-DEVELOP-LIB:STOP]
			bool			CopyKeyInfo(unsigned int index, GApp::Utils::Buffer *b);
//[REMOVE-IN-DEVELOP-LIB:START]
#endif
//[REMOVE-IN-DEVELOP-LIB:STOP]
		};
		class EXPORT ThreadSyncRequest
		{
		public:
			unsigned int		Command;
			union {
				bool			BoolValue;
				Int8			Int8Value;
				UInt8			UInt8Value;
				Int16			Int16Value;
				UInt16			UInt16Value;
				Int32			Int32Value;
				UInt32			UInt32Value;
				Int64			Int64Value;
				UInt64			UInt64Value;
				Float32			Float32Value;
				Float64			Float64Value;
				void*			PtrValue;		
				const char*		Text;
			} Values[8],Result;
			//bool				Done;
#if defined(PLATFORM_DEVELOP) || defined(PLATFORM_WINDOWS_DESKTOP)
			HANDLE				syncEvent;
#elif defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
			pthread_cond_t		SignalCondition;
			pthread_mutex_t		Mutex;
			bool				SignalState;
#else
			bool				isDone;
#endif


			ThreadSyncRequest();
			void				Create();
			void				Finish();
			void				SendCommand(unsigned int command);
			void				Clear();
			void				Wait();
		};

		class EXPORT FPSCounter
		{
		PROTECTED_INTERNAL:
			float				Fps;
			CoreReference		CoreContext;
			unsigned int		StartTime, ComputeInterval,Counter;

		public:
			FPSCounter(GApp::UI::Scene *scene);
			FPSCounter(GApp::UI::Scene *scene,unsigned int computeInterval);
			bool	SetComputeInterval(unsigned int value);
			float	GetFPS();
			void	Start();
			bool	Update();
			void	Reset();
		};
		class EXPORT Timer
		{
			CoreReference	CoreContext;
			unsigned int	timeStart,extraTime;
			unsigned int	Value;
			unsigned int	Status;

		public:
			Timer(GApp::UI::Scene *scene);
			Timer(GApp::UI::Application *app);
			Timer(GApp::UI::FrameworkObject *obj);
			void			Start();
			void			Stop();
			void			Pause();
			void			Resume();
			bool			IsStopped();
			bool			IsStarted();
			bool			IsPaused();
			unsigned int	GetValue();
		};
		struct DateTime
		{
			unsigned int Year;
			unsigned int Month;
			unsigned int Day;
			unsigned int Hour;
			unsigned int Minute;
			unsigned int Second;
			unsigned int DayOfWeek;
		};
		struct SceneTimer
		{
			unsigned int ID;
			unsigned int LastTime;
			unsigned int Interval;
			bool Enabled;
		};
		class EXPORT MapPoint
		{
		public:
			int CellX, CellY;
			int GridX, GridY;
			MapPoint();
		};
		class EXPORT MapTranslator
		{
			int Width, Height;
			int WidthUnits, HeightUnits;
			int CellWidth, CellHeight;
			int LeftMargin, TopMargin, RightMargin, BottomMargin;
			int ScrollLeft, ScrollTop;
		public:
			MapTranslator();
			bool Create(int width, int height, int widthUnits, int heightUnits);
			bool Create(int width, int height, int widthUnits, int heightUnits, int cellWidth, int cellHeight);
			bool SetCellSize(int width, int height);
			bool AlignGridToScreen(int gridX, int gridY, int screenX, int screenY);
			bool AlignMapPointScreen(MapPoint *point, int screenX, int screenY);
			bool AlignMapToScreen(int screenX, int screenY, GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			bool ComputeVisibleCells(int viewLeft, int viewTop, int viewRight, int viewBottom, GApp::Graphics::Rect *visibleCells);
			bool ComputeVisibleCells(GApp::Graphics::Rect *screenView, GApp::Graphics::Rect *visibleCells);
			bool GetCellRect(int cellX, int cellY, GApp::Graphics::Rect *rect);
			bool MoveMapPointBy(MapPoint *point, int addX, int addY);
			bool MoveMapPointInLineToGridPosition(MapPoint *point, int gridX, int gridY,int Steps);
			bool SetMapPointToCell(MapPoint *point, int X, int Y);
			bool SetMapPointToGrid(MapPoint *point, int X, int Y);
			int  GetMapPointScreenX(MapPoint *point);
			int  GetMapPointScreenY(MapPoint *point);
			bool ScreenToMapPoint(int screenX, int screenY, MapPoint *point);
			int	 GetCellID(MapPoint *point);
			int  GetCellID(int X, int Y);
			int	 GetScrollX();
			int	 GetScrollY();
			int  ConvertCellXToGridPosition(int X);
			int  GetGridX(MapPoint *point);
			int  ConvertCellYToGridPosition(int Y);
			int  GetGridY(MapPoint *point);
			void SetScrollXY(int left, int top);
		};
		class EXPORT CircularCounter
		{
		private:
			int min, max;
		public:
			int Value;

			void SetInterval(int min, int max);
			bool Increment();
			bool Decrement();
		};
		class EXPORT UpDownCounter
		{
		private:
			int min, max, add;
		public:
			int Value;

			void SetInterval(int min, int max);
			bool Update();
			void SetDirection(int dir);
		};
		class EXPORT NumericCounter
		{
		private:
			int min, max, step;
			unsigned int Flags;
		public:
			int Value;
			NumericCounter();
			void				Init(GAC_COUNTER_TYPE type, int min, int max, int step = 1,unsigned int Flags = 0);
			void				SetFlags(unsigned int Flags);
			bool				Update();
			GAC_COUNTER_TYPE	GetType();
			void				Disable();
			bool				IsEnabled();
		};
		class EXPORT FloatCounter
		{
		private:
			float min, max, step;
			unsigned int Flags;
		public:
			float Value;
			FloatCounter();
			void				Init(GAC_COUNTER_TYPE type, float min, float max, float step = 1.0f, unsigned int Flags = 0);
			void				SetFlags(unsigned int Flags);
			bool				Update();
			GAC_COUNTER_TYPE	GetType();
			void				Disable();
			bool				IsEnabled();
		};

	};
	namespace Platform
	{
		class EXPORT AdInterface
		{
			int				adID;
			unsigned int	adType;
			bool			loadOnStartup;
		PRIVATE_INTERNAL:
			CoreReference	CoreContext;
		public:
			AdInterface();
			bool	Init(CoreReference coreContext, int adID, bool _loadOnStartup, unsigned int _adType);
			bool	Show();
			bool	Hide();
			bool	Load();
			bool	IsVisible();
			bool	IsLoaded();
			bool	ShouldLoadOnStartup() { return loadOnStartup; }
		};
		class EXPORT AnalyticsInterface
		{
		PRIVATE_INTERNAL:
			CoreReference	CoreContext;
		public:
			AnalyticsInterface();
			bool	SendEvent(const char* name);
			bool	SendDoubleValue(const char* name, double value);
			bool	SendInt64Value(const char* name, Int64 value);
			bool	SendInt32Value(const char* name, Int32 value);
			bool	SendBoolValue(const char* name, bool value);

			bool	SendStandardEvent(GAC_ANALYTICS_STD_EVENT_TYPE type);
			bool	SendLevelReachEvent(unsigned int level);
			bool	SendSeasonReachEvent(unsigned int season);
			bool	SendLevelReachEvent(unsigned int season, unsigned int level);
			bool	SendRewardAvailableEvent(const char *rewardName);
			bool	SendRewardRequestedEvent(const char *rewardName);
			bool	SendRewardReceivedEvent(const char *rewardName);
			bool	SendUserProperty(const char * propertyName, const char * propertyValue);
			bool	SendCurrentScreen(const char * screenName);

		};
	}
	namespace Resources
	{
		class EXPORT Profile;


		class EXPORT GenericResource
		{
		PROTECTED_INTERNAL:
			CoreReference			CoreContext;
			Profile*				ProfileContext;
			unsigned int			Position;
			unsigned int			Size;
			unsigned int			Flags;
#ifdef REFLECTION_OBJECT_NAME
		public:
			const char*				Name;
#endif
		public:
			GenericResource();
			bool					IsLoaded();
			virtual bool			OnCreate();
			virtual bool			OnUpdate();
				
		};
		class EXPORT RawResource: public GenericResource
		{
			GApp::Utils::Buffer		Data;
		public:
			bool					Create(CoreReference coreContext,unsigned int pos,unsigned int size,bool storeLocally);
			bool					CopyRawDataBuffer(GApp::Utils::Buffer *buffer);
			GApp::Utils::Buffer*	GetRawDataBuffer();
			bool					Load();
			bool					Unload();
		};
		class EXPORT Bitmap: public GenericResource
		{
		public:
			float			Coordonates[14];
		PRIVATE_INTERNAL:
			int				TextureID;			
			unsigned int	TextureLeft,TextureTop;
			unsigned int	OriginalWidth, OriginalHeight;
#ifdef PLATFORM_DEVELOP
			GApp::Utils::Buffer *localImageBuffer;
#endif
		public:			
			Bitmap();
#ifdef PLATFORM_DEVELOP
			bool			Create(const void *picture, unsigned int size);
#endif
			
			unsigned int	Width,Height;		
			//bool			Create(CoreReference CoreContext, unsigned int currentScreenWidth, unsigned int currentScreenHeight, unsigned int *data, unsigned int count);
			bool			Create(unsigned int currentScreenWidth, unsigned int currentScreenHeight, unsigned int * data, unsigned int count);
			bool			Create(CoreReference core,unsigned int imageWidth,unsigned int imageHeight,unsigned int designScreenWidth,unsigned int designScreenHeight,unsigned int CurrentScreenWidth, unsigned int CurrentScreenHeight, unsigned int Flags,unsigned int resFilePos,unsigned int resourceBufferSize);
			bool			CreateRenderer(CoreReference core,GApp::Graphics::Renderer *renderer,GApp::Utils::Buffer *buffer);
		};
		class EXPORT Shader: public GenericResource
		{
			int				LastError;
			GLuint			ShadersIDs[2];
			GLuint			ProgramID;
			GLint			Attributes[4];
			GLint			Uniforms[4];
			GLfloat			VariableValue[4][4];
			unsigned char	VariableValueSize[4];
			unsigned int	UniformHash[4];
			bool			LoadShader(unsigned int ID,GLenum shaderType, const char *SourceCode);
		public:			
			Shader();
			bool			Load();
			bool			Create(CoreReference coreContext,unsigned int pos,unsigned int size);
			bool			Create(const char *VertexShader,const char *FragmentShader,const char *attributeList=NULL,const char *uniformList=NULL);
			bool			Unload();
			bool			SetPosition(float *dataPtr);
			bool			SetAttributeData(unsigned int index,GLfloat *dataPtr,int unitSize);
			bool			SetUniformData(unsigned int index,GLfloat v1);
			bool			SetUniformData(unsigned int index,GLfloat v1,GLfloat v2);
			bool			SetUniformData(unsigned int index,GLfloat v1,GLfloat v2,GLfloat v3);
			bool			SetUniformData(unsigned int index,GLfloat v1,GLfloat v2,GLfloat v3,GLfloat v4);

			bool			SetVariableValue(unsigned int index,GLfloat v1,bool clearAfterUsage);
			bool			SetVariableValue(unsigned int index,GLfloat v1,GLfloat v2,bool clearAfterUsage);
			bool			SetVariableValue(unsigned int index,GLfloat v1,GLfloat v2,GLfloat v3,bool clearAfterUsage);
			bool			SetVariableValue(unsigned int index,GLfloat v1,GLfloat v2,GLfloat v3,GLfloat v4,bool clearAfterUsage);

			bool			SetVariableColor(unsigned int index,unsigned int color,bool clearAfterUsage);

			bool			UpdateVaribleValues(bool UseDerivedShaderVariable);
			bool			Use();

			int				GetUniformIndex(const char *name);
		};
		class EXPORT Font: public GenericResource 
		{
		PRIVATE_INTERNAL:
			void			*Glyphs;
			float			SpaceWidth,CharacterSpacing,Baseline,Descent;
			unsigned int	Count;
			unsigned short	CharStart[3],CharEnd[3];
			unsigned int	CharIndex[3];
			void*			GetGlyph(unsigned short CharacterCode);
		public:
			Font();
			bool			Create(unsigned int glyphsCount);			
			bool			Add(unsigned int CharacterCode,Bitmap *bmp,float descend);
			bool			Prepare(unsigned int widthReferenceCharacter,float spaceWidth,float characterSpace);
		};
		class EXPORT Presentation: public GenericResource
		{
			struct BitmapHolder
			{
				Bitmap*		Object;
				const char*	Name;
			};
			struct FontHolder
			{
				Font*		Object;
				const char*	Name;
			};
			struct ShaderHolder
			{
				Shader*		Object;
				const char*	Name;
			};
			struct StringHolder
			{
				const char*	Object;
				const char*	Name;
			};
			struct StringValueHolder
			{
				GApp::Utils::String Value;
				const char*			Name;
			};
		public:
			BitmapHolder*			Images;
			FontHolder*				Fonts;
			ShaderHolder*			Shaders;
			StringHolder*			Strings;
			StringValueHolder*		StringsValues;
			unsigned int			ImagesCount;
			unsigned int			FontsCount;
			unsigned int			ShadersCount;
			unsigned int			StringsCount;
			unsigned int			StringValuesCount;
			GApp::Utils::Buffer		Data;

			Presentation();
			bool	Create(CoreReference coreContext, unsigned int resPos, unsigned int resSize, unsigned int nrImages, unsigned int nrFonts, unsigned int nrShaders, unsigned int nrStrings, unsigned int nrStringsValue);
			bool	SetImage(unsigned int index,Bitmap *bmp,const char* Name);
			bool	SetFont(unsigned int index, Font *font, const char* Name);
			bool	SetShader(unsigned int index, Shader *shader, const char* Name);
			bool	SetString(unsigned int index, char *str, const char* Name);
			bool	CreateStringValue(const char * name, unsigned int index);

			bool	SetImageForKey(const char* keyName, Bitmap *bmp);
			bool	SetFontForKey(const char* keyName, Font *font);
			bool	SetShaderForKey(const char* keyName, Shader *shader);
			bool	SetStringForKey(const char* keyName, const char* text);
			bool	SetStringValue(const char* ValueName, const char* text);

			bool	Load();
			bool	Unload();			
		};
		class EXPORT Sound: public GenericResource
		{
		PRIVATE_INTERNAL:
			int				SoundID;			
		public:
			Sound();
			bool			Create(CoreReference core,int soundID,unsigned int position,unsigned int size);
			bool			Load();
			bool			Unload();
		};
		class EXPORT TextureData
		{
		public:
			unsigned int	Width,Height,ID;
		};
		class EXPORT Profile
		{
		PROTECTED_INTERNAL:
			CoreReference				CoreContext;
			GenericResource**			Resources;			
			unsigned int				ResourcesCount;
			unsigned char				ProfileType;
			unsigned char				State;
			TextureData					Textures[16];	

			bool			CreateTexture(unsigned int ImageCount,bool RunsOnUIThread);
#ifdef REFLECTION_OBJECT_NAME
		public:
			const char*					Name;
#endif
		public:
			Profile();

			bool			Create(CoreReference coreContext,unsigned char profileType,unsigned int nrResources);
			bool			Load(bool RunsOnUIThread,bool onlyIfMarkedForDelayLoad=false);
			bool			Unload(bool RunsOnUIThread,bool markForDelayLoad=false);
			bool			MarkForDelayLoad();
			bool			IsLoaded();
			bool			IsDelayedProfile();

			bool			SetResource(unsigned int index,GApp::Resources::GenericResource *res);
			bool			LoadTextureToGraphicCard(GApp::Utils::ThreadSyncRequest &tsr);
			

			
		};
		class EXPORT ResourceData
		{
		protected:
		public:
			virtual bool					Init(CoreReference Context);
			virtual Profile*				GetProfilesList();
			virtual unsigned int			GetProfilesCount();
			virtual Sound*					GetSoundsList();
			virtual unsigned int			GetSoundsCount();
			virtual Platform::AdInterface*	GetAd(unsigned int index);
			virtual UI::Counter*			GetCountersList();
			virtual unsigned int			GetCountersCount();
			virtual UI::CountersGroup*		GetCountersGroupList();
			virtual unsigned int			GetCountersGroupCount();
			virtual UI::Alarm*				GetAlarmsList();
			virtual unsigned int			GetAlarmsCount();
		};
	};
	namespace Graphics
	{		
		struct BlendingInfo
		{
			unsigned int		Mode;
			union {
				float			Alpha;
				unsigned int	Color;
			};
		};
		class EXPORT Rect
		{
		public:
			int		Left,Top,Right,Bottom;
			Rect    ();
			Rect    (int x,int y,GAC_TYPE_ALIGNAMENT align,int Width,int Height);
			Rect	(int left, int top, int right, int bottom);

			bool	Set(int x,int y,GAC_TYPE_ALIGNAMENT align,int Width,int Height);
			bool	SetWH(int left, int top, int width, int height);
			bool	Set(int left, int top, int right, int bottom);
			bool	Set(Rect *r);
			void	Intersect(int left,int top,int right,int bottom);
			void	Intersect(Rect *r);
			bool	OverlapWith(int left, int top, int right, int bottom);
			bool	OverlapWith(Rect *r);
			int		GetCenterX();
			int		GetCenterY();
			int		GetWidth();
			int		GetHeight();
			bool	Contains(int x,int y);
			bool	SetWidthFromLeft(int newValue);
			bool	SetWidthFromRight(int newValue);
			bool	SetHeightFromTop(int newValue);
			bool	SetHeightFromBottom(int newValue);
			void	Offset(int x, int y);
			void	MoveTo(int left, int top);
			bool	MoveTo(int x, int Y, GAC_TYPE_ALIGNAMENT align);
		};
		class EXPORT RectF
		{
		public:
			float Left,Top,Right,Bottom;
			RectF   ();
			RectF   (float x,float y,GAC_TYPE_ALIGNAMENT align,float Width,float Height);
			RectF	(float left, float top, float right, float bottom);

			bool	Set(float x,float y,GAC_TYPE_ALIGNAMENT align,float Width,float Height);
			bool	SetWH(float left, float top, float width, float height);
			bool	Set(float left, float top, float right, float bottom);
			bool	Set(RectF *r);
			void	Intersect(float left,float top,float right,float bottom);
			void	Intersect(RectF *r);
			bool	OverlapWith(float left, float top, float right, float bottom);
			bool	OverlapWith(RectF *r);
			float	GetCenterX();
			float	GetCenterY();
			float	GetWidth();
			float	GetHeight();
			bool	Contains(float x,float y);
			bool	SetWidthFromLeft(float newValue);
			bool	SetWidthFromRight(float newValue);
			bool	SetHeightFromTop(float newValue);
			bool	SetHeightFromBottom(float newValue);
			void	Offset(float x, float y);
			void	MoveTo(float left, float top);
			bool	MoveTo(float x, float Y, GAC_TYPE_ALIGNAMENT align);
		};

		
		class EXPORT PaintMode
		{
			GAC_TYPE_RESAMPLEMETHOD		ResampleMode;
			GAC_TYPE_COLORBLENDTYPE		ColorBlendMode;
			unsigned int				TransparentColor;
			unsigned int				Flags;
			GAC_TYPE_MATRIXFLOAT		Matrix[9];
			PixelPositionCallback		PositionCallBack;
			PixelColorBlendCallback		ColorBlendCallBack;
			void*						CallBackContext;
			unsigned int				StrokeColor;
			unsigned int				FillColor;
			float						StrokeWidth;
			bool						AntiAliasing;
		public:
			PaintMode();
			void						Reset();
			void						ResetTransformationMatrix();

			void						SetResampleMode(GAC_TYPE_RESAMPLEMETHOD newMode);
			GAC_TYPE_RESAMPLEMETHOD		GetResampleMode();

			void						SetColorBlendMode(GAC_TYPE_COLORBLENDTYPE newMode);
			GAC_TYPE_COLORBLENDTYPE		GetColorBlendMode();

			void						SetTransparentColor(unsigned int color);
			unsigned int				GetTransparentColor();

			void						SetTranslate(float xOffset, float yOffset);
			void						SetRotate(float angle);
			void						SetRotate(float angle, float x, float y);

			bool						SetColorBlendCallBack(PixelColorBlendCallback fnc, void* Context = NULL);
			bool						SetPositionCallBack(PixelPositionCallback fnc, void *Context = NULL);
			void						SetCallBackContext(void *Context = NULL);

			void						ComputePixelPosition(int x, int y, int &resultX, int &resultY);
			unsigned int				ComputePixelColor(int x, int y, unsigned int currentColor, unsigned int appliedColor);

			void						SetStrokeColor(unsigned int color);
			void						SetFillColor(unsigned int color);
			void						SetStrokeWidth(float width);
			bool						HasStrokeColor();
			bool						HasFillColor();
			float						GetStrokeWidth();
			void						SetAntiAlias(bool value);
			bool						IsAntiAliasEnabled();

			unsigned int				GetStrokePixelColor(int x, int y);
			unsigned int				GetFillPixelColor(int x, int y);

		};

		class EXPORT Renderer
		{
			GApp::Utils::Buffer	*Data;
			unsigned int		Width,Height,LineStrip;
		public:
			Renderer();
			bool			Create(GApp::Utils::Buffer *b,unsigned int Width=1,unsigned int Height=1);			
			bool			LoadImage(GApp::Utils::Buffer *img);
			bool			Resize(unsigned int Width,unsigned int Height);
			unsigned int	GetWidth();
			unsigned int	GetHeight();
			bool			Clear(unsigned int color = 0);
			void			Dispose();

			unsigned int	GetPixel(int x,int y,unsigned int colorIfInvalidLocation=COLOR_Transparent);
			bool			SetPixel(int x,int y,unsigned int color,PaintMode *p=NULL);
			unsigned char*	GetPixelBuffer();
			unsigned char*	GetUnSafePixelPointer(int x, int y);
			unsigned char*	GetSafePixelPointer(int x, int y, unsigned char *ptrIfInvalidLocation = NULL);

			bool			DrawImage(Renderer *r,int x,int y,PaintMode *p=NULL);
			bool			DrawImage(Renderer *r,int x,int y,int newWidth,int newHeight, PaintMode *p=NULL);

			bool			DrawLine(int x1, int y1, int x2, int y2, PaintMode *p);
			bool			DrawRect(int x, int y, int rectWidth, int rectHeight, PaintMode *p);
			bool			DrawRect(int x, int y, int rectWidth, int rectHeight, float rx, float ry, PaintMode *p);
			bool			DrawEllipse(int cx, int cy, int rx, int ry, PaintMode *p);
			bool			DrawQuadraticBezierCurve(int x1, int y1, int x2, int y2, int x3, int y3, PaintMode *p);
			bool			DrawCubicBezierCurve(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, PaintMode *p);

		};

		class EXPORT Texture
		{
		PRIVATE_INTERNAL:
			CoreReference	CoreContext;			
			void*			Images;
			bool			Loaded;
			GLuint			TextureID;	
			unsigned int	ImageCount;
		public:
			Texture();
			bool			Create(CoreReference core,unsigned int ImageCount);
			bool			SetBitmap(unsigned int poz,GApp::Resources::Bitmap *image);
			bool			Load();
		};



		class EXPORT ImagePainter
		{
		PRIVATE_INTERNAL:
			GApp::Resources::Bitmap		*Image;
			GApp::Resources::Shader		*S;
			float						Left,Top,Width,Height;
			float						ViewLeft,ViewTop,ViewRight,ViewBottom,X,Y,ScaleWidth,ScaleHeight,ImageWidth,ImageHeight;
			unsigned int				Flags;
			float						ShaderParams[16];
			unsigned char				ShaderParamInfo[4];
			BlendingInfo				Blend;

		public:
			ImagePainter();
			void			SetViewRect(float left,float top,float right,float bottom);
			void			SetViewRectWH(float left, float top, float width, float height);
			void			SetShader(GApp::Resources::Shader *shader);
			void			SetAlphaBlending(float alpha);
			void			SetColorBlending(unsigned int color);
			bool			SetImage(GApp::Resources::Bitmap *bmp);
			void			SetLayout(GAC_TYPE_IMAGERESIZEMODE mode,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			void			SetPosition(float x,float y,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			void			SetScale(float scaleWidth,float scaleHeight=0);
			void			SetImageSize(float width,float height,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS);				
			void			Clear();
			void			ClearShaderParams();
			bool			Create(GApp::Resources::Bitmap *img,GAC_TYPE_IMAGERESIZEMODE mode = GAC_IMAGERESIZEMODE_NONE,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			bool			SetShaderUniformValue(const char * VariableName, unsigned int count, float v1 , float v2 = 0, float v3 = 0, float v4 = 0);
			bool			SetShaderUniformColor(const char * VariableName, unsigned int color);
			void			UpdateShaderParams();
		};
		class EXPORT TextPainter
		{
		PRIVATE_INTERNAL:
			GApp::Resources::Font			*TextFont;
			GApp::Resources::Shader			*S;
			GApp::Utils::Buffer				Characters;
			float							X,Y,ViewLeft,ViewTop,ViewBottom,ViewRight,SpaceWidth,CharacterSpacing,FontSizeValue,HeightPercentage,MaxTextWidth,MaxTextHeight,TextLeft,TextTop,PaintScale;
			unsigned int					Flags;
			float							ShaderParams[16];
			unsigned char					ShaderParamInfo[4];
			BlendingInfo					Blend;

		public:
			TextPainter();
			bool			SetFont(GApp::Resources::Font *fnt);
			void			SetViewRect(float left,float top,float right,float bottom);
			void			SetViewRectWH(float left, float top, float width, float height);
			void			SetViewRectWH(float x, float y, GAC_TYPE_ALIGNAMENT align, float Width, float Height);
			void			MoveViewRectTo(float x, float y, GAC_TYPE_ALIGNAMENT align);
			void			SetLineSpace(float spacePercentage);			
			bool			SetText(const char *text,int textSize = -1);
			bool			SetText(GApp::Utils::String &str);
			void			SetWordWrap(bool value);
			void			SetTextJustify(bool value);
			void			SetFontSize(GAC_TYPE_FONTSIZETYPE mode,float value);
			void			SetPosition(float x,float y,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);			
			void			SetDockPosition(GAC_TYPE_ALIGNAMENT dockAlignament);
			void			SetShader(GApp::Resources::Shader *shader);
			void			SetAlphaBlending(float alpha);
			void			SetColorBlending(unsigned int color);
			void			SetCharacterSpacing(float value);
			void			SetSpaceWidth(float value);
			float			GetX();
			float			GetY();
			float			GetTextWidth();
			float			GetTextHeight();
			void			GetTextRect(RectF *r);
			void			ClearShaderParams();
			bool			SetShaderUniformValue(const char * VariableName, unsigned int count, float v1, float v2 = 0, float v3 = 0, float v4 = 0);
			bool			SetShaderUniformColor(const char * VariableName, unsigned int color);
			void			UpdateShaderParams();
			unsigned int	GetCharactersCount();
			void			SetCharactesVisibility(unsigned int startIndex, unsigned int endIndex, bool visibility);

			bool			CopyText(GApp::Utils::String * txt);

		};
		class EXPORT TextImagePainter
		{			
			RectF			ViewRect,FaceViewRect,TextViewRect,MarginRect;
			float			ImageTextSpace;
			unsigned char	ImageTextRelation;
			unsigned char	ImageTextAlignament;
			unsigned char	MarginCoordType;
			unsigned char	ImageTextSpaceCoordType;
			unsigned char	FaceViewCoordType;
			unsigned char	TextViewCoordType;

			void			RecomputeLayout();
		
		PROTECTED_INTERNAL:
			ImagePainter	Background;
			ImagePainter	Face;
			TextPainter		Text;
		public:		
			TextImagePainter();
			void			SetShader(GApp::Resources::Shader *shader);
			void			SetAlphaBlending(float alpha);
			void			SetColorBlending(unsigned int color);
			void			SetViewRect(float left,float top,float right,float bottom);
			void			SetViewRectWH(float left, float top, float width, float height);
			bool			SetViewRectFromImage(GApp::Resources::Bitmap *backgroundImage);
			void			SetInternalMargins(float marginLeft,float marginTop,float marginRight,float marginBottom,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);						
			void			SetSpaceBetweenImageAndText(float value,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);
			void			SetImageTextRelation(GAC_TYPE_IMAGETEXTRELATION relation,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);

			void			SetBackgroundImage(GApp::Resources::Bitmap *image);
			void			SetBackgroundShader(GApp::Resources::Shader *shader);
			void			SetBackgroundAlphaBlending(float alpha);
			void			SetBackgroundColorBlending(unsigned int color);
			void			SetBackgroundLayout(GAC_TYPE_IMAGERESIZEMODE mode,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			void			SetBackgroundPosition(float x,float y,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			void			SetBackgroundScale(float scaleWidth,float scaleHeight=0);
			void			SetBackgroundImageSize(float width,float height,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS);	

			void			SetFaceImage(GApp::Resources::Bitmap *image);
			void			SetFaceShader(GApp::Resources::Shader *shader);
			void			SetFaceAlphaBlending(float alpha);
			void			SetFaceColorBlending(unsigned int color);
			void			SetFaceLayout(GAC_TYPE_IMAGERESIZEMODE mode,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			void			SetFacePosition(float x,float y,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			void			SetFaceScale(float scaleWidth,float scaleHeight=0);
			void			SetFaceImageSize(float width,float height,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS);	
			void			SetFaceViewRect(float left,float top,float right,float bottom,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);

			bool			SetTextFont(GApp::Resources::Font *fnt);
			void			SetTextViewRect(float left,float top,float right,float bottom,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);
			void			SetTextLineSpace(float spacePercentage);			
			bool			SetText(const char *text);						
			void			SetTextWordWrap(bool value);
			void			SetTextJustify(bool value);
			void			SetTextFontSize(GAC_TYPE_FONTSIZETYPE mode,float scale);
			void			SetTextPosition(float x,float y,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);			
			void			SetTextDockPosition(GAC_TYPE_ALIGNAMENT dockAlignament);
			void			SetTextShader(GApp::Resources::Shader *shader);
			void			SetTextAlphaBlending(float alpha);
			void			SetTextColorBlending(unsigned int color);
			void			SetTextCharacterSpacing(float value);
			void			SetTextSpaceWidth(float value);
		};



		class EXPORT GraphicsContext
		{
		PRIVATE_INTERNAL:
			unsigned int			BlendImageMode;
			float					LastColorImageAlpha;
			unsigned int			LastColorImageColor;
			int						LastError;
			float					Width,Height,rapX,rapY;
			float					TranslateX,TranslateY;
			Rect					ClipRect;
			RectF					DrawingBounds;
			bool					ClippingEnabled,StencilsEnabled,UseDerivedShaderVariable;
			GApp::Resources::Shader	Default;
			GApp::Resources::Shader	Image;
			GApp::Resources::Shader	ColorImage;
			GApp::Resources::Shader	ImageNoAlpha;
			GApp::Resources::Shader	*CurrentShader;
			Texture					*CurrentTexture;
			GAC_TYPE_COORDINATES	DefaultCoordinates;
		public:
			bool		Init(int Width,int Height);

			void		Prepare();
			void		Finish();

			bool		ChangeShader(GApp::Resources::Shader *s);

			bool		DrawLine(float x1,float y1,float x2,float y2,unsigned int color, float penWidth=1,GApp::Resources::Shader* shader=NULL,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawLine(float x1,float y1,float x2,float y2,GApp::Resources::Shader* shader=NULL,float penWidth=1,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			
			bool		DrawRect(float left,float top,float right,float bottom,unsigned int borderColor,unsigned int fillColor=0,float penWidth=1,GApp::Resources::Shader* shader=NULL,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawRectWH(float left, float top, float width, float height, unsigned int borderColor, unsigned int fillColor = 0, float penWidth = 1, GApp::Resources::Shader* shader = NULL, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawRect(float left,float top,float right,float bottom,GApp::Resources::Shader* shader=NULL,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawRectF(RectF *rect, unsigned int borderColor, unsigned int fillColor = 0, float penWidth = 1, GApp::Resources::Shader* shader = NULL, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			//bool	DrawRect(float left,float top,float right,float bottom, int borderColor = GApp::Constants::Color_Transparent, float penWidth=1,Shader *s=NULL);

			bool		DrawExclusionRect(float left, float top, float right, float bottom, unsigned int fillColor, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);

			bool		Draw(ImagePainter *imgPainter);

			bool		DrawImage(GApp::Resources::Bitmap *bmp,float x,float y, GApp::Resources::Shader* shader=NULL,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawImageWithAlphaBlending(GApp::Resources::Bitmap *bmp, float x, float y, float alpha = 1.0f, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawImageWithColorBlending(GApp::Resources::Bitmap *bmp, float x, float y, unsigned int color = 0xFFFFFFFF, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);


			bool		DrawImageScaled(GApp::Resources::Bitmap *bmp,float x,float y, float scale, GApp::Resources::Shader* shader=NULL);
			bool		DrawImageScaledWithAlphaBlending(GApp::Resources::Bitmap *bmp, float x, float y, float scale, float alpha = 1.0f);
			bool		DrawImageScaledWithColorBlending(GApp::Resources::Bitmap *bmp, float x, float y, float scale, unsigned int color = 0xFFFFFFFF);


			bool		DrawImageAligned(GApp::Resources::Bitmap *bmp,float x,float y, GAC_TYPE_ALIGNAMENT align, float scale, GApp::Resources::Shader* shader=NULL,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawImageAlignedWithAlphaBlending(GApp::Resources::Bitmap *bmp, float x, float y, GAC_TYPE_ALIGNAMENT align, float scale, float alpha = 1.0f, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawImageAlignedWithColorBlending(GApp::Resources::Bitmap *bmp, float x, float y, GAC_TYPE_ALIGNAMENT align, float scale, unsigned int color = 0xFFFFFFFF, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);

			bool		DrawImageResizedAndAligned(GApp::Resources::Bitmap *bmp, float x, float y, GAC_TYPE_ALIGNAMENT align, float scaleWidth, float scaleHeight, GApp::Resources::Shader* shader = NULL, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawImageResizedAndAlignedWithColorBlending(GApp::Resources::Bitmap *bmp, float x, float y, GAC_TYPE_ALIGNAMENT align, float scaleWidth, float scaleHeight, unsigned int color = 0xFFFFFFFF, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);


			bool		DrawImageResized(GApp::Resources::Bitmap *bmp,float x,float y, float w, float h, GApp::Resources::Shader* shader=NULL,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawImageResizedWithAlphaBlending(GApp::Resources::Bitmap *bmp, float x, float y, float w, float h, float alpha = 1.0f, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawImageResizedWithColorBlending(GApp::Resources::Bitmap *bmp, float x, float y, float w, float h, unsigned int color = 0xFFFFFFFF, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);


			bool		DrawImageResized(GApp::Resources::Bitmap *bmp, float imageLeft,float imageTop, float imageWidth, float imageHeight,float x, float y, float w, float h, GApp::Resources::Shader* shader = NULL);
			bool		DrawImageResizedWithAlphaBlending(GApp::Resources::Bitmap *bmp, float imageLeft, float imageTop, float imageWidth, float imageHeight, float x, float y, float w, float h, float alpha = 1.0f);
			bool		DrawImageResizedWithColorBlending(GApp::Resources::Bitmap *bmp, float imageLeft, float imageTop, float imageWidth, float imageHeight, float x, float y, float w, float h, unsigned int color = 0xFFFFFFFF);


			bool		DrawImageCentered(GApp::Resources::Bitmap *bmp,float x,float y,GApp::Resources::Shader* shader=NULL,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawImageCenteredWithAlphaBlending(GApp::Resources::Bitmap *bmp, float x, float y, float alpha = 1.0f, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DrawImageCenteredWithColorBlending(GApp::Resources::Bitmap *bmp, float x, float y, unsigned int color = 0xFFFFFFFF, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);


			bool		Draw9PatchImage(GApp::Resources::Bitmap *bmp, float x, float y, float w, float h, float pLeft, float pTop, float pRight, float pBottom, GApp::Resources::Shader *shader=NULL);
			
			bool		FillScreen(unsigned int color);
			bool		FillScreen(GApp::Resources::Bitmap *bmp,GApp::Resources::Shader *shader=NULL);	
			bool		FillScreenWithAlphaBlending(GApp::Resources::Bitmap *bmp, float alpha = 1.0f);
			bool		FillScreenWithColorBlending(GApp::Resources::Bitmap *bmp, unsigned int color = 0xFFFFFFFF);
			bool		FillScreen(GApp::Resources::Shader *shader);
			
			// DEBUG ONLY
			bool		DrawTexture(unsigned int texID);

			bool		DrawString(TextPainter *txtPainter,float offsetX=0.0f,float offsetY=0.0f);
			bool		Draw(TextImagePainter *txp);

			void		SetTranslate(float x,float y);
			void		ClearTranslate();

			bool		SetClip(int left,int top,int right,int bottom);
			bool		SetClip(Rect *rect);
			void		GetClipRect(Rect *rect);
			bool		ClearClip();

			bool		SetClipRect(bool inside, float x1, float y1, float x2, float y2, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		SetClipImage(bool inside, GApp::Resources::Bitmap *bmp, float x, float y, GAC_TYPE_ALIGNAMENT align, float scale, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			bool		DisableAdvancedClipping();

			bool		SetCoordinatesType(GAC_TYPE_COORDINATES newCoordType);
			
			bool		SetDrawingBoundsRect(float pxLeft,float pxTop,float pxRight,float pxBottom);
			float		CoordinateToXPixel(float value,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
			float		CoordinateToYPixel(float value,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_AUTO);
//[REMOVE-IN-DEVELOP-LIB:START]
#ifdef PLATFORM_DEVELOP
//[REMOVE-IN-DEVELOP-LIB:STOP]
			GLuint		fbo;
//[REMOVE-IN-DEVELOP-LIB:START]
#endif
//[REMOVE-IN-DEVELOP-LIB:STOP]
		};
	};
	namespace Media
	{
		class EXPORT AudioPlayer
		{	
		PRIVATE_INTERNAL:
			CoreReference	CoreContext;
		public:
			AudioPlayer();
			int				GetSoundVolume();
			int				GetMusicVolume();
			void			SetMusicVolume(int value);
			void			SetSoundVolume(int value);
			void			SetMusicEnabled(bool state);
			void			SetSoundEnabled(bool state);
			void			SetMediaEnabled(bool state);
			void			SetMusicLoop(bool loop);
			bool			IsMusicLooped();
			bool			IsMusicEnabled();
			bool			IsSoundEnabled();
			bool			IsMediaEnabled();
			bool			Play(GApp::Resources::Sound *snd);
			bool			Play(GApp::Resources::Sound *snd,bool loop);
			bool			Play(GApp::Resources::Sound *snd,int volume,bool loop);
			void			Stop(GApp::Resources::Sound *snd);
			bool			PlayBackgroundMusic(GApp::Resources::Sound *snd=NULL);
			void			StopBackgroundMusic();
			void			ChangeAudioSettings(GAC_TYPE_CHANGEAUDIOSETTINGSMETHOD method);

		};
	};
	namespace Controls
	{
		struct EXPORT TouchEvent
		{
			int PointerID;
			float X, Y;
			float OriginalX, OriginalY;
			float LastX, LastY;
			long Time;
			long TimeDelta;			
			float ZoomValue;
			float OriginalDistance;
			float LastDistance;
			float Distance;
			GAC_TYPE_TOUCHEVENTTYPE Type;
			bool IsClick;
			bool IsPartOfZoom;
		};
		class EXPORT KeyboardEvent
		{
		public:
			unsigned int	KeyCode;
			unsigned int	Flags;
			bool			IsBackPressed();
		};
		struct EXPORT LayoutData
		{
			float			X,Y,Width,Height;
			unsigned int	Flags;	

			float			LeftPixels,TopPixels,WidthInPixels,HeighInPixels,RelativeLeftPixels,RelativeTopPixels;
			float			ScrollXPixels,ScrollYPixels;
			float			TranslateX,TranslateY;
			int				ViewLeft,ViewTop,ViewBottom,ViewRight;
		};
		class EXPORT GenericControl
		{
		PRIVATE_INTERNAL:
			GenericControl		**Controls;
			unsigned int		ControlsCount;
			GenericControl		*Parent;
			unsigned int		Flags;
			int					ID,GroupID;
			GApp::UI::Scene*	HandlerScene;
			LayoutData			Layout;
			void				RecomputeLayout();
		PROTECTED_INTERNAL:
			CoreReference		CoreContext;
		protected:			
		public:
			GenericControl(void *scene,const char *layout);
			void			TriggerEvent(GAC_TYPE_EVENTTYPE EventType);
			void			MoveTo(float x,float y,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS);
			void			Resize(float width,float height,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS);
			bool			SetLayout(const char *format);
			bool			SetLayout(const char *format,float param1);
			bool			SetLayout(const char *format,float param1, float param2);
			bool			SetLayout(const char *format,float param1, float param2, float param3);
			bool			SetLayout(const char *format,float param1, float param2, float param3, float param4);
			void			SetLayout(float x,float y, GAC_TYPE_COORDINATES xyType,float width,float height,GAC_TYPE_COORDINATES whType,GAC_TYPE_ALIGNAMENT align);
			bool			AddControl(GenericControl *control);			
			void			SetVisible(bool value);
			void			GAC_SetVisible(bool value);
			void			SetEnabled(bool value);
			void			GAC_SetEnabled(bool value);
			void			SetChecked(bool value);
			void			SetID(int value);
			void			SetGroupID(int value);
			bool			IsVisible();
			bool			IsEnabled();
			bool			IsChecked();
			bool			HasTouch();
			int				GetID();
			int				GetGroupID();

			bool			ProcessTouchEventForClick(TouchEvent *evn);

			float			GetWidth();
			float			GetHeight();
			float			GetLeft();
			float			GetTop();
			float			GetRelativeLeft();
			float			GetRelativeTop();
			float			GetRight();
			float			GetBottom();
			bool			GetRect(Graphics::Rect *);
			float			GetScrollX();
			float			GetScrollY();
			
			GenericControl*	GetParent();
			unsigned int	GetControlsCount();
			GenericControl* GetControl(unsigned int index);
			
			
			virtual void	OnPaint();
			virtual void	OnReceiveFocus();
			virtual void	OnLoseFocus();
			virtual bool	OnTouchEvent(TouchEvent *evn);
			virtual bool	OnLayout();

			virtual			~GenericControl();

		};
		class EXPORT Button: public GenericControl
		{
			GApp::Graphics::TextImagePainter	Normal,Pressed,Inactive;
			void				_Create(int ID, GApp::Resources::Bitmap *BackgroundNormal, GApp::Resources::Bitmap *BackgroundPressed, GApp::Resources::Bitmap *BackgroundInactive, GApp::Resources::Bitmap *FaceNormal, GApp::Resources::Bitmap *FacePressed, GApp::Resources::Bitmap *FaceInactive);
		public:
			Button(void *scene, int ID, const char* layout, GApp::Resources::Bitmap *BackgroundNormal=NULL,GApp::Resources::Bitmap *BackgroundPressed=NULL,GApp::Resources::Bitmap *BackgroundInactive=NULL,GApp::Resources::Bitmap *FaceNormal=NULL,GApp::Resources::Bitmap *FacePressed=NULL,GApp::Resources::Bitmap *FaceInactive=NULL);
			Button(void *scene, int ID, const char* layout, GApp::Resources::Bitmap *Background, GApp::Resources::Bitmap *Face, unsigned int blendColorNormal,unsigned int blendColorPressed,unsigned int blendColorInactive = 0x80FFFFFF);
			
			// Background
			bool				SetBackgroundImage(GAC_BUTTON_STATE_TYPE state, GApp::Resources::Bitmap *Image, bool autoResize = false);
			bool				SetBackgroundShader(GAC_BUTTON_STATE_TYPE state, GApp::Resources::Shader *shader);
			bool				SetBackgroundAlphaBlending(GAC_BUTTON_STATE_TYPE state,float alpha);
			bool				SetBackgroundColorBlending(GAC_BUTTON_STATE_TYPE state, unsigned int color);
			bool				SetBackgroundShaderParams(GAC_BUTTON_STATE_TYPE state, const char *VariableName, unsigned int countParams, float p1, float p2 = 0, float p3 = 0, float p4 = 0);
			bool				SetBackgroundShaderColor(GAC_BUTTON_STATE_TYPE state, const char *VariableName, unsigned int color);
			bool				SetBackgroundLayout(GAC_BUTTON_STATE_TYPE state, GAC_TYPE_IMAGERESIZEMODE mode, GAC_TYPE_ALIGNAMENT align);

			// Face
			bool				SetFaceImage(GAC_BUTTON_STATE_TYPE state, GApp::Resources::Bitmap *Image);
			bool				SetFaceShader(GAC_BUTTON_STATE_TYPE state, GApp::Resources::Shader *shader);
			bool				SetFaceAlphaBlending(GAC_BUTTON_STATE_TYPE state, float alpha);
			bool				SetFaceColorBlending(GAC_BUTTON_STATE_TYPE state, unsigned int color);
			bool				SetFaceShaderParams(GAC_BUTTON_STATE_TYPE state, const char *VariableName, unsigned int countParams, float p1, float p2 = 0, float p3 = 0, float p4 = 0);
			bool				SetFaceShaderColor(GAC_BUTTON_STATE_TYPE state, const char *VariableName, unsigned int color);
			bool				SetFaceLayout(GAC_BUTTON_STATE_TYPE state, GAC_TYPE_IMAGERESIZEMODE mode, GAC_TYPE_ALIGNAMENT align);
			bool				SetFacePosition(GAC_BUTTON_STATE_TYPE state, float x, float y, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE, GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);

			bool				SetFaceViewRect(GAC_BUTTON_STATE_TYPE state, float left, float top, float right, float bottom, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);

			//Text
			bool				SetText(GAC_BUTTON_STATE_TYPE state, const char *text);
			bool				SetTextFont(GAC_BUTTON_STATE_TYPE state, GApp::Resources::Font *font);
			bool				SetTextFontSize(GAC_BUTTON_STATE_TYPE state, GAC_TYPE_FONTSIZETYPE mode, float value);
			bool				SetTextShader(GAC_BUTTON_STATE_TYPE state, GApp::Resources::Shader *shader);
			bool				SetTextAlphaBlending(GAC_BUTTON_STATE_TYPE state, float alpha);
			bool				SetTextColorBlending(GAC_BUTTON_STATE_TYPE state, unsigned int color);
			bool				SetTextCharacterSpacing(GAC_BUTTON_STATE_TYPE state, float value);
			bool				SetTextSpaceWidth(GAC_BUTTON_STATE_TYPE state, float value);
			bool				SetTextLineSpace(GAC_BUTTON_STATE_TYPE state, float value);
			bool				SetTextDockPosition(GAC_BUTTON_STATE_TYPE state, GAC_TYPE_ALIGNAMENT value);
			bool				SetTextWordWrap(GAC_BUTTON_STATE_TYPE state, bool value);
			bool				SetTextViewRect(GAC_BUTTON_STATE_TYPE state, float left, float top, float right, float bottom, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);
			bool				SetTextShaderParams(GAC_BUTTON_STATE_TYPE state, const char *VariableName, unsigned int countParams, float p1, float p2=0, float p3=0, float p4=0);
			bool				SetTextShaderColor(GAC_BUTTON_STATE_TYPE state, const char *VariableName, unsigned int color);


			bool				SetInternalMargins(GAC_BUTTON_STATE_TYPE state,float marginLeft, float marginTop, float marginRight, float marginBottom, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);
			bool				SetSpaceBetweenImageAndText(GAC_BUTTON_STATE_TYPE state,float value, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);
			bool				SetImageTextRelation(GAC_BUTTON_STATE_TYPE state,GAC_TYPE_IMAGETEXTRELATION relation, GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			
			void				OnPaint();
			bool				OnTouchEvent(GApp::Controls::TouchEvent *te);
			bool				OnLayout();	// true - daca s-a schimbat pozitia / size-ul, false altfel
		};
		class EXPORT AnimationButton: public GenericControl
		{
			GApp::Animations::AnimationObject *normal;
			GApp::Animations::AnimationObject *pressed;
			GApp::Animations::AnimationObject *inactive;
		public:
			AnimationButton(void *scene, int ID, const char* layout, GApp::Animations::AnimationObject * normal, GApp::Animations::AnimationObject * pressed, GApp::Animations::AnimationObject * inactive = NULL);
			AnimationButton(void *scene, int ID, const char* layout, GApp::Animations::AnimationObject * anim);
			void				OnPaint();
			bool				OnTouchEvent(GApp::Controls::TouchEvent *te);
			
			GApp::Animations::AnimationObject * GetAnimationForNormalState();
			GApp::Animations::AnimationObject * GetAnimationForPressedState();
			GApp::Animations::AnimationObject * GetAnimationForInactiveState();
			
			void				RestartAnimations();
		};
		class EXPORT AnimationControl : public GenericControl
		{
			GApp::Animations::AnimationObject *anim;
			bool				Loop;	
			bool				AboutToStart;
		public:
			AnimationControl(void *scene, const char* layout, GApp::Animations::AnimationObject * _anim = NULL, bool start = true, bool loop = false);
			void				SetAnimation(GApp::Animations::AnimationObject * _anim,bool start=true,bool loop=false);
			void				OnPaint();

			void				Start();
			void				Stop();
			void				Pause();
			void				Resume();
			bool				IsStarted();
			bool				IsPaused();

			bool				OnTouchEvent(GApp::Controls::TouchEvent *te);
			GApp::Animations::AnimationObject * GetAnimation() { return anim; }
		};
		class EXPORT CheckBox : public GenericControl
		{
			GApp::Graphics::TextImagePainter	NormalAndChecked, NormalAndUnChecked, PressedAndChecked, PressedAndUnChecked, InactiveAndChecked, InactiveAndUnChecked;
		public:
			CheckBox(void *scene, int ID, const char* layout, GApp::Resources::Bitmap *BackgroundNormal = NULL, GApp::Resources::Bitmap *BackgroundPressed = NULL, GApp::Resources::Bitmap *BackgroundInactive = NULL, GApp::Resources::Bitmap *FaceChecked = NULL, GApp::Resources::Bitmap *FaceUnchecked = NULL, GApp::Resources::Bitmap *FaceInactive = NULL);

			// Background
			bool				SetBackgroundImage(GAC_CHECKBOX_STATE_TYPE state, GApp::Resources::Bitmap *Image, bool autoResize = false);
			bool				SetBackgroundShader(GAC_CHECKBOX_STATE_TYPE state, GApp::Resources::Shader *shader);
			bool				SetBackgroundShaderParams(GAC_CHECKBOX_STATE_TYPE state, const char *VariableName, unsigned int countParams, float p1, float p2 = 0, float p3 = 0, float p4 = 0);
			bool				SetBackgroundShaderColor(GAC_CHECKBOX_STATE_TYPE state, const char *VariableName, unsigned int color);
			bool				SetBackgroundLayout(GAC_CHECKBOX_STATE_TYPE state, GAC_TYPE_IMAGERESIZEMODE mode, GAC_TYPE_ALIGNAMENT align);

			// Face
			bool				SetFaceImage(GAC_CHECKBOX_STATE_TYPE state, GApp::Resources::Bitmap *Image);
			bool				SetFaceShader(GAC_CHECKBOX_STATE_TYPE state, GApp::Resources::Shader *shader);
			bool				SetFaceShaderParams(GAC_CHECKBOX_STATE_TYPE state, const char *VariableName, unsigned int countParams, float p1, float p2 = 0, float p3 = 0, float p4 = 0);
			bool				SetFaceShaderColor(GAC_CHECKBOX_STATE_TYPE state, const char *VariableName, unsigned int color);
			bool				SetFaceLayout(GAC_CHECKBOX_STATE_TYPE state, GAC_TYPE_IMAGERESIZEMODE mode, GAC_TYPE_ALIGNAMENT align);

			bool				SetFaceViewRect(GAC_CHECKBOX_STATE_TYPE state, float left, float top, float right, float bottom, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);

			//Text
			bool				SetText(GAC_CHECKBOX_STATE_TYPE state, const char *text);
			bool				SetTextFont(GAC_CHECKBOX_STATE_TYPE state, GApp::Resources::Font *font);
			bool				SetTextFontSize(GAC_CHECKBOX_STATE_TYPE state, GAC_TYPE_FONTSIZETYPE mode, float value);
			bool				SetTextShader(GAC_CHECKBOX_STATE_TYPE state, GApp::Resources::Shader *shader);
			bool				SetTextCharacterSpacing(GAC_CHECKBOX_STATE_TYPE state, float value);
			bool				SetTextSpaceWidth(GAC_CHECKBOX_STATE_TYPE state, float value);
			bool				SetTextLineSpace(GAC_CHECKBOX_STATE_TYPE state, float value);
			bool				SetTextDockPosition(GAC_CHECKBOX_STATE_TYPE state, GAC_TYPE_ALIGNAMENT value);
			bool				SetTextWordWrap(GAC_CHECKBOX_STATE_TYPE state, bool value);
			bool				SetTextViewRect(GAC_CHECKBOX_STATE_TYPE state, float left, float top, float right, float bottom, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);
			bool				SetTextShaderParams(GAC_CHECKBOX_STATE_TYPE state, const char *VariableName, unsigned int countParams, float p1, float p2=0, float p3=0, float p4=0);
			bool				SetTextShaderColor(GAC_CHECKBOX_STATE_TYPE state, const char *VariableName, unsigned int color);


			bool				SetInternalMargins(GAC_CHECKBOX_STATE_TYPE state, float marginLeft, float marginTop, float marginRight, float marginBottom, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);
			bool				SetSpaceBetweenImageAndText(GAC_CHECKBOX_STATE_TYPE state, float value, GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);
			bool				SetImageTextRelation(GAC_CHECKBOX_STATE_TYPE state, GAC_TYPE_IMAGETEXTRELATION relation, GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);

			void				OnPaint();
			bool				OnTouchEvent(GApp::Controls::TouchEvent *te);
			bool				OnLayout();	// true - daca s-a schimbat pozitia / size-ul, false altfel
		};
		class EXPORT Panel: public GenericControl
		{
			GApp::Graphics::ImagePainter	Image;
		public:
			Panel(void* scene,const char *layout);
			Panel(void* scene,const char *layout,GApp::Resources::Bitmap *img,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER,GAC_TYPE_IMAGERESIZEMODE resizeMode =  GAC_IMAGERESIZEMODE_DOCK);

			void	SetImage(GApp::Resources::Bitmap *image);
			void	SetImageSize(float width,float height,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS);
			void	SetScale(float scaleWidth,float scaleHeight=0.0f);
			void	SetPosition(float x,float y,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);				
			void	SetImageLayout(GAC_TYPE_IMAGERESIZEMODE resizeMode, GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			void	SetShader(GApp::Resources::Shader *shader);

			void	OnPaint();			
			bool	OnLayout();	// true - daca s-a schimbat pozitia / size-ul, false altfel
		};
		class EXPORT ImageTextView: public GenericControl
		{
		protected:
			GApp::Graphics::TextImagePainter	Content;
		public:
			ImageTextView(void *scene, const char *layout,GApp::Resources::Bitmap *img,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER, GAC_TYPE_IMAGERESIZEMODE resizeMode =  GAC_IMAGERESIZEMODE_DOCK);
			ImageTextView(void *scene, const char *layout,GApp::Resources::Bitmap *img,const char *text,GApp::Resources::Font *font,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER,GAC_TYPE_IMAGERESIZEMODE resizeMode =  GAC_IMAGERESIZEMODE_DOCK);
			ImageTextView(void *scene, const char *layout,const char *text,GApp::Resources::Font *font);

			void				OnPaint();			
			bool				OnLayout();	// true - daca s-a schimbat pozitia / size-ul, false altfel

			void				SetBackgroundImage(GApp::Resources::Bitmap *image);
			void				SetBackgroundShader(GApp::Resources::Shader *shader);
			bool				SetBackgroundShaderParams(const char *VariableName, unsigned int countParams, float p1, float p2 = 0, float p3 = 0, float p4 = 0);
			bool				SetBackgroundShaderColor(const char *VariableName, unsigned int color);
			void				SetBackgroundLayout(GAC_TYPE_IMAGERESIZEMODE mode,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			void				SetBackgroundPosition(float x,float y,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			void				SetBackgroundScale(float scaleWidth,float scaleHeight=0);
			void				SetBackgroundImageSize(float width,float height,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS);
			void				SetBackgroundAlphaBlending(float alpha);
			void				SetBackgroundColorBlending(unsigned int color);


			void				SetFaceImage(GApp::Resources::Bitmap *FaceImage);
			void				SetFaceImageSize(float width,float height,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS);
			void				SetFaceScale(float scaleWidth,float scaleHeight=0.0f);
			void				SetFacePosition(float x,float y,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);				
			void				SetFaceLayout(GAC_TYPE_IMAGERESIZEMODE resizeMode, GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			void				SetFaceShader(GApp::Resources::Shader *shader);
			bool				SetFaceShaderParams(const char *VariableName, unsigned int countParams, float p1, float p2 = 0, float p3 = 0, float p4 = 0);
			bool				SetFaceShaderColor(const char *VariableName, unsigned int color);
			void				SetFaceViewRect(float left,float top,float right,float bottom,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);
			void				SetFaceAlphaBlending(float alpha);
			void				SetFaceColorBlending(unsigned int color);

			void				SetText(const char *text);
			void				SetTextFont(GApp::Resources::Font *font);
			void				SetTextFontSize(GAC_TYPE_FONTSIZETYPE mode,float value);
			void				SetTextShader(GApp::Resources::Shader *shader);
			bool				SetTextShaderParams(const char *VariableName, unsigned int countParams, float p1, float p2 = 0, float p3 = 0, float p4 = 0);
			bool				SetTextShaderColor(const char *VariableName, unsigned int color);
			void				SetTextCharacterSpacing(float value);
			void				SetTextSpaceWidth(float value);
			void				SetTextLineSpace(float value);
			void				SetTextDockPosition(GAC_TYPE_ALIGNAMENT value);
			void				SetTextWordWrap(bool value);
			void				SetTextJustify(bool value);
			void				SetTextPosition(float x,float y,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			void				SetTextViewRect(float left,float top,float right,float bottom,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);
			void				SetTextAlphaBlending(float alpha);
			void				SetTextColorBlending(unsigned int color);

			void				SetInternalMargins(float marginLeft,float marginTop,float marginRight,float marginBottom,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);						
			void				SetSpaceBetweenImageAndText(float value,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PERCENTAGE);
			void				SetImageTextRelation(GAC_TYPE_IMAGETEXTRELATION relation,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);

		};
		class EXPORT Toast: public ImageTextView
		{
			unsigned int			curentFrame,framesCount;
			unsigned int			uniformIndex;
			GApp::Resources::Shader	*shaderptr;
		public:
			Toast(void *scene, const char *layout,GApp::Resources::Bitmap *img,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER, GAC_TYPE_IMAGERESIZEMODE resizeMode =  GAC_IMAGERESIZEMODE_DOCK);
			Toast(void *scene, const char *layout,GApp::Resources::Bitmap *img,const char *text,GApp::Resources::Font *font,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER,GAC_TYPE_IMAGERESIZEMODE resizeMode =  GAC_IMAGERESIZEMODE_DOCK);
			Toast(void *scene, const char *layout,const char *text,GApp::Resources::Font *font);

			void	SetAlphaShader(GApp::Resources::Shader *shader,unsigned int uniformIndex = 0);
			void	SetShowTime(unsigned int frames);
			void	Show();
			void	Hide();
			void	OnPaint();
		};
		class EXPORT Label: public GenericControl
		{
			GApp::Graphics::TextPainter		Content;
		public:
			Label(void *scene, const char *layout, const char *text, GApp::Resources::Font *fnt, GAC_TYPE_ALIGNAMENT dock = GAC_ALIGNAMENT_CENTER, unsigned int colorBlend = 0xFFFFFFFF);
			Label(void *scene, const char *layout, const char *text, GApp::Resources::Font *fnt, GAC_TYPE_ALIGNAMENT dock, unsigned int colorBlend, float size, bool wordWrap);
			void				OnPaint();			
			bool				OnLayout();	// true - daca s-a schimbat pozitia / size-ul, false altfel
			void				SetText(const char *text);
			void				SetTextShader(GApp::Resources::Shader *shader);
			bool				SetTextShaderColor(const char *VariableName, unsigned int color);
			void				SetTextFont(GApp::Resources::Font *font);
			void				SetTextFontSize(GAC_TYPE_FONTSIZETYPE mode,float size);
			void				SetTextCharacterSpacing(float value);
			void				SetTextSpaceWidth(float value);
			void				SetTextLineSpace(float value);
			void				SetTextDockPosition(GAC_TYPE_ALIGNAMENT value);
			void				SetTextWordWrap(bool value);
			void				SetTextJustify(bool value);
			void				SetTextPosition(float x,float y,GAC_TYPE_COORDINATES coord = GAC_COORDINATES_PIXELS,GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER);
			void				SetTextColorBlending(unsigned int color);
			void				SetTextAlphaBlending(float alpha);
		};
		class EXPORT ScrollPanel: public GenericControl
		{
			float	GoToX,GoToY,OriginalScrollX,OriginalScrollY,AlignWidth,AlignHeight,SlideStep,ScrollWidth,ScrollHeight;
		public:
			ScrollPanel(void* scene,const char *layout);
			bool			SetSlideStep(float value);
			bool			SetScrollAlignamentSize(float width,float height);
			bool			SetScrollSize(float width,float height);
			void			MoveScrollTo(float X,float Y,bool slide);
			bool			MoveScrollToControl(GenericControl *control,  bool slide);
			void			OnPaint();
			bool			OnTouchEvent(GApp::Controls::TouchEvent *te);
			bool			OnLayout();	// true - daca s-a schimbat pozitia / size-ul, false altfel
			virtual void	OnScrollCompleted();
			virtual void	OnScrollChanged();
		};
		class EXPORT PageControl: public ScrollPanel
		{
			unsigned int	Rows,Columns;
		public:
			PageControl(void* scene,const char *layout,unsigned int columns,unsigned int row);			
			GenericControl* GetPage(unsigned int index);
			GenericControl* GetPage(unsigned int column,unsigned int row);
			unsigned int	GetCurrentPageIndex();
			unsigned int	GetPagesCount();
			GenericControl* GetCurrentPage();
			bool			SetCurrentPage(unsigned int index, bool slide);
			bool			GoToNextPage(bool slide);
			bool			GoToPreviousPage(bool slide);
			bool			GoToFirstPage(bool slide);
			bool			GoToLastPage(bool slide);
			bool			OnLayout();
			void			OnScrollCompleted();
		};
		class EXPORT SlideSelector : public ScrollPanel
		{
			unsigned int	Rows, Columns, CellWidth,CellHeight;
		public:
			SlideSelector(void* scene, const char *layout, unsigned int columns, unsigned int rows,unsigned int cellWidth,unsigned int cellHeight);
			GenericControl* GetCell(unsigned int index);
			GenericControl* GetCell(unsigned int column, unsigned int row);
			unsigned int	GetSelectedCellIndex();
			unsigned int	GetCellsCount();
			unsigned int	GetCellWidth();
			unsigned int	GetCellHeight();
			bool			SelectCell(unsigned int index, bool slide);
			bool			GoToNextCell(bool slide);
			bool			GoToPreviousCell(bool slide);
			bool			GoToFirstCell(bool slide);
			bool			GoToLastCell(bool slide);
			bool			OnLayout();
			void			OnScrollCompleted();
			void			OnScrollChanged();
			float			ComputeCellDistanceToCenter(unsigned int cellIndex);
		};
		class EXPORT PresentationControl :public GenericControl
		{
			unsigned int CurrentFrame,goto_frame;
			unsigned int RemainingTicks, CurrentBookmark,nrFrames,nrBookmarks;
			unsigned int Flags;
			unsigned int ObjectBufferOffset,FrameObjectsBufferOffset,FrameObjectsCount;
			float ResFactor,AnimationSpeed;
			GApp::Resources::Presentation	*Anim;
			GApp::Graphics::TextPainter		tp;

			bool			Prepare();
			void			Dispose();
			bool			PaintObject(int offset);
			bool			PaintFrame();
		public:
			PresentationControl(void *scene, const char* layout);
			PresentationControl(void *scene, const char* layout, GApp::Resources::Presentation *pres);
			PresentationControl(void *scene, const char* layout, GApp::Resources::Presentation *pres, bool autoGotoNextFrame, bool startPresentation, bool loop);
			bool			LoadPresentation(GApp::Resources::Presentation *pres,bool autoGotoNextFrame,bool startPresentation, bool loop);
			void			OnPaint();			
			//bool			OnLayout();	// true - daca s-a schimbat pozitia / size-ul, false altfel
			bool			Start();
			bool			Stop();
			bool			IsPresentationLoaded();
			bool			IsRunning();
			void			SetLoop(bool value);
			void			SetAutoGotoToNextFrame(bool value);
			unsigned int	GetFramesCount();
			unsigned int	GetCurrentFrame();
			bool			GoToFrame(unsigned int index);
			bool			GoToNextFrame();
			bool			GoToPreviousFrame();
			unsigned int	GetBookmarksCount();
			unsigned int	GetCurrentBookmark();
			bool			GoToBookmark(unsigned int index);
			bool			HasBookmarks();
			bool			GoToNextBookmark();
			bool			GoToPreviousBookmark();
			bool			SetPresentationSpeed(float speedFactor);
		};
		class EXPORT ModalFrame : protected GenericControl
		{
		protected:
			void			Hide();
		public:
			ModalFrame(void *scene);
			void			Show();
			void			Close(int controlID);
			bool			IsOpened();
			bool			AddControl(GenericControl *control);
			unsigned int	GetControlsCount();
			GenericControl* GetControl(unsigned int index);

			operator GenericControl*() { return (GenericControl*)this; }

			virtual void	OnPaint();
			virtual bool	OnTouchEvent(TouchEvent *evn);
			virtual void	OnShow();
			virtual void	OnClosing();
		};
		class EXPORT Popup : public ModalFrame
		{
			unsigned int backgroundColor;
			int alpha, alphaMax, alphaStep;
		public:
			Popup(void* scene, const char *layout, GApp::Resources::Bitmap *img, GAC_TYPE_ALIGNAMENT align = GAC_ALIGNAMENT_CENTER, GAC_TYPE_IMAGERESIZEMODE resizeMode = GAC_IMAGERESIZEMODE_DOCK);
			Popup(void* scene, const char *layout);
			
			void			SetBackgroundAlphaEfect(int maxAlpha, int step, unsigned int backColor = 0);
			bool			AddControl(GenericControl *control);
			virtual void	OnPaint();
			virtual void	OnShow();
			virtual void	OnClosing();
			bool			OnTouchEvent(GApp::Controls::TouchEvent *te);
		};
		class EXPORT AnimationPopup : public ModalFrame
		{
			GApp::Animations::AnimationObject * back;
			GApp::Animations::AnimationObject * end;
			unsigned char	Type;
			bool			Closing;
		public:
			AnimationPopup(void* scene, GApp::Animations::AnimationObject *backgroundAnimation, bool autoCloseWhenAnimationEnds);
			AnimationPopup(void* scene, GApp::Animations::AnimationObject *backgroundAnimation, GApp::Animations::AnimationObject *endAnimation );

			bool			AddControl(GenericControl *control);
			virtual void	OnShow();
			virtual void	OnPaint();
			virtual void	OnClosing();
			bool			OnTouchEvent(GApp::Controls::TouchEvent *te);

			GApp::Animations::AnimationObject * GetAnimation() { return back; }
		};
	};

	namespace UI
	{
		struct AutoEnableStateCondition
		{
			unsigned int		Condition;
			unsigned int		Value;
			void*				Ptr;
			bool				andOperation;
		};
		class EXPORT Alarm
		{
			GAC_TYPE_ALARMSTATE	ShouldTrigger_ExactDate(GApp::Utils::DateTime * dt, unsigned int secondsFrom2000, unsigned int &missed);
			GAC_TYPE_ALARMSTATE	ShouldTrigger_SpecificDate(GApp::Utils::DateTime * dt, unsigned int secondsFrom2000, unsigned int &missed);
			GAC_TYPE_ALARMSTATE	ShouldTrigger_MonthDay(GApp::Utils::DateTime * dt);
			GAC_TYPE_ALARMSTATE	ShouldTrigger_WeekDay(GApp::Utils::DateTime * dt, unsigned int &missed);
			GAC_TYPE_ALARMSTATE	ShouldTrigger_SpecificHourMinute(GApp::Utils::DateTime * dt, unsigned int secondsFrom2000, unsigned int &missed);
			GAC_TYPE_ALARMSTATE	ShouldTrigger_EveryXTime(unsigned int measurement, unsigned int secondsFrom2000, unsigned int &missed);
		public:
			unsigned int		Index,UniqueID;
			unsigned int		Year,Month,Day,Hour,Minute;
			unsigned int		LastTriggered;
			unsigned int		Duration;
			int					AlarmType;			
			const char*			PushNotificationString;
			bool				Enabled;
			bool				OneTimeOnly;

			void				Init(unsigned int _index,unsigned int _uniqueID, bool _enabled, bool _oneTimeOnly, unsigned int _duration, const char * pushNotificationMessage,  int _alarmType, unsigned int _year,unsigned int _month, unsigned int _day,unsigned int _hour,unsigned int _minute);
			bool				IsEnabled();
			GAC_TYPE_ALARMSTATE	ShouldTrigger(GApp::Utils::DateTime * dt, unsigned int secondsFrom2000, unsigned int &missed);
			void				Start();
			void				Stop();

			static bool			DateTimeToSecondsFrom2000(GApp::Utils::DateTime *dt, unsigned int &result);
		};




		class EXPORT Counter
		{
		public:
			unsigned int	Index;
			unsigned int	Value;
			unsigned int	Times;
			unsigned int	Interval;
			unsigned int	MaxTimes;
			unsigned int	Hash;
			unsigned int	Priority;
			unsigned int	ActionsCount;
			unsigned int	GoToSceneID;
			bool			Enabled;
			const char*		Name;
			const char*		Group;
			AutoEnableStateCondition	Conditions[MAX_AUTO_ENABLE_STATE_CONDITIONS];
			unsigned int				ConditionsCount;

			void			Init(unsigned int _index, unsigned int _interval, unsigned int _maxTimes, unsigned int _hash, unsigned int _priority, bool _enabled, const char *_name, const char * _group, unsigned int _gotoScene);
			bool			AddAutoEnableStateCondition(unsigned int Condition, unsigned int Value, void* Ptr, bool applyAndOperator);
			bool			Update();
			bool			IsEnabled();
			void			Enable();
			void			Disable();
			void			SetEnableState(bool value);
			void			UpdateEnableStateFromConditions(CoreReference CoreContext);
			void			Reset();
			void			IncrementActionsCount();
			void			ResetActionsCount();
			unsigned int	GetActionsCount();

			unsigned int	EncodeToPersistenValue();
			void			DecodeFromPersistentValue(unsigned int encoded_value);
		};
		class EXPORT CountersGroup
		{
		PRIVATE_INTERNAL:
			CoreReference	CoreContext;
		public:
			unsigned int	Index;
			unsigned int	UpdateMethod;
			unsigned int	Count,Capacity;
			unsigned int	CountTillEnabled;
			unsigned int	MinTimeLimit;
			unsigned int	StartTime;
			unsigned int	CumulativeTime;
			unsigned int	AfterUpdateTimerStatus;
			unsigned int	TimerBefausePauseStatus;
			unsigned int	StartTimeMethod;
			unsigned int	StartTimeScene;
			const char*		Name;
			bool			UseEnableConditionProperty;
			
			Counter**		Counters;
			Counter*		LastResult;

			bool			Init(CoreReference context, unsigned int index, const char * name, unsigned int _updateMethod, unsigned int _count, unsigned int _minTimeLimit, int _startTimeMethod, unsigned int _afterUpdateTimerStatus, unsigned int _gotoScene, bool _useEnableConditionProperty);
			bool			Add(Counter *element);
			Counter*		Update();
			Counter*		GetLastUpdateResult() { return LastResult; }

			bool			IsEnabled();
			void			Enable();
			void			Disable();
			void			DisableForMultipleUpdates(unsigned int nrOfUpdates);
			void			DisableForNextUpdate();
			void			EnableCountersUsingEnableConditionProperty();
			void			StartTimer();
			void			StopTimer();
			void			PauseTimer();
			void			ResumeTimer();



		private:
			Counter*		UpdateAllCounters();
			Counter*		UpdateBestCounter();
		};
		class EXPORT GlobalDataContainer
		{
		public:
			CoreReference				CoreContext;
			virtual void				__internal__Serialize(bool firstCall, bool onlyPersistent) {}
			virtual void				__internal__Deserialize(bool firstCall, bool onlyPersistent) {}
		};
		struct OSData
		{
			unsigned int					Width, Height, DesignWidth, DesignHeight;
			float							WidthInInch, HeightInInch;
			GApp::Resources::ResourceData*	Resources;
			GlobalDataContainer*			GlobalData;
			const char*						Name;
			const char*						OSName;
			GAC_TYPE_LANGUAGE				Lang;
			const char*						FacebookURL;
			const char*						YoutubeURL;
			const char*						TwitterURL;
			const char*						InstagramURL;
			const char*						ApplicationMarketURL;
			const char*						DeveloperMarketURL;
			const char*						DeveloperHomePageURL;
			unsigned int					AppVersion, OSVersion;
			unsigned int					SplashScreenResourceStart, SplashScreenResourceSize;
			unsigned int					SplashScreenLogoWidth, SplashScreenLogoHeight;
			float							SplashScreenViewWidth, SplashScreenViewHeight;
			unsigned int					SplashScreenMethod, SplashScreenMinimalWaitTime;
			bool							AnimatedSplashScreen;
			unsigned int					AnimatedSplashScreenSpriteWidth, AnimatedSplashScreenSpriteHeight, AnimatedSplashScreenSpritesCount;
			//unsigned int					RateInterval, RateMaxTimes;
			//unsigned int					ComercialInterval, ComercialMaxTimes;
			GApp::Utils::String				LastPurchaseInfo;
			char							RequestedItemPrice[128];
			bool							ApplicationIsClosing;
			unsigned int					SnapshotType;
			int								AlarmCheckUpdateTicks;


			//[REMOVE-IN-DEVELOP-LIB:START]
#ifdef PLATFORM_ANDROID

#endif
#if defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
			void*							IOSInterfaceInstance;
			int								ResourceAssetFD;
			char							AppPath[MAX_APP_PATH];
#endif
#ifdef PLATFORM_WINDOWS_DESKTOP
			HDC								hDC;
			HGLRC							hRC;
			HWND							hWnd;
			unsigned int*					AvailableResolutions;
			unsigned int					AvailableResolutionsCount;
			HANDLE							hThread;
#endif
			//[REMOVE-IN-DEVELOP-LIB:STOP]

		};
		class EXPORT OSContext
		{
		public:
			OSData							Data;
			OSContext();

			// FUNCTII DE COMUNICARE CU DEVICE_ul - toate virtuale pure - trebuie sa fie implementate intr-o clasa derivata
			virtual int						fnGetSystemTicksCount() = 0;
			virtual bool					fnGetSystemDateTime(GApp::Utils::DateTime *) = 0;
			virtual void					fnClose() = 0;
			virtual bool					fnReadBufferFromResources(unsigned int pos, unsigned char *Buffer, unsigned int size) = 0;
			virtual void					fnOpenBrowser(const char *url) = 0;

			virtual void					fnOpenAppHandler(unsigned int handlerID, const char * data) = 0;
			virtual bool					fnIsAppHandlerAvailable(unsigned int handlerID) = 0;

			virtual void					fnRateApp() = 0;
			virtual bool					fnIsAppInstalled(const char * appOrPackageName) = 0;
			virtual void					fnInstallApp(const char * appOrPackageName) = 0;

			virtual void					fnOpenApplicationMarketPage() = 0;
			virtual void					fnOpenDeveloperMarketPage() = 0;
			virtual bool					fnSaveBufferToLocalStorage(const char* fileName, const void *Buffer, unsigned int BufferSize) = 0;
			virtual bool					fnReadBufferFromLocalStorage(const char* fileName, GApp::Utils::Buffer *buffer) = 0;
			virtual void					fnPlaySound(int soundID, int volume, bool loop) = 0;
			virtual void					fnStopSound(int soundID) = 0;
			virtual void					fnSetSoundState(int soundID, int volume, bool loop) = 0;
			virtual bool					fnLoadSound(int soundID, unsigned int pos, unsigned int size) = 0;
			virtual void					fnUnLoadSound(int soundID) = 0;
			virtual void					fnThreadWait(unsigned int milliseconds) = 0;
			virtual bool					fnThreadRun(void(*runFunction)(void *threadObject), void*  threadObject) = 0;

			virtual void					fnQueryNewAd(int adID) = 0;
			virtual void					fnSetAdVisibility(int adID, bool visible) = 0;
			virtual bool					fnIsAdVisible(int adID) = 0;
			virtual bool					fnIsAdLoaded(int adID) = 0;

			virtual bool					fnSendBuyRequestForItem(const char *item) = 0;
			virtual bool					fnIsBillingSystemAvailable() = 0;
			virtual bool					fnGetItemPrice(const char* item, GApp::Utils::String &result) = 0;

			virtual void					fnSendEmail(const char *To, const char *Subject, const char *Content) = 0;

			virtual void					fnExecuteOsCommand(const char* command, GApp::Utils::String *result) = 0;
			virtual void					fnApplicationInitializeCompleted() = 0;

			virtual bool					fnSendAnalyticsEvent(const char* eventName, const void *buffer, GAC_ANALYTICS_EVENT_TYPE type) = 0;
			virtual void					fnShare(const char *text) = 0;
			virtual void					fnSendSMS(const char *message) = 0;

			// Google play services specific
			virtual bool					fnGooglePlayServices_IsSignedIn() = 0;
			virtual void					fnGooglePlayServices_SignIn() = 0;
			virtual void					fnGooglePlayServices_SignOut() = 0;
			virtual const char*				fnGooglePlayServices_GetCurrentPlayerName() = 0;
			virtual void					fnGooglePlayServices_SubmitScore(int leaderBoardID, int score) = 0;
			virtual void					fnGooglePlayServices_ShowLeaderBoard(int leaderBoardID) = 0;
			virtual void					fnGooglePlayServices_RequestCurrentPlayerScore(int leaderBoardID) = 0;
			virtual int						fnGooglePlayServices_GetCurrentPlayerScore(int leaderBoardID) = 0;
			virtual int						fnGooglePlayServices_GetCurrentPlayerRank(int leaderBoardID) = 0;
		};
		class EXPORT FrameworkObject;
		class EXPORT APIInterface
		{
		PRIVATE_INTERNAL:
			void*						Context;	//AppContextData
		public:
			APIInterface();
			
			bool						ChangeScene(unsigned int sceneID);
			bool						ReloadCurrentScene();
			bool						ChangeSceneUsingCountersGroup(unsigned int sceneID, GApp::UI::CountersGroup *grp, unsigned int gotoSceneID);
			bool						ReloadCurrentSceneUsingCountersGroup(GApp::UI::CountersGroup *grp, unsigned int gotoSceneID);
			bool						ChangeSceneUsingCountersGroup(unsigned int sceneID, GApp::UI::CountersGroup *grp);
			bool						ReloadCurrentSceneUsingCountersGroup(GApp::UI::CountersGroup *grp);

			bool						GoToNextScene();
			bool						SetNextScene(unsigned int sceneID);	
			Scene*						GetCurrentScene();
			bool						RaiseEvent(FrameworkObject* object, unsigned int eventID);

			int							Random(int maxValue);
			int							RandomInclusive(int maxValue);
			int							Random(int minValue, int maxValue);
			float						LogFromBase(float value, float base);
			float						NthRoot(float value, float n);

			void						SetTouchFlags(unsigned int add,unsigned int remove=0);
			void						DisableTouch();
			void						SetTouchBoundary(float left, float top, float right, float bottom);
			void						ClearTouchBoundary();

			float						ComputeResolutionAspectRatio(float width, float height);
			

			bool						ReadResource(unsigned int pos,unsigned char *Buffer,unsigned int size,GAC_TYPE_ENCRYPTION encryptionMode,void *Key);
			bool						ReadResource(unsigned int pos,GApp::Utils::Buffer *b,unsigned int size,GAC_TYPE_ENCRYPTION encryptionMode,void *Key);
			unsigned int				GetSystemTickCount();
			bool						GetSystemDateTime (GApp::Utils::DateTime *dt);
			void						Close();


			void						OpenBrowser (const char *url);
			void						OpenDeveloperWebPage();

			bool						OpenAppHandler(unsigned int handlerID);
			bool						IsAppHandlerAvailable(unsigned int handlerID);

			bool						IsAppInstalled(const char * appOrPackageName);
			void						InstallApp(const char * appOrPackageName);

			void						OpenFacebookPage();
			void						OpenYouTubePage();
			void						OpenTwitterPage();
			void						OpenInstagramPage();

			bool						IsFaceBookHandlerAvailable();
			bool						IsYouTubeHandlerAvailable();
			bool						IsTwitterHandlerAvailable();
			bool						IsInstagramHandlerAvailable();
			
			void						OpenApplicationMarketPage ();
			void						OpenDeveloperMarketPage ();
			void						RateApplication(/*bool disableCounterForRateScene*/);
			//void						DisableRate();

			void						DisableAds();
			bool						GetAdsEnableStatus();

			unsigned int				GetAppVersion();

			void						Sleep(unsigned int milliseconds);
			bool						ThreadRun(void (*runFunction)(void *threadObject),void*  threadObject);

			bool						SendBuyRequestForItem(const char *item);
			bool						IsBillingSystemAvailable();
			const char*					GetLastPurchaseInformation();
			const char*					GetItemPrice(const char* item);

			void						SendEmail(const char *To, const char *Subject, const char *Content);
			void						Share(const char * text);
			void						Share(const char * text, const char * title);
			void						SendSMS(const char * message);
			void						SendSMS(const char * message, const char * phoneNumber);

			void						ApplicationInitializeCompleted();

			const char*					ExecuteOsCommand(const char * command);

			// Google play services specifics
			bool						GooglePlayServices_IsSignedIn();
			void						GooglePlayServices_SignIn();
			void						GooglePlayServices_SignOut();
			void						GooglePlayServices_SubmitScore(int leaderBoardID, int score);
			const char*					GooglePlayServices_GetCurrentPlayerName();
			void						GooglePlayServices_ShowLeaderBoard(int leaderBoardID);
			void						GooglePlayServices_RequestCurrentPlayerScore(int leaderBoardID);
			int							GooglePlayServices_GetCurrentPlayerScore(int leaderBoardID);
			int							GooglePlayServices_GetCurrentPlayerRank(int leaderBoardID);

		};
		class EXPORT CoreSystem
		{
		public:
			unsigned int							Width,Height;
			unsigned int							ProgressValue,ProgressMax;
			unsigned int							FingerSize;
			float									WidthInInch, HeightInInch, DiagonalInInch;
			float									ResolutionAspectRatio;
			bool									SnapshotSaved;
			//bool									AlreadyVoted;
			bool									AdsEnabled;
			APIInterface							Api;
			Graphics::GraphicsContext				Graphics;
			Resources::ResourceData*				Resources;
			GlobalDataContainer*					GlobalData;
			GApp::Media::AudioPlayer				AudioInterface;
			GApp::Platform::AnalyticsInterface		AnalyticsObject;
			GApp::Utils::KeyDB						SettingsDB;
			GAC_TYPE_LANGUAGE						Lang;
			const char*								OSName;
			const char*								AppName;
			unsigned int							OSVersion;
			unsigned int							AppVersion;
			GApp::Utils::Buffer						TempBufferBig;
			GApp::Utils::Buffer						TempBufferMedium;
			GApp::Utils::Buffer						TempBufferSmall;
			GApp::Utils::String						TempString;
			GApp::Utils::String						InAppBillingItem;
			GApp::Utils::String						OSCommandString;
			GApp::Utils::ThreadSyncRequest			ThreadRequest;
			GApp::Resources::Profile*				DelayLoadProfiles[GAC_PROFILETYPE_COUNT];

		};


		class EXPORT Scene
		{
		public:			
			CoreReference					CoreContext;
			bool							Initialized;
			GApp::Controls::GenericControl*	MainControl;
			unsigned int					OnLoadWaitTime;
			GApp::Utils::SceneTimer			Timers[MAX_SCENE_TIMERS];
			UInt32							TimersCount;
		protected:		
			void							PaintControls();			
			bool							UseProfile(GApp::Resources::Profile *profile);
			bool							EnableProfileLoadingProgressBar(unsigned int millisecondsToWait = 0);
			bool							CreateTimer(unsigned int id, unsigned int interval, bool enabled);
			bool							EnableTimer(unsigned int id);
			bool							DisableTimer(unsigned int id);
			bool							RemoveTimer(unsigned int id);
			bool							ResetTimer(unsigned int id);
			bool							SetTimerInterval(unsigned int id, unsigned int newInterval);
			void							RemoveAllTimers();
			void							ResetAllTimers();
			unsigned int					GetTimersCount();
		public:
			bool							AddControl(Controls::GenericControl *control);
			Controls::GenericControl*		GetControlWithID(int ID);
			Controls::GenericControl*		GetCheckedControl(int groupID);
			unsigned int					GetControlsCount();
			Controls::GenericControl*		GetControl(unsigned int index);
			void							Enter();
		public:
			Scene();

			Controls::GenericControl*		GetRootControl();

			virtual bool					OnInit();
			virtual void					OnEnter();
			virtual void					OnLeave();
			virtual void					OnUpdate();
			virtual void					OnPaint();
			virtual void					OnLoadProfiles(float percentage);
			virtual void					OnLoadProfilesCompleted();

			virtual void					OnPause();
			virtual void					OnResume();
			virtual void					OnBeforeSaveState();
			virtual void					OnAfterRestoreState();

			virtual void					OnTouchEvent(Controls::TouchEvent *touchEvent);
			virtual void					OnMouseEvent(Controls::TouchEvent *touchEvent);
			virtual void					OnKeyEvent(Controls::KeyboardEvent *keyEvent);
			virtual void					OnControlEvent(Controls::GenericControl *control, GAC_TYPE_EVENTTYPE type);
			virtual bool					OnEvent(FrameworkObject *object, unsigned int eventID);
			virtual bool					OnServiceEvent(GAC_SERVICE_TYPE serviceID, GAC_SERVICES_EVENT_TYPE eventID);

			virtual void					OnTimer(unsigned int id, unsigned int timeDelta);

			virtual bool					OnBillingEvent(const char* itemName, GAC_PURCHASE_STATUS_TYPE purchaseStatus);
			virtual bool					OnAdEvent(GApp::Platform::AdInterface *adObject, GAC_ADEVENT_TYPE eventID);

			virtual void					__internal__Serialize(bool firstCall, bool onlyPersistent);
			virtual void					__internal__Deserialize(bool firstCall, bool onlyPersistent);
			virtual void					OnDebugCommand(unsigned int commandID, UInt64 parameters) { };			
		};
		class EXPORT Application
		{
		PRIVATE_INTERNAL:
			void*							Context;		//AppContextData	
			bool							StartApplicationInitThread();
			bool							StartSceneLoadingProfilesThread();
			bool							ExecuteIteration();
			bool							CheckAlarms();
		public:
			CoreReference					CoreContext;

			Application();
			bool							Create(OSContext &initData);
			void							LoopExecute();
			void							ProcessSystemEvent(unsigned int SysEventCode,unsigned int i1,unsigned int i2,float f1,float f2); 
			bool							RaiseEvent(unsigned int eventID) { return ((GApp::UI::CoreSystem*)CoreContext)->Api.RaiseEvent(NULL, eventID); }
			virtual bool					OnInit();
			virtual bool					OnActivate();
			virtual void					OnBeforeSaveState();
			virtual void					OnAfterRestoreState();
			virtual void					OnTerminate();
			virtual void					OnChangeScene(unsigned int oldSceneID,unsigned int newSceneID);
			virtual void					OnNewUpdate(unsigned int oldVersion, unsigned int newVersion);
			virtual void					OnBillingEvent(const char* itemName, GAC_PURCHASE_STATUS_TYPE purchaseStatus) { }
			virtual void					OnAdEvent(GApp::Platform::AdInterface *adObject, GAC_ADEVENT_TYPE eventID) { }
			virtual void					OnServiceEvent(GAC_SERVICE_TYPE serviceID, GAC_SERVICES_EVENT_TYPE eventID) { }
			bool							SetScene(unsigned int sceneID, Scene *);
			virtual void					OnDebugCommand(unsigned int commandID, UInt64 parameters) { };
			virtual bool					OnCreateScenes();
			virtual void					OnAlarm(Alarm *alarm, unsigned int missedCount, GAC_TYPE_ALARMSTATE state) { };
		};
		class EXPORT FrameworkObject
		{
		public:
			CoreReference				CoreContext;
			FrameworkObject() { CoreContext = NULL; }
			FrameworkObject(Scene *scene) { CoreContext = scene->CoreContext; }
			FrameworkObject(Application *app) { CoreContext = app->CoreContext; }
			FrameworkObject(FrameworkObject *obj) { CoreContext = obj->CoreContext; }
			bool RaiseEvent(unsigned int eventID) { return ((GApp::UI::CoreSystem*)CoreContext)->Api.RaiseEvent(this, eventID); }
		};
	};
	namespace Animations
	{
		class EXPORT AnimationObject;
		namespace Elements
		{
			class EXPORT Element
			{
			public:
				Element*					Parent;
				float						X, Y, ScaleWidth, ScaleHeight, WidthInPixels, HeightInPixels, LeftInPixels, TopInPixels;
				unsigned int				ColorTransform;
				unsigned char				Alignament;
				bool						Visible;
				bool						CallVirtualFunction;
				bool						CanProcessTouchEvents;
			
				Element();

				virtual void UpdateScreenRect(GApp::Animations::AnimationObject * animObj);
				virtual void Paint(GApp::Animations::AnimationObject * animObj);
				virtual void OnUpdatePosition(GApp::Animations::AnimationObject * animObj);
				virtual void OnUpdateSize(GApp::Animations::AnimationObject * animObj);
				virtual void OnUpdateBlendColor(GApp::Animations::AnimationObject * animObj);
				virtual void OnUpdateScale(GApp::Animations::AnimationObject * animObj);
				virtual bool OnTouchEvent(GApp::Animations::AnimationObject * animObj, GApp::Controls::TouchEvent *te);

				float GetX(GApp::Animations::AnimationObject * animObj, bool pixels);
				float GetY(GApp::Animations::AnimationObject * animObj, bool pixels);

				void ComputeElementRect(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT SingleImageElement: public Element
			{
			public:

				GApp::Resources::Bitmap		*Image;						
				void Paint(GApp::Animations::AnimationObject * animObj);
				void UpdateScreenRect(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT MultipleImageElement : public SingleImageElement
			{
			public:
				GApp::Resources::Bitmap**	ImageList;
				int							ImageIndex;
				int							Count;
			};
			class EXPORT TextElement: public Element
			{
			public:			
				GApp::Graphics::TextPainter	TP;
                float fontSize;

				void Paint(GApp::Animations::AnimationObject * animObj);
				void OnUpdatePosition(GApp::Animations::AnimationObject * animObj);
				void OnUpdateSize(GApp::Animations::AnimationObject * animObj);
				void OnUpdateBlendColor(GApp::Animations::AnimationObject * animObj);
				void OnUpdateScale(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT EntireSurfaceElement : public Element
			{
			public:
				void Paint(GApp::Animations::AnimationObject * animObj);
				void UpdateScreenRect(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT RectangleElement : public Element
			{
			public:
				void Paint(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT ExclusionRectElement : public Element
			{
			public:
				void Paint(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT ClippingElement : public Element
			{
			public:
				void Paint(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT DisableClippingElement : public Element
			{
			public:
				void Paint(GApp::Animations::AnimationObject * animObj);
				void UpdateScreenRect(GApp::Animations::AnimationObject * animObj);
			};
			struct SimpleButtonFaceContainer
			{
				GApp::Resources::Bitmap*	Background;
				GApp::Resources::Bitmap*	Symbol;
				GApp::Graphics::TextPainter	TP;
				unsigned int				BackgroundColorBlending, SymbolColorBlending, TextColorBlending;
				float						SymbolScaleWidth, SymbolScaleHeight, SymbolX, SymbolY;
				float						TextRectWidth, TextRectHeight, TextRectX, TextRectY;
				unsigned char				SymbolAlign;
				unsigned char				TextRectAlign;		
			};
			class EXPORT SimpleButtonElement : public Element
			{
			public:
				SimpleButtonFaceContainer	Normal, Pressed, Inactive;
				GApp::Resources::Sound*		ClickSound;
				int							ClickEvent;
				bool						Enabled;
				bool						UseBackgoundImage;
				bool						IsPressed;
                bool                        SendEventWhenAnimationEnds;

				void Paint(GApp::Animations::AnimationObject * animObj);
				bool OnTouchEvent(GApp::Animations::AnimationObject * animObj, GApp::Controls::TouchEvent *te);
				void UpdateScreenRect(GApp::Animations::AnimationObject * animObj);
			};
		};
		namespace Transformations
		{
			class EXPORT Transformation
			{
			public:
				bool __Started;

				Transformation();
				bool Update(GApp::Animations::AnimationObject * animObj);
				void Init(GApp::Animations::AnimationObject * animObj);
				virtual bool OnUpdate(GApp::Animations::AnimationObject * animObj) = 0;
				virtual void OnInit(GApp::Animations::AnimationObject * animObj) = 0;
			};
			class EXPORT Repeat : public Transformation
			{
			public:
				int NumberOfTimes, Counter;
				Transformation * trans;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT RepeatUntil : public Transformation
			{
			public:				
				Transformation * trans;
				bool ExitCondition,ForceExit;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT DoOnceUntil : public Transformation
			{
			public:
				Transformation * trans;
				bool ExitCondition, ForceExit;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
            class EXPORT PopupLoop : public Transformation
            {
            public:
                Transformation * trans;
                bool ForceExit;
                bool OnUpdate(GApp::Animations::AnimationObject * animObj);
                void OnInit(GApp::Animations::AnimationObject * animObj);
            };
            class EXPORT DoOncePopupLoop : public Transformation
            {
            public:
                Transformation * trans;
                bool ForceExit;
                bool OnUpdate(GApp::Animations::AnimationObject * animObj);
                void OnInit(GApp::Animations::AnimationObject * animObj);
            };
			class EXPORT WaitUntil : public Transformation
			{
			public:
				bool ExitCondition;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT IfElseBlock : public Transformation
			{
			public:
				Transformation * then_branch;
				Transformation * else_branch;
				bool IfElseCondition;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT BranchBlock : public Transformation
			{
			public:
				Transformation ** list;
				Transformation * trans;
				int NumberOfTransformations;
				int BranchIndex;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT Timer : public Transformation
			{
			public:
				int NumberOfTimes, Counter;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT ZOrder : public Transformation
			{
			public:
				int ZOrderID;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT Event : public Transformation
			{
			public:
				unsigned int EventID;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT TouchStatus : public Transformation
			{
			public:
				bool TouchEnabled;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT TouchBoundary : public Transformation
			{
			public:
				float X, Y, Width, Height;
				unsigned int Alignament;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT SoundPlay : public Transformation
			{
			public:
				GApp::Resources::Sound* sound;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT Continous : public Transformation
			{
			public:
				Transformation * trans;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT Stopper : public Transformation
			{
			public:
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT Sequance : public Transformation
			{
			public:
				Transformation ** list;
				unsigned int NumberOfTransformations, Counter;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT Parallel : public Transformation
			{
			public:
				Transformation ** list;
				unsigned int NumberOfTransformations;
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};

			class EXPORT LinearTransformation : public Transformation
			{
			public:
				float Start, End, Step, CurrentValue;
				int Steps, Counter;		
				Animations::Elements::Element	*Element;
			};
			class EXPORT ScaleLinear : public LinearTransformation
			{
			public:
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT ScaleWidthLinear: public LinearTransformation
			{
			public:
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT ScaleHeightLinear : public LinearTransformation
			{
			public:
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT AlphaBlendingLinear: public LinearTransformation
			{
			public:
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT ScaleForwardAndBack : public LinearTransformation
			{
			public:
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT ScaleWidthForwardAndBack : public LinearTransformation
			{
			public:
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT ScaleHeightForwardAndBack : public LinearTransformation
			{
			public:
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT AlphaBlendingForwardAndBack : public LinearTransformation
			{
			public:
				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};

			class EXPORT ColorBlendLinear : public Transformation
			{
			public:
				unsigned int Start, End;
				int Steps, Counter;
				Animations::Elements::Element *Element;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT ColorBlendForwardAndBack : public Transformation
			{
			public:
				unsigned int Start, End;
				int Steps, Counter;
				Animations::Elements::Element *Element;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};

			class EXPORT MoveRelativeLinear : public Transformation
			{
			public:
				float OffsetX, OffsetY;
				int Steps, Counter;
				Animations::Elements::Element *Element;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT MoveAbsoluteLinear : public Transformation
			{
			public:
				float X, Y, StepSize, addX, addY;
				int Counter;
				Animations::Elements::Element *Element;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT QuadraticBezier : public Transformation
			{
			public:
				float StartPointX, StartPointY;
				float ControlPointX, ControlPointY;
				float EndPointX, EndPointY;
				int Steps, Counter;
				Animations::Elements::Element *Element;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};

			class EXPORT SetNewRelativePosition : public Transformation
			{
			public:
				float OffsetX, OffsetY;
				Animations::Elements::Element *Element;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT SetNewAbsolutePosition : public Transformation
			{
			public:
				float X, Y;
				Animations::Elements::Element *Element;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};

			class EXPORT VisibleState : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				bool Visible;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT ButtonEnable : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				bool Enabled;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT ColorBlendState : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				unsigned int Color;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT AlphaBlendingState : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				float Alpha;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT SetNewText : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				GApp::Utils::String Text;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT SetNewImage : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				GApp::Resources::Bitmap* Image;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT SetImageIndex : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				int	ImageIndex;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT ImageIndexLinear : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				int StartIndex,EndIndex,Step,FramesInterval,CurrentIndex,Timer;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};

			

			class EXPORT TextFlow : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				int FramesUpdate, Counter;
				unsigned int Poz, MaxChars;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT TextCenterFlow : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				int FramesUpdate, Counter;
				unsigned int Poz, MaxChars;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT TextCharacterVisibility : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				int Start,End,Step;
				bool Visibility;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT FontSize : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				float Size;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT NumericFormatter : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				union {
					float floatValue;
					int intValue;
				};
				unsigned char FormatType;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT NumberIncrease : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				union {
					float floatStart;
					int intStart;
				};
				union {
					float floatEnd;
					int intEnd;
				};
				int step, Steps;
				unsigned char FormatType;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};
			class EXPORT CountDown : public Transformation
			{
			public:
				Animations::Elements::Element *Element;
				int SecondsLeft,CurrentValue;
				unsigned char FormatType,Counter;

				bool OnUpdate(GApp::Animations::AnimationObject * animObj);
				void OnInit(GApp::Animations::AnimationObject * animObj);
			};

		};

		class EXPORT AnimationObject : public GApp::UI::FrameworkObject
		{
		public:
			float						Width, Height;
			float						OffsetX, OffsetY;
			unsigned int				Flags;
            int                         AnimationEndEventID;
			Elements::Element*			TouchCaptureElement;
		public:


			bool						Start();
			bool						Stop();
			bool						Pause();
			bool						Resume();
			bool						IsRunning();
			bool						IsPaused();

			virtual bool				OnStart() = 0;
			virtual void				Paint() = 0;
			virtual void				ButtonPaint(bool visible) = 0;
			virtual bool				ControlPaint(bool loop) = 0;
			virtual void				SetZOrder(int index) = 0;
			virtual bool				OnTouchEvent(GApp::Controls::TouchEvent *te) = 0;

			void						MoveWithOffset(float offsetX, float offsetY, GAC_TYPE_COORDINATES coord);
			void						ResetMovementOffsets();
			bool						ProcessTouchEvents(GApp::Controls::TouchEvent *te, Elements::Element* zOrder[], int zOrderElementsCount);
            void                        ExitPopupLoop();
			
		};
	};
};


//[REMOVE-IN-DEVELOP-LIB:START] 
#define KEYDB_DATATYPE_UINT8		0
#define KEYDB_DATATYPE_UINT16		1
#define KEYDB_DATATYPE_UINT32		2
#define KEYDB_DATATYPE_UINT64		3
#define KEYDB_DATATYPE_INT8			4
#define KEYDB_DATATYPE_INT16		5
#define KEYDB_DATATYPE_INT32		6
#define KEYDB_DATATYPE_INT64		7
#define KEYDB_DATATYPE_FLOAT32		8
#define KEYDB_DATATYPE_FLOAT64		9
#define KEYDB_DATATYPE_BOOL			10
#define KEYDB_DATATYPE_STRING		11




#define MAX_EVENTS_IN_QUE			1024
#define MAX_TOUCH_POINTERS			8
#define INVALID_SCENE_ID			(0xFFFFFFFF)

#define GAC_CONTROL_FLAG_VISIBLE				0x000001
#define GAC_CONTROL_FLAG_ENABLED				0x000002
#define GAC_CONTROL_FLAG_CHECKED				0x000004
#define GAC_CONTROL_FLAG_TOUCHED				0x000008
#define GAC_CONTROL_FLAG_OUTSIDE_PARENT_RECT	0x000010		
#define GAC_CONTROL_FLAG_TABSTOP				0x000020	
#define GAC_CONTROL_FLAG_FOCUSED				0x000040
#define GAC_CONTROL_FLAG_NO_TOUCH				0x000080
#define GAC_CONTROL_FLAG_MODAL_FRAME			0x000100
#define GAC_CONTROL_FLAG_MAIN					0x000200


#define GAC_THREADCMD_PROFILE_LOADTEXTURES		1
#define GAC_THREADCMD_PROFILE_LOADSHADER		2
#define GAC_THREADCMD_PROFILE_UNLOADTEXTURES	3
#define GAC_THREADCMD_PROFILE_UNLOADSHADER		4
#define GAC_THREADCMD_PROFILE_LOADSOUND			5
#define GAC_THREADCMD_PROFILE_UNLOADSSOUND		6
#define GAC_THREADCMD_APP_ONINIT				7
#define GAC_THREADCMD_APP_ONNEWUPDATE			8
#define GAC_THREADCMD_APP_PLAYBACKGROUNDMUSIC	9
#define GAC_THREADCMD_APP_ONACTIVATE			10
#define GAC_THREADCMD_APP_NOTIFY_APP_INITED		11
#define GAC_THREADCMD_APP_ONAFTERRESTORESTATE	12
#define GAC_THREADCMD_SCENE_ONINIT				13
#define GAC_THREADCMD_SCENE_ONAFTERRESTORESTATE	14
#define GAC_THREADCMD_LOAD_AD					15
#define GAC_LOAD_PROFILE_THREAD_REQUEST			0x08000000

struct ThreadExecutionContext
{
	void					(*RunFunction) (void* ThreadObject);
	void*					ThreadObject;
	GApp::UI::OSContext*	OS;
#if defined(PLATFORM_DEVELOP) || defined(PLATFORM_WINDOWS_DESKTOP)
	HANDLE					SyncEvent;
#else
	bool					Signaled;
#endif
};
struct GlyphLocation
{
	GApp::Resources::Bitmap	*Image;
	float					x,y;
	unsigned int			Code;
	bool					Visible;
};
struct BitmapTextureLocation
{
	GApp::Resources::Bitmap	*Image;
	unsigned int			Left,Right,Top,Bottom;
};
struct EventQue
{
	bool			EventIsFree;
	unsigned int	Time;
	unsigned int	EventType;
	unsigned int	UintValue;
	float			FloatValues[3];
};
struct GlyphInformation
{
	unsigned short			Character;
	GApp::Resources::Bitmap*	Image;
	float					Descend;
};
struct ExtendedTouchEvent
{
	GApp::Controls::TouchEvent		Event;
	GApp::Controls::GenericControl*	Control;
	float							OriginalXPixels,OriginalYPixels;
	float							LastXPixels,LastYPixels;
	float							XPixel,YPixel;	
};
struct ZoomTouchEvent
{
	GApp::Controls::TouchEvent		Event;
	int								Pointer1, Pointer2;
};
struct AdEvent
{
	int								AdID, EventID;
};
//class SceneChangeCounter
//{
//public:
//	SceneChangeCounter();
//	unsigned int		Count, Times, Interval, MaxTimes,SceneID;
//	bool				Enabled;
//
//	void	Init(unsigned int interval, unsigned int maxTimes, GApp::UI::Scene* Scenes[MAX_SCENES],unsigned int sceneID);
//	bool	Update();
//};

#define GAC_SCENELOAD_NOTHINGTOLOAD 0xFFFFFFFF
#define GAC_SCENELOAD_NOPROGRESSBAR 0xFFFFFFFE

class AppContextData
{
public:
	GApp::UI::Scene*				Scenes[MAX_SCENES];
	GApp::UI::Scene*				CurrentScene;
	GApp::UI::Application*			App;
	EventQue						Events[MAX_EVENTS_IN_QUE];
	unsigned int					EventsQueRead,EventsQueWrite;
	unsigned int					CurrentSceneID,NextSceneID,NewSceneID;
	unsigned int					MediaState;
	int								CurrentMusicID;
	GApp::UI::CoreSystem			CoreObject;
	ExtendedTouchEvent				TouchPointers[MAX_TOUCH_POINTERS];
	ZoomTouchEvent					ZoomEvent;
	GApp::Controls::GenericControl*	FocusesControl;
	GApp::UI::OSContext*			OS;
	GApp::Resources::Bitmap			LogoBitmap;
	GApp::Graphics::ImagePainter	Logo;
	GApp::Resources::Profile		LogoProfile;	
	GApp::UI::Alarm*				Alarms;
	unsigned int					AlarmsCount;
	int								AlarmCheckTimer;
	int								AlarmCheckUpdateTicks;
	unsigned int					LogoAnimatedIndex;
	float							MinDistForClickSquared;
	unsigned int					TouchFlags;
	unsigned int					ApplicationState;
	bool							ApplicationInitedSuccesifully;
	bool							RestoreApplicationSettings;
	int								MaxTextureSize;
	int								BillingEventCode;
	GApp::Graphics::RectF			TouchBoundary;
	bool							TouchBoundaryEnabled;

	


	bool							ChangeScene(unsigned int sceneID);
	bool							ReloadCurrentScene();
	bool							ChangeSceneUsingCountersGroup(unsigned int sceneID, GApp::UI::CountersGroup *grp, unsigned int grpScene);
	bool							ReloadCurrentSceneUsingCountersGroup(GApp::UI::CountersGroup *grp, unsigned int grpScene);
	bool							ChangeSceneUsingCountersGroup(unsigned int sceneID, GApp::UI::CountersGroup *grp);
	bool							ReloadCurrentSceneUsingCountersGroup(GApp::UI::CountersGroup *grp);

	bool							SetNextScene(unsigned int sceneID);
	bool							GoToNextScene();

	bool							QueTouchDownEvent(unsigned int pointer, float x, float y,unsigned int time);
	bool							QueTouchUpEvent(unsigned int pointer, float x, float y,unsigned int time);
	bool							QueTouchMoveEvent(unsigned int pointer, float x, float y,unsigned int time);
	bool							QueKeyEvent(unsigned int keyState);
	bool							QueAdEvent(int AdID, int EventdID);
	bool							QueServiceEvent(int serviceID, int EventdID);

	void							SetFocusControl(GApp::Controls::GenericControl* control);
	GApp::Controls::GenericControl* TouchPositionToControl(GApp::Controls::GenericControl* control,float x,float y);
	bool							InitControlsOnSceneActivate(GApp::Controls::GenericControl *control,GApp::UI::Scene *Scene, CoreReference CoreContext);
	bool							InitCurrentScene();
	void							ProcessEventTouchDownForScene(ExtendedTouchEvent *etp);
	void							ProcessNormalTouchEvent(ExtendedTouchEvent *etp);
	bool							ProcessEventForScene(EventQue *e);
};
//[REMOVE-IN-DEVELOP-LIB:STOP]

#endif
