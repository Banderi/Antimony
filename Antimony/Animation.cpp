#include "Animation.h"
#include "Geometry.h"
#include "CConvertions.h"
#include "Path.h"
#include "Debug.h"
#include "Console.h"

///

void Joint::update(mat *t)
{
	transform = *t;

	if (parent)
	{
		transform *= parent->transform;
	}
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

		newjoint.name = cc_wstr(node->GetName());

		FbxAMatrix matrix = parent ? node->EvaluateLocalTransform() : node->EvaluateGlobalTransform();
		newjoint.bind_pose = FbxToMat(&matrix);
		newjoint.transform = FbxToMat(&matrix);

		siblings->push_back(newjoint);
		current = &siblings->back();

		depth++;
	}

	// iterate trough children
	for (int i = 0; i < node->GetChildCount(); i++)
	{
		initialize(node->GetChild(i), current, depth);
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
		transform_sequential.push_back(&branch->at(i).transform);
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

void AnimationController::initialize(FbxScene *scene)
{
	if (!skeleton.valid)
	{
		skeleton.initialize(scene->GetRootNode(), NULL, 0);
		if (!skeleton.validate())
			return;
	}

	this->scene = scene;

	// check for serialized files in cache
	std::wstring cachefile = cc_wstr(scene->GetName()) + L".animcache";
	if (loadCached(cachefile))
		return;

	int numAnims = scene->GetSrcObjectCount<FbxAnimStack>();
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

		// fill anim curves - each curve is linked to a joint
		for (int i = 0; i < skeleton.joints_sequential.size(); i++)
		{

			// find node by name
			std::wstring joint_name = skeleton.joints_sequential.at(i)->name;
			std::string joint_name_s = cc_str(joint_name);
			LPCSTR cname = joint_name_s.c_str();
			FbxNode* node = scene->FindNodeByName(cname);

			skeleton.joints_sequential.at(i)->node = node;

		}
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


	if (time != prev_time)
	{
		//updateSkeleton(0);

		for (UINT i = 0; i < skeleton.joints_sequential.size(); i++)
		{
			Joint *joint = skeleton.joints_sequential.at(i);

			/*wchar_t buf[64];
			swprintf_s(buf, L"%p", (void *)joint);
			Antimony::log(L"Joint: " + std::to_wstring(i) + L" " + joint->name + L" " + std::wstring(buf), CSL_INFO);*/

			joint->update(&getMat(i));

			/*Antimony::log(L" T0", CSL_INFO);

			if (joint->parent)
			{
				joint->transform *= joint->parent->transform;
				Antimony::log(L" T1", CSL_INFO);
			}*/

			//Antimony::log(L"\n", CSL_INFO);
		}
	}
}
void AnimationController::updateSkeleton(UINT index)
{
	Joint *joint = NULL;
	joint = skeleton.joints_sequential.at(index);

	joint->transform = getMat(index);

	if (joint->parent)
		joint->transform *= joint->parent->transform;

	for (UINT i = 0; i < joint->children.size(); i++)
		updateSkeleton(joint->children.at(i).index);
}
mat AnimationController::getMat(UINT index)
{
	mat xmm;

	if (index != -1)
	{
		if (animations.at(current_anim).duration == 0)										// anim is 0s long or track has only one key -> use bind pose
			xmm = skeleton.joints_sequential.at(index)->bind_pose;
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