#include "LoadDefines.h"

int TabColIntValue::svalue = -1;
TabColIntValue::TabColIntValue()
{
	eType = PxcUtil::CCSVTableOperator::ECol_Int;
}

long long TabColInt64Value::svalue = -1;
TabColInt64Value::TabColInt64Value()
{
	eType = PxcUtil::CCSVTableOperator::ECol_Int64;
}

float TabColFloatValue::svalue = -1.0f;
TabColFloatValue::TabColFloatValue()
{
	eType = PxcUtil::CCSVTableOperator::ECol_Float;
}

std::string TabColStringValue::svalue = std::string();
TabColStringValue::TabColStringValue()
{
	eType = PxcUtil::CCSVTableOperator::ECol_String;
}

std::vector<int> TabColIntArray::svalue = std::vector<int>();
TabColIntArray::TabColIntArray()
{
	eType = PxcUtil::CCSVTableOperator::ECol_IntArray;
}

std::vector<long long> TabColInt64Array::svalue = std::vector<long long>();
TabColInt64Array::TabColInt64Array()
{
	eType = PxcUtil::CCSVTableOperator::ECol_Int64Array;
}

std::vector<float> TabColFloatArray::svalue = std::vector<float>();
TabColFloatArray::TabColFloatArray()
{
	eType = PxcUtil::CCSVTableOperator::ECol_FloatArray;
}

std::vector<std::string> TabColStringArray::svalue = std::vector<std::string>();
TabColStringArray::TabColStringArray()
{
	eType = PxcUtil::CCSVTableOperator::ECol_StringArray;
}