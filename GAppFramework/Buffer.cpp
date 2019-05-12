#include "GApp.h"
#define ImplementBufferTypeFunctions(tip,name) \
tip  GApp::Utils::Buffer::Get##name	(unsigned int poz,tip defaultValue) { if (poz+sizeof(tip)-1>=Size) {return defaultValue;} else {return *(tip *)&(Data[poz]);} } \
bool GApp::Utils::Buffer::Copy##name (unsigned int poz,tip &value) { if (poz+sizeof(tip)-1>=Size) {return false;} else {value = *(tip *)&(Data[poz]); return true; } } \
bool GApp::Utils::Buffer::Set##name	(unsigned int poz,tip value,bool resizeIfBigger) { return SetData(poz,&value,sizeof(value),resizeIfBigger); } \
bool GApp::Utils::Buffer::Insert##name (unsigned int poz,tip value) { return InsertData(poz,&value,sizeof(value)); } \
bool GApp::Utils::Buffer::Push##name (tip value) { return InsertData(Size,&value,sizeof(value)); } \
bool GApp::Utils::Buffer::TryPop##name	(tip &value) { return false; } \
tip	 GApp::Utils::Buffer::Pop##name	(tip defaultValue) { return defaultValue; } \
bool GApp::Utils::Buffer::Set##name##Vector	(unsigned int poz,tip vector[],unsigned int count,bool resizeIfBigger) { return SetData(poz,vector,sizeof(tip)*count,resizeIfBigger); }\
bool GApp::Utils::Buffer::Insert##name##Vector (unsigned int poz,tip vector[],unsigned int count) { return InsertData(poz,vector,sizeof(tip)*count); } \
bool GApp::Utils::Buffer::Push##name##Vector (tip vector[],unsigned int count) { return InsertData(Size,vector,sizeof(tip)*count); } \
tip  GApp::Utils::Buffer::Read##name##FromStream(tip defaultValue) { if (StreamOffset+sizeof(tip)-1>=Size) {StreamOffset+=sizeof(tip);return defaultValue;} else {tip tempValue = *(tip *)&(Data[StreamOffset]); StreamOffset+=sizeof(tip);return tempValue; } }


#define GET_VALUE(type,defValue) if (poz+sizeof(type)-1>=Size) {return defValue;} else {return *(type *)&(Data[poz]);}
#define COPY_VALUE(type) if (poz+sizeof(type)-1>=Size) {return false;} else {value = *(type *)&(Data[poz]); return true; }
#define INIT_BUFFER(size_obj) { if (Data==NULL) { CHECK(Create(size_obj),false,""); }; if (size_obj>Size) { CHECK(Resize(size_obj),false,""); } }

GApp::Utils::Buffer::Buffer()
{
	Data = NULL;
	Size = 0;
	Allocated = 0;
	StreamOffset = 0;
}
GApp::Utils::Buffer::Buffer(unsigned int initialAllocatedSize)
{
	Data = NULL;
	Size = 0;
	Allocated = 0;
	StreamOffset = 0;
	Create(initialAllocatedSize);
}
GApp::Utils::Buffer::~Buffer()
{
	Free();
}
void GApp::Utils::Buffer::Free()
{
	if (Data!=NULL)
	{
		//LOG_INFO("GApp::Utils::Buffer::Free(Ptr=%p,Size=%d)",Data,Allocated);
		delete Data;
	}
	Size = 0;
	Allocated = 0;
	Data = NULL;
	StreamOffset = 0;
}
unsigned int GApp::Utils::Buffer::GetSize()
{
	return Size;
}
unsigned char* GApp::Utils::Buffer::GetBuffer()
{
	return Data;
}
bool GApp::Utils::Buffer::Resize(unsigned int newSize)
{
	CHECK(newSize<=0x80000000,false,"New size should be smaller than 0x80000000 (0x%08X)",newSize);
	if (newSize<=Allocated)
	{
		Size = newSize;
		return true;
	}
	unsigned int newAlloc = (((newSize >> 4)+1)<<4);
	unsigned char *Temp;
	//LOG_INFO("GApp::Utils::Buffer::Resize(from %d to %d)",Allocated,newAlloc);
	CHECK((Temp = new unsigned char[newAlloc])!=NULL,false,"");
	if (Data!=NULL)
	{
		memcpy(Temp, Data, Allocated);
		//unsigned char *s = Data;
		//unsigned char *t = Temp;
		//unsigned char *e = Data+Allocated;
		//while (s<e) { (*t) = (*s); t++;s++; }
		delete Data;
	}
	Data = Temp;
	Allocated = newAlloc;
	Size = newSize;
	return true;
}
bool GApp::Utils::Buffer::Create(unsigned int initialAllocatedSize)
{
	Free();	
	if (initialAllocatedSize%4!=0)
		initialAllocatedSize = (((initialAllocatedSize>>4)+1)<<4);
	CHECK((initialAllocatedSize>0) && (initialAllocatedSize<0x80000000),false,"Invalid size to allocate %d bytes",initialAllocatedSize);
	//LOG_INFO("GApp::Utils::Buffer::Create(%d)",initialAllocatedSize);
	CHECK((Data = new unsigned char[initialAllocatedSize]) != NULL, false, "Unable toallocate %d bytes", initialAllocatedSize);
	Allocated = initialAllocatedSize;	
	Size = 0;
	return true;
}

ImplementBufferTypeFunctions(bool,Bool)
ImplementBufferTypeFunctions(float,Float)
ImplementBufferTypeFunctions(double,Double)
ImplementBufferTypeFunctions(Int8,Int8)
ImplementBufferTypeFunctions(Int16,Int16)
ImplementBufferTypeFunctions(Int32,Int32)
ImplementBufferTypeFunctions(Int64,Int64)
ImplementBufferTypeFunctions(UInt8,UInt8)
ImplementBufferTypeFunctions(UInt16,UInt16)
ImplementBufferTypeFunctions(UInt32,UInt32)
ImplementBufferTypeFunctions(UInt64,UInt64)




void GApp::Utils::Buffer::Clear(unsigned char value)
{
	Clear(0,Size,value);
}
void GApp::Utils::Buffer::Clear(unsigned int start,unsigned int dataSize,unsigned char value)
{
	if ((Data!=NULL) || (start>=Size))
		return;
	unsigned char *p = &Data[start];
	if (start+dataSize>Size)
		dataSize = Size-start;
	unsigned char *e = p+dataSize;
	while (p<e) { (*p) = value; p++; }
}

bool GApp::Utils::Buffer::SetData(unsigned int poz,const void* Buffer,unsigned int dataSize,bool resizeIfBigger)
{
	CHECK(Buffer!=NULL,false,"");
	CHECK(dataSize>0,false,"");
	if (resizeIfBigger)
	{
		INIT_BUFFER(poz+dataSize);
	}
	CHECK(poz<Size,false,"");	
	CHECK(poz+dataSize<=Size,false,"");
	memcpy(&Data[poz], Buffer, dataSize);
	//unsigned char *p = &Data[poz];
	//const unsigned char *b = (const unsigned char *)Buffer;
	//unsigned char *e = p+dataSize;
	//for (;p<e;p++,b++)
	//{
	//	(*p) = (*b);
	//}
	return true;
}
bool GApp::Utils::Buffer::CopyData(unsigned int poz,void* Dest,unsigned int destSize)
{
	CHECK(Data!=NULL,false,"");
	CHECK(Dest!=NULL,false,"");
	CHECK(destSize>0,false,"");
	CHECK(poz+destSize<=Size,false,"");

	unsigned char *p = &Data[poz];
	unsigned char *b = (unsigned char *)Dest;
	unsigned char *e = p+destSize;
	while (p<e) { (*b) = (*p); p++; b++; }

	return true;
}

bool GApp::Utils::Buffer::SetString(unsigned int poz,const char *text,bool resizeIfBigger)
{
	CHECK(text!=0,false,"");
	unsigned int len = 0;
	const char *tmp = text;

	for (len=0;(*tmp)!=0;tmp++,len++);	
	return SetData(poz,text,len+1,resizeIfBigger);
}
bool GApp::Utils::Buffer::SetString(unsigned int poz,GApp::Utils::String *txt,bool resizeIfBigger)
{
	CHECK(txt!=NULL,false,"");
	CHECK(SetData(poz,txt->GetText(),txt->Len()+1,resizeIfBigger),false,"");
	return true;
}
/*
bool GApp::Utils::Buffer::SetVector(unsigned int poz,Vector *v,bool resizeIfBigger)
{
	CHECK(v!=NULL,false,"");
	CHECK(SetData(poz,v->GetVector(),v->GetSize()*v->GetElementSize(),resizeIfBigger),false,"");
	return true;
}
*/
bool GApp::Utils::Buffer::InsertData(unsigned int poz,const void* Buffer,unsigned int dataSize)
{
	CHECK(Buffer!=NULL,false,"");	
	CHECK(dataSize>0,false,"");
	CHECK(poz<=Size,false,"");

	unsigned int sz = Size;
	INIT_BUFFER(sz+dataSize);

	unsigned char *p;
	unsigned char *e;
	unsigned char *b;

	if (sz>0)
	{
		p = &Data[sz-1];
		e = &Data[poz];
		b = p+dataSize;
		while (p>=e) { (*b) = (*p); p--;b--; }
	}
	
	p = &Data[poz];
	b = (unsigned char *)Buffer;
	e = p+dataSize;
	while (p<e) { (*p) = (*b);p++;b++; }

	return true;
}
bool GApp::Utils::Buffer::InsertBuffer(unsigned int poz,GApp::Utils::Buffer *b)
{
	CHECK(b!=NULL,false,"");
	CHECK(InsertData(poz,b->GetBuffer(),b->GetSize()),false,"");
	return true;
}

bool GApp::Utils::Buffer::InsertString(unsigned int poz,const char *text)
{
	CHECK(text!=NULL,false,"");
	unsigned int len = 0;
	const char *tmp = text;

	for (len=0;(*tmp)!=0;tmp++,len++);	
	return InsertData(poz,text,len+1);
}
bool GApp::Utils::Buffer::InsertString(unsigned int poz,GApp::Utils::String *s)
{
	CHECK(s!=NULL,false,"");
	CHECK(InsertData(poz,s->GetText(),s->Len()+1),false,"");
	return true;
}
/*
bool GApp::Utils::Buffer::InsertVector(unsigned int poz,GApp::Utils::Vector *v)
{
	CHECK(v!=NULL,false,"");
	CHECK(InsertData(poz,v->GetVector(),v->GetSize()*v->GetElementSize()),false,"");
	return true;
}
*/

bool GApp::Utils::Buffer::Delete(unsigned int poz,unsigned int dataSize)
{
	CHECK(Data!=NULL,false,"");
	CHECK(poz<Size,false,"");
	if (poz+dataSize>Size)
		dataSize = Size-poz;
	if (dataSize==0)
		return true;

	unsigned char *p=&Data[poz+dataSize];
	unsigned char *e=&Data[Size];
	unsigned char *b=&Data[poz];
	while (p<e) { (*b) = (*p);p++;b++; }
	Size-=dataSize;
	return true;
}

bool GApp::Utils::Buffer::PushData(const void* Buffer,unsigned int dataSize)
{
	return InsertData(Size,Buffer,dataSize);
}
bool GApp::Utils::Buffer::PushBuffer(GApp::Utils::Buffer *b)
{
	CHECK(b!=NULL,false,"");
	CHECK(PushData((const void *)b->GetBuffer(),b->GetSize()),false,"");
	return true;
}

bool GApp::Utils::Buffer::PushString(const char *text)
{
	return InsertString(Size,text);
}
bool GApp::Utils::Buffer::PushString(GApp::Utils::String *txt)
{
	CHECK(txt!=NULL,false,"");
	CHECK(PushData(txt->GetText(),txt->Len()+1),false,"");
	return true;
}
/*
bool GApp::Utils::Buffer::PushVector(GApp::Utils::Vector *v)
{
	CHECK(v!=NULL,false,"");
	CHECK(PushData(v->GetVector(),v->GetSize()*v->GetElementSize()),false,"");
	return true;
}
*/
unsigned int GApp::Utils::Buffer::GetUInt24(unsigned int poz,unsigned int defaultValue)
{
	if (poz+3>=Size)
		return defaultValue;
	return ((unsigned int)Data[poz])|(((unsigned int)Data[poz+1])<<8)|(((unsigned int)Data[poz+2])<<16);
}
unsigned int GApp::Utils::Buffer::ReadCompactUInt32FromStream(unsigned int defaultValue)
{
	if (StreamOffset>=Size)
		return defaultValue;
	unsigned int value = 0;
	unsigned char* p = (unsigned char*)(&Data[StreamOffset]);
	do
	{
		value = (value<<7)|((*p) & 0x7F);
		StreamOffset++;
		if (((*p) & 0x80)==0)
			return value;
		p++;
	} while (StreamOffset<Size);
	return defaultValue;
}
bool GApp::Utils::Buffer::SetStreamOffset(unsigned int ofs)
{
	CHECK(ofs<Size,false,"");
	StreamOffset = ofs;
	return true;
}
unsigned int GApp::Utils::Buffer::GetStreamOffset()
{
	return StreamOffset;
}
bool GApp::Utils::Buffer::IsStreamAtEnd()
{
	return (StreamOffset>=Size);
}
#ifdef PLATFORM_DEVELOP
bool GApp::Utils::Buffer::AddDebugKeyDaya(char *Name, unsigned int dataType, void *Buffer, unsigned int BufferSize)
{
	CHECK(Name != NULL, false, "");
	unsigned int sz = GApp::Utils::String::Len(Name);
	CHECK(sz < 255, false, "Variable name must be less than 255 characters - '%s' has %d characters", Name, sz);
	dataType = dataType & 0x0F;
	CHECK(PushUInt8((unsigned char)sz),false,"");
	CHECK(PushString(Name), false, "");
	CHECK(PushUInt8(dataType & 0x0F), false, "");
	CHECK(PushUInt32(BufferSize), false, "");
	if (BufferSize>0)
	{
		CHECK(Buffer != NULL, false, "");
		CHECK(PushData(Buffer, BufferSize), false, "");
	}
	return true;
}
#endif