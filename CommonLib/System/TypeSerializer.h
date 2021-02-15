#pragma once

#include "../System/Typedefs.h"
#include "../System/String.h"
#include "../System/json.h"
#include <type_traits>

template<typename _TData, Bool _IsClass>
class JsonSerializerHelper {
public:
	static _TData	ReadFrom(json_value* root) {
		return _TData(root);
	}

	static void	ReadFrom(json_value* root, _TData& val) {
		if (!root) {
			return;
		}

		int length = root->u.object.length;
		for (int x = 0; x < length; x++) {
			ReadFrom(root->u.object.values[x].value);
		}
	}
};

template<>
class JsonSerializerHelper<int, false> {
public:
	static int	ReadFrom(json_value* root) {
		assert(root->type == json_type::json_integer);
		return root->u.integer;
	}

	static void	ReadFrom(json_value* root, int& val) {

	}
};

template<>
class JsonSerializerHelper<float, false> {
public:
	static int	ReadFrom(json_value* root) {
		assert(root->type == json_type::json_double);
		return root->u.dbl;
	}

	static void	ReadFrom(json_value* root, float& val) {

	}
};

template<>
class JsonSerializerHelper<double, false> {
public:
	static int	ReadFrom(json_value* root) {
		assert(root->type == json_type::json_double);
		return root->u.dbl;
	}

	static void	ReadFrom(json_value* root, double& val) {

	}
};

template<>
class JsonSerializerHelper<const char*, false> {
public:
	static const char*	ReadFrom(json_value* root) {
		assert(root->type == json_type::json_double);
		return root->u.string.ptr;
	}

	static void	ReadFrom(json_value* root, const char* val, int length) {
		
	}
};
template<>
class JsonSerializerHelper<String, false> {
public:
	static const char*	ReadFrom(json_value* root) {
		assert(root->type == json_type::json_double);
		return root->u.string.ptr;
	}

	static void	ReadFrom(json_value* root, String& val) {
		val.ReadFrom(root);
	}
};


template<class _TData>
class TypeSerializer {
public:
	/*static _TData	ReadFrom(File& file) {
		return _TData( file );
	}*/

	static _TData	ReadFrom(json_value* root) {
		return _TData(root);
	}

	/*static void	ReadFrom(File& file, _TData& val) {
		file.Read(val);
	}*/

	static void	ReadFrom(json_value* root, _TData& val) {
		val.ReadFrom(root);
	}

	/*static void	WriteTo(File& file, const _TData& val) {
		file.Write(val);
	}*/
};

template<class _TData>
class TypeSerializer<_TData*> {
public:
	/*static _TData	ReadFrom(File& file) {
		return new _TData(file);
	}*/

	static _TData	ReadFrom(json_value* root) {
		return new _TData(root);
	}

	/*static void	ReadFrom(File& file, _TData* val) {
		val->ReadFrom(file);
	}*/

	static void	ReadFrom(json_value* root, _TData* val) {
		val->ReadFrom(root);
	}

	/*static void	WriteTo(File& file, const _TData* val) {
		val->WriteTo(file);
	}*/
};