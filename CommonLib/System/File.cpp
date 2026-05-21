#include "File.h"
#include "List.h"

StaticString	File::WorkingDirectory;
StaticString	File::WorkingDataDirectory;

void File::SetWorkingDirectory(const Char* path) {
	WorkingDirectory = String::Format("%s\\", path);
	WorkingDataDirectory = String::Format("%sData\\", WorkingDirectory.CStr());
}

const Char* File::BuildFullPath(StaticString& relativePath) {
	return String::Format("%s\\%s", WorkingDirectory.CStr(), relativePath.CStr());
}

const Char* File::BuildFullPath(const Char* relativePath) {
	return String::Format("%s\\%s", WorkingDirectory.CStr(), relativePath);
}

const Char* File::RebuildFullPath(String& path, const Char* removePath, const Char* addPath) {
	return RebuildFullPath(path.CStr(), removePath, addPath);
}

const Char* File::RebuildFullPath(const Char* path, const Char* removePath, const Char* addPath) {
	Int32 index = String::FindIndexOf(path, removePath);
	if (index < 0) {
		UInt32 pathLen = String::Length(path);
		for (int ix = 0, count = STATIC_ARRAY_LENGTH(File::Delimiters); ix < count; ++ix) {
			index = String::FindLastOf(path, pathLen, File::Delimiters[ix]);
			if (index >= 0) {
				break;
			}
		}
	}
	return String::Replace(path, 0, index + 1, addPath);
}

const Char* File::RebuildFullPath(String& path, const Char* newWorkingDirPath) {
	return RebuildFullPath(path, "", newWorkingDirPath);
}

const Char* File::RebuildFullPath(const Char* path, const Char* newWorkingDirPath) {
	return RebuildFullPath(path, File::WorkingDirectory.CStr(), newWorkingDirPath);
}

const Char* File::RebuildFullPath(String& inoutFullPath) {
	return RebuildFullPath(inoutFullPath, File::WorkingDirectory.CStr());
}

const Char* File::RebuildFullDataPath(String& inoutFullPath) {
	return RebuildFullPath(inoutFullPath, File::WorkingDataDirectory.CStr());
}

const Char* File::RebuildFullPath(const Char* inFullPath) {
	return RebuildFullPath(inFullPath, File::WorkingDirectory.CStr());
}

const Char* File::RebuildFullDataPath(const Char* inPath) {
	return RebuildFullPath(inPath, File::WorkingDataDirectory.CStr(), File::WorkingDataDirectory.CStr());
}

Bool File::ReadContents(List<Byte>& outContents) {
	assert(IsOpen());

	errno = 0;//Reset the error num

	auto startIndex = outContents.Length();
	outContents.EnsureSize(startIndex + Length());
	UInt32 numElementsRead = fread_s((void*)&outContents[startIndex], outContents.Length() - startIndex, 1, Length(), m_File);
	assert(numElementsRead <= Length());
	auto newLength = numElementsRead + startIndex;
	outContents.Resize(newLength);//I have seen it load extra stuff to the end when loading JSON.  We need to resize to correct lenth

	assert(!ReportedError());
	return true;
}

#include "StackString.h"
#include <windows.h>
const Char* FilePath::GetLocalPath(const StaticString& fullPath) {
	try {
		STACK_STRING(systemPath, MAX_PATH);

		Int32 systemPathLength = GetCurrentDirectory(systemPath.Length(), systemPath.Str());
		auto index = fullPath.FindIndexOf(systemPath.CStr());
		if (index < 0) {
			return GetFileName(fullPath);
		}
		assert(!index);//Index better be at start
		return &fullPath.CStr()[index + systemPathLength + 1];
	}
	catch (...) {
		return	nullptr;
	}
}