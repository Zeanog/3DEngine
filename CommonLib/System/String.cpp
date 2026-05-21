#include "System/String.h"
#include "System/StackString.h"
#include <stdarg.h>

const Char* String::Replace(const Char* str, UInt32 offset, Int32 count, const Char* newSubString) {
	try {
		auto removeStart = str + offset;
		auto removeEnd = removeStart + count;

		UInt32 sizeInBytes = Length(removeEnd) + 1;//Including null terminator
		STACK_STRING(post, sizeInBytes);
		String::StrCpy(post.Str(), sizeInBytes, removeEnd);

		sizeInBytes = offset + 1;//Including null terminator
		STACK_STRING(pre, sizeInBytes);
		if (sizeInBytes > 1) {
			String::StrCpy(pre.Str(), sizeInBytes, str);
		}
		else {
			assert(sizeInBytes == 1);
			pre = "";
		}

		return String::Format("%s%s%s", pre.CStr(), newSubString, post.CStr());
	}
	catch (...) {
		return nullptr;
	}
}

Bool String::StrCpy(Char* dst, UInt32 size, const Char* src) {
	return !strcpy_s(dst, size, src);
}

Char* String::Format(const char* format, ...) {
	va_list args;
	va_start(args, format);
	auto str = Format(format, args);
	va_end(args);
	return str;
}