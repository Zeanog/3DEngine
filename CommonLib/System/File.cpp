#include "File.h"
#include "List.h"

Bool File::ReadContents(List<Byte>& outContents) {
	assert(IsOpen());

	outContents.Resize(Length());
	UInt32 numElementsRead = fread_s((void*)&outContents[0], outContents.Length(), 1, outContents.Length(), m_File);
	outContents.Resize(numElementsRead);//I have seen it load extra stuff to the end.  We need to resize to correct lenth

	assert(!ReportedError());
	return numElementsRead <= outContents.Length();
}