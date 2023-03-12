#include "pch.h"
#include "Object.h"

Object::Object(OBJECT_TYPE type) : m_objectType(type)
{
	static uint32 idGenerator = 1;
	m_id = idGenerator;
	idGenerator++;
}

Object::~Object()
{

}