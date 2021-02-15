#pragma once

#include "Typedefs.h"
#include "rapidjson\document.h"
#include "System\json.h"

using namespace rapidjson;

template< typename TData >
class JsonSerializer;

template<>
class JsonSerializer<UInt32> {
public:
	static UInt32	ReadFrom(const Value& jsonVal) {
		assert(jsonVal.IsUint());
		return jsonVal.GetUint();
	}

	static void	ReadFrom(const Value& jsonVal, UInt32& outVal) {
		outVal = ReadFrom(jsonVal);
	}

	static void	ReadFrom(json_value* jsonVal, UInt32& outVal) {
		outVal = (UInt32)jsonVal->u.integer;
	}

	static UInt32	ReadFrom(json_value* jsonVal) {
		return (UInt32)jsonVal->u.integer;
	}
};

template<>
class JsonSerializer<Int32> {
public:
	static Int32	ReadFrom(const Value& jsonVal) {
		assert(jsonVal.IsInt());
		return jsonVal.GetInt();
	}

	static void	ReadFrom(const Value& jsonVal, Int32& outVal) {
		outVal = ReadFrom(jsonVal);
	}

	static Int32	ReadFrom(json_value* jsonVal) {
		return (Int32)jsonVal->u.integer;
	}
};

template<>
class JsonSerializer<Float32> {
public:
	static Float32	ReadFrom(const Value& jsonVal) {
		assert(jsonVal.IsFloat());
		return jsonVal.GetFloat();
	}

	static void	ReadFrom(const Value& jsonVal, Float32& outVal) {
		outVal = ReadFrom(jsonVal);
	}

	static Float32	ReadFrom(json_value* jsonVal) {
		return (Float32)jsonVal->u.dbl;
	}
};

template<>
class JsonSerializer<Float64> {
public:
	static Float64	ReadFrom(const Value& jsonVal) {
		assert(jsonVal.IsDouble());
		return jsonVal.GetDouble();
	}

	static void	ReadFrom(const Value& jsonVal, Float64& outVal) {
		outVal = ReadFrom(jsonVal);
	}

	static Float64	ReadFrom(json_value* jsonVal) {
		return jsonVal->u.dbl;
	}
};