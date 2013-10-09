#pragma once

#include "Events/Parameter.h"

class VariantUtils
{
private:
	static bool			BuildInt32(Parameter* dst, VARIANTARG* param);
	static bool			BuildInt64(Parameter* dst, VARIANTARG* param);
	static bool			BuildBSTR(Parameter* dst, VARIANTARG* param);
	static bool			BuildString(Parameter* dst, VARIANTARG* param);
	static bool			BuildSingle(Parameter* dst, VARIANTARG* param);
	static bool			BuildDouble(Parameter* dst, VARIANTARG* param);
	static bool			BuildBool(Parameter* dst, VARIANTARG* param);
	static bool			BuildChar(Parameter* dst, VARIANTARG* param);
	static bool			BuildArray(Parameter* dst, VARIANTARG* param);

	static Parameter	BuildInt32(VARIANTARG* param);
	static Parameter	BuildInt64(VARIANTARG* param);
	static Parameter	BuildBSTR(VARIANTARG* param);
	static Parameter	BuildString(VARIANTARG* param);
	static Parameter	BuildSingle(VARIANTARG* param);
	static Parameter	BuildDouble(VARIANTARG* param);
	static Parameter	BuildBool(VARIANTARG* param);
	static Parameter	BuildChar(VARIANTARG* param);
	static Parameter	BuildArray(VARIANTARG* param);

public:
	
	static bool			BuildParameter(Parameter* dst, VARIANTARG* param);
	static Parameter	BuildParameter(VARIANTARG* param);
	static bool			CreateVariant(VARIANT*, Parameter* p);
};