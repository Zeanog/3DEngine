#pragma once

#include "System/Typedefs.h"
#include "System/StaticString.h"
#include "rapidjson\document.h"

namespace rapidjson {
	Bool	LoadFrom(const StaticString& filePath, rapidjson::Document& outDoc);
	Bool	LoadFrom(const Char* filePath, rapidjson::Document& outDoc);
}
