#pragma once

#include "..\System/String.h"
#include "..\System/StaticString.h"
#include "..\System/Map.h"

class ConfigurationSection {
protected:
	Map<StaticString, StaticString>	m_KeyValues;

public:
	Bool			LoadFrom(const Char* sectionName, const Char* fileName);

	/*template<typename TValue>
	Bool			GetValue(const StaticString& key, TValue& outValue) const;*/

	Bool			GetValue(const StaticString& key, const Char*& outValue) const {
		try {
			outValue = m_KeyValues.Find(key).CStr();
			return true;
		}
		catch (...) {
			//AOB: Print something out here
			return false;
		}
	}

	Bool			GetValue(const StaticString& key, String& outValue) const {
		try {
			outValue = m_KeyValues[key].CStr();
			return true;
		}
		catch (...) {
			//AOB: Print something out here
			return false;
		}
	}

	Bool			GetValue(const StaticString& key, Float32& outValue) const {
		String str;
		if (!GetValue(key, str)) {
			return false;
		}

		scanf_s(str.CStr(), "%f", &outValue);
		return true;
	}

	Bool			GetValue(const StaticString& key, Int32& outValue) const {
		String str;
		if (!GetValue(key, str)) {
			return false;
		}

		scanf_s(str.CStr(), "%d", &outValue);
		return true;
	}

	Bool			GetValue(const StaticString& key, UInt32& outValue) const {
		String str;
		if (!GetValue(key, str)) {
			return false;
		}

		scanf_s(str.CStr(), "%d", &outValue);
		return true;
	}
};

class Configuration {
public:

protected:
	StaticString		m_FilePath;
	Map<StaticString, ConfigurationSection>	m_Sections;

public:
	Bool	LoadFrom(const Char* relativeFilePath);

	Bool	GetValue(const StaticString& section, const StaticString& key, String& outValue) const;

	Bool	GetSection(const StaticString& section, const ConfigurationSection*& outConfig) const;
};