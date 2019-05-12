#include "GApp.h"

#define LAYOUT_FLAG_XY_TYPE(flags)		(((flags)>>4) & 0x0F)
#define LAYOUT_FLAG_WH_TYPE(flags)		(((flags)>>8) & 0x0F)
#define LAYOUT_FLAG_ALIGNAMENT(flags)	((flags) & 0x0F)

#define LAYOUT_FLAG_IN_USE				0x80000000
//===================================================================================================================
#define VALUE_TYPE_NUMERIC			-1


#define ANALIZE_NUMBER_FORMAT(numericValue,textBuffer,valueType) { \
	if ((textBuffer[0]=='p') && (textBuffer[1]=='x')) { \
		(valueType)=GAC_COORDINATES_PIXELS; \
		textBuffer+=2; \
	} else if (textBuffer[0]=='%') { \
		(valueType)=GAC_COORDINATES_PERCENTAGE; \
		numericValue /= 100.0f; \
		textBuffer+=1; \
	} else { \
		(valueType)=VALUE_TYPE_NUMERIC; \
	} \
}

#define COMPUTE_DIV_UNITS(minValue,maxValue,parts,index,type) { \
	if ((parts)<1) RETURNERROR(false,"invalid division units method (parts should be bigger than 1) - is %f",(float)(parts)); \
	if ((minValue)>=(maxValue)) RETURNERROR(false,"invalid division units method (minValue should be smaller than maxValue) - minValue=%f, maxValue=%f",(float)(minValue),(float)(maxValue)); \
	delta = ((maxValue)-(minValue))/(parts); \
	result->Value = ((index)*delta)-(delta/2.0f)+(minValue); \
	result->Type = (type); \
	}

#define ADD_REG_TYPE(caracter) { CHECK(regFormatIndex<30,false,"regFormatIndex should be smaller than 30 (curent value is %d)",regFormatIndex);regFormat[regFormatIndex++]=(caracter); }
#define SET_ALIGNAMENT_FROM_TEXT(text,align) { if (IsOneOf(value,text)) { lcd->Alignament = (align); return true; } }

struct InternalLayoutDataValue
{
	float Value;
	unsigned int Type;
};
struct InternalLayoutComputeData
{
	InternalLayoutDataValue X,Y,Width,Height;	
	InternalLayoutDataValue GridLeftMargin,GridRightMargin,GridTopMargin,GridBottomMargin,HorizontalCellSpace,VerticalCellSpace;
	GAC_TYPE_ALIGNAMENT Alignament;
	bool xSet,ySet,widthSet,heightSet,alignamentSet;
};

bool IsOneOf(char *text,const char *list)
{
	char *orig = text;
	while (true)
	{
		if ((*text)!=(*list))
		{
			// duc list pana la urmatorul 
			while (((*list)!=0) && ((*list)!=','))
				list++;
			if ((*list)==0)
				return false;
			text = orig;
			list++;
		} else {
			list++;
			text++;
			if (((*text)==0) && (((*list)==0) || ((*list)==',')))
				return true;
		}
	}
	return false;
}

char* StringToFloat(char *value,float *result,unsigned int *type)
{
	float val = 0;
	float exponent = 0;
	char ch;
	bool negativ = false;
	bool punct = false;

	if (value[0]==0)
		RETURNERROR(NULL,"value is not a number (empty)");
	if ((*value)=='-')
	{
		negativ = true;
		value++;
		if (value[0]==0)
			RETURNERROR(NULL,"value is not a number (its only '-') character");
	}
	for (;(*value)!=0;value++)
	{
		ch = (*value);
		if (((ch>='0') && (ch<='9')) || (ch=='.'))
		{
			if (ch!='.')
			{
				if (punct)
				{
					val = val + ((float)(ch-'0'))/exponent;
					exponent = exponent * 10.0f;
				} else
					val = val * 10.0f + (ch-'0');
			} else {
				if (punct)
					RETURNERROR(NULL,"two '.' characters in a flotant number");
				punct = true;
				exponent=10.0f;
			}
		} else {
			break;
		}
	}
	ANALIZE_NUMBER_FORMAT(val,value,(*type));
	if (negativ)
		val = -val;
	(*result)=val;
	return value;
}
bool ComputeParameterValue(char *value,InternalLayoutDataValue *result,float *paramValues,unsigned int paramValuesCount)
{
	float values[8];
	float delta;	
	unsigned int valuesTypes[8];
	unsigned int index = 0;	
	char regFormat[32];
	unsigned int regFormatIndex = 0;

	CHECK(value[0]!=0,false,"Null (empty) value field");

	while ((*value)!=0)
	{
		switch (*value)
		{
			case '[': 
			case ']': 
			case '/': 
				ADD_REG_TYPE((*value));
				value++;
				break;
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': 
				CHECK(index<8,false,"Expecting a index smaller than 8 (current=%d)",index);
				value = StringToFloat(value,&values[index],&valuesTypes[index]);
				CHECK(value!=NULL,false,"Unable to convert String to Float");
				if (valuesTypes[index]==VALUE_TYPE_NUMERIC)
					ADD_REG_TYPE('n')		
				else if (valuesTypes[index]==GAC_COORDINATES_PIXELS)
					ADD_REG_TYPE('p')
				else if (valuesTypes[index]==GAC_COORDINATES_PERCENTAGE)
					ADD_REG_TYPE('%')
				else  
					RETURNERROR(false,"Invalid numeric value type (accepted are numeric,pixel and percentage)!");
				index++;
				break;
			case '{':
				value++;
				CHECK(index<4,false,"Expecting a index for optional parameters smaller than 4 (current=%d)",index);
				CHECK((value[0]>='0') && (value[0]<='9') && (value[1]=='}'),false,"");
				CHECK(((unsigned int)(value[0]-'0'))<paramValuesCount,false,"");
				values[index] = paramValues[value[0]-'0'];
				value+=2;
				ANALIZE_NUMBER_FORMAT(values[index],value,valuesTypes[index]);				
				if (valuesTypes[index]==VALUE_TYPE_NUMERIC)
					ADD_REG_TYPE('n')
				else if (valuesTypes[index]==GAC_COORDINATES_PIXELS)
					ADD_REG_TYPE('p')
				else if (valuesTypes[index]==GAC_COORDINATES_PERCENTAGE)
					ADD_REG_TYPE('%')
				else  
					RETURNERROR(false,"Invalid numeric value type (accepted are numeric,pixel and percentage)!");
				index++;
				break;
			default:
				RETURNERROR(false,"invalid character '%d' - Char:'%c'",(*value),(*value));
		}
	}
	// verific formate 
	regFormat[regFormatIndex] = 0;
	//LOG_INFO("--> %s,%s,%s [%f,%f,%f,%f]", value, paramValues, regFormat,values[0],values[1],values[2],values[3]);
	
	if (IsOneOf(regFormat,"p,%"))		
	{
		result->Value = values[0];
		result->Type = valuesTypes[0];
		return true;
	}
	if (GApp::Utils::String::Equals(regFormat,"n"))
	{
		result->Value = values[0];
		result->Type  = GAC_COORDINATES_PIXELS;
		return true;
	}
	if (IsOneOf(regFormat,"n/n,[n/n]"))
	{
		COMPUTE_DIV_UNITS(0,1.0f,values[1],values[0],GAC_COORDINATES_PERCENTAGE);
		return true;
	}
	if (IsOneOf(regFormat,"%/p,[%/p],%/n,[%/n]"))
	{
		result->Value = values[0]*values[1];
		result->Type = GAC_COORDINATES_PIXELS;
		return true;
	}	

	if (GApp::Utils::String::Equals(regFormat,"%[n/n]"))
	{
		COMPUTE_DIV_UNITS(values[0],1.0f,values[2],values[1],GAC_COORDINATES_PERCENTAGE);
		return true;
	}	
	if (GApp::Utils::String::Equals(regFormat,"[n/n]%"))
	{
		COMPUTE_DIV_UNITS(0,(1.0f-values[2]),values[1],values[0],GAC_COORDINATES_PERCENTAGE);
		return true;
	}
	if (GApp::Utils::String::Equals(regFormat,"%[n/n]%"))	
	{
		COMPUTE_DIV_UNITS(values[0],(1.0f-values[3]),values[2],values[1],GAC_COORDINATES_PERCENTAGE);
		return true;
	}

	if (IsOneOf(regFormat,"n/n/n,n/n/p,[n/n/n],[n/n/p]"))
	{
		COMPUTE_DIV_UNITS(0,values[2],values[1],values[0],GAC_COORDINATES_PIXELS);
		return true;
	}

	if (IsOneOf(regFormat,"%[n/n/n],%[n/n/p]"))
	{
		COMPUTE_DIV_UNITS(values[0]*values[3],values[3],values[2],values[1],GAC_COORDINATES_PIXELS);
		return true;
	}
	if (IsOneOf(regFormat,"[n/n/n]%,[n/n/p]%"))
	{
		COMPUTE_DIV_UNITS(0,((1-values[3])*values[2]),values[1],values[0],GAC_COORDINATES_PIXELS);
		return true;
	}
	if (IsOneOf(regFormat,"%[n/n/n]%,%[n/n/p]%"))
	{
		COMPUTE_DIV_UNITS(values[0],((1-values[4])*values[3]),values[2],values[1],GAC_COORDINATES_PIXELS);
		return true;
	}
	
	// pixel[procent/pixel]

	RETURNERROR(false,"unknown numeric reprezentation for a value - '%s'",regFormat);
}
bool ComputeGroupParameter(InternalLayoutComputeData *lcd,char *value)
{
	// format: g=a[10%/50%/40%/10%](aaabbc|aaabbc|dddddd)
	// format: g=a(aaabbc|aaabbc|dddddd)
	char margin_value[64];
	float margin[4];
	char positionName = value[0];
	int pos = 0;
#ifdef ENABLE_ERROR_LOGGING
	char* expr = value;
#endif
	margin[0] = margin[1] = margin[2] = margin[3] = 0;
	CHECK(lcd->alignamentSet,false,"Alignament option (a,align or alignament) should be call before using group option");
	CHECK((positionName>=32) && (positionName<=127) && (positionName!='|') && (positionName!=':') && (positionName!='[') && (positionName!=']')&& (positionName!='(')&& (positionName!=')'),false,"Invalid character for position name");
	value++;
	//CHECK(,false,"Expecting ':' - format is character:grid or character:[left-margin:top-margin:right-margin:bottom-margin]:grid");
	if (*value=='[')
	{
		for (int tr=0;tr<4;tr++)
		{
			value++;
			pos = 0;
			while ((pos<63) && ((*value)!=0) && ((*value)!='%'))
			{
				margin_value[pos] = (*value);
				value++;
				pos++;
			}
			margin_value[pos] = 0;
			CHECK(pos<63,false,"Margin value too large: %s%s",margin_value,value);
			CHECK((*value)=='%',false,"Margins should be percentages values: %s%s",margin_value,value);
			CHECK(GApp::Utils::String::ConvertToFloat32(margin_value,margin[tr]),false,"Invalid float value: %s",margin_value);
			value++;
			if (tr<3)
			{
				CHECK((*value)=='/',false,"Expecting ':' found '%c' in %s",*value,value);
			} else {
				CHECK((*value)==']',false,"Expecting ':' found '%c' in %s",*value,value);
			}		
		}
		value++;
	}
	CHECK((*value=='('),false,"Expecting '(' in %s [%s]",value,expr);
	value++;
	// la value este grid-ul
	int sx,sy,ex,ey;
	sx = sy = ex = ey = -1;
	int width = -1;
	int height = 0;
	int x = 0;
#ifdef ENABLE_ERROR_LOGGING
	char *grid = value;
#endif
	for (;((*value)!=0) && ((*value)!=')');value++)
	{
		if ((*value)==positionName)
		{
			ex = x;
			ey = height;
			if (sx<0) { sx = ex;sy = ey; }
		}		
		if ((*value)=='|')
		{
			if (width<0)
			{
				width = x;
			} else {
				CHECK(width==x,false,"Invalid grid format (all rows should have the same width) (width=%d,found=%d) in %s",width,x,expr);
			}
			height++;
			x = 0;
		} else {
			x++;
		}
	}
	if (x>0)
	{
		if (width<0)
		{
			width = x;
		} else {
			CHECK(width==x,false,"Invalid grid format (all rows should have the same width) (width=%d,found=%d) in %s",width,x,expr);
		}
	}
	height++;
	// am weight si height calculate - calculez x,y,width,height
	CHECK((*value==')'),false,"Expecting ')' at the end of grid definition: [%s]",grid);
	value++;
	CHECK((*value)==0,false,"Invalid characters after ')' in grid definition: %s'",grid);
	CHECK(width>0,false,"Invalid grid format - expecting at least one character");
	CHECK(sx>=0,false,"Character '%c' was not found in grid definition '%s'",positionName,grid);
	CHECK((margin[0]+margin[2])<100.0f,false,"Margin left (%f) and margin right (%f) if added should not be bigger than 100 in '%s'!",margin[0],margin[2],expr);
	CHECK((margin[1]+margin[3])<100.0f,false,"Margin top (%f) and margin bottom (%f) if added should not be bigger than 100 in '%s'!",margin[1],margin[3],expr);
	// calculez pozitiile
	float x1,y1,x2,y2;
	x1 = margin[0]+(sx *(100.0f-(margin[0]+margin[2])))/((float)width);
	x2 = margin[0]+((ex+1) *(100.0f-(margin[0]+margin[2])))/((float)width);
	y1 = margin[1]+(sy *(100.0f-(margin[1]+margin[3])))/((float)height);
	y2 = margin[1]+((ey+1) *(100.0f-(margin[1]+margin[3])))/((float)height);
	// am aflat valorile
	ALIGN_OBJECT_TO_RECT(lcd->X.Value,lcd->Y.Value,x1,y1,x2,y2,0,0,lcd->Alignament);
	lcd->X.Type = GAC_COORDINATES_PERCENTAGE;
	lcd->X.Value /= 100.0f;
	lcd->Y.Type = GAC_COORDINATES_PERCENTAGE;
	lcd->Y.Value /= 100.0f;
	lcd->Width.Value = (x2-x1)/100.0f;
	lcd->Width.Type = GAC_COORDINATES_PERCENTAGE;
	lcd->Height.Value = (y2-y1)/100.0f;
	lcd->Height.Type = GAC_COORDINATES_PERCENTAGE;
	lcd->xSet = true;
	lcd->ySet = true;
	lcd->widthSet = true;
	lcd->heightSet = true;
	return true;
}
bool ComputeAlignamentParameter(InternalLayoutComputeData *lcd,char *value)
{
	SET_ALIGNAMENT_FROM_TEXT("c,center",GAC_ALIGNAMENT_CENTER);
	SET_ALIGNAMENT_FROM_TEXT("tl,lt,topleft,lefttop",GAC_ALIGNAMENT_TOPLEFT);
	SET_ALIGNAMENT_FROM_TEXT("tr,rt,topright,righttop",GAC_ALIGNAMENT_TOPRIGHT);
	SET_ALIGNAMENT_FROM_TEXT("bl,lb,bottomleft,leftbottom",GAC_ALIGNAMENT_BOTTOMLEFT);
	SET_ALIGNAMENT_FROM_TEXT("br,rb,bottomright,rightbottom",GAC_ALIGNAMENT_BOTTOMRIGHT);
	SET_ALIGNAMENT_FROM_TEXT("tc,ct,t,top,topcenter,centertop",GAC_ALIGNAMENT_TOPCENTER);
	SET_ALIGNAMENT_FROM_TEXT("bc,cb,b,bottom,bottomcenter,centerbottom",GAC_ALIGNAMENT_BOTTOMCENTER);
	SET_ALIGNAMENT_FROM_TEXT("lc,cl,l,left,leftcenter,centerleft",GAC_ALIGNAMENT_LEFTCENTER);
	SET_ALIGNAMENT_FROM_TEXT("rc,cr,r,right,rightcenter,centerright",GAC_ALIGNAMENT_RIGHTCENTER);
	RETURNERROR(false,"Unknown value for alignament: '%s'",value);
}
bool ComputeLayoutParameter(char* key,char *value,InternalLayoutComputeData *lcd,float *params,unsigned int paramsCount)
{
	// iau pe rand fiecare parametru
	if (GApp::Utils::String::Equals(key,"x"))
	{
		CHECK(ComputeParameterValue(value,&lcd->X,params,paramsCount),false,"");
		lcd->xSet = true;
		return true;
	}
	if (GApp::Utils::String::Equals(key,"y"))
	{
		CHECK(ComputeParameterValue(value,&lcd->Y,params,paramsCount),false,"");
		lcd->ySet = true;
		return true;
	}
	if (IsOneOf(key,"w,width"))
	{
		CHECK(ComputeParameterValue(value,&lcd->Width,params,paramsCount),false,"");
		lcd->widthSet = true;
		return true;
	}
	if (IsOneOf(key,"h,height"))
	{
		CHECK(ComputeParameterValue(value,&lcd->Height,params,paramsCount),false,"");
		lcd->heightSet = true;
		return true;
	}
	if (IsOneOf(key,"as,autosz,autosize"))
	{		
		lcd->heightSet = lcd->widthSet = false;
		return true;
	}
	if (IsOneOf(key,"a,align,alignament"))
	{
		CHECK(ComputeAlignamentParameter(lcd,value),false,"");
		lcd->alignamentSet = true;
		return true;
	}
	if (IsOneOf(key,"g,group"))
	{
		CHECK(ComputeGroupParameter(lcd,value),false,"");
		return true;
	}
	if (IsOneOf(key,"d,dock"))
	{
		CHECK(ComputeAlignamentParameter(lcd,value),false,"");
		switch (lcd->Alignament)
		{
			case GAC_ALIGNAMENT_TOPLEFT: lcd->X.Value = lcd->Y.Value = 0.0f; break;
			case GAC_ALIGNAMENT_TOPCENTER: lcd->X.Value = 0.5f; lcd->Y.Value = 0.0f; break;
			case GAC_ALIGNAMENT_TOPRIGHT: lcd->X.Value = 1.0f; lcd->Y.Value = 0.0f; break;
			case GAC_ALIGNAMENT_RIGHTCENTER: lcd->X.Value = 1.0f; lcd->Y.Value = 0.5f; break;
			case GAC_ALIGNAMENT_BOTTOMRIGHT: lcd->X.Value = 1.0f; lcd->Y.Value = 1.0f; break;
			case GAC_ALIGNAMENT_BOTTOMCENTER: lcd->X.Value = 0.5f; lcd->Y.Value = 1.0f; break;
			case GAC_ALIGNAMENT_BOTTOMLEFT: lcd->X.Value = 0.0f; lcd->Y.Value = 1.0f; break;
			case GAC_ALIGNAMENT_LEFTCENTER: lcd->X.Value = 0.0f; lcd->Y.Value = 0.5f; break;
			case GAC_ALIGNAMENT_CENTER: lcd->X.Value = lcd->Y.Value = 0.5f; break;
		}
		lcd->X.Type = lcd->Y.Type = GAC_COORDINATES_PERCENTAGE;
		lcd->alignamentSet = lcd->xSet = lcd->ySet = true;
		return true;
	}
	RETURNERROR(false,"unknown key for layout format - Key='%s', Value='%s'",key,value);
}
bool CompueLayoutParams(const char *format,float *params,unsigned int paramsCount,GApp::Controls::LayoutData *layout)
{
	CHECK(format!=NULL,false,"");
	
	char ch;
	char key[48];
	char value[256];
	bool onKey = true;
	int poz = 0;
	InternalLayoutComputeData lcd;
	
	CHECK(layout!=NULL,false,"");
	
	// initializam structura
	lcd.xSet = lcd.ySet = lcd.widthSet = lcd.heightSet = lcd.alignamentSet = false;
	
	lcd.X.Type = lcd.Y.Type = LAYOUT_FLAG_XY_TYPE(layout->Flags);
	lcd.Width.Type = lcd.Height.Type = LAYOUT_FLAG_WH_TYPE(layout->Flags);
		
	if (format[0]!=0)
	{
		for (;;format++)
		{
			ch = (*format);
			if ((ch==0) || (ch==';') || (ch==','))
			{
				if (onKey) {
					key[poz]=0;
					value[0] = 0;
				} else
					value[poz]=0;
				// procesez perechea cheie valoare
				CHECK(ComputeLayoutParameter(key,value,&lcd,params,paramsCount),false,"");
				poz = 0;		
				onKey = true;
				if (ch==0)
					break;
				continue;
			}
			if (ch==':')
			{
				CHECK(onKey==true,false,"");
				key[poz]=0;
				onKey=false;
				poz=0;
				continue;
			}		
			if ((ch>='A') && (ch<='Z'))
				ch |= 0x20;
			if ((ch>32) && (ch<127))
			{
				if (onKey)
				{
					CHECK(poz<47,false,"");
					key[poz]=ch;
				}
				else {
					CHECK(poz<255,false,"");
					value[poz]=ch;
				}
				poz++;
			}
		}
	}
	CHECK(lcd.X.Type==lcd.Y.Type,false,"");
	CHECK(lcd.Width.Type == lcd.Height.Type,false,"");
	
	// setez layout=ul
	unsigned int xyType = LAYOUT_FLAG_XY_TYPE(layout->Flags);
	unsigned int whType = LAYOUT_FLAG_WH_TYPE(layout->Flags);
	unsigned int align = LAYOUT_FLAG_ALIGNAMENT(layout->Flags);
	if (lcd.xSet)
	{
		layout->X = lcd.X.Value;
		xyType = lcd.X.Type;
	}
	if (lcd.ySet)
	{
		layout->Y = lcd.Y.Value;
		xyType = lcd.Y.Type;
	}
	if (lcd.widthSet)
	{
		layout->Width = lcd.Width.Value;
		whType = lcd.Width.Type;
	}
	if (lcd.heightSet)
	{
		layout->Height = lcd.Height.Value;
		whType = lcd.Height.Type;
	}
	if (lcd.alignamentSet)
		align = lcd.Alignament;

	layout->Flags = (align) | (xyType << 4) | (whType << 8);

	return true;
}
void UncheckControl(GApp::Controls::GenericControl* control, GApp::Controls::GenericControl* skip, int groupID)
{
	if ((groupID >= 0) && (control->GroupID == groupID) && (control!=skip))
	{
		control->SetChecked(false);
	}
	// iau fiecare copil
	for (unsigned int tr = 0; tr < control->ControlsCount; tr++)
	{
		if (control->Controls[tr]!=NULL)
			UncheckControl(control->Controls[tr], skip, groupID);
	}
}
//============================================================================================================================================
GApp::Controls::GenericControl::GenericControl(void *scene,const char *layout)
{
	ID = GroupID = -1;
	HandlerScene = NULL;
	CoreContext = NULL;
	Controls = NULL;
	Parent = NULL;
	ControlsCount = 0;
	Flags = GAC_CONTROL_FLAG_VISIBLE | GAC_CONTROL_FLAG_ENABLED;
	// initalizez layout
	Layout.X = Layout.Y = 0.5f;
	Layout.Width = Layout.Height = 1.0f;
	Layout.Flags = GAC_ALIGNAMENT_CENTER | (GAC_COORDINATES_PERCENTAGE << 4) | (GAC_COORDINATES_AUTO << 8);
	Layout.ScrollXPixels = 0;
	Layout.ScrollYPixels = 0;
	Layout.TopPixels = 0;
	Layout.WidthInPixels = 0;
	Layout.HeighInPixels = 0;
	Layout.LeftPixels = 0;
	Layout.ViewLeft = 0;
	Layout.ViewBottom = 0;
	Layout.ViewRight = 0;
	Layout.ViewTop = 0;
	Layout.TranslateX = 0;
	Layout.TranslateY = 0;
	if (scene!=NULL)
	{
		CoreContext = ((GApp::UI::Scene *)scene)->CoreContext;
	}
	if (layout!=NULL)
	{
		SetLayout(layout);
	}
}
GApp::Controls::GenericControl::~GenericControl()
{
}
//============================================================================================================================================
bool GApp::Controls::GenericControl::AddControl(GenericControl *ctrl)
{
	CHECK(ctrl!=NULL,false,"Invalid control (NULL)");
	//POPUP-urile pot fi adaugate doar de main control - adica de un control care nu are parinte
	if ((ctrl->Flags & GAC_CONTROL_FLAG_MODAL_FRAME) != 0)
	{
		CHECK((this->Flags & GAC_CONTROL_FLAG_MAIN) != 0, false, "Popups can only be added by a scene ! Use scene.AddControl (...)");
	}
	//LOG_INFO("AddControl(this=%p,Control=%p,Controls List=%p,Count=%d", this, ctrl, Controls, ControlsCount);
	if (Controls==NULL) // first time
	{
		//LOG_INFO("Allocate 8 controls for current container %p", this);
		Controls = new GenericControl*[8];
		CHECK(Controls!=NULL,false,"");
		ControlsCount = 0;
	} else {
		if (((ControlsCount%8)==0) && (ControlsCount>0))
		{
			// grow
			GenericControl	**tmp = new GenericControl*[ControlsCount+8];
			CHECK(tmp!=NULL,false,"");
			for (unsigned int tr=0;tr<ControlsCount;tr++)
				tmp[tr] = Controls[tr];
			delete Controls;
			Controls = tmp;
		}
	}
	// popup-urile se pun tot timpul primele
	if ((ctrl->Flags & GAC_CONTROL_FLAG_MODAL_FRAME) != 0)
	{
		for (unsigned int tr = ControlsCount; tr>0; tr--)
			Controls[tr] = Controls[tr - 1];
		Controls[0] = ctrl;
		ControlsCount++;
	} else {
		Controls[ControlsCount++] = ctrl;
	}
	ctrl->Parent = this;
	if (ctrl->CoreContext==NULL)
		ctrl->CoreContext = this->CoreContext;
	return true;
}
void GApp::Controls::GenericControl::RecomputeLayout()
{
	float p_lpx,p_tpx,p_width,p_height;
	int p_vl,p_vt,p_vb,p_vr;
	bool already_called_on_layout;
	
	if (CoreContext==NULL)
		return;
	if ((Layout.Flags & LAYOUT_FLAG_IN_USE)!=0)
		return;
	if (Parent!=NULL)
	{
		p_lpx = Parent->Layout.LeftPixels - Parent->Layout.ScrollXPixels;
		p_tpx = Parent->Layout.TopPixels - Parent->Layout.ScrollYPixels;
		p_vl = Parent->Layout.ViewLeft;
		p_vt = Parent->Layout.ViewTop;
		p_vr = Parent->Layout.ViewRight;
		p_vb = Parent->Layout.ViewBottom;
		p_width = Parent->Layout.WidthInPixels;
		p_height = Parent->Layout.HeighInPixels;
	} else {
		p_lpx = 0;
		p_tpx = 0;
		p_vl = 0;
		p_vr = (int)G.Width;;
		p_vt = 0;
		p_vb = (int)G.Height;
		p_width = G.Width;
		p_height = G.Height;
	}
	
	already_called_on_layout = false;
	while (true)
	{
		TRANSLATE_COORD_TO_PIXELS(Layout.LeftPixels,Layout.TopPixels,Layout.X,Layout.Y,p_width,p_height,LAYOUT_FLAG_XY_TYPE(Layout.Flags));
		TRANSLATE_COORD_TO_PIXELS(Layout.WidthInPixels,Layout.HeighInPixels,Layout.Width,Layout.Height,p_width,p_height,LAYOUT_FLAG_WH_TYPE(Layout.Flags));
		ALIGN_COORD(Layout.LeftPixels,Layout.TopPixels,Layout.LeftPixels+p_lpx,Layout.TopPixels+p_tpx,Layout.WidthInPixels,Layout.HeighInPixels,LAYOUT_FLAG_ALIGNAMENT(Layout.Flags));
		Layout.RelativeLeftPixels = Layout.LeftPixels - p_lpx;
		Layout.RelativeTopPixels = Layout.TopPixels - p_tpx;

		// calculez si translarea in OpenGL
		//Layout.TranslateX = ((Layout.LeftPixels*2.0f)/(G.Width));
		//Layout.TranslateY = ((Layout.TopPixels*2.0f)/(G.Height));
		Layout.TranslateX = Layout.LeftPixels;
		Layout.TranslateY = Layout.TopPixels;
		// Transformare in coordonate OpenGL
		Layout.ViewLeft = GAC_MAX((int)Layout.LeftPixels,p_vl);
		Layout.ViewRight = GAC_MIN((int)(Layout.LeftPixels + Layout.WidthInPixels), p_vr);
		Layout.ViewTop = GAC_MAX((int)(G.Height - (Layout.TopPixels + Layout.HeighInPixels)), p_vt);
		Layout.ViewBottom = GAC_MIN((int)(G.Height - (Layout.TopPixels)), p_vb);


		// validez ca este visibil
		if ((Layout.ViewLeft<Layout.ViewRight) & (Layout.ViewTop<Layout.ViewBottom)) {
			REMOVE_BIT(Flags,GAC_CONTROL_FLAG_OUTSIDE_PARENT_RECT);
		} else {
			SET_BIT(Flags,GAC_CONTROL_FLAG_OUTSIDE_PARENT_RECT)
		};

		if (already_called_on_layout==false)
		{
			SET_BIT(Layout.Flags,LAYOUT_FLAG_IN_USE);
			already_called_on_layout = OnLayout();
			REMOVE_BIT(Layout.Flags,LAYOUT_FLAG_IN_USE);
			if (already_called_on_layout)
				continue;
		}
		break;
	}
	if (Controls!=NULL)
	{
		for (unsigned int tr=0;tr<ControlsCount;tr++)
			Controls[tr]->RecomputeLayout();
	}
}
bool GApp::Controls::GenericControl::SetLayout(const char *format)
{		
	CHECK(CompueLayoutParams(format,NULL,0,&Layout),false,"");	
	RecomputeLayout();
	return true;
}
bool GApp::Controls::GenericControl::SetLayout(const char *format,float p1)
{	
	float params[] = {p1};	
	CHECK(CompueLayoutParams(format,params,1,&Layout),false,"");	
	RecomputeLayout();
	return true;
}
bool GApp::Controls::GenericControl::SetLayout(const char *format,float p1,float p2)
{	
	float params[] = {p1,p2};	
	CHECK(CompueLayoutParams(format,params,2,&Layout),false,"");	
	RecomputeLayout();
	return true;
}
bool GApp::Controls::GenericControl::SetLayout(const char *format,float p1,float p2,float p3)
{	
	float params[] = {p1,p2,p3};	
	CHECK(CompueLayoutParams(format,params,3,&Layout),false,"");	
	RecomputeLayout();
	return true;
}
bool GApp::Controls::GenericControl::SetLayout(const char *format,float p1,float p2,float p3,float p4)
{	
	float params[] = {p1,p2,p3,p4};	
	CHECK(CompueLayoutParams(format,params,4,&Layout),false,"");	
	return true;
}
void GApp::Controls::GenericControl::MoveTo(float x,float y,GAC_TYPE_COORDINATES coord)
{
	Layout.X = x;
	Layout.Y = y;
	Layout.Flags = (Layout.Flags & 0xFFFFFF0F) | ((coord & 0x0F) << 4);
	RecomputeLayout();
}
void GApp::Controls::GenericControl::Resize(float width,float height,GAC_TYPE_COORDINATES coord)
{
	Layout.Width = width;
	Layout.Height = height;
	Layout.Flags = (Layout.Flags & 0xFFFFF0FF) | ((coord & 0x0F) << 8);
	RecomputeLayout();
}
void GApp::Controls::GenericControl::SetLayout(float x,float y, GAC_TYPE_COORDINATES xyType,float width,float height,GAC_TYPE_COORDINATES whType,GAC_TYPE_ALIGNAMENT align)
{
	Layout.X = x;
	Layout.Y = y;
	Layout.Flags = (Layout.Flags & 0xFFFFFF0F) | ((xyType & 0x0F) << 4);
	Layout.Width = width;
	Layout.Height = height;
	Layout.Flags = (Layout.Flags & 0xFFFFF0FF) | ((whType & 0x0F) << 8);
	Layout.Flags = (Layout.Flags & 0xFFFFFFF0) | (align & 0x0F);
}
void GApp::Controls::GenericControl::TriggerEvent(GAC_TYPE_EVENTTYPE EventCode)
{
	if (HandlerScene!=NULL)
	{
		((GApp::UI::Scene *)HandlerScene)->OnControlEvent(this,EventCode);
	} else {
		LOG_ERROR("TriggerEvent: HandlerScene is NULL - unable to send event");
	}
}
void GApp::Controls::GenericControl::SetVisible(bool value)
{
	if (value) {
		SET_BIT(Flags,GAC_CONTROL_FLAG_VISIBLE);
	} else {
		REMOVE_BIT(Flags,GAC_CONTROL_FLAG_VISIBLE);
	}
}
void GApp::Controls::GenericControl::GAC_SetVisible(bool value)
{
	// nu pot sa aplic operatia pe controalele main si modal frame
	if ((Flags & (GAC_CONTROL_FLAG_MAIN | GAC_CONTROL_FLAG_MODAL_FRAME)) != 0)
		return;
	if (value) {
		SET_BIT(Flags, GAC_CONTROL_FLAG_VISIBLE);
	}
	else {
		REMOVE_BIT(Flags, GAC_CONTROL_FLAG_VISIBLE);
	}
}
void GApp::Controls::GenericControl::SetEnabled(bool value)
{
	if (value) {
		SET_BIT(Flags,GAC_CONTROL_FLAG_ENABLED);
	} else {
		REMOVE_BIT(Flags,GAC_CONTROL_FLAG_ENABLED);
	}
}
void GApp::Controls::GenericControl::GAC_SetEnabled(bool value)
{
	// nu pot sa aplic operatia pe controalele main si modal frame
	if ((Flags & (GAC_CONTROL_FLAG_MAIN | GAC_CONTROL_FLAG_MODAL_FRAME)) != 0)
		return;
	if (value) {
		SET_BIT(Flags, GAC_CONTROL_FLAG_ENABLED);
	}
	else {
		REMOVE_BIT(Flags, GAC_CONTROL_FLAG_ENABLED);
	}
}
void GApp::Controls::GenericControl::SetChecked(bool value)
{
	if (value) {
		LOG_INFO("Setting check for ID=%d, G-ID=%d", ID, GroupID);
		SET_BIT(Flags, GAC_CONTROL_FLAG_CHECKED);
		if (GroupID >= 0)
		{
			LOG_INFO("Unchecking group: %d", GroupID);
			// curat tot ce e check la cele care au acelasi grup ID cu mine
			GenericControl* root = this;
			while (root->Parent != NULL) root = root->Parent;
			// am root-ul - fac uncheck la toti
			UncheckControl(root, this, GroupID);
		}
	}
	else {
		REMOVE_BIT(Flags, GAC_CONTROL_FLAG_CHECKED);
	}
}
bool GApp::Controls::GenericControl::IsVisible()
{
	return ((Flags & GAC_CONTROL_FLAG_VISIBLE)!=0);
}
bool GApp::Controls::GenericControl::HasTouch()
{
	return ((Flags & GAC_CONTROL_FLAG_TOUCHED)!=0);
}
bool GApp::Controls::GenericControl::IsEnabled()
{
	return ((Flags & GAC_CONTROL_FLAG_ENABLED)!=0);
}
bool GApp::Controls::GenericControl::IsChecked()
{
	return ((Flags & GAC_CONTROL_FLAG_CHECKED) != 0);
}	
int  GApp::Controls::GenericControl::GetID()
{
	return ID;
}
void GApp::Controls::GenericControl::SetID(int value)
{
	ID = value;
}
int  GApp::Controls::GenericControl::GetGroupID()
{
	return GroupID;
}
void GApp::Controls::GenericControl::SetGroupID(int value)
{
	GroupID = value;
}
float GApp::Controls::GenericControl::GetWidth()
{
	return Layout.WidthInPixels;
}
float GApp::Controls::GenericControl::GetHeight()
{
	return Layout.HeighInPixels;
}
float GApp::Controls::GenericControl::GetScrollX()
{
	return Layout.ScrollXPixels;
}
float GApp::Controls::GenericControl::GetScrollY()
{
	return Layout.ScrollYPixels;
}
float GApp::Controls::GenericControl::GetLeft()
{
	return Layout.LeftPixels;
}
float GApp::Controls::GenericControl::GetTop()
{
	return Layout.TopPixels;
}
float GApp::Controls::GenericControl::GetRelativeLeft()
{
	return Layout.RelativeLeftPixels;
}
float GApp::Controls::GenericControl::GetRelativeTop()
{
	return Layout.RelativeTopPixels;
}
float GApp::Controls::GenericControl::GetRight()
{
	return Layout.LeftPixels + Layout.WidthInPixels;
}
float GApp::Controls::GenericControl::GetBottom()
{
	return Layout.TopPixels + Layout.HeighInPixels;
}
bool GApp::Controls::GenericControl::GetRect(GApp::Graphics::Rect *r)
{
	CHECK(r != NULL, false, "Invalid (NULL) rect object !");
	r->Left = (int)Layout.LeftPixels;
	r->Top = (int)Layout.TopPixels;
	r->Bottom = (int)(Layout.TopPixels + Layout.HeighInPixels);
	r->Right = (int)(Layout.LeftPixels + Layout.WidthInPixels);
	return true;
}
GApp::Controls::GenericControl*	GApp::Controls::GenericControl::GetParent()
{
	return Parent;
}
unsigned int	GApp::Controls::GenericControl::GetControlsCount()
{
	return ControlsCount;
}
GApp::Controls::GenericControl* GApp::Controls::GenericControl::GetControl(unsigned int index)
{
	if (index < ControlsCount)
		return Controls[index];
	return NULL;
}

void GApp::Controls::GenericControl::OnPaint()
{
}
void GApp::Controls::GenericControl::OnReceiveFocus()
{
}
void GApp::Controls::GenericControl::OnLoseFocus()
{
}
bool GApp::Controls::GenericControl::OnTouchEvent(TouchEvent *evn)
{
	return false;
}
bool GApp::Controls::GenericControl::OnLayout()
{
	return false;
}
bool GApp::Controls::GenericControl::ProcessTouchEventForClick(TouchEvent *te)
{
	if (te==NULL)
		return false;
	switch (te->Type)
	{
		case GAC_TOUCHEVENTTYPE_CLICK:
			TriggerEvent(GAC_EVENTTYPE_CLICK);
			return true;
		case GAC_TOUCHEVENTTYPE_DOWN:
			return true;
		case GAC_TOUCHEVENTTYPE_MOVE:
			return te->IsClick;
		default:
			return false;
	}
}