#include "stdafx.h"
#include "VariantUtils.h"
#include "comutil.h"


bool VariantUtils::CreateVariant(VARIANT* pVarResult, Parameter* p)
{
	VariantInit(pVarResult);
	switch(p->GetDataType())
	{
	case PARAM_TYPE_ARRAY:
		{
			//Not implemented
			break;
		}
	case PARAM_TYPE_INT32:
		{
			V_VT(pVarResult) = VT_I4;
			V_I4(pVarResult) = p->GetInt32Value();
			break;
		}
	case PARAM_TYPE_INT64:
		{
			V_VT(pVarResult) = VT_I8;
			V_I8(pVarResult) = p->GetInt64Value();
			break;
		}
	case PARAM_TYPE_SINGLE:
		{
			V_VT(pVarResult) = VT_R4;
			V_R4(pVarResult) = p->GetSingleValue();
			break;
		}
	case PARAM_TYPE_DOUBLE:
		{
			V_VT(pVarResult) = VT_R8;
			V_R8(pVarResult) = p->GetDoubleValue();
			break;
		}
	case PARAM_TYPE_BOOL:
		{
			V_VT(pVarResult) = VT_BOOL;
			if(false == p->GetBoolValue())
			{
				V_BOOL(pVarResult) = 0;
			}
			else
			{
				V_BOOL(pVarResult) = -1;
			}
			break;
		}
	case PARAM_TYPE_STRING: 
		{
			V_VT(pVarResult) = VT_BSTR;
			_bstr_t b(p->GetStringValue().c_str());
			V_BSTR(pVarResult) = b;
			break;
		}
	case PARAM_TYPE_BSTR:
		{
			V_VT(pVarResult) = VT_BSTR;
			BSTR str = SysAllocStringLen(p->GetBSTRValue(), p->GetCComBSTRValue().Length());
			V_BSTR(pVarResult) = str;
			break;
		}
	case PARAM_TYPE_NONE:
		{
			break;
		}
	}
	return true;
}


bool VariantUtils::BuildParameter(Parameter* p, VARIANTARG* param)
{
	switch(param->vt)
		{
			case VT_DISPATCH: { return BuildArray(p, param); break; }	//Array
			case VT_BSTR: {return BuildBSTR(p, param); break; }		//String tipo BSTR
			case VT_BOOL: {return BuildBool(p, param); break; }		//Booleano
			case VT_INT: {return BuildInt32(p, param); break; }		//Entero
			case VT_I4: {return BuildInt32(p, param); break; }			//Entero 32 bits
			case VT_I8: {return BuildInt64(p, param); break; }			//Entero 64 bits
			case VT_R4: {return BuildSingle(p, param); break; }		//Coma flotante 32 bits
			case VT_R8: {return BuildDouble(p, param); break; }		//Coma flotante 64 bits
			case VT_DECIMAL: {return BuildSingle(p, param); break; }   //Coma flotante
			default:
				{
					p = 0;
					return false;
					break;
				}
		}
	return true;
}


Parameter VariantUtils::BuildParameter(VARIANTARG* param)
{
	switch(param->vt)
		{
			case VT_DISPATCH: { return BuildArray(param); break; }	//Array
			case VT_BSTR: {return BuildBSTR(param); break; }		//String tipo BSTR
			case VT_BOOL: {return BuildBool(param); break; }		//Booleano
			case VT_INT: {return BuildInt32(param); break; }		//Entero
			case VT_I4: {return BuildInt32(param); break; }			//Entero 32 bits
			case VT_I8: {return BuildInt64(param); break; }			//Entero 64 bits
			case VT_R4: {return BuildSingle(param); break; }		//Coma flotante 32 bits
			case VT_R8: {return BuildDouble(param); break; }		//Coma flotante 64 bits
			case VT_DECIMAL: {return BuildSingle(param); break; }   //Coma flotante
			default:
				{
					return Parameter();
					break;
				}
		}
	return Parameter();
}

bool VariantUtils::BuildArray(Parameter* p, VARIANTARG* param)
{
	
	IDispatch *pDisp = param->pdispVal;
	pDisp->AddRef();

	DISPPARAMS dispparamsNoArgs = { NULL, NULL, 0, 0 };

	VARIANT var;
	VariantInit(&var);

	HRESULT hr = pDisp->Invoke(DISPID_NEWENUM, IID_NULL, GetUserDefaultLCID(), DISPATCH_PROPERTYGET, &dispparamsNoArgs, &var, NULL, NULL);
	
	if (SUCCEEDED(hr)) 
	{
		if (var.vt == VT_UNKNOWN) 
		{
			
			IEnumVARIANT *pEnum = NULL;
			if SUCCEEDED(var.punkVal->QueryInterface(IID_IEnumVARIANT, (void**) &pEnum)) 
			{
				int count = 0;
				int arraySize = 0;
				VARIANT item;
				
				VariantInit(&item);

				pEnum->Reset();
				//Contamos los elementos del array. No hay un size()??
				while ( (pEnum->Next(1, &item, NULL) && S_FALSE) != S_FALSE) { arraySize++; VariantClear(&item);}
				
				Parameter* params = new Parameter[arraySize];
				
				pEnum->Reset();
				while ( (pEnum->Next(1, &item, NULL) && S_FALSE) != S_FALSE) 
				{
					params[count] = BuildParameter(&item);
					VariantClear(&item);
					count++;
				}
				VariantClear(&item);
				p->SetArraySize(arraySize);
				p->SetDataType(PARAM_TYPE_ARRAY);
				p->SetValue((void*)params);
			   pEnum->Release();
			   pEnum = 0;
			}
			var.punkVal->Release();
			var.punkVal = 0;
		}
	}
	VariantClear(&var);
	pDisp->Release();
	return true;
}


bool VariantUtils::BuildSingle(Parameter* p, VARIANTARG* param)
{
	if(0 == p) return false;
	float* val = new float();
	*val = param->fltVal;
	p->SetDataType(PARAM_TYPE_SINGLE);
	p->SetValue((void*)val);
	return true;
}


bool VariantUtils::BuildDouble(Parameter* p, VARIANTARG* param)
{
	if(0 == p) return false;
	double* val = new double();
	*val = param->dblVal;
	p->SetDataType(PARAM_TYPE_DOUBLE);
	p->SetValue((void*)val);
	return true;
}

bool VariantUtils::BuildInt64(Parameter* p, VARIANTARG* param)
{
	if(0 == p) return false;
	__int64* val = new __int64();
	*val = (__int64)param->pintVal;
	p->SetDataType(PARAM_TYPE_INT64);
	p->SetValue((void*)val);
	return true;
}

bool VariantUtils::BuildInt32(Parameter* p, VARIANTARG* param)
{
	if(0 == p) return false;
	__int32* val = new __int32();
	*val = param->intVal;
	p->SetDataType(PARAM_TYPE_INT32);
	p->SetValue((void*)val);
	return true;
}

bool VariantUtils::BuildBool(Parameter* p, VARIANTARG* param)
{
	if(0 == p) return false;
	short tval = param->boolVal;
	bool* val = new bool();
	*val = (tval==0?false:true);
	p->SetDataType(PARAM_TYPE_BOOL);
	p->SetValue((void*)val);
	
	return true;
}
bool VariantUtils::BuildBSTR(Parameter* p, VARIANTARG* param)
{
	if(0 == p) return false;
	CComBSTR* val = new CComBSTR(param->bstrVal);
	p->SetDataType(PARAM_TYPE_BSTR);
	p->SetValue((void*)val);
	return true;
}



Parameter VariantUtils::BuildSingle(VARIANTARG* param)
{
	Parameter p;
	BuildSingle(&p, param);
	return p;
}


Parameter VariantUtils::BuildDouble(VARIANTARG* param)
{
	Parameter p;
	BuildDouble(&p, param);
	return p;
}

Parameter VariantUtils::BuildInt64(VARIANTARG* param)
{
	Parameter p;
	BuildInt64(&p, param);
	return p;
}

Parameter VariantUtils::BuildInt32(VARIANTARG* param)
{
	Parameter p;
	BuildInt32(&p, param);
	return p;
}

Parameter VariantUtils::BuildBool(VARIANTARG* param)
{
	Parameter p;
	BuildBool(&p, param);
	
	return p;
}
Parameter VariantUtils::BuildBSTR(VARIANTARG* param)
{
	Parameter p;
	BuildBSTR(&p, param);
	return p;
}


Parameter VariantUtils::BuildArray(VARIANTARG* param)
{
	Parameter p;

	IDispatch *pDisp = param->pdispVal;
	pDisp->AddRef();

	DISPPARAMS dispparamsNoArgs = { NULL, NULL, 0, 0 };

	VARIANT var;
	VariantInit(&var);

	HRESULT hr = pDisp->Invoke(DISPID_NEWENUM, IID_NULL, GetUserDefaultLCID(), DISPATCH_PROPERTYGET, &dispparamsNoArgs, &var, NULL, NULL);
	
	if (SUCCEEDED(hr)) 
	{
		if (var.vt == VT_UNKNOWN) 
		{
			
			IEnumVARIANT *pEnum = NULL;
			if SUCCEEDED(var.punkVal->QueryInterface(IID_IEnumVARIANT, (void**) &pEnum)) 
			{
				int count = 0;
				int arraySize = 0;
				VARIANT item;
				
				VariantInit(&item);

				pEnum->Reset();
				//Contamos los elementos del array. No hay un size()??
				while ( (pEnum->Next(1, &item, NULL) && S_FALSE) != S_FALSE) { arraySize++; VariantClear(&item);}
				
				Parameter* params = new Parameter[arraySize];
				
				pEnum->Reset();
				while ( (pEnum->Next(1, &item, NULL) && S_FALSE) != S_FALSE) 
				{
					params[count] = BuildParameter(&item);
					VariantClear(&item);
					count++;
				}
				VariantClear(&item);
				p.SetArraySize(arraySize);
				p.SetDataType(PARAM_TYPE_ARRAY);
				p.SetValue((void*)params);
			   pEnum->Release();
			   pEnum = 0;
			}
			var.punkVal->Release();
			var.punkVal = 0;
		}
	}
	VariantClear(&var);
	pDisp->Release();
	return p;
}
