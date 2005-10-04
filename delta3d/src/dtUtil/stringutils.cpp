#include "dtUtil/stringutils.h"

#include <cstdio>                         // for sscanf

float dtUtil::ToFloat(const std::string& str)
{
   float tmp(0.0f);
   sscanf(str.c_str(), " %f", &tmp);
   return tmp;
}

