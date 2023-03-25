#include "pch.h"
#include "Object.h"

Object::Object(OBJECT_TYPE type) : m_gameObjectType(type), m_clientID{ -1 }
{
	static uint32 idGenerator = 1;
	m_id = idGenerator;
	idGenerator++;
}

Object::~Object()
{

}