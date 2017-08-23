#include "Animation.h"
#include "Geometry.h"
#include "CConvertions.h"
#include "Path.h"
#include "Debug.h"
#include "Console.h"

///

mat PoseTRS::matrix()
{
	mat mat_orient = MRotQuaternion(rotation);						// orientation of bone (rotation)
	mat mat_offset = MTranslVector(translation / scale * 1.24);		// bone's origin offset (translation adj. scaling)
	mat mat_final = mat_orient * mat_offset;						// final matrix of translation & orientation

	return mat_final;
}
mat PoseTRS::matrix(mat *mat_parent)
{
	mat mat_orient = MRotQuaternion(rotation);						// orientation of bone (rotation)
	mat mat_offset = MTranslVector(translation);					// bone's origin offset (translation)
	mat mat_scaling = MScalVector(scale);							// bone's scaling factor
	mat mat_final = mat_orient * mat_scaling * mat_offset * *mat_parent;			// final matrix of translation & orientation + parent

	return mat_final;
}

PoseTRS FbxToTRS(FbxAMatrix *fbx)
{
	PoseTRS pose;
	pose.translation = FbxToFloat3(&fbx->GetT());
	pose.rotation = FbxToQuaternion(&fbx->GetQ());
	pose.scale = FbxToFloat3(&fbx->GetS());
	return pose;
}

void Skeleton::initialize(FbxNode *node, Joint *parent, int depth)
{
	bool skeleton_found = false;
	bool process_node = false;
	Joint *current = NULL;

	FbxNodeAttribute *node_attribute = node->GetNodeAttribute();
	process_node = ((parent != NULL) || (node_attribute && node_attribute->GetAttributeType() == FbxNodeAttribute::eSkeleton));

	if (process_node)
	{
		skeleton_found = true;
		Joint newjoint;

		std::vector<Joint> *siblings = NULL;
		if (parent)
			siblings = &parent->children;
		else
			siblings = &joints_hierarchy;

		//siblings->resize(siblings->size() + 1);			// add a new empty joint to the list to fill later
		//current = &siblings->back();						// select the newly created container as current joint
		auto cname = node->GetName();
		newjoint.name = cc_wstr(cname);

		//newjoint.parent = parent;							// DOESN'T WORK

		FbxAMatrix matrix = parent ? node->EvaluateLocalTransform() : node->EvaluateGlobalTransform();
		newjoint.bind_pose = FbxToTRS(&matrix);
		newjoint.bind_pose_mat = FbxToMat(&matrix);

		siblings->push_back(newjoint);
		current = &siblings->back();
		//skeleton->joints_sequential.push_back(current);	// DOESN'T WORK

		depth++;
	}

	// iterate trough children
	for (int i = 0; i < node->GetChildCount(); i++)
	{
		FbxNode *child = node->GetChild(i);
		initialize(child, current, depth);
		/*if (process_node)
		GetSkeleton(child, skeleton, current, depth);
		else
		GetSkeleton(child, skeleton, NULL, depth);*/
	}
}
bool Skeleton::validate()
{
	if (valid)
		return true;
	if (numJoints() > Antimony::skel_maxJoints)
		return false;

	valid = buildSequentialVector(&joints_hierarchy, NULL);
	return valid;
}
bool Skeleton::buildSequentialVector(std::vector<Joint> *branch, Joint *parent)
{
	if (branch->size() == 0)
		return false;
	for (UINT i = 0; i < branch->size(); i++)
	{
		branch->at(i).parent = parent;
		joints_sequential.push_back(&branch->at(i));
		branch->at(i).index = joints_sequential.size()-1;
		buildSequentialVector(&branch->at(i).children, &branch->at(i));
	}
	return true;
}
UINT Skeleton::numJoints()
{
	return joints_sequential.size();
}
UINT Skeleton::numSoAJoints()
{
	return (numJoints() + 3) / 4;
}

bool AnimCurve::validate(float duration)
{
	float previous_time = -1.f;
	for (UINT i = 0; i < keys.size(); i++)
	{
		const float key_time = keys.at(i).time;

		if (key_time < 0.f || key_time > duration)
			return false;
		if (key_time <= previous_time)
			return false;
		previous_time = key_time;
	}
	return true;
}

bool Animation::validate()
{
	if (duration <= 0.f)
		return false;
	if (curves.size() > Antimony::skel_maxJoints)
		return false;
	for (UINT i = 0; i < curves.size(); i++)
	{
		AnimCurve curve = curves[i];
		if (!curve.validate(duration))		// ensure all keys to be in ascending order and within range [0:duration]
			return false;
	}
	return true;
}

void AnimationController::initialize(FbxScene *scene, float samplingrate)
{
	if (!skeleton.valid)
	{
		skeleton.initialize(scene->GetRootNode(), NULL, 0);
		if (!skeleton.validate())
			return;
	}

	this->scene = scene;
	FbxAnimEvaluator *evaluator = scene->GetAnimationEvaluator();

	// check for serialized files in cache
	std::wstring cachefile = cc_wstr(scene->GetName()) + L"_" + std::to_wstring((int)samplingrate) + L".animcache";
	if (saveCached(cachefile))
		return;

	int numAnims = scene->GetSrcObjectCount<FbxAnimStack>(); //(FbxCriteria::ObjectType(FbxAnimStack::ClassId))
	animations.resize(numAnims);

	for (int i = 0; i < numAnims; i++)
	{
		FbxAnimStack *anim = scene->GetSrcObject<FbxAnimStack>(i);
		Animation *newanim = &animations.at(i);

		scene->SetCurrentAnimationStack(anim);
		FbxString stackName = anim->GetName();
		std::string sStackName = stackName;
		newanim->name = stackName;

		// get animation duration
		FbxTimeSpan time_spawn;
		FbxTakeInfo* take_info = scene->GetTakeInfo(stackName);
		if (take_info)
			time_spawn = take_info->mLocalTimeSpan;
		else
			scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(time_spawn);
		float start = (float)(time_spawn.GetStart().GetSecondDouble());
		float end = (float)(time_spawn.GetStop().GetSecondDouble());

		// default duration is 1 second
		if (end > start)
			newanim->duration = end - start;
		else
			newanim->duration = 1.0;

		//// get framerate
		//FbxTime::EMode mode = scene->GetGlobalSettings().GetTimeMode();
		//float scene_frame_rate = (float)((mode == FbxTime::eCustom)
		//	? scene->GetGlobalSettings().GetCustomFrameRate()
		//	: FbxTime::GetFrameRate(mode));

		//// get sampling period
		//float sampling_rate;
		//if (samplingrate > 1)
		//	sampling_rate = samplingrate;
		//else
		//	sampling_rate = scene_frame_rate;
		//float sampling_period = (end - start) / sampling_rate;

		//newanim->curves.resize(skeleton.joints_sequential.size());

		// fill anim curves - each curve is linked to a joint
		for (int i = 0; i < skeleton.joints_sequential.size(); i++)
		{
			//AnimCurve *curve = &newanim->curves[i];

			// find node by name
			std::wstring joint_name = skeleton.joints_sequential.at(i)->name;
			std::string joint_name_s = cc_str(joint_name);
			LPCSTR cname = joint_name_s.c_str();
			FbxNode* node = scene->FindNodeByName(cname);

			skeleton.joints_sequential.at(i)->node = node;

			//if (!node)
			//{
			//	Antimony::log(L"No animation track found for joint \"" + joint_name + L"\"; using skeleton bind pose instead\n", CSL_ERR_GENERIC);

			//	// get joint's bind pose
			//	PoseTRS bind_pose = skeleton.joints_sequential.at(i)->bind_pose;
			//	mat bind_pose_mat = skeleton.joints_sequential.at(i)->bind_pose_mat;

			//	curve->keys.push_back({ 0, bind_pose, bind_pose_mat });

			//	continue;
			//}

			//// reserve keys in animation
			//int max_keys = (int)(3.f + (end - start) / sampling_period);
			//curve->keys.reserve(max_keys);


			if (samplingrate == 1 && false)
			{
				//int numAnimLayers = anim->GetSrcObjectCount<FbxAnimLayer>();
				//for (int l = 0; l < numAnimLayers; l++)
				//{
				//	// get curves of all components for each layer
				//	FbxAnimLayer* layer = anim->GetSrcObject<FbxAnimLayer>(l);

				//	FbxAnimCurve* curve_TX = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
				//	FbxAnimCurve* curve_TY = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
				//	FbxAnimCurve* curve_TZ = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);

				//	FbxAnimCurve* curve_RX = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
				//	FbxAnimCurve* curve_RY = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
				//	FbxAnimCurve* curve_RZ = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);

				//	FbxAnimCurve* curve_SX = node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
				//	FbxAnimCurve* curve_SY = node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
				//	FbxAnimCurve* curve_SZ = node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);

				//	if (curve_TX)
				//	{
				//		int keys = curve_TX->KeyGetCount();
				//		for (int k = 0; k < keys; k++)
				//		{
				//			FbxAnimCurveKey key = curve_TX->KeyGet(k);
				//			float lKeyValue = key.GetValue();
				//		}
				//	}
				//}
			}
			else
			{
				//for (float t = start; (t - sampling_period) < end; t += sampling_period)
				//{
				//	if (t >= end)
				//	{
				//		t = end;
				//		//loop = false;
				//	}

				//	// evaluate transformation matrix at t.
				//	FbxAMatrix matrix = skeleton.joints_sequential.at(i)->parent == NULL
				//		? evaluator->GetNodeGlobalTransform(node, FbxTimeSeconds(t))
				//		: evaluator->GetNodeLocalTransform(node, FbxTimeSeconds(t));

				//	// convert to split members for key
				//	PoseTRS pose = FbxToTRS(&matrix);
				//	mat matrix_xmm = FbxToMat(&matrix);

				//	// fill track with key
				//	float local_time = t - start;
				//	curve->keys.push_back({ local_time, pose, matrix_xmm });
				//}
			}
		}

		////////

		//int numLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
		//for (int j = 0; j < numLayers; ++j)
		//{
		//	FbxAnimLayer *lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(j);

		//	FbxString layerName = lAnimLayer->GetName();
		//	std::string sLayerName = layerName;

		//	std::vector<FbxAnimCurve*> animCurves;

		//	FbxNode *pRoot = scene->GetRootNode();
		//	if (pRoot)
		//	{
		//		for (int n = 0; n < pRoot->GetChildCount(); n++)							// for each child node after the root node
		//		{
		//			FbxNode *pNode = pRoot->GetChild(n);

		//			FbxAnimCurve *lAnimCurve = pNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		//			animCurves.push_back(lAnimCurve);




		//		}
		//	}
		//}

		//KFbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo();

		//KTime start = lTakeInfo->mLocalTimeSpan.GetStart();
		//KTime end = lTakeInfo->mLocalTimeSpan.GetStop();

		//// now you know how many seconds the animation runs, and can figure out how many
		//// keyframes you need. I usually export animations as 30fps
	}

	saveCached(cachefile);
}
void AnimationController::update(double delta)
{
	time += delta * 2;

	if (animations.at(current_anim).duration == 0)
		time = 0;

	while (time >= animations.at(current_anim).duration)
		time -= animations.at(current_anim).duration;

	if (time < 0)	// dunno how it'd happen, but why not
		time = 0;

	///

	if (animations.at(current_anim).curves.size() == 0)
	{
		m_keys = 0;

		m_k0 = 0;
		m_k1 = 0;
		m_k2 = 0;
		m_k3 = 0;

		return;
	}
	AnimCurve *curve = &animations.at(current_anim).curves.at(0);
	m_keys = curve->keys.size();

	for (UINT i = 0; i < m_keys; i++)												// look for a key range in which the current time belongs
	{
		if (time >= curve->keys.at(i).time && time < curve->keys.at(i + 1).time)	// found a range; i is the index of the closest previous key
		{
			UINT final_i = m_keys - 1;

			// check left-hand size
			//if (i == 0)
			//{									//	 v
			//	i0 = final_i;					//	(i)	(-)	(-)	 -	 -	 -	(0)
			//}
			//else
			//{									//			 v
			//	i0 = i - 1;						//	 -	(0)	(i)	(-)	(-)	 -	 -
			//}

			//// check right-hand size
			//if (i == final_i)
			//{									//							 v
			//	i2 = 0;							//	(2)	(-)	 -	 -	 -	(-)	(i)
			//	i3 = 1;							//	(-)	(3)	 -	 -	 -	(-)	(i)
			//}
			//else if (i + 1 == final_i)
			//{									//						 v
			//	i2 = final_i;					//	(-)	 -	 -	 -	(-)	(i)	(2)
			//	i3 = 0;							//	(3)	 -	 -	 -	(-)	(i)	(-)
			//}
			//else
			//{									//			 v
			//	i2 = i + 1;						//	 -	(-)	(i)	(2)	(-)	 -	 -
			//	i3 = i + 2;						//	 -	(-)	(i)	(-)	(3)	 -	 -
			//}

			m_k0 = i - 1;
			m_k1 = i;
			m_k2 = i + 1;
			m_k3 = i + 2;

			while (m_k0 < 0)
				m_k0 += m_keys;
			while (m_k3 >= m_keys)
				m_k3 -= m_keys;

			AnimKey *k0 = &curve->keys.at(m_k0);
			AnimKey *k1 = &curve->keys.at(m_k1);
			AnimKey *k2 = &curve->keys.at(m_k2);
			AnimKey *k3 = &curve->keys.at(m_k3);

			double t0 = k0->time;
			double t1 = k1->time;
			double t2 = k2->time;
			double t3 = k3->time;

			m_keytime_1_2 = (time - t1) / (t2 - t1);
			m_keytime_0_4 = (time - t1 + abs(t1 - t0)) /
				((t2 - t1) + abs(t1 - t0) + abs(t3 - t2));

			break;
		}
	}
}
PoseTRS AnimationController::getPose(int index)
{
	PoseTRS pose;

	if (index != -1)
	{
		AnimCurve *curve = &animations.at(current_anim).curves.at(index);

		if (animations.at(current_anim).duration == 0 || curve->keys.size() == 1)			// anim is 0s long or track has only one key
			pose = curve->keys.at(0).pose;
		else																				// track has got more than one key
		{
			// TODO

			AnimKey *k0 = &curve->keys.at(m_k0);
			AnimKey *k1 = &curve->keys.at(m_k1);
			AnimKey *k2 = &curve->keys.at(m_k2);
			AnimKey *k3 = &curve->keys.at(m_k3);

			double t0 = k0->time;
			double t1 = k1->time;
			double t2 = k2->time;
			double t3 = k3->time;

			pose = k1->pose;

			pose.translation = CubicInterpolate(
				k0->pose.translation,
				k1->pose.translation,
				k2->pose.translation,
				k3->pose.translation, m_keytime_1_2);

			pose.rotation = MQuatSlerp(
				k1->pose.rotation,
				k2->pose.rotation, m_keytime_1_2);

			/*XMVECTOR q1, q2, q3;
			MQuatSquadS(&q1, &q2, &q3,
				k0->pose.rotation,
				k1->pose.rotation,
				k2->pose.rotation,
				k3->pose.rotation);

			pose.rotation = MQuatSquad(k0->pose.rotation,
				q1, q2, q3, m_keytime_0_4);*/

			pose.scale = CubicInterpolate(
				k0->pose.scale,
				k1->pose.scale,
				k2->pose.scale,
				k3->pose.scale, m_keytime_1_2);
		}
	}

	return pose;
}
mat AnimationController::getMat(int index)
{
	mat xmm;

	if (index != -1)
	{
		if (animations.at(current_anim).duration == 0)										// anim is 0s long or track has only one key -> use bind pose
			xmm = skeleton.joints_sequential.at(index)->bind_pose_mat;
		else																				// track has got more than one key -> evaluate at current time
		{
			FbxAnimEvaluator *evaluator = scene->GetAnimationEvaluator();
			scene->SetCurrentAnimationStack(scene->GetSrcObject<FbxAnimStack>(current_anim));
			FbxNode* node = skeleton.joints_sequential.at(index)->node;

			FbxAMatrix matrix = skeleton.joints_sequential.at(index)->parent == NULL
				? evaluator->GetNodeGlobalTransform(node, FbxTimeSeconds(time))
				: evaluator->GetNodeLocalTransform(node, FbxTimeSeconds(time));

			xmm = FbxToMat(&matrix);
		}
	}

	return xmm;
}

bool AnimationController::loadCached(std::wstring file)
{
	std::wstring fullpath;
	fullpath = Antimony::FilePath(L"\\Models\\", file);

	char *fullpathFBX = new char[MAX_PATH];
	FbxWCToUTF8(fullpath.c_str(), fullpathFBX);


	/*std::ifstream ifs(fullpath, std::ios::in | std::ifstream::binary);
	std::istreambuf_iterator<char> iter(ifs);
	std::istreambuf_iterator<char> end{};
	std::copy(iter, end, std::back_inserter(anims));*/

	//std::ifstream INFILE(fullpath, std::ios::in | std::ifstream::binary);
	//std::istreambuf_iterator<char> iter(INFILE);
	////std::copy(iter.begin(),iter.end(),std::back_inserter(newVector)); //this doesn't compile
	//std::copy(iter, std::istreambuf_iterator<char>{}, std::back_inserter(anims)); // this leaves newVector empty


	/*std::ifstream INFILE(fullpath, std::ios::in | std::ifstream::binary);
	std::istreambuf_iterator iter(INFILE);
	std::copy(iter.begin(), iter.end(), std::back_inserter(newVector));*/



	return false;
}
bool AnimationController::saveCached(std::wstring file)
{
	std::wstring fullpath;
	fullpath = Antimony::FilePath(L"\\Models\\", file);


	/*std::ifstream ifs(fullpath, std::ios::in | std::ifstream::binary);
	std::istreambuf_iterator<char> iter(ifs);
	std::istreambuf_iterator<char> end{};
	std::copy(iter, end, std::back_inserter(anims));*/

	/*std::ofstream FILE(fullpath, std::ios::out | std::ofstream::binary);
	std::copy(anims->begin(), anims->end(), std::ostreambuf_iterator<char>(FILE));*/


	return false;
}

namespace Antimony
{
	UINT skel_maxJointBits = 10;
	UINT skel_maxJoints = (1 << skel_maxJointBits) - 1;
	UINT skel_maxSoAJoints = (skel_maxJoints + 3) / 4;
	UINT skel_parentIndex = skel_maxJoints;
}