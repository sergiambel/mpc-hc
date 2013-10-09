#pragma once
#include <string>
#include <atlbase.h>

using namespace std;
using namespace ATL;

//Data type enumeration
enum DataType
{	
	PARAM_TYPE_INT32,
	PARAM_TYPE_INT64,
	PARAM_TYPE_BSTR,
	PARAM_TYPE_STRING,
	PARAM_TYPE_SINGLE,
	PARAM_TYPE_DOUBLE,
	PARAM_TYPE_BOOL,
	PARAM_TYPE_BYTE,
	PARAM_TYPE_ARRAY,
	PARAM_TYPE_NONE
};

class Parameter
{
private:

	void*			m_pData;		//Puntero a los datos
	unsigned int	m_arraySize;	//Tamaño del array (si es un array, sino es 0)
	DataType		m_dataType;		//Tipo de dato
	void			CopyData(Parameter* dst, Parameter* src);

public:

	int						m_id;

	//Constructor
	Parameter();
	//Destructor
	~Parameter();
	//Copy constructor
	Parameter(const Parameter& p);
	//Equal operator overload
	Parameter& operator=(const Parameter &param);
	//Comparison operator overload
	bool operator==(const Parameter& param) const;
	bool operator!=(const Parameter& param) const {return !(*this==param);}

	static Parameter		CreateInt32Param(int value);
	static Parameter		CreateInt64Param(__int64 value);
	static Parameter		CreateBSTRParam(CComBSTR value);
	static Parameter		CreateStringParam(wstring value);
	static Parameter		CreateSingleParam(float value);
	static Parameter		CreateDoubleParam(double value);
	static Parameter		CreateBoolParam(bool value);
	static Parameter		CreateByteParam(unsigned char value);

	inline void				SetDataType(DataType dataType) {m_dataType = dataType;}
	inline void				SetArraySize(int size);
	inline void				SetValue(void* data) {m_pData = data;}

	inline int				GetArraySize() const {return m_arraySize;}
	inline DataType			GetDataType() {return m_dataType;}
	inline void*			GetpValue() {return m_pData;}

	inline int*				GetpInt32Value()	{return (int*) m_pData;}
	inline __int64*			GetpInt64Value()	{return (__int64*) m_pData;}
	inline CComBSTR*		GetpBSTRValue()		{return (CComBSTR*) m_pData;}
	inline wstring*			GetpStringValue()	{return (wstring*) m_pData;}
	inline float*			GetpSingleValue()	{return (float*) m_pData;}
	inline double*			GetpDoubleValue()	{return (double*) m_pData;}
	inline bool*			GetpBoolValue()		{return (bool*) m_pData;}
	inline unsigned char*	GetpByteValue()		{return (unsigned char*) m_pData;}
	inline Parameter*		GetpArrayValue(unsigned position = 0)
	{
		if(position<m_arraySize) return (((Parameter*) m_pData)+position);
		else return NULL;
	}
	inline Parameter		GetArrayValue(unsigned position) const
	{
		if(position<m_arraySize) return *(((Parameter*) m_pData)+position);
		else return Parameter();
	}


	inline int				GetInt32Value() const		{return *((int*) m_pData);}				
	inline __int64			GetInt64Value() const		{return *((__int64*) m_pData);}
	inline CComBSTR			GetCComBSTRValue() const	{return *((CComBSTR*) m_pData);}
	inline BSTR				GetBSTRValue() const      
	{
		CComBSTR* ccbstr = (CComBSTR*)m_pData;
		return ccbstr->m_str;
	}
	inline wstring			GetStringValue() const	{return *((wstring*) m_pData);}
	inline float			GetSingleValue() const	{return *((float*) m_pData);}
	inline double			GetDoubleValue() const	{return *((double*) m_pData);}
	inline bool				GetBoolValue() const	{return *((bool*) m_pData);}
	inline unsigned char	GetByteValue()		{return *((unsigned char*) m_pData);}

	void					SetInt32Value(int value);
	void					SetInt64Value(__int64 value);
	void					SetBSTRValue(CComBSTR value);
	void					SetStringValue(wstring value);
	void					SetSingleValue(float value);
	void					SetDoubleValue(double value);
	void					SetBoolValue(bool value);
	void					SetByteValue(unsigned char value);
	void					Dump(int depth = 0);
	wstring					DumpToString(unsigned int maxStringChars=0);
	void					DeleteData(DataType* DataType, void** data, unsigned int* dataType);
	void					DeleteCurrentData();

	INT64					ConvertToInt64Value();
	int						ConvertToInt32Value();
	
};