#include "GApp.h"

#define ARRAY_LIST_STRIP		16
#define RESIZE_ARRAY(newCapacity) { \
	if ((newCapacity)>Capacity) { \
		unsigned int _newCapacity = AlignCapacity(newCapacity,ARRAY_LIST_STRIP); \
		void** newE = new void*[_newCapacity]; \
		CHECK(newE!=NULL,false,"Unable to allocate %u items to grow the array list",_newCapacity); \
		if (Elements!=NULL) { \
			void** s = Elements; \
			void** d = newE; \
			for (unsigned int tr=0;tr<Size;tr++,s++,d++) (*d) = (*s); \
		} \
		Elements = newE; \
		Capacity = _newCapacity; \
	} \
}

unsigned int	AlignCapacity(unsigned int value,unsigned int strip)
{
	if (value==0)
		return strip;
	if ((value % strip)==0)
		return value;
	return ((value / strip)+1)*strip;
}

GApp::Utils::ArrayList::ArrayList()
{
	Elements=NULL;
	Size=Capacity=0;
}
GApp::Utils::ArrayList::~ArrayList()
{
	Dispose();
}
bool GApp::Utils::ArrayList::Create(unsigned int initialCapacity,bool createArray)
{
	Dispose();
	initialCapacity = AlignCapacity(initialCapacity,ARRAY_LIST_STRIP);
	Size = 0;
	if (createArray)
	{
		CHECK((Elements = new void*[initialCapacity])!=NULL,false,"Unable to allocate %u items",initialCapacity);
		for (unsigned int tr=0;tr<initialCapacity;tr++)
			Elements[tr]=NULL;
		Size = initialCapacity;
	}
	Capacity = initialCapacity;
	return true;
}
bool GApp::Utils::ArrayList::Dispose()
{
	if (Elements!=NULL)
		delete Elements;
	Elements = NULL;
	Size = Capacity = 0;
	return true;
}
unsigned int	GApp::Utils::ArrayList::GetSize()
{
	return Size;
}
void**			GApp::Utils::ArrayList::GetArrayList()
{
	return Elements;
}
void*			GApp::Utils::ArrayList::Get(unsigned int index)
{
	CHECK(index<Size,NULL,"Invalid index (%d) - should be in interval [0..%u)",index,Size);
	return Elements[index];
}
bool			GApp::Utils::ArrayList::Set(unsigned int index,void* element)
{
	CHECK(index<Size,false,"Invalid index (%d) - should be in interval [0..%u)",index,Size);
	Elements[index] = element;
	return true;
}
bool			GApp::Utils::ArrayList::Push(void *element)
{
	RESIZE_ARRAY(Size+1);
	Elements[Size] = element;
	Size++;
	return true;
}
bool			GApp::Utils::ArrayList::Insert(void *Element,unsigned int index)
{
	CHECK(index<=Size,false,"Invalid insertion index (%d)",index);
	RESIZE_ARRAY(Size+1);
	for (unsigned int tr=Size+1;tr>index;tr--)
		Elements[tr] = Elements[tr-1];
	Elements[index] = Element;
	Size++;
	return true;
}
bool			GApp::Utils::ArrayList::Delete(unsigned int index)
{
	CHECK(index<Size,false,"Invalid insertion index (%d)",index);	
	for (unsigned int tr=index+1;tr<Size;tr++)
		Elements[tr-1] = Elements[tr];
	Size--;
	return true;
}
bool			GApp::Utils::ArrayList::Clear()
{
	Size = 0;
	return true;
}