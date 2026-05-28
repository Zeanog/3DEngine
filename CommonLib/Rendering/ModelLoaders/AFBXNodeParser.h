#pragma once

#include "System/Typedefs.h"
#include "System/Singleton.h"
#include "System/StaticString.h"
#include "System/List.h"
#include "System/Map.h"
#include "Rendering/ModelLoaders/FbxConversions.h"

template<typename TFbxObj, typename TOutDataList>
class AFBXParser;

template<typename TFbxObj, typename TOutData>
class AFBXParser<TFbxObj, TYPELIST_1(TOutData)> {
	CLASS_TYPEDEFS(AFBXParser)

public:
	virtual Bool	Parse(TFbxObj* obj, TOutData& outData) = 0;
	virtual Bool	CanParse(TFbxObj* obj) const { return true; }

	virtual void	Clear() = 0;
};

template<typename TFbxObj, typename TOutData1, typename TOutData2>
class AFBXParser<TFbxObj, TYPELIST_2(TOutData1, TOutData2)> {
	CLASS_TYPEDEFS(AFBXParser)

public:
	virtual Bool	Parse(TFbxObj* obj, TOutData1& outData1, TOutData2& outData2) = 0;
	virtual Bool	CanParse(TFbxObj* obj) const { return true; }

	virtual void	Clear() = 0;
};

template<typename TFbxObj, typename TOutData1, typename TOutData2, typename TOutData3>
class AFBXParser<TFbxObj, TYPELIST_3(TOutData1, TOutData2, TOutData3)> {
	CLASS_TYPEDEFS(AFBXParser)

public:
	virtual Bool	Parse(TFbxObj* obj, TOutData1& outData1, TOutData2& outData2, TOutData3& outData3) = 0;
	virtual Bool	CanParse(TFbxObj* obj) const { return true; }

	virtual void	Clear() = 0;
};

template<typename TOutDataList>
class AFBXSceneParser;

template<typename TOutData>
class AFBXSceneParser<TYPELIST_1(TOutData)> : public AFBXParser<fbxsdk::FbxScene, TYPELIST_1(TOutData)> {
	typedef AFBXParser<fbxsdk::FbxScene, TYPELIST_1(TOutData)> _TSuper;
	INHERITED_CLASS_TYPEDEFS(AFBXSceneParser, _TSuper)

public:
	virtual Bool	Parse(fbxsdk::FbxScene* obj, TOutData& outData) = 0;
	virtual Bool	CanParse(fbxsdk::FbxScene* scene) const override { return true; }
	virtual void	Clear() = 0;
};

template<typename TOutData1, typename TOutData2>
class AFBXSceneParser<TYPELIST_2(TOutData1, TOutData2)> : public AFBXParser<fbxsdk::FbxScene, TYPELIST_2(TOutData1, TOutData2)> {
	typedef AFBXParser<fbxsdk::FbxScene, TYPELIST_2(TOutData1, TOutData2)> _TSuper;
	INHERITED_CLASS_TYPEDEFS(AFBXSceneParser, _TSuper)

public:
	virtual Bool	Parse(fbxsdk::FbxScene* obj, TOutData1& outData1, TOutData2& outData2) = 0;
	virtual Bool	CanParse(fbxsdk::FbxScene* scene) const override { return true; }
	virtual void	Clear() = 0;

	template<typename TCallback>
	void			VisitNode(fbxsdk::FbxNode* node, TOutData1& outData1, TOutData2& outData2, TCallback callback) const {
		callback(node, outData1, outData2);
		for (int ix = 0; ix < node->GetChildCount(); ++ix) {
			VisitNode(node->GetChild(ix), outData1, outData2, callback);
		}
	}

	template<typename TCallback, typename ...TData>
	Bool			VisitNode(fbxsdk::FbxNode* node, TCallback callback, TData... data) const {
		if (!callback(node, data...)) {
			return false;
		}
		for (int ix = 0; ix < node->GetChildCount(); ++ix) {
			if (!VisitNode(node->GetChild(ix), callback, data...)) {
				return false;
			}
		}
		return true;
	}
};

template<typename TOutData1, typename TOutData2, typename TOutData3>
class AFBXSceneParser<TYPELIST_3(TOutData1, TOutData2, TOutData3)> : public AFBXParser<fbxsdk::FbxScene, TYPELIST_3(TOutData1, TOutData2, TOutData3)> {
	typedef AFBXParser<fbxsdk::FbxScene, TYPELIST_3(TOutData1, TOutData2, TOutData3)> _TSuper;
	INHERITED_CLASS_TYPEDEFS(AFBXSceneParser, _TSuper)

public:
	virtual Bool	Parse(fbxsdk::FbxScene* obj, TOutData1& outData1, TOutData2& outData2, TOutData3& outData3) = 0;
	virtual Bool	CanParse(fbxsdk::FbxScene* scene) const override { return true; }
	virtual void	Clear() = 0;

	template<typename TCallback, typename ...TData>
	Bool			VisitNode(fbxsdk::FbxNode* node, TCallback callback, TData... data) const {
		if (!callback(node, data...)) {
			return false;
		}
		for (int ix = 0; ix < node->GetChildCount(); ++ix) {
			if (!VisitNode(node->GetChild(ix), callback, data...)) {
				return false;
			}
		}
		return true;
	}
};

template<typename TOutDataList>
class AFBXNodeParser;

#include "Rendering/Mesh.h"
template<typename TOutData>
class AFBXNodeParser<TYPELIST_1(TOutData)> : public AFBXParser<fbxsdk::FbxNode, TYPELIST_1(TOutData)> {
	typedef AFBXParser<fbxsdk::FbxNode, TYPELIST_1(TOutData)> _TSuper;
	INHERITED_CLASS_TYPEDEFS(AFBXNodeParser, _TSuper)

protected:
	fbxsdk::FbxNodeAttribute::EType		m_NodeType = fbxsdk::FbxNodeAttribute::eNull;

public:
	DEFINE_GETSET(NodeType)

	virtual Bool CanParse(fbxsdk::FbxNode* node) const override {
		if (!node) {
			return false;
		}
		auto attr = node->GetNodeAttribute();
		if (!attr) {
			return false;
		}
		return attr->GetAttributeType() == NodeType();
	}
};

template<typename TOutData1, typename TOutData2>
class AFBXNodeParser<TYPELIST_2(TOutData1, TOutData2)> : public AFBXParser<fbxsdk::FbxNode, TYPELIST_2(TOutData1, TOutData2)> {
	typedef AFBXParser<fbxsdk::FbxNode, TYPELIST_2(TOutData1, TOutData2)> _TSuper;
	INHERITED_CLASS_TYPEDEFS(AFBXNodeParser, _TSuper)

protected:
	fbxsdk::FbxNodeAttribute::EType		m_NodeType = fbxsdk::FbxNodeAttribute::eNull;

public:
	DEFINE_GETSET(NodeType)

	virtual Bool CanParse(fbxsdk::FbxNode* node) const override {
		if (!node) {
			return false;
		}
		auto attr = node->GetNodeAttribute();
		if (!attr) {
			return false;
		}
		return attr->GetAttributeType() == NodeType();
	}
};