#include "System/Configuration.h"
#include "System/StackString.h"
#include <windows.h>

Bool ConfigurationSection::LoadFrom(const Char* sectionName, const Char* fileName) {
	try {
		STACK_STRING(sectionKeys, 1024);
		STACK_STRING(sectionVal, 64);
		GetPrivateProfileString(sectionName, NULL, "", sectionKeys.Str(), sectionKeys.Allocated(), fileName);

		const Char* curKey = sectionKeys.CStr();
		while (curKey && curKey[0]) {
			int l = String::Length(curKey);

			GetPrivateProfileString(sectionName, curKey, "", sectionVal.Str(), sectionVal.Allocated(), fileName);

			m_KeyValues.Add(curKey, sectionVal.CStr());

			curKey += (l + 1);
		}

		return true;
	}
	catch (...) {
		return false;
	}
}

Bool Configuration::LoadFrom(const Char* relativeFilePath) {
	m_FilePath = relativeFilePath;

	try {
		STACK_STRING(sectionNames, 1024);
		GetPrivateProfileString(NULL, NULL, "", sectionNames.Str(), sectionNames.Allocated(), m_FilePath.CStr());

		const Char* curSec = sectionNames.CStr();
		while (curSec && curSec[0]) {
			int l = String::Length(curSec);

			ConfigurationSection section;
			section.LoadFrom(curSec, m_FilePath.CStr());
			m_Sections.Add(curSec, section);

			curSec += (l + 1);
		}

		return true;
	}
	catch (...) {
		return false;
	}
}

Bool Configuration::GetValue(const StaticString& sectionName, const StaticString& key, String& outValue) const {
	const ConfigurationSection* section{};
	if (!GetSection(sectionName, section)) {
		return false;
	}
	return section->GetValue(key, outValue);
}

Bool Configuration::GetSection(const StaticString& sectionName, const ConfigurationSection*& outConfig) const {
	assert(m_Sections.Contains(sectionName));
	outConfig = &m_Sections[sectionName];
	return true;
}