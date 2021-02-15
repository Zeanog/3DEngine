#include "System/Configuration.h"
#include "System/StackString.h"
#include <windows.h>
#include <cstring>

Bool ConfigurationSection::LoadFrom(const Char* sectionName, const Char* fileName) {
	STACK_STRING(sectionKeys, 512);
	STACK_STRING(sectionVal, 64);
	GetPrivateProfileString(sectionName, NULL, "", sectionKeys.CStr(), sectionKeys.Allocated(), fileName);

	const Char* curKey = sectionKeys.CStr();
	while (curKey && curKey[0]) {
		int l = String::StrLen(curKey);

		GetPrivateProfileString(sectionName, curKey, "", sectionVal.CStr(), sectionVal.Allocated(), fileName);

		m_KeyValues.Add(curKey, sectionVal.CStr());

		curKey += (l + 1);
	}

	return true;
}

Bool Configuration::LoadFrom(const Char* relativeFilePath) {
	m_FilePath = relativeFilePath;

	STACK_STRING(sectionNames, 1024);
	GetPrivateProfileString(NULL, NULL, "", sectionNames.CStr(), sectionNames.Allocated(), m_FilePath.CStr());
	
	const Char* curSec = sectionNames.CStr();
	while( curSec && curSec[0] ) {
		int l = String::StrLen(curSec);

		ConfigurationSection section;
		section.LoadFrom(curSec, m_FilePath.CStr());
		m_Sections.Add(curSec, section);

		curSec += (l + 1);
	}

	return true;
}

Bool Configuration::GetValue(const StaticString& section, const StaticString& key, String& outValue) const {
	STACK_STRING(tempValue, 256);
	GetPrivateProfileString(section.CStr(), key.CStr(), "", tempValue.CStr(), tempValue.Allocated(), m_FilePath.CStr());
	outValue = tempValue.CStr();
	return true;
}

Bool Configuration::GetSection(const StaticString& section, const ConfigurationSection*& outConfig) const {
	outConfig = &m_Sections[section.CStr()];
	return true;
}