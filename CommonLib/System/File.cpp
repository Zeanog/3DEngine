#include "File.h"
#include "List.h"

StaticString	File::DefaultDataPath;

void File::SetWorkingDirectory(const Char* path) {
	DefaultDataPath = String::Format("%s\\Data\\", path);
}

const Char* File::BuildFullPath(StaticString& relativePath) {
	return String::Format("%s%s", DefaultDataPath.CStr(), relativePath.CStr());
}

const Char* File::BuildFullPath(const Char* relativePath) {
	return String::Format("%s%s", DefaultDataPath.CStr(), relativePath);
}

const Char* File::RebuildFullPath(String& inoutFullPath) {
	Int32 index = inoutFullPath.FindIndexOf(File::DefaultDataPath.Str());
	if (index < 0) {
		for (int ix = 0, count = STATIC_ARRAY_LENGTH(File::Delimiters); ix < count; ++ix) {
			index = inoutFullPath.FindLastOf(File::Delimiters[ix]);
			if (index >= 0) {
				inoutFullPath.Replace(0, index + 1, File::DefaultDataPath.CStr());
				break;
			}
		}
	}
	return inoutFullPath.CStr();
}

const Char* File::RebuildFullPath(const Char* inFullPath) {
	Int32 index = String::FindIndexOf(inFullPath, File::DefaultDataPath.CStr());
	if (index < 0) {
		for (int ix = 0, count = STATIC_ARRAY_LENGTH(File::Delimiters); ix < count; ++ix) {
			index = String::FindLastOf(inFullPath, File::Delimiters[ix]);
			if (index >= 0) {
				return String::Replace(inFullPath, 0, index + 1, File::DefaultDataPath.CStr());
			}
		}
	}
	return inFullPath;
}

Bool File::ReadContents(List<Byte>& outContents) {
	assert(IsOpen());

	errno = 0;//Reset the error num

	outContents.EnsureSize(Length());
	UInt32 numElementsRead = fread_s((void*)&outContents[0], outContents.Length(), 1, outContents.Length(), m_File);
	outContents.Resize(numElementsRead);//I have seen it load extra stuff to the end when loading JSON.  We need to resize to correct lenth

	assert(!ReportedError());
	return numElementsRead <= outContents.Length();
}