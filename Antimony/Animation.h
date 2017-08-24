#pragma once

#include <vector>
#include "DirectX.h"
#include "FBX.h"

///

struct Joint
{
	std::wstring name;
	int index;
	std::vector<Joint> children;
	Joint *parent;
	FbxNode* node;
	mat bind_pose;
	mat transform;
	float length;
	bool is_leaf;					// Set to 1 for a leaf, 0 for a branch.

	Joint()
	{
		index = -1;
		parent = NULL;
		is_leaf = true;
	}
};

class Skeleton
{
public:
	std::vector<Joint> joints_hierarchy;
	std::vector<Joint*> joints_sequential;
	std::vector<mat*> transform_sequential;
	bool valid;

	void initialize(FbxNode *node, Joint *parent, int depth);
	bool validate();
	bool buildSequentialVector(std::vector<Joint> *branch, Joint *parent);
	UINT numJoints();
	UINT numSoAJoints();

	//std::vector<std::string> getJointNames();
	//std::vector<mat> getJointTransforms();

	Skeleton()
	{
		valid = false;
	}
};

class Animation
{
public:
	std::string name;
	float duration;

	Animation()
	{
		duration = 1.0f;
	};
};

class AnimationController
{
public:
	Skeleton skeleton;
	FbxScene *scene;
	std::vector<Animation> animations;
	UINT current_anim;
	UINT next_anim;
	bool reversed;
	double time;
	double prev_time;

	void initialize(FbxScene *scene);
	void update(double delta);
	void updateSkeleton(UINT index);
	mat getMat(UINT index);

	bool loadCached(std::wstring file);
	bool saveCached(std::wstring file);

	AnimationController()
	{
		current_anim = 0;
		next_anim = 0;
		reversed = false;
		time = -1;
		prev_time = -1;
	}
};

namespace Antimony
{
	extern UINT skel_maxJointBits;			// Limits the number of joints in order to control the number of bits required to store a joint index
	extern UINT skel_maxJoints;				// Maximum number of joints; last index is reserved for the parent join's index
	extern UINT skel_maxSoAJoints;			// Maximum number of SoA elements required to store the max. number of joints
	extern UINT skel_parentIndex;			// Index of the parent of the root joint (no parent, in fact)
}