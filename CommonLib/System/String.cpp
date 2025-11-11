#include "System/String.h"
#include "StackString.h"

const Char* String::Replace(const Char* str, UInt32 offset, Int32 count, const Char* newSubString) {
	auto removeStart = str + offset;
	auto removeEnd = removeStart + count;

	Int32 sizeInBytes = StrLen(removeEnd) + 1;//Including null terminator
	STACK_STRING(post, sizeInBytes);
	String::StrCpy(post.CStr(), sizeInBytes, removeEnd);

	sizeInBytes = offset + 1;//Including null terminator
	STACK_STRING(pre, sizeInBytes);
	if (sizeInBytes > 1) {
		String::StrCpy(pre.CStr(), sizeInBytes, str);
	}
	else {
		assert(sizeInBytes == 1);
		pre = "";
	}

	const Char* newStr = String::Format("%s%s%s", pre.CStr(), newSubString, post.CStr());
	return newStr;
}