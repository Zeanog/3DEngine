#include "SceneLoader.h"
#include "System/File.h"
#include "ModelManager.h"
#include "System/Audio/AudioSystem.h"
#include "System/JsonLoader.h"

#include "Rendering/Model.h"
#include "System/Reflector.h"

void SceneLoader::ParseModels(const rapidjson::Value& value) {
	glm::vec3 pos;
	glm::vec3 rot;

	FOREACH(modelIter, value) {
		auto&& pathVal = modelIter->FindMember("path");
		auto modelPath = File::BuildFullPath(pathVal->value.GetString());
		auto model = Singleton<ModelManager>::GetInstance()->Get(modelPath);

		ParseOrientation(*modelIter, pos, rot);
		model->Position(pos);
		model->Rotation(glm::quat(glm::radians(rot)));

		m_Models.Add(model);
	}
}

void SceneLoader::ParseAudioChannels(const rapidjson::Value& value) {
	FOREACH_MEMBER(memberIter, value) {
		auto channelName = memberIter->name.GetString();
		auto numChannelsIter = memberIter->value.FindMember("Channels");
		assert(numChannelsIter != memberIter->value.MemberEnd());

		auto sampleRateIter = memberIter->value.FindMember("SampleRate");
		assert(sampleRateIter != memberIter->value.MemberEnd());

		UInt32 numChannels{};
		Singleton<ValueParser<decltype(numChannels)>>::GetInstance()->Get(numChannelsIter->value, numChannels);

		UInt32 sampleRate{};
		Singleton<ValueParser<decltype(sampleRate)>>::GetInstance()->Get(sampleRateIter->value, sampleRate);

		Singleton<AudioSystem>::GetInstance()->AddCategory(channelName, numChannels, sampleRate);

		auto shaderIter = memberIter->value.FindMember("Shader");
		if (shaderIter != memberIter->value.MemberEnd()) {
			auto fxPath = File::BuildFullPath(shaderIter->value.GetString());
			auto numEffects = Singleton<AudioSystem>::GetInstance()->LoadEffects(fxPath, channelName);
		}

		SourceVoice* selectedVoice{};
		auto pathsIter = memberIter->value.FindMember("Paths");
		FOREACH(pathIter, pathsIter->value) {
			Singleton<AudioSystem>::GetInstance()->Play(pathIter->GetString(), channelName, selectedVoice);
		}
	}
}

void SceneLoader::ParseOrientation(const rapidjson::Value& value, glm::vec3& outPos, glm::vec3& outRot) {
	auto&& posVal = value.FindMember("position");
	Singleton<ValueParser<glm::vec3>>::GetInstance()->Get(posVal->value, outPos);

	auto&& rotVal = value.FindMember("rotation");
	Singleton<ValueParser<glm::vec3>>::GetInstance()->Get(rotVal->value, outRot);
}

Bool	SceneLoader::Load(const Char* fileName) {
	rapidjson::Document	doc;
	auto scenePath = File::BuildFullPath("Data\\Scene.json");
	verify(rapidjson::LoadFrom(scenePath, doc));

#pragma region Camera
	auto&& cameraVal = doc["Camera"];
	assert(cameraVal.IsObject());

	glm::vec3 pos;
	glm::vec3 rot;
	ParseOrientation(cameraVal, pos, rot);
	CameraPosition(pos);
	CameraRotation(rot);
#pragma endregion

	ParseModels(doc["Models"]);
	ParseAudioChannels(doc["AudioChannels"]);

	return true;
}

void	SceneLoader::Clear() {
	m_Models.Clear();
}