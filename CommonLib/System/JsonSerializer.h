#pragma once

#include "Typedefs.h"
#include "rapidjson\document.h"

class StaticString;

namespace rapidjson {
	Bool	LoadFrom(const StaticString& filePath, rapidjson::Document& outDoc);
	Bool	LoadFrom(const Char* filePath, rapidjson::Document& outDoc);
};

template< typename TData >
class JsonSerializer;

template<>
class JsonSerializer<UInt32> {
public:
	static UInt32	ReadFrom(const rapidjson::Value& jsonVal) {
		assert(jsonVal.IsUint());
		return jsonVal.GetUint();
	}

	static void	ReadFrom(const rapidjson::Value& jsonVal, UInt32& outVal) {
		outVal = ReadFrom(jsonVal);
	}
};

template<>
class JsonSerializer<Int32> {
public:
	static Int32	ReadFrom(const rapidjson::Value& jsonVal) {
		assert(jsonVal.IsInt());
		return jsonVal.GetInt();
	}

	static void	ReadFrom(const rapidjson::Value& jsonVal, Int32& outVal) {
		outVal = ReadFrom(jsonVal);
	}
};

template<>
class JsonSerializer<Float32> {
public:
	static Float32	ReadFrom(const rapidjson::Value& jsonVal) {
		assert(jsonVal.IsDouble());
		return (Float32)jsonVal.GetDouble();
	}

	static void	ReadFrom(const rapidjson::Value& jsonVal, Float32& outVal) {
		outVal = ReadFrom(jsonVal);
	}
};

template<>
class JsonSerializer<Float64> {
public:
	static Float64	ReadFrom(const rapidjson::Value& jsonVal) {
		assert(jsonVal.IsDouble());
		return jsonVal.GetDouble();
	}

	static void	ReadFrom(const rapidjson::Value& jsonVal, Float64& outVal) {
		outVal = ReadFrom(jsonVal);
	}
};