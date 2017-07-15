#include "Antimony.h"

///

btDiscreteDynamicsWorld* Antimony::getBtWorld()
{
	return m_btWorld;
}
void Antimony::tickCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep)
{
	m_objectsCollisions.clear();
	m_objectsCollisionPoints.clear();
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold *contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		auto *objA = contactManifold->getBody0();
		auto *objB = contactManifold->getBody1();
		auto& manifoldsA = m_objectsCollisions[objA];
		auto& manifoldsB = m_objectsCollisions[objA];
		manifoldsA.push_back(contactManifold);
		manifoldsB.push_back(contactManifold);
		auto& collisionsA = m_objectsCollisionPoints[objA];
		auto& collisionsB = m_objectsCollisionPoints[objB];
		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			collisionsA.push_back(&pt);
			collisionsB.push_back(&pt);
		}
	}
}
void Antimony::staticCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep)
{
	antimony.tickCallback(dynamicsWorld, timeStep);
}
void Antimony::addPhysEntity(btObject *obj)
{
	m_physEntities.push_back(obj);
}
std::vector<btObject*>* Antimony::getEntities()
{
	return &m_physEntities;
}
void Antimony::resetPhysics()
{
	for (int i = 0; i < m_physEntities.size(); i++)
		m_physEntities.at(i)->reset();
}

double Antimony::getTick()
{
	return m_delta * m_worldSpeed;
}
double Antimony::getDelta()
{
	return m_delta;
}