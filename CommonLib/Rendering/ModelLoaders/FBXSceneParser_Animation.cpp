#include "FBXSceneParser_Animation.h"
#include "System/DebugConsole.h"

Bool FBXSceneParser_Animation::Parse(fbxsdk::FbxScene* scene, const MeshLoader_FBX::TNodeTypeToNodeListMap& nodeMap, const Neo::Skeleton& skeleton, const ModelDef::TInFileAnimationMap& inFileAnimMap, List<AnimationClip*>& outAnims) {
	Clear();

	const MeshLoader_FBX::TNodeList& skelNodeList = nodeMap[fbxsdk::FbxNodeAttribute::eSkeleton];

	Int32 numStacks = scene->GetSrcObjectCount<FbxAnimStack>();
	FOR(Int32, ix, 0, numStacks, 1) {
		FbxAnimStack* pAnimStack = scene->GetSrcObject<FbxAnimStack>(ix);
		if (!pAnimStack) {
			continue;
		}

		auto animStackName = pAnimStack->GetName();
		scene->SetCurrentAnimationStack(pAnimStack);
		auto pAnimEvaluator = scene->GetAnimationEvaluator();

		auto&& animTimeSpan = pAnimStack->GetLocalTimeSpan();
		auto&& animDuration = animTimeSpan.GetDuration();
		auto&& frameCount = animDuration.GetFrameCount(fbxsdk::FbxTime::EMode::eFrames24);
		auto frameRate = (Float32)animDuration.GetFrameRate(fbxsdk::FbxTime::EMode::eFrames24);

		auto& animMap = inFileAnimMap[animStackName];
		auto fileDuration = animDuration.GetSecondDouble();
		FOREACH_CONST(animIter, animMap) {
			AnimationClip* animClip = new AnimationClip();
			outAnims.Add(animClip);
			
			animClip->Name(animIter->first);
			auto& range = animIter->second;
			auto startTime = ((Float32)range.StartFrame / frameCount) * fileDuration;
			Float64 endFrame = range.EndFrame < 0 ? frameCount : range.EndFrame;
			auto endTime = (endFrame / frameCount) * fileDuration;
			animClip->Duration(endTime - startTime);
			animClip->FrameRate(frameRate);

			AnimKeyFrame* pKeyFrame = NULL;
			glm::mat4 mat;
			fbxsdk::FbxTime time;
			FOR(Float64, fFrameIndex, range.StartFrame, endFrame, 1.0) {
				pKeyFrame = new	AnimKeyFrame();
				animClip->AddFrame(pKeyFrame);
				pKeyFrame->Time(fFrameIndex / animClip->FrameRate());

				time.SetSecondDouble(pKeyFrame->Time());
				for( UInt32 iy = 0; iy < skelNodeList.Length(); ++iy) {
					fbxsdk::FbxNode* node = skelNodeList[iy];
					
					const fbxsdk::FbxAMatrix& localTransform = pAnimEvaluator->GetNodeLocalTransform(node, time);
					FbxConversions::Convert(mat, localTransform);
					pKeyFrame->SetLocalTransform(iy, mat);

#if _DEBUG
					auto skelJoint = skeleton.FindJoint(iy);
					auto jointName = skelJoint->Name();
					auto nodeName = node->GetNameOnly();

					assert(nodeName.Find(jointName.CStr()) >= 0);
#endif
				}
			}
		}
	}
	return true;
}