#include "Parameter.h"
#include <comdef.h>


Parameter::Parameter(const Parameter& p)
{
	m_pData = 0;
	m_dataType = PARAM_TYPE_NONE;
	m_arraySize = p.GetArraySize();
	CopyData(this, (Parameter*)&p);
}

Parameter& Parameter::operator=(const Parameter &p)
{
	DeleteData(&m_dataType, &m_pData, &m_arraySize);
	m_arraySize = p.GetArraySize();
	CopyData(this, (Parameter*)&p);
	return *this;
}

bool Parameter::operator==(const Parameter& _dst) const
{
	Parameter dst = _dst;
	//If the types are not the same, return false
	if(dst.GetDataType() != m_dataType)
	{
		return false;
	}

	switch(dst.GetDataType())
	{
		case PARAM_TYPE_NONE: {return true;}
		case PARAM_TYPE_BSTR: {return GetCComBSTRValue() == dst.GetCComBSTRValue();}
		case PARAM_TYPE_STRING: {return GetStringValue() == dst.GetStringValue();}
		case PARAM_TYPE_SINGLE: {return GetSingleValue() == dst.GetSingleValue();}
		case PARAM_TYPE_DOUBLE: {return GetDoubleValue() == dst.GetDoubleValue();}
		case PARAM_TYPE_BOOL: {return GetBoolValue() == dst.GetBoolValue();}
		case PARAM_TYPE_INT32: {return GetInt32Value() == dst.GetInt32Value();}
		case PARAM_TYPE_INT64: {return GetInt64Value() == dst.GetInt64Value();}
		case PARAM_TYPE_ARRAY: 
			{
				if(GetArraySize() != dst.GetArraySize())
				{
					return false;
				}
				for(unsigned int i=0; i<m_arraySize; i++)
				{
					Parameter pSrc = GetArrayValue(i);
					Parameter pDst = dst.GetArrayValue(i);
					if(!(pSrc == pDst))
					{
						return false;
					}
				}
			}
	}
	return true;

}

void Parameter::CopyData(Parameter* dst, Parameter* src)
{
	switch(src->GetDataType())
	{
		case PARAM_TYPE_NONE:
			{ dst->SetDataType(PARAM_TYPE_NONE); break;}
		case PARAM_TYPE_BSTR:
			{ dst->SetBSTRValue(src->GetBSTRValue()); break; }
		case PARAM_TYPE_STRING:
			{ dst->SetStringValue(src->GetStringValue()); break; }
		case PARAM_TYPE_SINGLE:
			{ dst->SetSingleValue(src->GetSingleValue()); break; }
		case PARAM_TYPE_DOUBLE:
			{ dst->SetDoubleValue(src->GetDoubleValue()); break; }
		case PARAM_TYPE_BOOL:
			{ dst->SetBoolValue(src->GetBoolValue()); break; }
		case PARAM_TYPE_INT32:
			{ dst->SetInt32Value(src->GetInt32Value()); break; }
		case PARAM_TYPE_INT64:
			{ dst->SetInt64Value(src->GetInt64Value()); break; }	
		case PARAM_TYPE_BYTE:
			{ dst->SetByteValue(src->GetByteValue()); break; }	
		case PARAM_TYPE_ARRAY:
			{
			
			dst->SetArraySize(src->GetArraySize());
			Parameter* pData = dst->GetpArrayValue();
			dst->SetDataType(src->GetDataType());
			for(int i=0;i<src->GetArraySize();i++) 
				{
					Parameter* pDstArrayPos = (Parameter*)src->GetpValue();
					CopyData(&pData[i], &pDstArrayPos[i]);
				}
			dst->SetValue((void*) pData);
			}
	}
}

Parameter::Parameter()
{
	m_pData = 0;
	m_arraySize = 0;
	m_dataType = PARAM_TYPE_NONE;
}


Parameter::~Parameter()
{
	DeleteData(&m_dataType, &m_pData, &m_arraySize);
}


Parameter Parameter::CreateInt32Param(int value)
{
	Parameter	p;
	p.SetInt32Value(value);
	return p;
}


Parameter Parameter::CreateInt64Param(__int64 value)
{
	Parameter	p;
	p.SetInt64Value(value);
	return p;
}


Parameter Parameter::CreateBSTRParam(CComBSTR value)
{
	Parameter	p;
	p.SetBSTRValue(value);
	return p;
}


Parameter Parameter::CreateStringParam(wstring value)
{
	Parameter	p;
	p.SetStringValue(value);
	return p;
}


Parameter Parameter::CreateSingleParam(float value)
{
	Parameter	p;
	p.SetSingleValue(value);
	return p;
}


Parameter Parameter::CreateDoubleParam(double value)
{
	Parameter	p;
	p.SetDoubleValue(value);
	return p;
}


Parameter Parameter::CreateBoolParam(bool value)
{
	Parameter	p;
	p.SetBoolValue(value);
	return p;
}


Parameter Parameter::CreateByteParam(unsigned char value)
{
	Parameter	p;
	p.SetByteValue(value);
	return p;
}


void Parameter::SetArraySize(int size)
{
	DeleteCurrentData();
	m_pData = new Parameter[size];
	m_arraySize = size;
	m_dataType = PARAM_TYPE_ARRAY;
}

void Parameter::DeleteCurrentData()
{
	if(m_pData)
	{
		DeleteData(&m_dataType, &m_pData, &m_arraySize);	
	}
}

void Parameter::DeleteData(DataType* dataType, void** ppData, unsigned int* arraySize)
{
	void* pData = *ppData;
	if(pData) 
	{
		switch(*dataType)
		{
		case PARAM_TYPE_ARRAY:
			{
				delete [](Parameter*)pData;
				pData = 0;
				break;
			}
		case PARAM_TYPE_BSTR:
			{
				delete (CComBSTR*) pData;
				pData = 0;
				break;
			}
		case PARAM_TYPE_STRING:
			{
				wstring* w = (wstring*) pData;
				delete w;
				w = 0;
				break;
			}
		case PARAM_TYPE_DOUBLE:
			{
				double* d = (double*) pData;
				delete d;
				d = 0;
				break;
			}
		case PARAM_TYPE_SINGLE:
			{
				float* d = (float*) pData;
				delete d;
				d = 0;
				break;
			}
		case PARAM_TYPE_BOOL:
			{
				bool* b = (bool*) pData;
				delete b;
				b = 0;
				break;
			}
		case PARAM_TYPE_INT32:
			{
				delete (int*) pData;
				pData = 0;
				break;
			}
		case PARAM_TYPE_INT64:
			{
				__int64* i = (__int64*) pData;
				delete i;
				i = 0;
				break;
			}
		case PARAM_TYPE_BYTE:
			{
				unsigned char* c = (unsigned char*) pData;
				delete c;
				c = 0;
				break;
			}
		}
	}
	*dataType = PARAM_TYPE_NONE;
	arraySize = 0;
}

wstring Parameter::DumpToString(unsigned int maxStringChars)
{
	wstring res = L"";
	switch(m_dataType)
	{
		case PARAM_TYPE_BOOL: 
			{ 
				res = L"Bool:";
				res += (GetBoolValue()==true)?L"true":L"false"; 
				break;
			}
		case PARAM_TYPE_ARRAY: 
			{ 
				res = L"Array:";
				for(unsigned int i=0; i<m_arraySize; i++)
				{
					if(i!=0) 
					{
						res += L"|";
					}
					res += GetpArrayValue()[i].DumpToString();
				}
				break;
			}
		case PARAM_TYPE_BSTR: 
			{ 
				res = L"String [BSTR]:";
				if (m_pData != NULL) {
					CComBSTR	*pBSTR = GetpBSTRValue();
					if (pBSTR->m_str != NULL)
						res += pBSTR->m_str;
					else {
						int i = 0;
					}
				}
				break;
				
			}
		case PARAM_TYPE_STRING: 
			{ 
				res = L"String: ";
				res += GetStringValue();
				break;
			}

		case PARAM_TYPE_INT32: 
			{ 
				res = L"Int32:";
				WCHAR val[512];
				swprintf_s(val, 512, L"%d", GetInt32Value());
				res += val;
				//printf("Int32: %d\n", GetInt32Value()); break;}
				break;
			}
		case PARAM_TYPE_INT64: 
			{ 
				res = L"Int64:";
				WCHAR val[512];
				swprintf_s(val, 512, L"%lld", GetInt64Value());
				res += val;
				break;
			}
		case PARAM_TYPE_SINGLE: 
			{ 
				res = L"Single:";
				WCHAR val[512];
				swprintf_s(val, 512, L"%f", GetSingleValue());
				res += val;
				break;
				
			}
		case PARAM_TYPE_DOUBLE: 
			{	
				res = L"Double:";
				WCHAR val[512];
				swprintf_s(val, 512, L"%f", GetDoubleValue());
				res += val;
				break;
			}

		case PARAM_TYPE_NONE: 
			{
				res = L"NO VALUE";
				break;
			}
	}
	if(maxStringChars > 0)
	{
		return res.substr(0, maxStringChars);
	}

	return res;
}

void Parameter::Dump(int depth)
{
	wstring space = L"";
	for(int i=0;i<depth;i++)
	{
		space += L" ";
	}
	switch(m_dataType)
	{
		case PARAM_TYPE_BOOL: 
			{ 
				//Logger::Log(space + L"- Boolean", (GetBoolValue()==true)?L"true":L"false"); break;
			}
		case PARAM_TYPE_ARRAY: 
			{ 
				//Logger::Log(space + L"- Array", L"%d elements", m_arraySize);
				for(unsigned int i=0; i<m_arraySize; i++)
				{
					GetpArrayValue()[i].Dump(depth + 1);
				}
				break;
			}
		case PARAM_TYPE_BSTR: 
			{ 
				_bstr_t bstr(GetBSTRValue());
				wstring str(bstr);
				//Logger::LogSafe(space + L"- String [BSTR]", GetpBSTRValue()->m_str);
				break;
			}
		case PARAM_TYPE_STRING: 
			{ 
				
				//Logger::LogSafe(space + L"- String", GetStringValue());
				break;
			}


		case PARAM_TYPE_INT32: 
			{ 
				//Logger::Log(space + L"- Int32", L"%d", GetInt32Value());
				break;
			}
		case PARAM_TYPE_INT64: 
			{ 
				//Logger::Log(space + L"- Int64", L"%l", GetInt64Value());
				break;
			}
		case PARAM_TYPE_SINGLE: 
			{ 
				//Logger::Log(space + L"- Single", L"%f", GetSingleValue());
				break;
			}
		case PARAM_TYPE_DOUBLE: 
			{	
				//Logger::Log(space + L"- Double", L"%f", GetDoubleValue());
				break;
			}

		case PARAM_TYPE_NONE: 
			{
				//Logger::Log(space + L" - No value", L"");
				break;
			}
	}
}

void Parameter::SetInt32Value(int value)
{
	DeleteCurrentData();
	SetDataType(PARAM_TYPE_INT32);
	int* i = new int(value);
	SetValue((void*)i);
}

void Parameter::SetInt64Value(__int64 value)
{
	DeleteCurrentData();
	SetDataType(PARAM_TYPE_INT64);
	__int64* i = new __int64(value);
	SetValue((void*)i);
}

void Parameter::SetBSTRValue(CComBSTR value)
{
	DeleteCurrentData();
	CComBSTR* val = new CComBSTR(value);
	SetDataType(PARAM_TYPE_BSTR);
	SetValue((void*)val);
}

void Parameter::SetStringValue(wstring value)
{
	DeleteCurrentData();
	SetDataType(PARAM_TYPE_STRING);
	wstring* s = new wstring(value);
	SetValue((void*)s);
}

void Parameter::SetSingleValue(float value)
{
	DeleteCurrentData();
	SetDataType(PARAM_TYPE_SINGLE);
	float* f = new float(value);
	SetValue((void*)f);
}

void Parameter::SetDoubleValue(double value)
{
	DeleteCurrentData();
	SetDataType(PARAM_TYPE_DOUBLE);
	double* d = new double(value);
	SetValue((void*)d);
}

void Parameter::SetBoolValue(bool value)
{
	DeleteCurrentData();
	SetDataType(PARAM_TYPE_BOOL);
	bool* b = new bool(value);
	SetValue((void*)b);
}

void Parameter::SetByteValue(unsigned char value)
{
	DeleteCurrentData();
	SetDataType(PARAM_TYPE_BYTE);
	unsigned char* c = new unsigned char(value);
	SetValue((void*)c);
}

int Parameter::ConvertToInt32Value()
{	
	if(m_dataType == PARAM_TYPE_DOUBLE)
	{
		return (int)(*((double*) m_pData));
	}
	else return 0;
}

INT64 Parameter::ConvertToInt64Value()
{
	if(m_dataType == PARAM_TYPE_DOUBLE)
	{
		return (INT64)(*((double*) m_pData));
	}
	else return 0;
}