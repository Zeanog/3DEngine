#include "JsonLoader.h"
#include "System/File.h"

namespace rapidjson {
	Bool	LoadFrom(const StaticString& filePath, rapidjson::Document& outDoc) {
		return LoadFrom(filePath.CStr(), outDoc);
	}

	Bool	LoadFrom(const Char* filePath, rapidjson::Document& outDoc) {
		File file;
		if (!file.Open(filePath, "r")) {
			return false;
		}

		if (file.Length() <= 0) {
			return true;
		}

		// Avoids an allocation.  
		// But could take up a lot of stack space.  
		// If this becomes an issue change back to String and eat the allocation
		STACK_STRING(jsonBody, file.Length());
		//String	jsonBody;
		verify(file.ReadContents(jsonBody));
		outDoc.Parse(jsonBody.CStr());
		return !outDoc.IsNull();
	}
}
