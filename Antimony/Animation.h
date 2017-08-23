#pragma once

#include <vector>
#include "DirectX.h"
#include "FBX.h"

///

struct PoseTRS
{
	float3 translation;
	Quaternion rotation;
	float3 scale;

	mat matrix();
	mat matrix(mat *mat_parent);

	PoseTRS() {}
	PoseTRS(float3 t, Quaternion r, float3 s) : translation(t), rotation(r), scale(s) {}
};

PoseTRS FbxToTRS(FbxAMatrix *fbx);

struct Joint
{
	std::wstring name;
	int index;
	std::vector<Joint> children;
	Joint *parent;
	FbxNode* node;
	PoseTRS bind_pose;
	mat bind_pose_mat;
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
private:
	//// iterate through joint hierarchy depth-first
	//template<typename funct> void iterDF(std::vector<Joint> children, Joint* parent, funct& functor)
	//{
	//	for (size_t i = 0; i < children.size(); ++i)
	//	{
	//		Joint current = children[i];
	//		functor(current, parent);
	//		iterDF(current.children, &current, functor);
	//	}
	//}

	//// iterate through joint hierarchy breadth-first
	//template<typename funct> void iterBF(std::vector<Joint> children, Joint* parent, funct& functor)
	//{
	//	for (size_t i = 0; i < children.size(); ++i)
	//	{
	//		Joint& current = children[i];
	//		functor(current, parent);
	//	}
	//	for (size_t i = 0; i < children.size(); ++i)
	//	{
	//		Joint& current = children[i];
	//		iterBF(current.children, &current, functor);
	//	}
	//}

public:
	std::vector<Joint> joints_hierarchy;
	std::vector<Joint*> joints_sequential;
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

	/*template<typename funct> funct iterDF(funct functor)
	{
		iterDF(roots, NULL, functor);
		return functor;
	}
	template<typename funct> funct iterBF(funct functor)
	{
		iterBF(roots, NULL, functor);
		return functor;
	}*/
};

struct AnimKey
{
	double time;
	PoseTRS pose;
	mat matrix;
};

struct AnimCurve
{
	std::vector<AnimKey> keys;
	bool validate(float duration);
};

class Animation
{
private:
public:
	std::string name;
	float duration;
	std::vector<AnimCurve> curves;			// number of animation curves, one for each bone

	bool validate();

	Animation()
	{
		duration = 1.0f;
	};
	//~Animation();
};

class AnimationController
{
private:
	UINT m_keys;
	int m_k0;
	int m_k1;
	int m_k2;
	int m_k3;
	double m_keytime_1_2;
	double m_keytime_0_4;

public:
	Skeleton skeleton;
	FbxScene *scene;
	std::vector<Animation> animations;
	UINT current_anim;
	UINT next_anim;
	bool reversed;
	double time;

	void initialize(FbxScene *scene, float samplingrate);
	void update(double delta);
	PoseTRS getPose(int index);
	mat getMat(int index);

	bool loadCached(std::wstring file);
	bool saveCached(std::wstring file);

	AnimationController()
	{
		current_anim = 0;
		next_anim = 0;
		reversed = false;
		time = 0;

		m_keys = 0;
		m_k0 = 0;
		m_k1 = 0;
		m_k2 = 0;
		m_k3 = 0;
		m_keytime_1_2 = 0;
		m_keytime_0_4 = 0;
	}
};

namespace Antimony
{
	extern UINT skel_maxJointBits;			// Limits the number of joints in order to control the number of bits required to store a joint index
	extern UINT skel_maxJoints;				// Maximum number of joints; last index is reserved for the parent join's index
	extern UINT skel_maxSoAJoints;			// Maximum number of SoA elements required to store the max. number of joints
	extern UINT skel_parentIndex;			// Index of the parent of the root joint (no parent, in fact)
}