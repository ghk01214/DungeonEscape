#include "pch.h"
#include "Object.h"

Object::Object(OBJECT_TYPE type) : m_gameObjectType(type)
{
	static uint32 idGenerator = 1;
	m_id = idGenerator;
	idGenerator++;
}

Object::~Object()
{

}