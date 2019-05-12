#include "GApp.h"

#define KEYDB_DATASIZE_MASK		0xFFFFF

#define ImplementationKeyDBTypeFunctions(tip,name,defValue,dataType) \
bool GApp::Utils::KeyDB::Set##name (const char *key,tip value) { return SetData(key,&value,sizeof(value),dataType); } \
bool GApp::Utils::KeyDB::Set##name(String *key, tip value) { CHECK(key != NULL, false, ""); return SetData(key->GetText(), &value, sizeof(value), dataType); } \
bool GApp::Utils::KeyDB::Set##name##Vector(const char *key, tip vector[], unsigned int count) { return SetData(key, vector, sizeof(tip)*count, dataType); } \
bool GApp::Utils::KeyDB::Set##name##Vector(String *key, tip vector[], unsigned int count) { CHECK(key != NULL, false, ""); return SetData(key->GetText(), vector, sizeof(tip)*count, dataType); } \
tip	 GApp::Utils::KeyDB::Get##name (const char *key,tip defaultValue) { tip temp; if (CopyData(key,&temp,sizeof(tip),dataType)==false) return defaultValue; return temp; } \
tip	 GApp::Utils::KeyDB::Get##name (String *key,tip defaultValue) { tip temp; if (key==NULL) return defaultValue; if (CopyData(key->GetText(),&temp,sizeof(tip),dataType)==false) return defaultValue; return temp; } \
bool GApp::Utils::KeyDB::Copy##name	(const char *key,tip &value) { return CopyData(key,&value,sizeof(value),dataType); } \
bool GApp::Utils::KeyDB::Copy##name	(String *key,tip &value) { CHECK(key!=NULL,false,"");return CopyData(key->GetText(),&value,sizeof(value),dataType); } \
bool GApp::Utils::KeyDB::Copy##name##Vector	(const char *key,tip vector[],unsigned int maxElements,unsigned int &elementsCopied) { return CopyVector(key,vector,sizeof(tip),maxElements,elementsCopied); } \
bool GApp::Utils::KeyDB::Copy##name##Vector	(String *key,tip vector[],unsigned int maxElements,unsigned int &elementsCopied) { CHECK(key!=NULL,false,""); return CopyVector(key->GetText(),vector,sizeof(tip),maxElements,elementsCopied); }

// BYTE		szString
// BYTE*3	DataSize + DataType (4 Bit - DataType), 20 Bit-DataSize
// char		String[szString+1]
// BYTE		Data[DataSize]

int  _KeyData_Get_InsertSort_Locus(unsigned int *indexes,unsigned char *Data,unsigned int DataSize,const char *KeyName,int lo,int hi,bool *foundElement,unsigned int *elementDataSize,unsigned int *textSize,unsigned int *dataPoz,unsigned int *dataType)
{
	int mij;
	int res;
	unsigned int index,info,txSize,elDataSize,elDataType;
	(*foundElement) = false;
	do
	{
		mij=(lo+hi)/2;
		index = indexes[mij];
		if (index+4>DataSize)
			return -1;
		info = *(unsigned int *)&Data[index];
		txSize = info & 0xFF;
		elDataSize = ((info >> 8) & KEYDB_DATASIZE_MASK);
		elDataType = ((info >> 28) & 0x0F);
		if (txSize==0)
			return -1;
		if ((index + 4 + txSize + 1 + elDataSize)>DataSize)
			return -1;
		if ((res=GApp::Utils::String::Compare(KeyName,(const char *)&Data[index+4]))==0) 
		{
			if (dataPoz!=NULL)
				(*dataPoz) = index;
			if (elementDataSize!=NULL)
				(*elementDataSize) = elDataSize;
			if (dataType != NULL)
				(*dataType) = elDataType;
			if (textSize!=NULL)
				(*textSize) = txSize;
			if (foundElement!=NULL)
				(*foundElement) = true;
			return mij;
		}
		if (res<0) hi=mij-1;
		if (res>0) lo=mij+1;
	} while ((lo<=hi) && (hi>=0));
	

	if( res<0 ) return lo;
	else return hi+1;
}
const void*	_KeyData_GetPointer(GApp::Utils::Buffer &Indexes,GApp::Utils::Buffer &Data,const char *key,unsigned int *bufferSize = NULL,unsigned int *dataType = NULL)
{
	CHECK(key!=NULL,NULL,"Null key");
	bool elementExists = false;
	unsigned int elementDataSize = 0,dataPoz = 0,txSize=0;
	int count = Indexes.GetSize()/sizeof(UInt32);
	CHECK(count>0,NULL,"Key ('%s') not found ! (no records)",key);
	int poz = _KeyData_Get_InsertSort_Locus((unsigned int *)Indexes.Data, Data.GetBuffer(), Data.GetSize(), key, 0, count - 1, &elementExists, &elementDataSize, &txSize, &dataPoz, dataType);
	if ((elementExists) && (poz>=0) && (poz<count))
	{
		dataPoz+=(txSize+1+4);
		CHECK((dataPoz+elementDataSize)<=Data.GetSize(),NULL,"Corrupted data (data outside the buffer size) for key '%s'",key);
		if (bufferSize!=NULL)
			(*bufferSize) = elementDataSize;
		return (const void *)(Data.GetBuffer()+dataPoz);
	}
	RETURNERROR(NULL,"Key ('%s') not found !",key);
}

#define KeyDBPreparePointer \
	unsigned int elementSize = 0; \
	unsigned int elementDataType = 0; \
	const void *ptr = _KeyData_GetPointer(Indexes,Data,key,&elementSize,&elementDataType); \
	CHECK(ptr!=NULL,false,"Unable to find key '%s' in database",key);

//===========================================================================================
GApp::Utils::KeyDB::KeyDB()
{
	WriteEnabled = false;
}
bool GApp::Utils::KeyDB::SetData(const char* key, const void *Buffer, unsigned int BufferSize, unsigned int DataType)
{
	LOG_INFO("KeyDB::SetData(Key = %s, Buffer=%p, BufferSize = %d, BufferType = %d)", key, Buffer, BufferSize, DataType);
	CHECK(key!=NULL,false,"Invalid (NULL) key");
	CHECK(key[0]!=0,false,"Invalid (empty) key");
	CHECK(Buffer!=NULL,false,"Buffer is NULL (not set)");
	CHECK(BufferSize>0,false,"Invalid Buffer Size (0)");
	CHECK(WriteEnabled==true, false, "Unable to write key: '%s' -> Settings can only be use for writing in the following functions: OnSaveState and OnTerminate",key);

	if ((Indexes.GetSize()==0) || (Data.GetSize()==0))
	{
		CHECK(Indexes.Create(sizeof(UInt32)*64),false,"Unable to create a 64 int index vector !");
		CHECK(Data.Create(1024),false,"Unable to allocate 1024 bytes for data");
	}
	CHECK(BufferSize < KEYDB_DATASIZE_MASK, false, "A key can not hold more than %d bytes. The required size for key: %s is %d bytes", KEYDB_DATASIZE_MASK, key, BufferSize);
	unsigned int nkLen = GApp::Utils::String::Len(key);
	CHECK(nkLen<250,false,"Key '%s' should be smaller than 250 characters (%d)",key,nkLen);

	int poz = 0;
	bool elementExists = false;
	unsigned int elementDataSize = 0;
	unsigned int elementDataType = 0;
	unsigned int keySize = 0;
	unsigned int dataPoz = 0;
	unsigned int tmp = 0;
	DataType = (DataType & 0x0F) << 28;
	if (Indexes.GetSize()>0)
	{
		poz = _KeyData_Get_InsertSort_Locus((unsigned int *)Indexes.Data, Data.GetBuffer(), Data.GetSize(), key, 0, (Indexes.GetSize() / sizeof(UInt32)) - 1, &elementExists, &elementDataSize, &keySize, &dataPoz, &elementDataType);
		CHECK(poz>=0,false,"Key '%s' not found",key);
	}
	//LOG_INFO("Setting data for: %s -> %d bytes (PTR=%p) Exists=%d", key, BufferSize, Buffer, elementExists);
	if (elementExists)
	{
		if (elementDataSize == BufferSize)
		{
			CHECK(Data.SetData(dataPoz+keySize+1+4,Buffer,BufferSize,false),false,"");
		} else {
			CHECK(Data.Delete(dataPoz+keySize+1+4,elementDataSize),false,"");
			CHECK(Data.InsertData(dataPoz+keySize+1+4,Buffer,BufferSize),false,"");
			// updatam in vector
			unsigned int *i = (unsigned int *)Indexes.Data;
			int count = Indexes.GetSize()/sizeof(UInt32);
			for (int tr=0;tr<count;tr++)
			{
				if (i[tr]>dataPoz)
				{
					i[tr] = i[tr]+BufferSize-elementDataSize;
				}
			}	
		}
		// updatez si campul cu valorile 
		CHECK(Data.SetUInt32(dataPoz, (keySize & 0xFF) | ((BufferSize & KEYDB_DATASIZE_MASK) << 8) | DataType, false), false, "");
	} else {
		tmp = Data.GetSize();
		CHECK(Indexes.InsertUInt32(poz*sizeof(UInt32),tmp),false,"");
		CHECK(Data.PushUInt32((nkLen & 0xFF) | ((BufferSize & KEYDB_DATASIZE_MASK) << 8) | DataType),false,"");
		CHECK(Data.PushString(key),false,"");
		CHECK(Data.PushData(Buffer,BufferSize),false,"");
	}
	//LOG_INFO("All ok - item added (%s)", key);
	return true;
}
bool GApp::Utils::KeyDB::CopyData(const char *key, void* Buffer, unsigned int BufferSize, unsigned int DataType)
{
	KeyDBPreparePointer;
	CHECK(Buffer!=NULL,false,"");
	CHECK(BufferSize>0,false,"");
	if (elementDataType != DataType)
	{
		LOG_INFO("Settings::Changing the type of %s from %d to %d", key, elementDataType, DataType);
	}
	unsigned char *s = (unsigned char *)(ptr);
	unsigned char *e = s + GAC_MIN(elementSize, BufferSize);
	unsigned char *d = (unsigned char *)Buffer;
	while (s<e) { (*d) = (*s);d++;s++; }
	return true;
}
bool GApp::Utils::KeyDB::SetData(String *key, const void *Buffer, unsigned int BufferSize, unsigned int DataType)
{
	CHECK(key!=NULL,false,"");
	return SetData(key->GetText(),Buffer,BufferSize,DataType);
}

bool GApp::Utils::KeyDB::CopyString(const char *key,String *text)
{
	KeyDBPreparePointer;
	CHECK(text!=NULL,false,"");
	CHECK(text->Set((const char *)ptr,elementSize),false,"");
	return true;
}
bool GApp::Utils::KeyDB::CopyString(const char *key,char *destination,unsigned int destinationSize)
{
	CHECK(key!=NULL,false,"");
	CHECK(destination!=NULL,false,"");
	CHECK(destinationSize>0,false,"");
	CHECK(CopyData(key,destination,destinationSize,KEYDB_DATATYPE_STRING),false,"");
	destination[destinationSize-1] = 0;
	return true;
}
char* GApp::Utils::KeyDB::GetString(const char *key,const char *defaultValue)
{	
	const void *ptr;
	while (true)
	{		
		CHECKBK((ptr = _KeyData_GetPointer(Indexes,Data,key))!=NULL,"");		
		return (char *)ptr;		
	}
	return (char *)defaultValue;
}
bool GApp::Utils::KeyDB::HasKey(const char *key)
{
	CHECK(key!=NULL,false,"");	
	bool elementExists = false;
	int count = Indexes.GetSize()/sizeof(UInt32);
	if (count<1)
		return false;
	_KeyData_Get_InsertSort_Locus((unsigned int *)Indexes.Data,Data.GetBuffer(),Data.GetSize(),key,0,count-1,&elementExists,NULL,NULL,NULL,NULL);
	return elementExists;
}
unsigned int GApp::Utils::KeyDB::Len()
{
	return Indexes.GetSize()/sizeof(UInt32);
}
unsigned int GApp::Utils::KeyDB::GetSize()
{
	return Indexes.GetSize()/sizeof(UInt32);
}
bool GApp::Utils::KeyDB::SetString(const char *key,const char *Text)
{
	CHECK(Text!=NULL,false,"");
	return SetData(key, Text, GApp::Utils::String::Len(Text) + 1, KEYDB_DATATYPE_STRING);
}
bool GApp::Utils::KeyDB::SetVector(const char *key, const void *vector, unsigned int ElementsCount, unsigned int dataType)
{
	unsigned int ElementSize = 0;
	switch (dataType)
	{
		case KEYDB_DATATYPE_INT8:
		case KEYDB_DATATYPE_UINT8:
			ElementSize = sizeof(Int8);
			break;
		case KEYDB_DATATYPE_INT16:
		case KEYDB_DATATYPE_UINT16:
			ElementSize = sizeof(Int16);
			break;
		case KEYDB_DATATYPE_INT32:
		case KEYDB_DATATYPE_UINT32:
			ElementSize = sizeof(Int32);
			break;
		case KEYDB_DATATYPE_INT64:
		case KEYDB_DATATYPE_UINT64:
			ElementSize = sizeof(Int64);
			break;
		case KEYDB_DATATYPE_FLOAT32:
			ElementSize = sizeof(Float32);
			break;
		case KEYDB_DATATYPE_FLOAT64:
			ElementSize = sizeof(Float64);
			break;
		case KEYDB_DATATYPE_BOOL:
			ElementSize = sizeof(bool);
			break;
	}
	CHECK(ElementSize > 0, false, "KeyBD can not add a vector for this type: %d", dataType);
	CHECK(SetData(key,vector,ElementSize*ElementsCount,dataType),false,"");
	return true;
}
/*
bool GApp::Utils::KeyDB::SetVector(const char *key,Vector *v)
{
	CHECK(v!=NULL,false,"");
	CHECK(SetData(key,v->GetVector(),v->GetSize()*v->GetElementSize()),false,"");
	return true;
}
*/
bool GApp::Utils::KeyDB::CopyVector(const char *key,void *Vector,unsigned int ElemSize,unsigned int maxElements,unsigned int &copiedElements)
{
	KeyDBPreparePointer;
	CHECK(Vector!=NULL,false,"");
	CHECK(ElemSize>0,false,"");
	CHECK(elementSize>=ElemSize,false,"");
	CHECK((elementSize % ElemSize)==0,false,"");
	if (elementSize>=(maxElements*ElemSize))
		memcpy(Vector,ptr,elementSize);
	else {
		LOG_ERROR("Buffer to small to read data from key: %s [buffer size = %d, data size = %d]", key, maxElements*ElemSize, elementSize);
	}
	if (!IS_NULLREF(copiedElements))
		copiedElements = (elementSize / ElemSize);

	return true;
}


bool GApp::Utils::KeyDB::CheckIntegrity()
{
	unsigned int count = Indexes.GetSize()/sizeof(UInt32);
	if (count==0)
		return true;
	for (unsigned int tr=0;tr<count;tr++)
	{
		UInt32 poz = Indexes.GetUInt32(tr*sizeof(UInt32));
		unsigned int dataSize = 0,txSize = 0;
		CHECK(Data.CopyUInt32(poz,dataSize),false,"");
		txSize = dataSize & 0xFF;
		dataSize = ((dataSize >> 8) & KEYDB_DATASIZE_MASK);
		CHECK(poz+4+txSize+1+dataSize<=Data.GetSize(),false,"");
	}
	return true;
}
void GApp::Utils::KeyDB::Clear()
{
	Indexes.Resize(0);
	Data.Resize(0);
}
#ifdef PLATFORM_DEVELOP
bool GApp::Utils::KeyDB::CopyKeyInfo(unsigned int index, GApp::Utils::Buffer *b)
{
	CHECK(index >= 0, false, "");
	unsigned int count = Indexes.GetSize() / sizeof(UInt32);
	CHECK(index<count, false, "");
	unsigned int poz = Indexes.GetUInt32(index*sizeof(UInt32));
	unsigned int dataSize = 0, txSize = 0, dataType = 0;
	CHECK(Data.CopyUInt32(poz, dataSize), false, "");
	txSize = dataSize & 0xFF;
	dataType = (dataSize >> 28) & 0x0F;
	dataSize = ((dataSize >> 8) & KEYDB_DATASIZE_MASK);
	CHECK(poz + 4 + txSize + 1 + dataSize <= Data.GetSize(), false, "");
	CHECK(b->AddDebugKeyDaya((char *)(Data.GetBuffer() + poz + 4), dataType, Data.GetBuffer() + 5 + txSize + poz, dataSize), false, "");
	return true;
}
#endif
#ifdef ENABLE_INFO_LOGGING
#define KEYDB_SHOW_VALUE(format,type) while (s < e) { CHECK(str->AddFormatedEx(format, *((type *)s)), false, "");s += sizeof(type); }
bool GApp::Utils::KeyDB::GetKeyInfo(unsigned int index, GApp::Utils::String *str)
{
	CHECK(index >= 0, false, "");
	unsigned int count = Indexes.GetSize() / sizeof(UInt32);
	CHECK(index<count, false, "");
	unsigned int poz = Indexes.GetUInt32(index*sizeof(UInt32));
	unsigned int dataSize = 0, txSize = 0, dataType = 0;
	CHECK(Data.CopyUInt32(poz, dataSize), false, "");
	txSize = dataSize & 0xFF;
	dataType = (dataSize >> 28) & 0x0F;
	dataSize = ((dataSize >> 8) & KEYDB_DATASIZE_MASK);
	CHECK(poz + 4 + txSize + 1 + dataSize <= Data.GetSize(), false, "");
	CHECK(str->Add((const char *)(Data.GetBuffer() + poz + 4), txSize), false, "");
	CHECK(str->Add("|"), false, "");

	switch (dataType)
	{
		case KEYDB_DATATYPE_INT8:
			CHECK(str->Add("Int8|"), false, "");
			break;
		case KEYDB_DATATYPE_INT16:
			CHECK(str->Add("Int16|"), false, "");
			break;
		case KEYDB_DATATYPE_INT32:
			CHECK(str->Add("Int32|"), false, "");
			break;
		case KEYDB_DATATYPE_INT64:
			CHECK(str->Add("Int64|"), false, "");
			break;
		case KEYDB_DATATYPE_UINT8:
			CHECK(str->Add("UInt8|"), false, "");
			break;
		case KEYDB_DATATYPE_UINT16:
			CHECK(str->Add("UInt16|"), false, "");
			break;
		case KEYDB_DATATYPE_UINT32:
			CHECK(str->Add("UInt32|"), false, "");
			break;
		case KEYDB_DATATYPE_UINT64:
			CHECK(str->Add("UInt64|"), false, "");
			break;
		case KEYDB_DATATYPE_FLOAT32:
			CHECK(str->Add("Float32|"), false, "");
			break;
		case KEYDB_DATATYPE_FLOAT64:
			CHECK(str->Add("Float64|"), false, "");
			break;
		case KEYDB_DATATYPE_BOOL:
			CHECK(str->Add("Bool|"), false, "");
			break;
		case KEYDB_DATATYPE_STRING:
			CHECK(str->Add("String|"), false, "");
			break;
		default:
			CHECK(str->AddFormatedEx("Unk%{ui32}|",dataType), false, "");
			break;
	}
	CHECK(str->AddFormatedEx("%{ui32}|", dataSize), false, "");
	unsigned char *s = (unsigned char *)(Data.GetBuffer() + 5 + txSize + poz);
	unsigned char *e = s + dataSize;

	switch (dataType)
	{
	case KEYDB_DATATYPE_INT8:
		KEYDB_SHOW_VALUE("%{i8},", Int8);
		break;
	case KEYDB_DATATYPE_INT16:
		KEYDB_SHOW_VALUE("%{i16},", Int16);
		break;
	case KEYDB_DATATYPE_INT32:
		KEYDB_SHOW_VALUE("%{i32},", Int32);
		break;
	case KEYDB_DATATYPE_INT64:
		KEYDB_SHOW_VALUE("%{i64},", Int64);
		break;
	case KEYDB_DATATYPE_UINT8:
		CHECK(str->Add("UInt8|"), false, "");
		KEYDB_SHOW_VALUE("%{ui8},", UInt8);
		break;
	case KEYDB_DATATYPE_UINT16:
		KEYDB_SHOW_VALUE("%{ui16},", UInt16);
		break;
	case KEYDB_DATATYPE_UINT32:
		KEYDB_SHOW_VALUE("%{ui32},", UInt32);
		break;
	case KEYDB_DATATYPE_UINT64:
		KEYDB_SHOW_VALUE("%{ui64},", UInt64);
		break;
	case KEYDB_DATATYPE_FLOAT32:
		KEYDB_SHOW_VALUE("%{float},", Float32);
		break;
	case KEYDB_DATATYPE_FLOAT64:
		KEYDB_SHOW_VALUE("%{double},", Float64);
		break;
	case KEYDB_DATATYPE_BOOL:
		while (s < e) { 
			if ((*((bool *)s)) == true) {
				CHECK(str->Add("True,"), false, "");
			}
			else {
				CHECK(str->Add("True,"), false, "");
			}
			s += sizeof(bool); 
		}
		break;
	case KEYDB_DATATYPE_STRING:
		while (s < e) {
			if (((*s) >= ' ') && ((*s)<=127)) {
				CHECK(str->AddChar(*s), false, "");
			}
			else {
				CHECK(str->AddChar('?'), false, "");
			}
			s += sizeof(char);
		}
		break;
	default:
		break;
	}

	return true;
}
#endif

ImplementationKeyDBTypeFunctions(bool, Bool, false, KEYDB_DATATYPE_BOOL)
ImplementationKeyDBTypeFunctions(float, Float, 0.0f, KEYDB_DATATYPE_FLOAT32)
ImplementationKeyDBTypeFunctions(double, Double, 0.0, KEYDB_DATATYPE_FLOAT64)
ImplementationKeyDBTypeFunctions(Int8, Int8, 0, KEYDB_DATATYPE_INT8)
ImplementationKeyDBTypeFunctions(Int16, Int16, 0, KEYDB_DATATYPE_INT16)
ImplementationKeyDBTypeFunctions(Int32, Int32, 0, KEYDB_DATATYPE_INT32)
ImplementationKeyDBTypeFunctions(Int64, Int64, 0, KEYDB_DATATYPE_INT64)
ImplementationKeyDBTypeFunctions(UInt8, UInt8, 0, KEYDB_DATATYPE_UINT8)
ImplementationKeyDBTypeFunctions(UInt16, UInt16, 0, KEYDB_DATATYPE_UINT16)
ImplementationKeyDBTypeFunctions(UInt32, UInt32, 0, KEYDB_DATATYPE_UINT32)
ImplementationKeyDBTypeFunctions(UInt64, UInt64, 0, KEYDB_DATATYPE_UINT64)

