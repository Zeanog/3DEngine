#include "SceneLoader.h"
#include "System/File.h"
#include "ModelManager.h"
#include "Audio/AudioSystem.h"
#include "System/JsonValueParsers.h"
#include "Math/Reflector_glm.h"
#include "Rendering/Model.h"
#include "System/Singleton.h"

void SceneLoader::ParseModels(const rapidjson::Value& value) {
	glm::vec3 pos;
	glm::vec3 rot;

	FOREACH(modelIter, value) {
		auto&& pathVal = modelIter->FindMember("path");//Linear search
		auto modelPath = File::BuildFullPath(pathVal->value.GetString());
		auto model = Singleton<ModelManager>::GetInstance()->Get(modelPath);

		ParseOrientation(*modelIter, pos, rot);
		model->Position(pos);
		auto eulerRad = glm::radians(rot);
		model->Rotation(glm::eulerAngleXYZ(eulerRad.x, eulerRad.y, eulerRad.z));

		m_Models.Add(model);
	}
}

#define CASE_SENSITIVE_CHANNEL_NAMES

void SceneLoader::ParseAudioChannels(const rapidjson::Value& value) {
	FOREACH_MEMBER(memberIter, value) {
#ifdef CASE_SENSITIVE_CHANNEL_NAMES
		auto nameLen = String::Length(memberIter->name.GetString());
		auto allocateLen = nameLen + 1;
		STACK_STRING(chName, allocateLen);
		chName = memberIter->name.GetString();//String copy
		chName[0] = std::toupper(chName[0]);
		StaticString channelName(chName.CStr());//String copy
#else
		StaticString channelName(memberIter->name.GetString());
#endif

		//TODO: FindMember is a linear search.  Possibly just do a for loop here and check names manually if performance becomes an issue.
		UInt16 numChannels{};
		auto numChannelsIter = memberIter->value.FindMember("channels");//Linear search
		assert(numChannelsIter != memberIter->value.MemberEnd());
		Singleton<ValueParser<decltype(numChannels)>>::GetInstance()->Get(numChannelsIter->value, numChannels);

		UInt16 sampleRate{};
		auto sampleRateIter = memberIter->value.FindMember("sampleRate");//Linear search
		assert(sampleRateIter != memberIter->value.MemberEnd());
		Singleton<ValueParser<decltype(sampleRate)>>::GetInstance()->Get(sampleRateIter->value, sampleRate);

		Singleton<AudioSystem>::GetInstance()->AddCategory(channelName, numChannels, sampleRate);

		auto shaderIter = memberIter->value.FindMember("fxChain");//Linear search
		if (shaderIter != memberIter->value.MemberEnd()) {
			auto fxPath = File::BuildFullPath(shaderIter->value.GetString());
			auto numEffects = Singleton<AudioSystem>::GetInstance()->LoadEffectsChain(fxPath, channelName);
		}

		SourceVoice* selectedVoice{};
		auto pathsIter = memberIter->value.FindMember("paths");//Linear search
		if (pathsIter != memberIter->value.MemberEnd()) {
			FOREACH(pathIter, pathsIter->value) {
				Singleton<AudioSystem>::GetInstance()->Play(pathIter->GetString(), channelName, selectedVoice);
			}
		}
	}
}

void SceneLoader::ParseOrientation(const rapidjson::Value& value, glm::vec3& outPos, glm::vec3& outRot) {
	auto&& posVal = value.FindMember("position");//Linear search
	Singleton<ValueParser<TypeInfo<decltype(outPos)>::TUndecorated>>::GetInstance()->Get(posVal->value, outPos);

	auto&& rotVal = value.FindMember("rotation");//Linear search
	Singleton<ValueParser<TypeInfo<decltype(outRot)>::TUndecorated>>::GetInstance()->Get(rotVal->value, outRot);
}

Bool	SceneLoader::Load(const Char* fileName) {
	rapidjson::Document	doc;
	auto scenePath = File::RebuildFullDataPath(fileName);
	verify(rapidjson::LoadFrom(scenePath, doc));

#pragma region Camera
	auto&& cameraVal = doc["camera"];
	assert(cameraVal.IsObject());

	glm::vec3 pos;
	glm::vec3 rot;
	ParseOrientation(cameraVal, pos, rot);
	CameraPosition(pos);
	CameraRotation(glm::quat(glm::radians(rot)));
#pragma endregion

	ParseModels(doc["models"]);
	ParseAudioChannels(doc["audioChannels"]);

	return true;
}

void	SceneLoader::Clear() {
	m_Models.Clear();
}