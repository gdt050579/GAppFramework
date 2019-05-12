#include "GApp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//#define TXT(x)	L##x
#define TXT(x)	x


static const CHARACTER *convert_array_upper={TXT("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")};
static const CHARACTER *convert_array_lower={TXT("0123456789abcdefghijklmnopqrstuvwxyz")};


#define STRING_FLAG_STATIC_BUFFER		1
#define STRING_FLAG_CONSTANT			2


#define GSTRING_TEST_AND_INIT(returnValue)	{ if (Text==NULL) { if (Create(64)==false) return returnValue; } }
#define EXIT_IF_CONSTANT(returnValue)		{ if ((Flags & STRING_FLAG_CONSTANT)!=0) return returnValue; }
#define GSTRING_GROW_WITH(x,returnValue)	{ if (x+Size+1>=MaxSize) {	if (Grow(x+Size+32)==false)	return returnValue; }  }
#define LOWER_CHAR(x) (((x>='A') && (x<='Z'))?x|0x20:x)
#define CONVERT_TO_COMPARE(x,ignore) (((ignore) && (x>='A') && (x<='Z'))?x|0x20:x)
#define MEMMOVE	memmove
#define MEMCOPY	memcpy
#define MAX_FORMATED_EX_TEMP_BUF	512
#define MAX_EXTVALUE				32

struct DataConvertInfo
{
	enum {
		TYPE_None = 0,
		TYPE_Bool,
		TYPE_Char,
		TYPE_WChar,
		TYPE_UInt8,
		TYPE_UInt16,
		TYPE_UInt32,
		TYPE_UInt64,
		TYPE_Int8,
		TYPE_Int16,
		TYPE_Int32,
		TYPE_Int64,
		TYPE_Ascii,
		TYPE_Unicode,
		TYPE_Float,
		TYPE_Double,		
	};
	enum {
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};
	enum {
		FLAG_UPPER = 1,
		FLAG_TRUNCATE = 2,
		FLAG_BE = 4,
	};
	enum {
		EXTVALUE_AlignSize = 0,
		EXTVALUE_Base,
		EXTVALUE_Zecimals,
		EXTVALUE_GROUP,
		EXTVALUE_FillChar
	};
	unsigned int	DataType;
	unsigned int	AlignSize;
	unsigned char	Align;
	unsigned int	Base;
	unsigned int	Flags;
	unsigned int	Zecimals;
	unsigned int	Group;
	CHARACTER		FillChar;
	unsigned int	ExternalValue[MAX_EXTVALUE];
	unsigned int	ExternalValuesCount;
};
union DataValueUnion
{
	unsigned char	vUInt8;
	unsigned short	vUInt16;
	unsigned int	vUInt32;
	UInt64		vUInt64;

	char			vInt8;
	short			vInt16;
	int				vInt32;
	UInt64		vInt64;

	char			vChar;
	short			vWChar;

	char*			vAscii;
	CHARACTER*		vUnicode;
	bool			vBool;

	float			vFloat;
	double			vDouble;
};
// tipuri
struct StructTypeName
{
	const CHARACTER		*Name;
	unsigned int	Id;
};
static StructTypeName __TypeNames[] = {
	{TXT("b"),			DataConvertInfo::TYPE_Bool},
	{TXT("bool"),		DataConvertInfo::TYPE_Bool},

	{TXT("i8"),			DataConvertInfo::TYPE_Int8},
	{TXT("int8"),		DataConvertInfo::TYPE_Int8},
	{TXT("char"),		DataConvertInfo::TYPE_Int8},

	{TXT("c"),			DataConvertInfo::TYPE_Char},
	
	{TXT("uc"),			DataConvertInfo::TYPE_WChar},
	{TXT("wchar"),		DataConvertInfo::TYPE_WChar},

	{TXT("i16"),		DataConvertInfo::TYPE_Int16},
	{TXT("int16"),		DataConvertInfo::TYPE_Int16},
	{TXT("short"),		DataConvertInfo::TYPE_Int16},

	{TXT("i32"),		DataConvertInfo::TYPE_Int32},
	{TXT("int32"),		DataConvertInfo::TYPE_Int32},
	{TXT("int"),		DataConvertInfo::TYPE_Int32},

	{TXT("i64"),		DataConvertInfo::TYPE_Int64},
	{TXT("int64"),		DataConvertInfo::TYPE_Int64},

	{TXT("ui8"),		DataConvertInfo::TYPE_UInt8},
	{TXT("uint8"),		DataConvertInfo::TYPE_UInt8},
	{TXT("byte"),		DataConvertInfo::TYPE_UInt8},

	{TXT("ui16"),		DataConvertInfo::TYPE_UInt16},
	{TXT("uint16"),		DataConvertInfo::TYPE_UInt16},
	{TXT("w"),			DataConvertInfo::TYPE_UInt16},

	{TXT("ui32"),		DataConvertInfo::TYPE_UInt32},
	{TXT("uint32"),		DataConvertInfo::TYPE_UInt32},
	{TXT("uint"),		DataConvertInfo::TYPE_UInt32},
	{TXT("dw"),			DataConvertInfo::TYPE_UInt32},

	{TXT("ui64"),		DataConvertInfo::TYPE_UInt64},
	{TXT("uint64"),		DataConvertInfo::TYPE_UInt64},
	{TXT("qw"),			DataConvertInfo::TYPE_UInt64},

	{TXT("s"),			DataConvertInfo::TYPE_Ascii},
	{TXT("str"),		DataConvertInfo::TYPE_Ascii},
	{TXT("string"),		DataConvertInfo::TYPE_Ascii},
	{TXT("a"),			DataConvertInfo::TYPE_Ascii},
	{TXT("ascii"),		DataConvertInfo::TYPE_Ascii},

	{TXT("u"),			DataConvertInfo::TYPE_Unicode},
	{TXT("unicode"),	DataConvertInfo::TYPE_Unicode},

	{TXT("f"),			DataConvertInfo::TYPE_Float},
	{TXT("float"),		DataConvertInfo::TYPE_Float},

	{TXT("r"),			DataConvertInfo::TYPE_Double},
	{TXT("dbl"),		DataConvertInfo::TYPE_Double},
	{TXT("double"),		DataConvertInfo::TYPE_Double},

};


bool Simple_IsCharMatching(CHARACTER ct,CHARACTER cm,bool IgnoreCase)
{
	if (IgnoreCase)
	{
		if ((ct>='A') && (ct<='Z')) ct+=32;
		if ((cm>='A') && (cm<='Z')) cm+=32;
	}
	if (cm=='?') cm=ct;
	return (ct==cm);
}
int  Simple_MatchFindNext(const CHARACTER *text,CHARACTER car,bool IgnoreCase)
{
	int			tr;

	if (car==0) return -2;

	for (tr=0;text[tr]!=0;tr++)
		if (Simple_IsCharMatching(text[tr],car,IgnoreCase)) 
			return tr;
	return -1;
}
bool Simple_Match(const CHARACTER *text,const CHARACTER *masca,bool IgnoreCase,bool bStartWith)
{
	int		tr,found,tpoz;
	CHARACTER	ct,cm;

	for (tr=0;(text[tr]!=0) && (masca[tr]!=0);tr++)
	{
		ct=text[tr];
		cm=masca[tr];
		if (cm=='*')
		{
			tpoz=tr;
			do
			{
				tpoz++;
				found=Simple_MatchFindNext(&text[tpoz],masca[tr+1],IgnoreCase);
				if (found==-2) 
					return true;
				if (found<0) 
					return false;
				tpoz+=found;
			} 
			while ((text[tpoz]!=0) && (Simple_Match(&text[tpoz],&masca[tr+1],IgnoreCase,bStartWith)==false));
			return (text[tpoz]!=0);
		} else {
			if (Simple_IsCharMatching(ct,cm,IgnoreCase)==false) 
				return false;
		}
	}
	if (bStartWith)
	{
		return (masca[tr]==0);
	} else {
		return ((text[tr]==masca[tr]) && (text[tr]==0));
	}
}


bool StringToNumber(const CHARACTER *text,unsigned int len,UInt64 *value,unsigned int base)
{
	UInt64			cVal;
	unsigned int		tr;

	if ((text==NULL) || (value==NULL) || (base<2) || (base>16))
		return false;
	(*value)=0;
	for (tr=0;(tr<len) && ((*text)!=0);tr++,text++)
	{
		if (((*text)>='0') && ((*text)<='9'))
			cVal = (*text)-'0';
		else if (((*text)>='A') && ((*text)<='Z'))
				cVal = (*text)-'A'+10;
		else if (((*text)>='a') && ((*text)<='z'))
				cVal = (*text)-'a'+10;
		else return false;
		if (cVal>=base)
			return false;
		(*value) = (*value)*base+cVal;
	}
	return true;
}
bool GetStringNumberInformations(const CHARACTER *text,unsigned int len,bool *isSigned,unsigned int *base,unsigned int *startValue,unsigned int *textLen)
{
	unsigned int start=0;

	if ((text==NULL) || (isSigned==NULL) || (base==NULL) || (len==0) || (startValue==NULL) || (textLen==NULL))
		return false;
	
	(*isSigned)=false;
	(*base)=10;

	if (text[0]=='-')
		(*isSigned)=true;
	if ((text[0]=='-') || (text[0]=='+'))
		start++;
	if ((start+2<len) && (text[start]=='0') && (LOWER_CHAR(text[start+1])=='x'))
	{
		(*base)=16;
		start+=2;
	} else
	if (text[len-1]=='h')
	{
		(*base)=16;
		len--;
	} else
	if (text[start]=='0')
	{
		(*base)=8;
		start++;
	}
	(*startValue)=start;
	(*textLen) = len;
	return true;
}
bool ConvertStringToNumber(const CHARACTER *text,UInt64 *value,unsigned int base,int textSize,bool onlyUnsigned,bool *_isSigned=NULL)
{
	bool			isSigned=false;
	unsigned int	start=0,numberLen,_base;
	UInt64		result;
	
	if ((value==NULL) || (text==NULL))
		return false;
	if (textSize==-1)
	{
		for (textSize=0;text[textSize]!=0;textSize++);
	}
	if (GetStringNumberInformations(text,textSize,&isSigned,&_base,&start,&numberLen)==false)
		return false;
	if (base==0)
		base = _base;
	if ((onlyUnsigned) && (isSigned))
		return false;
	if (StringToNumber(&text[start],numberLen,&result,base)==false)
		return false;
	(*value) = result;
	if (_isSigned)
		(*_isSigned) = isSigned;
	return true;
}

bool __StrEq(const CHARACTER *s1,int s1Size,const CHARACTER *s2,bool ignoreCase=false)
{
	int			tr;
	CHARACTER	c1,c2;

	for (tr=0;(s2[tr]!=0) && (tr<s1Size);tr++)
	{
		c1=CONVERT_TO_COMPARE(s1[tr],ignoreCase);
		c2=CONVERT_TO_COMPARE(s2[tr],ignoreCase);
		if (c1!=c2)
			return false;
	}
	if ((s2[tr]==0) && (tr==s1Size))
		return true;
	return false;
}
bool __StrToUInt32(const CHARACTER *s1,int s1Size,unsigned int *value)
{
	(*value)=0;
	if (s1Size<1)
		return false;
	for (int tr=0;tr<s1Size;tr++)
	{
		if ((s1[tr]>='0') && (s1[tr]<='9'))
		{
			(*value) = (*value) * 10 + s1[tr]-'0';
		} else {
			return false;
		}
	}
	return true;
}
void ResetDataConvertInfo(DataConvertInfo *dci)
{
	if (dci==NULL)
		return;
	dci->Align = DataConvertInfo::ALIGN_LEFT;
	dci->Base = 10;
	dci->Flags = 0;
	dci->DataType = DataConvertInfo::TYPE_None;
	dci->Zecimals = 0;
	dci->AlignSize = 0;
	dci->Group = 0xFFFFFFFF;
	dci->FillChar = ' ';
	dci->ExternalValuesCount = 0;
}
bool UpdateDataConvertInfo(DataConvertInfo *dci,const CHARACTER *info,int infoSize)
{
	unsigned int	tr;
	
	if ((info==NULL) || (info[0]==0) || (dci==NULL))
		return false;
	// tipul
	if ((info[0]>='a') && (info[0]<='z'))
	{
		for (tr=0;tr<sizeof(__TypeNames)/sizeof(StructTypeName);tr++)
		{
			if (__StrEq(info,infoSize,__TypeNames[tr].Name,false))
			{
				dci->DataType = __TypeNames[tr].Id;
				return true;
			}
		}
	}
	// verific daca nu e o baza cunoscuta
	if (__StrEq(info,infoSize,TXT("hex"),true))
	{
		dci->Base = 16;
		if (info[0]=='H') 
			dci->Flags |= DataConvertInfo::FLAG_UPPER;
		return true;
	}
	if (__StrEq(info,infoSize,TXT("oct"),true))
	{
		dci->Base = 8;
		if (info[0]=='O') 
			dci->Flags |= DataConvertInfo::FLAG_UPPER;
		return true;
	}
	if (__StrEq(info,infoSize,TXT("bin"),true))
	{
		dci->Base = 2;
		if (info[0]=='B') 
			dci->Flags |= DataConvertInfo::FLAG_UPPER;
		return true;
	}
	if (__StrEq(info,infoSize,TXT("dec"),true))
	{
		dci->Base = 10;
		if (info[0]=='D') 
			dci->Flags |= DataConvertInfo::FLAG_UPPER;
		return true;
	}
	// flag de BigEndia	
	if ((__StrEq(info,infoSize,TXT("be"),true)) || (__StrEq(info,infoSize,TXT("bigendian"),true)))
	{
		dci->Flags |= DataConvertInfo::FLAG_BE;
		return true;
	}
	if ((info[0]|0x20)=='b')
	{
		if ((infoSize==3) && (info[1]=='%') && (info[2]=='%'))
		{
			dci->ExternalValue[dci->ExternalValuesCount++] = DataConvertInfo::EXTVALUE_Base;
			if (dci->ExternalValuesCount == MAX_EXTVALUE)
				return false;
		} else {
			if (__StrToUInt32(&info[1],infoSize-1,&dci->Base)==false)
				return false;
			if ((dci->Base<2) || (dci->Base>=36))
				return false;
		}
		if (info[0]=='B') 
			dci->Flags |= DataConvertInfo::FLAG_UPPER;
		return true;
	}
	// verific alignamentul
	if (info[0]=='L')
	{
		if ((infoSize==3) && (info[1]=='%') && (info[2]=='%'))
		{
			dci->ExternalValue[dci->ExternalValuesCount++] = DataConvertInfo::EXTVALUE_AlignSize;
			if (dci->ExternalValuesCount == MAX_EXTVALUE)
				return false;
		} else {
			if (info[1]!=0)
			{
				if (__StrToUInt32(&info[1],infoSize-1,&dci->AlignSize)==false)
					return false;
			}
		}
		dci->Align = DataConvertInfo::ALIGN_LEFT;		
		return true;
	}
	if (info[0]=='R')
	{
		if ((infoSize==3) && (info[1]=='%') && (info[2]=='%'))
		{
			dci->ExternalValue[dci->ExternalValuesCount++] = DataConvertInfo::EXTVALUE_AlignSize;
			if (dci->ExternalValuesCount == MAX_EXTVALUE)
				return false;
		} else {
			if (info[1]!=0)
			{
				if (__StrToUInt32(&info[1],infoSize-1,&dci->AlignSize)==false)
					return false;
			}
		}
		dci->Align = DataConvertInfo::ALIGN_RIGHT;
		return true;
	}
	if (info[0]=='C')
	{
		if ((infoSize==3) && (info[1]=='%') && (info[2]=='%'))
		{
			dci->ExternalValue[dci->ExternalValuesCount++] = DataConvertInfo::EXTVALUE_AlignSize;
			if (dci->ExternalValuesCount == MAX_EXTVALUE)
				return false;
		} else {
			if (info[1]!=0)
			{
				if (__StrToUInt32(&info[1],infoSize-1,&dci->AlignSize)==false)
					return false;
			}
		}
		dci->Align = DataConvertInfo::ALIGN_CENTER;
		return true;
	}
	// flag de trunchiere
	if (__StrEq(info,infoSize,TXT("trunc"),true))
	{
		dci->Flags |= DataConvertInfo::FLAG_TRUNCATE;
		return true;
	}
	if (info[0]=='Z')
	{
		if ((infoSize==3) && (info[1]=='%') && (info[2]=='%'))
		{
			dci->ExternalValue[dci->ExternalValuesCount++] = DataConvertInfo::EXTVALUE_Zecimals;
			if (dci->ExternalValuesCount == MAX_EXTVALUE)
				return false;
		} else {
			if (__StrToUInt32(&info[1],infoSize-1,&dci->Zecimals)==false)
				return false;
		}
		return true;
	}
	if (info[0]=='F')
	{
		if ((infoSize==3) && (info[1]=='%') && (info[2]=='%'))
		{
			dci->ExternalValue[dci->ExternalValuesCount++] = DataConvertInfo::EXTVALUE_FillChar;
			if (dci->ExternalValuesCount == MAX_EXTVALUE)
				return false;
		} else {
			if (infoSize!=2)
				return false;
			dci->FillChar = info[1];
		}
		return true;
	}
	if (info[0]=='G')
	{
		if ((infoSize==3) && (info[1]=='%') && (info[2]=='%'))
		{
			dci->ExternalValue[dci->ExternalValuesCount++] = DataConvertInfo::EXTVALUE_GROUP;
			if (dci->ExternalValuesCount == MAX_EXTVALUE)
				return false;
		} else {
			if (__StrToUInt32(&info[1],infoSize-1,&dci->Group)==false)
				return false;
			if (dci->Group==0)
				return false;
		}
		return true;
	}
	return false;
}
CHARACTER* ConvertIntegerNumberToString(DataConvertInfo *dci,Int64 data,CHARACTER *tempBuffer,int tempBufferSize,int *resultSize)
{
	bool			negativ = false;
	const CHARACTER*		letters = convert_array_lower;
	int				poz = tempBufferSize-1;
	unsigned int	gPoz=0;
	


	if (dci->Flags & DataConvertInfo::FLAG_UPPER)
		letters = convert_array_upper;

	if (data<0)
	{
		data = -data;
		negativ = true;
	}
	if (dci->Flags & DataConvertInfo::FLAG_BE)
	{
		poz = 0;
		if (negativ)
		{
			tempBuffer[0] = '-';
			poz++;
		}
		do
		{
			tempBuffer[poz++] = letters[data % dci->Base];
			data = data / dci->Base;
			gPoz++;
			if ((gPoz==dci->Group) && (data>0))
			{
				tempBuffer[poz++]=',';
				gPoz=0;
			}		
		} while ((data>0) && (poz+2<tempBufferSize));
		if (data!=0)
			return NULL;
		tempBuffer[poz] = 0;
		(*resultSize) = poz;
		return &tempBuffer[0];		
	} else {
		tempBuffer[poz]=0;
		poz--;	
	do
	{
		tempBuffer[poz--] = letters[data % dci->Base];
		data = data / dci->Base;
		gPoz++;
		if ((gPoz==dci->Group) && (data>0))
		{
			tempBuffer[poz--]=',';
			gPoz=0;
		}		
	} while ((data>0) && (poz>=2));
	if (data!=0)
		return NULL;
	if (negativ)
		tempBuffer[poz--] = '-';
	// totul e ok
	poz++;
	(*resultSize) = (tempBufferSize-1)-poz;
	return &tempBuffer[poz];
	}
}
CHARACTER* ConvertUIntegerNumberToString(DataConvertInfo *dci,UInt64 data,CHARACTER *tempBuffer,int tempBufferSize,int *resultSize)
{
	const CHARACTER*		letters = convert_array_lower;
	int				poz = tempBufferSize-1;
	unsigned int	gPoz=0;
	
	tempBuffer[poz]=0;
	poz--;

	if (dci->Flags & DataConvertInfo::FLAG_UPPER)
		letters = convert_array_upper;

	do
	{
		tempBuffer[poz--] = letters[data % dci->Base];
		data = data / dci->Base;
		gPoz++;
		if ((gPoz==dci->Group) && (data>0))
		{
			tempBuffer[poz--]=',';
			gPoz=0;
		}		
	} while ((data>0) && (poz>=2));
	if (data!=0)
		return NULL;
	// totul e ok
	poz++;
	(*resultSize) = (tempBufferSize-1)-poz;
	return &tempBuffer[poz];
}
/*
CHARACTER* ConvertDoubleNumberToString(DataConvertInfo *dci,double data,const CHARACTER *tempBuffer,int tempBufferSize,int *resultSize)
{
	int		pctPoz,size,countZecimals;
	CHARACTER*	ptr;

	size = sprintf_s(tempBuffer,tempBufferSize-1,"%lf",data);
	if ((size<0) || (size>=tempBufferSize))
		return NULL;	
	// caut punctul
	ptr = tempBuffer;
	for (pctPoz = 0;((*ptr)!=0) && ((*ptr)!='.');ptr++,pctPoz++);
	if ((*ptr)==0)
	{
		(*ptr)='.';
		size++;
	}
	if (dci->Zecimals==0)
	{
		(*resultSize)=pctPoz;
		return tempBuffer;
	}
	countZecimals = (size-1)-pctPoz;
	if (countZecimals>=(int)dci->Zecimals)
	{
		(*resultSize)=pctPoz+dci->Zecimals+1;
		return tempBuffer;
	}
	ptr = &tempBuffer[size];
	while ((size<tempBufferSize) && (countZecimals<dci->Zecimals))
	{
		(*ptr)='0';
		ptr++;
		size++;
		countZecimals++;
	}
	(*resultSize) = size;
	return tempBuffer;
}
*/
//-------------------------------------------------- FUNCTII STATICE ---------------------------------------------------------------------
int	 GApp::Utils::String::Len(const CHARACTER *string)
{
	if (string==NULL)
		return 0;
	const CHARACTER* start = string;
	while ((*string)!=0) { string++; }	
	return (string-start);
}
bool GApp::Utils::String::Add(CHARACTER *destination, const CHARACTER *text, int maxDestinationSize, int destinationTextSize, int textSize)
{
	int tr;

	if ((destination==NULL) || (text==NULL))
		return false;
	if (destinationTextSize==-1)
		destinationTextSize = Len(destination);
	if (textSize==-1)
		textSize=Len(text);
	if (destinationTextSize+textSize+1>maxDestinationSize)
		return false;
	for (tr=0;tr<textSize;tr++,destinationTextSize++)
		destination[destinationTextSize] = text[tr];
	destination[destinationTextSize]=0;

	return true;
}
bool GApp::Utils::String::Set(CHARACTER *destination, const CHARACTER *text, int maxDestinationSize, int textSize)
{
	if (destination==NULL)
		return false;
	destination[0]=0;
	return Add(destination,text,maxDestinationSize,0,textSize);
}
int  GApp::Utils::String::Compare(const CHARACTER *sir1, const CHARACTER *sir2, bool ignoreCase)
{
	CHARACTER	c1,c2;
	if ((sir1==NULL) || (sir2==NULL))
		return -1;
	for (;((*sir1)!=0) && ((*sir2)!=0);sir1++,sir2++)
	{
		c1=CONVERT_TO_COMPARE((*sir1),ignoreCase);
		c2=CONVERT_TO_COMPARE((*sir2),ignoreCase);
		if (c1<c2)
			return -1;
		if (c1>c2)
			return 1;
	}
	if (((*sir1)==0) && ((*sir2)==0))
		return 0;
	if (((*sir1)==0) && ((*sir2)!=0))
		return -1;
	return 1;
}
bool GApp::Utils::String::Equals(const CHARACTER *sir1,const CHARACTER *sir2,bool ignoreCase)
{
	return (bool)(Compare(sir1,sir2,ignoreCase)==0);
}
bool GApp::Utils::String::StartsWith(const CHARACTER *sir1, const CHARACTER *sir2, bool ignoreCase)
{
	CHARACTER	c1,c2;
	if ((sir1==NULL) || (sir2==NULL))
		return false;
	for (;((*sir1)!=0) && ((*sir2)!=0);sir1++,sir2++)
	{
		c1=CONVERT_TO_COMPARE((*sir1),ignoreCase);
		c2=CONVERT_TO_COMPARE((*sir2),ignoreCase);
		if (c1!=c2)
			return false;
	}
	if ((*sir2)==0)
		return true;
	return false;
}
bool GApp::Utils::String::EndsWith(const CHARACTER *sir, const CHARACTER *text, bool ignoreCase, int sirTextSize, int textSize)
{
	if (sirTextSize==-1)
		sirTextSize = GApp::Utils::String::Len(sir);
	if (textSize==-1)
		textSize = GApp::Utils::String::Len(text);
	if (textSize>sirTextSize)
		return false;
	return (bool)(Compare(&sir[sirTextSize-textSize],text,ignoreCase)==0);
}
int  GApp::Utils::String::Find(const CHARACTER *source, const CHARACTER *text, bool ignoreCase, int startPoz, bool searchForward, int sourceSize)
{
	if ((source==NULL) || (text==NULL))
		return -1;
	if (sourceSize==-1)
		sourceSize = Len(source);
	if (startPoz<0)
	{
		startPoz = sourceSize+startPoz;
		if (startPoz<0) startPoz=0;
	}
	if (searchForward)
	{
		while (startPoz<sourceSize)
		{
			if (StartsWith(&source[startPoz],text,ignoreCase))
				return startPoz;
			startPoz++;
		}
	} else {
		while (startPoz>=0)
		{
			if (StartsWith(&source[startPoz],text,ignoreCase))
				return startPoz;
			startPoz--;
		}
	}
	return -1;
}
bool GApp::Utils::String::Contains(const CHARACTER *text, const CHARACTER *textToFind,bool ignoreCase)
{
	return (Find(text,textToFind,ignoreCase)>=0);
}
bool GApp::Utils::String::Delete(CHARACTER *source,int start,int end,int sourceSize)
{
	if (source==NULL)
		return false;
	if (sourceSize<0)
		sourceSize = Len(source);
	if ((start<0) || (start>end) || (end>sourceSize))
		return false;	
	MEMMOVE(&source[start],&source[end],((sourceSize+1)-end)*sizeof(CHARACTER));
	return true;
}
bool GApp::Utils::String::Insert(CHARACTER *source,const CHARACTER *text,int pos,int maxSourceSize,int sourceSize,int textSize)
{
	if ((source==NULL) || (text==NULL) || (maxSourceSize<0) || (pos<0))
		return false;
	if (sourceSize<0)
		sourceSize = Len(source);
	if (textSize<0)
		textSize = Len(text);
	if (pos>sourceSize)
		return false;
	if (sourceSize+textSize+1>=maxSourceSize)
		return false;
	MEMMOVE(&source[pos+textSize],&source[pos],(sourceSize+1-pos)*sizeof(CHARACTER));
	MEMCOPY(&source[pos],text,textSize*sizeof(CHARACTER));
	return true;
}
bool GApp::Utils::String::ReplaceOnPos(CHARACTER *source,const CHARACTER *text,int start,int end,int maxSourceSize,int sourceSize,int textSize)
{
	int szBuf,newPos;

	if ((source==NULL) || (text==NULL))
		return false;
	if (sourceSize<0)
		sourceSize = Len(source);
	if (textSize<0)
		textSize=Len(text);
	if ((end<start) || (start<0) || (end>sourceSize))
		return false;
	// verific daca incape
	szBuf = end-start;
	newPos = start+textSize;
	if (((sourceSize+textSize+1)-szBuf)>=maxSourceSize)
		return false;
	if (szBuf!=textSize)
	{
		MEMMOVE(&source[newPos],&source[end],(sourceSize+1-end)*sizeof(CHARACTER));
	}
	MEMCOPY(&source[start],text,textSize*sizeof(CHARACTER));
	return true;
}
bool GApp::Utils::String::Replace(CHARACTER *source,const CHARACTER *pattern,const CHARACTER *newText,int maxSourceSize,bool ignoreCase,int sourceSize,int patternSize,int newTextSize)
{
	int pos,start;

	if ((source==NULL) || (pattern==NULL) || (newText==NULL))
		return false;
	if (sourceSize<0)
		sourceSize = Len(source);
	if (patternSize<0)
		patternSize = Len(pattern);
	if (newTextSize<0)
		newTextSize = Len(newText);

	start=0;
	do
	{
		pos = Find(source,pattern,ignoreCase,start,true,sourceSize);
		if (pos>=0)
		{
			if (ReplaceOnPos(source,newText,pos,pos+patternSize,maxSourceSize,sourceSize,newTextSize)==false)
				return false;
			start=pos+newTextSize;
		}
	} while (pos>=0);
	return true;
}
bool GApp::Utils::String::ConvertToUInt8(const CHARACTER *text,UInt8 &value,unsigned int base,int textSize)
{
	UInt64	result;
	if (ConvertStringToNumber(text,&result,base,textSize,true)==false)
		return false;
	(value) = (UInt8)result;
	return true;
}
bool GApp::Utils::String::ConvertToUInt16(const CHARACTER *text,UInt16 &value,unsigned int base,int textSize)
{
	UInt64	result;
	if (ConvertStringToNumber(text,&result,base,textSize,true)==false)
		return false;
	(value) = (UInt16)result;
	return true;
}
bool GApp::Utils::String::ConvertToUInt32(const CHARACTER *text,UInt32 &value,unsigned int base,int textSize)
{
	UInt64	result;
	if (ConvertStringToNumber(text,&result,base,textSize,true)==false)
		return false;
	(value) = (UInt32)result;
	return true;
}

bool GApp::Utils::String::ConvertToUInt64(const CHARACTER *text,UInt64 &value,unsigned int base,int textSize)
{
	UInt64	result;
	if (ConvertStringToNumber(text,&result,base,textSize,true)==false)
		return false;
	(value) = result;
	return true;
}

bool GApp::Utils::String::ConvertToInt8(const CHARACTER *text,Int8 &value,unsigned int base,int textSize)
{
	UInt64	result;
	bool		isSigned;
	if (ConvertStringToNumber(text,&result,base,textSize,false,&isSigned)==false)
		return false;
	if (isSigned)
		(value) = -((Int8)result);
	else
		(value) = (Int8)result;
	return true;
}

bool GApp::Utils::String::ConvertToInt16(const CHARACTER *text,Int16 &value,unsigned int base,int textSize)
{
	UInt64	result;
	bool		isSigned;
	if (ConvertStringToNumber(text,&result,base,textSize,false,&isSigned)==false)
		return false;
	if (isSigned)
		(value) = -((Int16)result);
	else
		(value) = (Int16)result;
	return true;
}
bool GApp::Utils::String::ConvertToInt32(const CHARACTER *text,Int32 &value,unsigned int base,int textSize)
{
	UInt64	result;
	bool		isSigned;
	if (ConvertStringToNumber(text,&result,base,textSize,false,&isSigned)==false)
		return false;
	if (isSigned)
		(value) = -((Int32)result);
	else
		(value) = (Int32)result;
	return true;
}

bool GApp::Utils::String::ConvertToInt64(const CHARACTER *text,Int64 &value,unsigned int base,int textSize)
{
	UInt64	result;
	bool		isSigned;
	if (ConvertStringToNumber(text,&result,base,textSize,false,&isSigned)==false)
		return false;
	if (isSigned)
		(value) = -((Int64)result);
	else
		(value) = (Int64)result;
	return true;
}
bool GApp::Utils::String::ConvertToFloat64(const CHARACTER *text,Float64 &value,int textSize)
{
	int		punctCount,tr;
	Float64	val,cCifra,rap;
	bool	negative = false;

	if(text == NULL){
		return false;
	}
	if(textSize == -1){
		textSize = Len(text);
	}
	if(textSize < 1){
		return false;
	}
	val = 0;
	rap = 10;
	tr = 0;
	if (text[0]=='-')
	{
		tr++;
		negative = true;
		text++;
	}
	for (punctCount=0;tr<textSize;tr++,text++)
	{
		if ((*text)=='.')
		{
			punctCount++;
			if (punctCount>1)
		return false;
			continue;
		}
		if (((*text)>='0') && ((*text)<='9'))
	{
			cCifra = (Float64)((*text)-'0');
			if (punctCount==0) {
				val = val * 10+ cCifra;
			} else {
				val+=cCifra/rap;
				rap*=10;
			}
			continue;
		}
		// nu e un caracter valid
		return false;
	}
	if (negative)
		val = -val;
	(value) = val;
	return true;
}
bool GApp::Utils::String::ConvertToFloat32(const CHARACTER *text,Float32 &value,int textSize)
{
	Float64 val;
	if (ConvertToFloat64(text,val,textSize)==false)
		return false;
	(value) = (Float32)val;
	return true;
}

bool GApp::Utils::String::ConvertToBool(const CHARACTER *text,Bool &value)
{
	if(text == NULL){
		return false;
	}
	if(Compare(text, "true", true) == 0 || Compare(text, "1") == 0){
		value = true;
	}else if(Compare(text, "false", true) == 0 || Compare(text, "0") == 0){
		value = false;
	}else{
		return false;
	}
	return true;
}

bool GApp::Utils::String::UInt8ToString(UInt8 value,CHARACTER *text,UInt32 textSize)
{
	if ((text==NULL) || (textSize<2))
		return false;
	text[textSize-1] = 0;
	int len = SNPRINTF(text,textSize-1,"%u",value);
	return ((len>0) && (len<(int)textSize));
}
bool GApp::Utils::String::UInt16ToString(UInt16 value,CHARACTER *text,UInt32 textSize)
{
	if ((text==NULL) || (textSize<2))
		return false;
	text[textSize-1] = 0;
	int len = SNPRINTF(text,textSize-1,"%u",value);
	return ((len>0) && (len<(int)textSize));
}
bool GApp::Utils::String::UInt32ToString(UInt32 value,CHARACTER *text,UInt32 textSize)
{
	if ((text==NULL) || (textSize<2))
		return false;
	text[textSize-1] = 0;
	int len = SNPRINTF(text,textSize-1,"%u",value);
	return ((len>0) && (len<(int)textSize));
}
bool GApp::Utils::String::UInt64ToString(UInt64 value,CHARACTER *text,UInt32 textSize)
{
	if ((text==NULL) || (textSize<2))
		return false;
	text[textSize-1] = 0;
	int len = SNPRINTF(text,textSize-1,"%llu",value);
	return ((len>0) && (len<(int)textSize));
}
bool GApp::Utils::String::Int8ToString(Int8 value,CHARACTER *text,UInt32 textSize)
{
	if ((text==NULL) || (textSize<2))
		return false;
	text[textSize-1] = 0;
	int len = SNPRINTF(text,textSize-1,"%d",value);
	return ((len>0) && (len<(int)textSize));
}
bool GApp::Utils::String::Int16ToString(Int16 value,CHARACTER *text,UInt32 textSize)
{
	if ((text==NULL) || (textSize<2))
		return false;
	text[textSize-1] = 0;
	int len = SNPRINTF(text,textSize-1,"%d",value);
	return ((len>0) && (len<(int)textSize));
}
bool GApp::Utils::String::Int32ToString(Int32 value,CHARACTER *text,UInt32 textSize)
{
	if ((text==NULL) || (textSize<2))
		return false;
	text[textSize-1] = 0;
	int len = SNPRINTF(text,textSize-1,"%d",value);
	return ((len>0) && (len<(int)textSize));
}
bool GApp::Utils::String::Int64ToString(Int64 value,CHARACTER *text,UInt32 textSize)
{
	if ((text==NULL) || (textSize<2))
		return false;
	text[textSize-1] = 0;
	int len = SNPRINTF(text,textSize-1,"%lld",value);
	return ((len>0) && (len<(int)textSize));
}
bool GApp::Utils::String::BoolToString(bool value,CHARACTER *text,UInt32 textSize)
{
	if (value)
		return Set(text,"true",textSize);
	else
		return Set(text,"false",textSize);
}
bool GApp::Utils::String::CharToString(char value,CHARACTER *text,UInt32 textSize)
{
	if ((text==NULL) || (textSize<2))
		return false;
	text[textSize-1] = 0;
	int len = SNPRINTF(text,textSize-1,"%c",value);
	return ((len>0) && (len<(int)textSize));
}
bool GApp::Utils::String::Float32ToString(Float32 value,CHARACTER *text,UInt32 textSize)
{
	if ((text==NULL) || (textSize<2))
		return false;
	text[textSize-1] = 0;
	int len = SNPRINTF(text,textSize-1,"%f",value);
	return ((len>0) && (len<(int)textSize));
}
bool GApp::Utils::String::Float64ToString(Float64 value,CHARACTER *text,UInt32 textSize)
{
	if ((text==NULL) || (textSize<2))
		return false;
	text[textSize-1] = 0;
	int len = SNPRINTF(text,textSize-1,"%lf",value);
	return ((len>0) && (len<(int)textSize));
}
int GApp::Utils::String::GetChar(const CHARACTER *text,int index,int len, int defaultValue)
{
	if (text==NULL)
		return defaultValue;
	if (len<=0)
		return defaultValue;
	if (index>=0)
	{
		if (index<len)
			return text[index];
		return defaultValue;
	}
	len+=index;
	if (len<0)
		return defaultValue;
	return text[len];
}
//--------------------------------------------------- CONSTRUCTORI OBIECT ----------------------------------------------------------------
GApp::Utils::String::String(void)
{
	Text=NULL;
	Size=MaxSize=0;
	Flags = 0;
}
GApp::Utils::String::String(const CHARACTER *text,bool createConstantString)
{
	Text = NULL;
	Size=MaxSize=0;
	Flags = 0;
	if (createConstantString)
	{
		Create(text);
	} else {
	if ((text!=NULL) && (text[0]!=0))
		Set(text);
	}
}
GApp::Utils::String::String(const GApp::Utils::String &s)
{
	Text=NULL;
	Size=MaxSize=0;
	Flags = 0;
	if (Create(s.Size+32))
	{
		Set(s.Text,s.Size);
	}
}
GApp::Utils::String::~String(void)
{
	Distroy();
}
void GApp::Utils::String::Distroy()
{
	if ((Text!=NULL) && (Flags==0)) 
	{
		delete Text;
	}
	Text=NULL;
	Size=MaxSize=0;
	Flags=0;
}

bool GApp::Utils::String::Create(int Size)
{
	if (Size<1)
		return false;
	Distroy();
	return Grow(Size);
}
bool GApp::Utils::String::Create(const CHARACTER* text)
{
	if (text==NULL)
		return false;
	Distroy();
	MaxSize = Size = Len(text);
	Flags = STRING_FLAG_CONSTANT;
	Text = (char*)text;
	return true;
}
bool GApp::Utils::String::Create(CHARACTER* buffer,int bufferSize,bool emptyString)
{
	if ((buffer==NULL) || (bufferSize<1))
		return false;
	Distroy();
	Text = buffer;
	MaxSize = bufferSize;
	if (emptyString)
	{
		Size = 0;
		Text[Size] = 0;
	} else {
		for (Size=0;(Size<bufferSize-1) && (Text[Size]!=0);Size++);
		Text[Size] = 0;
	}	
	Flags = STRING_FLAG_STATIC_BUFFER;
	return true;
}


bool GApp::Utils::String::Clear()
{
	if (Flags == STRING_FLAG_CONSTANT)
		return false;
	if (Text!=NULL)
	{
		Text[0] = 0;
		Size = 0;
		return true;
	}	
	return false;
}
bool GApp::Utils::String::Realloc(int newSize)
{
	if (newSize<=MaxSize)
		return true;
	return Grow(newSize);
}
bool GApp::Utils::String::Grow(int newSize)
{
	CHARACTER	*temp;
	if ((Flags & STRING_FLAG_CONSTANT)!=0) 
		return false;
	if (newSize<MaxSize) 
		return true;
	if ((Flags & STRING_FLAG_STATIC_BUFFER)!=0) // nu pot mari acel buffer
		return false;
	
	
	temp=Text;
	if ((Text=new CHARACTER[newSize])==NULL)
	{
		Text = temp;
		return false;
	}
	if (temp) 
	{ 
		MEMCOPY(Text,temp,Size*sizeof(CHARACTER));
		Text[Size]=0;
		delete temp; 
	} else { 
		Size=0;
		Text[0]=0; 
	}
	MaxSize=newSize;
	return true;
}
//--------------------------------------------------- FUNCTII OBIECT  ---------------------------------------------------------------------
bool GApp::Utils::String::Add(const CHARACTER *ss,int txSize)
{
	GSTRING_TEST_AND_INIT(false);	
	EXIT_IF_CONSTANT(false);

	if (txSize<0)
		txSize = Len(ss);
	GSTRING_GROW_WITH(txSize,false);
	if (GApp::Utils::String::Add(Text,ss,MaxSize,Size,txSize))
	{
		Size+=txSize;
		return true;
	}
	return false;
}
bool GApp::Utils::String::AddChar(CHARACTER ch)
{
	CHARACTER temp[2];
	temp[0]=ch;
	temp[1]=0;
	return Add(temp);
}
bool GApp::Utils::String::AddChars(CHARACTER ch,int count)
{
	GSTRING_TEST_AND_INIT(false);
	return InsertChars(ch,Size,count);
}
bool GApp::Utils::String::Add(GApp::Utils::String *ss,int txSize)
{
	GSTRING_TEST_AND_INIT(false);
	EXIT_IF_CONSTANT(false);
	CHECK(ss!=NULL,false,"");
	if (txSize<0)
		txSize = ss->GetSize();
	
	GSTRING_GROW_WITH(txSize,false);
	if (GApp::Utils::String::Add(Text,ss->GetText(),MaxSize,Size,txSize))
	{
		Size+=txSize;
		return true;
	}
	return false;	
}
bool GApp::Utils::String::Set(const CHARACTER *ss,int txSize)
{
	GSTRING_TEST_AND_INIT(false);
	EXIT_IF_CONSTANT(false);
	Text[0]=0;
	Size=0;
	return Add(ss,txSize);
}
bool GApp::Utils::String::Set(GApp::Utils::String *ss,int txSize)
{
	GSTRING_TEST_AND_INIT(false);
	EXIT_IF_CONSTANT(false);
	Text[0]=0;
	Size=0;
	return Add(ss,txSize);
}

int  GApp::Utils::String::GetChar(int index)
{
	if (Text==NULL)
		return 0;
	if ((index>=0) && (index<Size))
		return Text[index];
	if ((index<0) && (index>=(-Size)))
		return Text[Size+index];
	return 0;
}
bool GApp::Utils::String::SetChar(int index,CHARACTER value)
{
	if (Text==NULL)
		return false;
	EXIT_IF_CONSTANT(false);
	if ((index>=0) && (index<Size))
	{
		Text[index] = value;
		return true;
	}
	if ((index<0) && (index>=(-Size))) 
	{
		Text[Size+index] = value;
		return true;
	}
	return false;
}
bool GApp::Utils::String::SetFormat(const CHARACTER *format, ...)
{
    va_list args;
    int     len;

	GSTRING_TEST_AND_INIT(false);
	EXIT_IF_CONSTANT(false);
    va_start( args, format );
    len = vsnprintf(NULL, 0, format, args);
    va_end(args); 
	if (len<0)
		return false;
	GSTRING_GROW_WITH(len+1,false);
	va_start( args, format );
	len = vsnprintf( Text, len+1, format, args );
	va_end (args); 
    if (len<0)
		return false;
	Text[len]=0;
	Size = len;
	
	return true;
}
CHARACTER* GApp::Utils::String::Format(const CHARACTER *format, ...)
{
    va_list args;
    int     len;

	GSTRING_TEST_AND_INIT(NULL);
	EXIT_IF_CONSTANT(NULL);
    va_start( args, format );
	if ((len = vsnprintf(NULL, 0, format, args ))<0)
		return NULL;
	va_end (args);
	
	GSTRING_GROW_WITH(len+1,NULL);
	va_start( args, format );
    if ((len = vsnprintf( Text, len+1, format, args ))<0)
		return NULL;
	va_end (args);
	
	Text[len]=0;
	Size = len;
	
	return Text;
}


bool GApp::Utils::String::Truncate(int newText)
{
	EXIT_IF_CONSTANT(false);
	if ((newText<=Size) && (newText>=0)) 
	{
		Size=newText;
		Text[Size]=0;
		return true;
	}
	return false;
}
int  GApp::Utils::String::UpdateSize()
{
	EXIT_IF_CONSTANT(-1);
	if ((Text==NULL) || (MaxSize<1))
		return -1;
	Size=0;
	while (Size<MaxSize)
	{
		if (Text[Size]!=0)
			break;
		Size++;
	}
	if (Size==MaxSize)
		Size--;
	Text[Size]=0;
	return Size;
}
int  GApp::Utils::String::Compare(const CHARACTER *ss,bool ignoreCase) const
{
	return Compare(Text,ss,ignoreCase);
}
int  GApp::Utils::String::Compare(const GApp::Utils::String &ss,bool ignoreCase) const
{
	//CHECK(ss!=NULL,-1,"");
	return Compare(Text,ss.GetText(),ignoreCase);
}
int  GApp::Utils::String::Compare(const GApp::Utils::String *ss,bool ignoreCase) const
{
	CHECK(ss!=NULL,-1,"");
	return Compare(Text,ss->GetText(),ignoreCase);
}


bool GApp::Utils::String::StartsWith(const CHARACTER *ss,bool ignoreCase)
{
	return StartsWith(Text,ss,ignoreCase);
}
bool GApp::Utils::String::StartsWith(GApp::Utils::String *ss,bool ignoreCase)
{
	CHECK(ss!=NULL,false,"");
	return StartsWith(Text,ss->GetText(),ignoreCase);
}
bool GApp::Utils::String::EndsWith(const CHARACTER *ss,bool ignoreCase)
{
	return EndsWith(Text,ss,ignoreCase,Size);
}
bool GApp::Utils::String::EndsWith(GApp::Utils::String *ss,bool ignoreCase)
{
	CHECK(ss!=NULL,false,"");
	return EndsWith(Text,ss->GetText(),ignoreCase,Size,ss->GetSize());
}
bool GApp::Utils::String::Contains(const CHARACTER *ss,bool ignoreCase)
{
	return (bool)(Find(ss,ignoreCase)!=-1);
}
bool GApp::Utils::String::Contains(GApp::Utils::String *ss,bool ignoreCase)
{
	return (bool)(Find(ss,ignoreCase)!=-1);
}

int	 GApp::Utils::String::Find(const CHARACTER *ss,bool ignoreCase,int startPoz,bool searchForward)
{
	return Find(Text,ss,ignoreCase,startPoz,searchForward,Size);
}
int	 GApp::Utils::String::Find(GApp::Utils::String *ss,bool ignoreCase,int startPoz,bool searchForward)
{
	if (ss==NULL)
		return -1;
	return Find(ss->GetText(),ignoreCase,startPoz,searchForward);
}
int  GApp::Utils::String::FindLast(const CHARACTER *ss, bool ignoreCase)
{
	if (Size<1)
		return -1;
	return Find(Text,ss,ignoreCase,Size-1,false,-1);
}
int  GApp::Utils::String::FindLast(GApp::Utils::String *ss,bool ignoreCase)
{
	if ((Size<1) || (ss==NULL) || (ss->GetText()==NULL) || (ss->GetSize()<1))
		return -1;
	return Find(Text,ss->GetText(),ignoreCase,Size-1,false,ss->GetSize());
}
bool GApp::Utils::String::Equals(const CHARACTER *ss,bool ignoreCase)
{
	return (Compare(ss,ignoreCase)==0);
}
bool GApp::Utils::String::Equals(GApp::Utils::String *ss,bool ignoreCase)
{
	return (Compare(ss,ignoreCase)==0);
}
bool GApp::Utils::String::Delete(int start,int end)
{
	EXIT_IF_CONSTANT(false);
	if (Delete(Text,start,end))
	{
		Size-=(end-start);
		return true;
	}
	return false;
}
bool GApp::Utils::String::DeleteChar(int pos)
{
	return Delete(pos,pos+1);
}
bool GApp::Utils::String::Insert(const CHARACTER *ss,int pos)
{
	int sz = Len(ss);
	
	GSTRING_TEST_AND_INIT(false);
	EXIT_IF_CONSTANT(false);
	GSTRING_GROW_WITH(sz,false);

	if (Insert(Text,ss,pos,MaxSize,Size,sz)==true)
	{
		Size+=sz;
		return true;
	}
	return false;
}
bool GApp::Utils::String::InsertChar(CHARACTER ch,int pos)
{
	CHARACTER t[2];
	t[0]=ch;
	t[1]=0;

	return Insert(t,pos);
}
bool GApp::Utils::String::InsertChars(CHARACTER ch,int pos,int count)
{
	if (count<1)
		return false;

	GSTRING_TEST_AND_INIT(false);
	EXIT_IF_CONSTANT(false);
	GSTRING_GROW_WITH(count+1,false);
	MEMMOVE(&Text[pos+count],&Text[pos],(Size-pos)*sizeof(CHARACTER));
	for (int tr=0;tr<count;tr++,pos++)
		Text[pos]=ch;	
	Size+=count;
	Text[Size]=0;
	return true;
}
bool GApp::Utils::String::Insert(GApp::Utils::String *ss,int pos)
{
	GSTRING_TEST_AND_INIT(false);
	EXIT_IF_CONSTANT(false);
	CHECK(ss!=NULL,false,"");
	GSTRING_GROW_WITH(ss->GetSize(),false);

	if (Insert(Text,ss->GetText(),pos,MaxSize,Size,ss->GetSize())==true)
	{
		Size+=ss->GetSize();
		return true;
	}
	return false;
	
}
bool GApp::Utils::String::ReplaceOnPos(int start,int end,const CHARACTER *ss,int szSS)
{	
	if (Text==NULL)
		return false;
	EXIT_IF_CONSTANT(false);
	if (szSS<0)
		szSS=Len(ss);

	GSTRING_TEST_AND_INIT(false);
	GSTRING_GROW_WITH(szSS,false);

	if (ReplaceOnPos(Text,ss,start,end,MaxSize,Size,szSS)==true)
	{
		Size += szSS;
		Size -= (end-start);
		return true;
	}
	return false;
}
bool GApp::Utils::String::ReplaceOnPos(int start,int end,GApp::Utils::String *ss)
{
	if ((Text==NULL)  || (ss==NULL))
		return false;
	EXIT_IF_CONSTANT(false);
	GSTRING_TEST_AND_INIT(false);
	GSTRING_GROW_WITH(ss->GetSize(),false);

	return ReplaceOnPos(start,end,ss->GetText(),ss->GetSize());
}

bool GApp::Utils::String::Replace(const CHARACTER *pattern, const CHARACTER *newText, bool ignoreCase, int patternSize, int textSize)
{
	int pos;

	if ((Text==NULL) || (pattern==NULL) || (newText==NULL))
		return false;

	GSTRING_TEST_AND_INIT(false);	
	EXIT_IF_CONSTANT(false);	
	if (patternSize<0)
		patternSize = Len(pattern);
	if (textSize<0)
		textSize = Len(newText);

	pos = 0;
	do
	{
		pos = Find(Text,pattern,ignoreCase,pos,true,Size);
		if (pos>=0)
		{
			if (ReplaceOnPos(pos,pos+patternSize,newText,textSize)==false)
				return false;
			pos+=textSize;
		}
	} while (pos>=0);
	return true;
}
bool GApp::Utils::String::Replace(GApp::Utils::String *pattern,GApp::Utils::String *newText,bool ignoreCase)
{
	if ((Text==NULL) || (pattern==NULL) || (newText==NULL))
		return false;
	EXIT_IF_CONSTANT(false);
	return Replace(pattern->GetText(),newText->GetText(),ignoreCase,pattern->GetSize(),newText->GetSize());
}
bool GApp::Utils::String::Split(const CHARACTER *separator, GApp::Utils::String *arrayList, int arrayListCount, int *elements,int separatorSize)
{
	int pos,start,count;
	if ((Text==NULL) || (separator==NULL) || (arrayListCount<0))
		return false;
	if ((arrayList!=NULL) && (arrayListCount<1))
		return false;
	if (separatorSize==-1)
		separatorSize = Len(separator);

	start = 0;
	count = 0;
	do
	{
		pos = Find(Text,separator,false,start);
		if (pos>=0)
		{
			if (arrayList!=NULL)
			{
				if (count>=arrayListCount)
				{
					if (elements)
						*elements = count;
					return true;
				}
				if (arrayList[count].Set(&Text[start],pos-start)==false)
					return false;
			}
			count++;
			start = pos+separatorSize;
		}
	} while (pos>=0);
	if ((start>=0) && (start<Size))
	{
		if (arrayList!=NULL)
		{
			if (count>=arrayListCount)
			{
				if (elements)
					*elements = count;
				return true;
			}
			if (arrayList[count].Set(&Text[start],Size-start)==false)
				return false;
		}
		count++;
	}
	if (elements)
		*elements = count;
	return true;
}
bool GApp::Utils::String::Split(GApp::Utils::String *separator,GApp::Utils::String *arrayList,int arrayListCount,int *elements)
{
	if (separator==NULL)
		return false;
	return Split(separator->GetText(),arrayList,arrayListCount,elements,separator->GetSize());
}
bool GApp::Utils::String::SplitInTwo(const CHARACTER *separator,GApp::Utils::String *leftPart,GApp::Utils::String *rightPart,int separatorIndex,bool ignoreCase)
{
	int		poz = -1;
	if (separator==NULL)
		return false;
	if (separatorIndex>0)
	{
		poz = -1;
		do
		{
			poz = Find(separator,ignoreCase,poz+1,true);
			separatorIndex--;
		} while ((separatorIndex>0) && (poz!=-1));
	} else {
		poz = Size;
		do
		{
			poz = Find(separator,ignoreCase,poz-1,false);
			separatorIndex++;
		} while ((separatorIndex<0) && (poz!=-1));
	}
	if (poz<0)
		return false;
	if (leftPart!=NULL)
	{
		if (leftPart->Set(Text,poz)==false)
			return false;
	}
	if (rightPart!=NULL)
	{
		if (rightPart->Set(&Text[poz+Len(separator)])==false)
			return false;
	}
	return true;
}
bool GApp::Utils::String::SplitInThree(const CHARACTER *separatorLeft,const CHARACTER *separatorRight,GApp::Utils::String *leftPart,GApp::Utils::String *middlePart,GApp::Utils::String *rightPart,int separatorLeftIndex,int separatorRightIndex,bool ignoreCase)
{
	int pozLeft,pozRight;
	if ((separatorLeft==NULL) || (separatorRight==NULL))
		return false;
	// caut partea din stanga
	if (separatorLeftIndex>0)
	{
		pozLeft = -1;
		do
		{
			pozLeft = Find(separatorLeft,ignoreCase,pozLeft+1,true);
			separatorLeftIndex--;
		} while ((separatorLeftIndex>0) && (pozLeft!=-1));
	} else {
		pozLeft = Size;
		do
		{
			pozLeft = Find(separatorLeft,ignoreCase,pozLeft-1,false);
			separatorLeftIndex++;
		} while ((separatorLeftIndex<0) && (pozLeft!=-1));
	}
	if (pozLeft<0)
		return false;
	// caut pe partea dreapta
	if (separatorRightIndex>0)
	{
		pozRight = pozLeft;
		do
		{
			pozRight = Find(separatorRight,ignoreCase,pozRight+1,true);
			separatorRightIndex--;
		} while ((separatorRightIndex>0) && (pozRight!=-1));
	} else {
		pozRight = Size;
		do
		{
			pozRight = Find(separatorRight,ignoreCase,pozRight-1,false);
			separatorRightIndex++;
		} while ((separatorRightIndex<0) && (pozRight!=-1));
	}
	if (pozRight<0)
		return false;
	int pozMiddle = pozLeft+Len(separatorLeft);
	if (pozRight<pozMiddle)
		return false;
	if (leftPart!=NULL)
	{
		if (leftPart->Set(Text,pozLeft)==false)
			return false;
	}
	if (middlePart!=NULL)
	{		
		if (middlePart->Set(&Text[pozMiddle],pozRight-pozMiddle)==false)
			return false;
	}
	if (rightPart!=NULL)
	{
		if (rightPart->Set(&Text[pozRight+Len(separatorRight)])==false)
			return false;
	}
	return true;
}
bool GApp::Utils::String::SplitInTwo(GApp::Utils::String *separator,GApp::Utils::String *leftPart,GApp::Utils::String *rightPart,int separatorIndex,bool ignoreCase)
{
	if (separator==NULL)
		return false;
	return SplitInTwo(separator->GetText(),leftPart,rightPart,separatorIndex,ignoreCase);
}

bool GApp::Utils::String::SplitFromLeft(const CHARACTER *separator,GApp::Utils::String *leftPart,GApp::Utils::String *rightPart,bool ignoreCase)
{
	int		poz,sz;
	if (separator==NULL)
		return false;
	if ((poz = Find(separator,ignoreCase,0,true))<0)
		return false;
	if (leftPart!=NULL)
	{
		if (leftPart->Set(Text,poz)==false)
			return false;
	}
	if (rightPart!=NULL)
	{
		sz = Len(separator);	
		if (rightPart->Set(&Text[sz+poz])==false)
			return false;
	}
	return true;
}
bool GApp::Utils::String::SplitFromLeft(GApp::Utils::String *separator,GApp::Utils::String *leftPart,GApp::Utils::String *rightPart,bool ignoreCase)
{
	if (separator==NULL)
		return false;
	return SplitFromLeft((const CHARACTER *)separator->GetText(),leftPart,rightPart,ignoreCase);
}
bool GApp::Utils::String::SplitFromRight(const CHARACTER *separator,GApp::Utils::String *leftPart,GApp::Utils::String *rightPart,bool ignoreCase)
{
	int		poz,sz;
	if (separator==NULL)
		return false;
	if ((poz = Find(separator,ignoreCase,Size,false))<0)
		return false;
	if (leftPart!=NULL)
	{
		if (leftPart->Set(Text,poz)==false)
			return false;
	}
	if (rightPart!=NULL)
	{
		sz = Len(separator);
		if (rightPart->Set(&Text[sz+poz])==false)
			return false;
	}
	return true;
}
bool GApp::Utils::String::SplitFromRight(GApp::Utils::String *separator,GApp::Utils::String *leftPart,GApp::Utils::String *rightPart,bool ignoreCase)
{
	if (separator==NULL)
		return false;
	return SplitFromRight((const CHARACTER *)separator->GetText(),leftPart,rightPart,ignoreCase);
}


bool GApp::Utils::String::Strip(const CHARACTER *charList,bool stripFromLeft,bool stripFromRight)
{
	int		tr,gr;
	bool	found;

	if (charList==NULL)
		charList = TXT(" \t\n\r");
	if (Text==NULL)
		return true;
	EXIT_IF_CONSTANT(false);
	if (stripFromLeft)
	{
		for (tr=0;tr<Size;tr++)
		{
			found = false;
			for (gr=0;(charList[gr]!=0) && (found==false);gr++)
				if (Text[tr]==charList[gr])
					found = true;
			if (!found)
				break;
		}
		if (Delete(0,tr)==false)
			return false;
	}

	if (stripFromRight)
	{
		for (tr=Size-1;tr>=0;tr--)
		{
			found = false;
			for (gr=0;(charList[gr]!=0) && (found==false);gr++)
				if (Text[tr]==charList[gr])
					found = true;
			if (!found)
				break;
		}
		tr++;
		if (tr<0)
			tr=0;
		Size = tr;
		Text[Size]=0;
	}
	return true;
}
bool GApp::Utils::String::CopyNextLine(GApp::Utils::String *line,int &position)
{
	int start;
	//if (/*(position==NULL) ||*/ (line==NULL))
	if (line==NULL)
		return false;

	line->Set(TXT(""));
	start = (position);
	
	if (((position)>=Size) || ((position)<0))
		return false;
	
	while ((position)<Size)
	{
		if ((Text[(position)]==13) || (Text[(position)]==10))
		{
			if (line->Set(&Text[start],(position)-start)==false)
				return false;
			(position)++;
			if ((((position)<Size)) && 
				((Text[(position)]==13) || (Text[(position)]==10)) &&
				(Text[(position)]!=Text[(position)-1]))
			{
				(position)++;
			}
			return true;
		}
		(position)++;
	}
	if (line->Set(&Text[start],(position)-start)==false)
		return false;
	return true;
}
bool GApp::Utils::String::CopyNext(GApp::Utils::String *token,const CHARACTER *separator,int &position,bool ignoreCase)
{
	int start;

	if ((separator==NULL) || (token==NULL))
		return false;
	
	token->Set(TXT(""));
	start = (position);
	
	if (((position)>=Size) || ((position)<0))
		return false;

	(position) = Find(separator,ignoreCase,start);

	if ((position)<0)
	{
		if (token->Set(&Text[start],Size-start)==false)
			return false;
		(position)=Size;
		return true;
	} else {
		if (token->Set(&Text[start],(position)-start)==false)
			return false;
		(position)+=Len(separator);
		return true;
	}
}
bool GApp::Utils::String::CopyFromLeft(GApp::Utils::String *text,unsigned int nrChars)
{
	if (nrChars>(unsigned int)Size)
		nrChars = Size;
	if (text==NULL)
		return false;
	return text->Set(Text,nrChars);
}
bool GApp::Utils::String::CopyFromRight(GApp::Utils::String *text,unsigned int nrChars)
{
	if (nrChars>(unsigned int)Size)
		nrChars = Size;
	if (text==NULL)
		return false;
	return text->Set(&Text[Size-nrChars]);
}
bool GApp::Utils::String::CopySubString(GApp::Utils::String *text,unsigned int start,unsigned int size)
{
	if ((start>(unsigned int)Size) || (start+size>(unsigned int)Size) || (text==NULL))
		return false;
	return text->Set(&Text[start],size);
}
bool GApp::Utils::String::CopyPathName(GApp::Utils::String *path)
{
	if (path==NULL)
		return false;
	int index = FindLast(TXT("\\"));
	if (index==-1)
		return path->Set(TXT(""));
	else
		return path->Set(Text,index);
}
bool GApp::Utils::String::CopyFileName(GApp::Utils::String *path)
{
	if (path==NULL)
		return false;
	int index = FindLast(TXT("\\"));
	if (index==-1)
		return path->Set(TXT(""));
	else
		return path->Set(&Text[index+1]);
}
bool GApp::Utils::String::PathJoinName(const CHARACTER *name,CHARACTER separator)
{
	CHARACTER	txt[2]={separator,0};
	
	if (EndsWith(txt))
	{
		return Add(name);
	} else {
		if (Add(txt)==false)
			return false;
		return Add(name);
	}
}
bool GApp::Utils::String::PathJoinName(GApp::Utils::String *name,CHARACTER separator)
{
	if (name==NULL)
		return false;
	return PathJoinName(name->GetText(),separator);
}
bool GApp::Utils::String::AddFormatedEx(const CHARACTER *format, ...)
{
    va_list				args;
    const CHARACTER*	start;
	DataConvertInfo		dci;
	DataValueUnion		val;
	CHARACTER			temp[MAX_FORMATED_EX_TEMP_BUF];
	const CHARACTER			*toAdd;
	int					toAddSize;
	int					extraAdd;
	unsigned int		tr;
	int 				tempInt;


	GSTRING_TEST_AND_INIT(false);
	EXIT_IF_CONSTANT(false);
    va_start( args, format );
	
	start = format;
	while (true)
	{		
		while (((*format)!=0) && ((*format)!='%'))
			format++;
		// adaug ce am gasit pana acuma
		if (Add(start,(int)(format-start))==false)
			return false;
		if ((*format)==0)
			return true;
		if (((*format)=='%') && (format[1]=='{'))
		{
			//procesez datele %{...}
			format+=2;
			ResetDataConvertInfo(&dci);			
			while (((*format)!=0) && ((*format)!='}'))
			{
				start = format;
				while (((*format)!=0) && ((*format)!=',') && ((*format)!='}'))
					format++;
				if (UpdateDataConvertInfo(&dci,start,(int)(format-start))==false)
					return false;
				if ((*format)==',')
					format++;
			}
			// format invalid
			if ((*format)==0)
				return false;
			// sunt pe o acolada
			format++;
			start = format;
			// citesc valoarea
			switch (dci.DataType)
			{
				case DataConvertInfo::TYPE_Int8:
					val.vInt8 = va_arg(args,int);
					break;
				case DataConvertInfo::TYPE_Int16:
					val.vInt16 = va_arg(args,int);
					break;
				case DataConvertInfo::TYPE_Int32:
					val.vInt32 = va_arg(args,int);
					break;
				case DataConvertInfo::TYPE_Int64:
					val.vInt64 = va_arg(args,Int64);
					break;

				case DataConvertInfo::TYPE_UInt8:
					val.vUInt8 = va_arg(args,int);
					break;
				case DataConvertInfo::TYPE_UInt16:
					val.vUInt16 = va_arg(args,int);
					break;
				case DataConvertInfo::TYPE_UInt32:
					val.vUInt32 = va_arg(args,unsigned int);
					break;
				case DataConvertInfo::TYPE_UInt64:
					val.vUInt64 = va_arg(args,UInt64);
					break;

				case DataConvertInfo::TYPE_Char:
					val.vChar = va_arg(args,int);
					break;
				case DataConvertInfo::TYPE_WChar:
					val.vWChar = va_arg(args,int);
					break;

				case DataConvertInfo::TYPE_Bool:
					tempInt = va_arg(args,int);
					val.vBool = (bool)(tempInt);
					break;

				case DataConvertInfo::TYPE_Ascii:
					val.vAscii = va_arg(args,char*);
					break;
				case DataConvertInfo::TYPE_Unicode:
					//val.vUnicode = va_arg(args,short*);
					return false;
					break;

				case DataConvertInfo::TYPE_Float:
					val.vFloat= (float)(va_arg(args,double));
					break;
				case DataConvertInfo::TYPE_Double:
					val.vDouble= va_arg(args,double);
					break;

				default:
					return false;
			};
			// citesc si valorile externe
			for (tr=0;tr<dci.ExternalValuesCount;tr++)
			{
				switch (dci.ExternalValue[tr])
				{
					case DataConvertInfo::EXTVALUE_Base:
						dci.Base = va_arg(args,unsigned int);
						if ((dci.Base<2) ||  (dci.Base>36))
							return false;
						break;
					case DataConvertInfo::EXTVALUE_AlignSize:
						dci.AlignSize = va_arg(args,unsigned int);
						break;
					case DataConvertInfo::EXTVALUE_FillChar:
						dci.FillChar = va_arg(args,int);
						if (dci.FillChar == 0)
							return false;
						break;
					case DataConvertInfo::EXTVALUE_GROUP:
						dci.Group = va_arg(args,unsigned int);
						if (dci.Group==0)
							return false;
						break;
					case DataConvertInfo::EXTVALUE_Zecimals:
						dci.Zecimals = va_arg(args,unsigned int);
						break;
					default:
						return false;
				};
			}
			// adaug conversia
			switch (dci.DataType)
			{
				case DataConvertInfo::TYPE_Int8:
					toAdd = ConvertIntegerNumberToString(&dci,(Int64)val.vInt8,temp,MAX_FORMATED_EX_TEMP_BUF,&toAddSize);
					break;
				case DataConvertInfo::TYPE_Int16:
					toAdd = ConvertIntegerNumberToString(&dci,(Int64)val.vInt16,temp,MAX_FORMATED_EX_TEMP_BUF,&toAddSize);
					break;
				case DataConvertInfo::TYPE_Int32:
					toAdd = ConvertIntegerNumberToString(&dci,(Int64)val.vInt32,temp,MAX_FORMATED_EX_TEMP_BUF,&toAddSize);
					break;
				case DataConvertInfo::TYPE_Int64:
					toAdd = ConvertIntegerNumberToString(&dci,(Int64)val.vInt64,temp,MAX_FORMATED_EX_TEMP_BUF,&toAddSize);
					break;

				case DataConvertInfo::TYPE_UInt8:
					toAdd = ConvertUIntegerNumberToString(&dci,(UInt64)val.vUInt8,temp,MAX_FORMATED_EX_TEMP_BUF,&toAddSize);
					break;
				case DataConvertInfo::TYPE_UInt16:
					toAdd = ConvertUIntegerNumberToString(&dci,(UInt64)val.vUInt16,temp,MAX_FORMATED_EX_TEMP_BUF,&toAddSize);
					break;
				case DataConvertInfo::TYPE_UInt32:
					toAdd = ConvertUIntegerNumberToString(&dci,(UInt64)val.vUInt32,temp,MAX_FORMATED_EX_TEMP_BUF,&toAddSize);
					break;
				case DataConvertInfo::TYPE_UInt64:
					toAdd = ConvertUIntegerNumberToString(&dci,(UInt64)val.vUInt64,temp,MAX_FORMATED_EX_TEMP_BUF,&toAddSize);
					break;
				case DataConvertInfo::TYPE_Char:
					toAdd = temp;
					temp[0] = val.vChar;
					toAddSize = 1;
					break;
				case DataConvertInfo::TYPE_WChar:
					toAdd = temp;
					temp[0] = (CHARACTER)val.vWChar;
					toAddSize = 1;
					break;
				case DataConvertInfo::TYPE_Bool:
					if (val.vBool)
					{
						toAdd = TXT("True");
						toAddSize = 4;
					} else {
						toAdd = TXT("False");
						toAddSize = 5;
					}
					break;
				case DataConvertInfo::TYPE_Unicode:
					toAdd = val.vUnicode;
					toAddSize = Len(toAdd);
					break;
				case DataConvertInfo::TYPE_Ascii:
					toAdd = val.vAscii;
					toAddSize = Len(toAdd);
					break;					
					/*
				case DataConvertInfo::TYPE_Float:
					toAdd = ConvertDoubleNumberToString(&dci,(double)val.vFloat,temp,MAX_FORMATED_EX_TEMP_BUF,&toAddSize);
					break;
				case DataConvertInfo::TYPE_Double:
					toAdd = ConvertDoubleNumberToString(&dci,(double)val.vDouble,temp,MAX_FORMATED_EX_TEMP_BUF,&toAddSize);
					break;
					*/
				default:
					return false;
			};
			// citire align si align size
			if (dci.AlignSize==0) // fara aliniere
			{
				extraAdd = 0;
			} else {
				extraAdd = dci.AlignSize - toAddSize;
				if ((extraAdd<=0) && (dci.Flags & DataConvertInfo::FLAG_TRUNCATE))
				{
					extraAdd = 0;
					toAddSize = dci.AlignSize;
				}
			}
			switch (dci.Align)
			{
				case DataConvertInfo::ALIGN_LEFT:
					if (Add(toAdd,toAddSize)==false)
						return false;
					if (extraAdd>0)
					{
						if (AddChars(dci.FillChar,extraAdd)==false)
							return false;
					}
					break;
				case DataConvertInfo::ALIGN_RIGHT:
					if (extraAdd>0)
					{
						if (AddChars(dci.FillChar,extraAdd)==false)
							return false;
					}
					if (Add(toAdd,toAddSize)==false)
						return false;
					break;
				case DataConvertInfo::ALIGN_CENTER:
					if ((extraAdd/2)>0)
					{
						if (AddChars(dci.FillChar,extraAdd/2)==false)
							return false;
					}
					if (Add(toAdd,toAddSize)==false)
						return false;
					if ((extraAdd - extraAdd/2)>0)
					{
						if (AddChars(dci.FillChar,extraAdd - extraAdd/2)==false)
							return false;
					}
					break;
			};
		} else {
			// trec la urmatorul element
			start = format;
			format++;
		}
	}
	return true;
}
bool GApp::Utils::String::MatchSimple(const CHARACTER *mask,bool ignoreCase)
{
	if ((Text==NULL) || (mask==NULL))
		return false;
	return Simple_Match(Text,mask,ignoreCase,false);
}
bool GApp::Utils::String::MatchSimple(GApp::Utils::String *mask,bool ignoreCase)
{
	if (mask==NULL)
		return false;
	return MatchSimple(mask->GetText(),ignoreCase);
}
bool GApp::Utils::String::ConvertToInt8(Int8 &value,unsigned int base)
{
	return ConvertToInt8(Text,value,base,Size);
}
bool GApp::Utils::String::ConvertToInt16(Int16 &value,unsigned int base)
{
	return ConvertToInt16(Text,value,base,Size);
}
bool GApp::Utils::String::ConvertToInt32(Int32 &value,unsigned int base)
{
	return ConvertToInt32(Text,value,base,Size);
}
bool GApp::Utils::String::ConvertToInt64(Int64 &value,unsigned int base)
{
	return ConvertToInt64(Text,value,base,Size);
}
bool GApp::Utils::String::ConvertToUInt8(UInt8 &value,unsigned int base)
{
	return ConvertToUInt8(Text,value,base,Size);
}
bool GApp::Utils::String::ConvertToUInt16(UInt16 &value,unsigned int base)
{
	return ConvertToUInt16(Text,value,base,Size);
}
bool GApp::Utils::String::ConvertToUInt32(UInt32 &value,unsigned int base)
{
	return ConvertToUInt32(Text,value,base,Size);
}
bool GApp::Utils::String::ConvertToUInt64(UInt64 &value,unsigned int base)
{
	return ConvertToUInt64(Text,value,base,Size);
}
bool GApp::Utils::String::ConvertToFloat64(Float64 &value)
{
	return ConvertToFloat64(Text,value,Size);
}
bool GApp::Utils::String::ConvertToFloat32(Float32 &value)
{
	return ConvertToFloat32(Text,value,Size);
}
bool GApp::Utils::String::ConvertToLower()
{
	for (int tr=0;tr<Size;tr++)
	{
		if ((Text[tr]>='A') && (Text[tr]<='Z'))
			Text[tr]|=0x20;
	}
	return true;
}
bool GApp::Utils::String::ConvertToUpper()
{
	for (int tr=0;tr<Size;tr++)
	{
		if ((Text[tr]>='a') && (Text[tr]<='z'))
			Text[tr]-=0x20;
	}
	return true;
}

CHARACTER* GApp::Utils::String::UInt8ToString(UInt8 value)
{
	char temp[32];
	if (GApp::Utils::String::UInt8ToString(value,temp,32))
		if (Set(temp))
			return Text;
	return NULL;
}
CHARACTER* GApp::Utils::String::UInt16ToString(UInt16 value)
{
	char temp[32];
	if (GApp::Utils::String::UInt16ToString(value,temp,32))
		if (Set(temp))
			return Text;
	return NULL;
}
CHARACTER* GApp::Utils::String::UInt32ToString(UInt32 value)
{
	char temp[32];
	if (GApp::Utils::String::UInt32ToString(value,temp,32))
		if (Set(temp))
			return Text;
	return NULL;
}
CHARACTER* GApp::Utils::String::UInt64ToString(UInt64 value)
{
	char temp[32];
	if (GApp::Utils::String::UInt64ToString(value,temp,32))
		if (Set(temp))
			return Text;		
	return NULL;
}
CHARACTER* GApp::Utils::String::Int8ToString(Int8 value)
{
	char temp[32];
	if (GApp::Utils::String::Int8ToString(value,temp,32))
		if (Set(temp))
			return Text;		
	return NULL;
}
CHARACTER* GApp::Utils::String::Int16ToString(Int16 value)
{
	char temp[32];
	if (GApp::Utils::String::Int16ToString(value,temp,32))
		if (Set(temp))
			return Text;		
	return NULL;
}
CHARACTER* GApp::Utils::String::Int32ToString(Int32 value)
{
	char temp[32];
	if (GApp::Utils::String::Int32ToString(value,temp,32))
		if (Set(temp))
			return Text;		
	return NULL;
}
CHARACTER* GApp::Utils::String::Int64ToString(Int64 value)
{
	char temp[32];
	if (GApp::Utils::String::Int64ToString(value,temp,32))
		if (Set(temp))
			return Text;		
	return NULL;
}
CHARACTER* GApp::Utils::String::BoolToString(bool value)
{
	char temp[32];
	if (GApp::Utils::String::BoolToString(value,temp,32))
		if (Set(temp))
			return Text;		
	return NULL;
}
CHARACTER* GApp::Utils::String::CharToString(char value)
{
	char temp[32];
	if (GApp::Utils::String::CharToString(value,temp,32))
		if (Set(temp))
			return Text;		
	return NULL;
}
CHARACTER* GApp::Utils::String::Float32ToString(Float32 value)
{
	char temp[32];
	if (GApp::Utils::String::Float32ToString(value,temp,32))
		if (Set(temp))
			return Text;		
	return NULL;
}
CHARACTER* GApp::Utils::String::Float64ToString(Float64 value)
{
	char temp[32];
	if (GApp::Utils::String::Float64ToString(value,temp,32))
		if (Set(temp))
			return Text;		
	return NULL;
}

//------------------------------- OPERATORI -----------------------------
GApp::Utils::String& GApp::Utils::String::operator=  (const GApp::Utils::String &s) 
{ 
	Set(s.Text,s.Size);
	return *this; 
}
bool     GApp::Utils::String::operator>  (const GApp::Utils::String &s) const
{
	return this->Compare(s)>0;
}
bool     GApp::Utils::String::operator<  (const GApp::Utils::String &s) const
{
	return this->Compare(s)<0;
}
bool     GApp::Utils::String::operator== (const GApp::Utils::String &s) const
{
	return this->Compare(s)==0;
}
bool     GApp::Utils::String::operator!= (const GApp::Utils::String &s) const
{
	return this->Compare(s)!=0;
}
bool     GApp::Utils::String::operator>= (const GApp::Utils::String &s) const
{
	return this->Compare(s)>=0;
}
bool     GApp::Utils::String::operator<= (const GApp::Utils::String &s) const
{
	return this->Compare(s)<=0;
}
CHARACTER&   GApp::Utils::String::operator[] (int poz)
{
	if ((Text==NULL) || (Size==0) || ((Flags & STRING_FLAG_CONSTANT)!=0))
	{
		CHARACTER	*temp = NULL;
		return	(*temp);
	}
	if (poz>=0)
	{
		if (poz>Size-1)
			poz = Size-1;
		return Text[poz];
	} else {
		poz+=Size;
		if (poz<0)
			poz = 0;
		return Text[poz];
	}
}