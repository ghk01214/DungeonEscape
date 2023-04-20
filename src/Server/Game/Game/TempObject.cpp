#include "pch.h"
#include "TempObject.h"

namespace game
{
	CTempObject::CTempObject()
	{
	}

	CTempObject::CTempObject(const std::wstring& name, TempTrans& trans) :
		m_fbxName{ name },
		m_trans{ trans }
	{
	}

	CTempObject::~CTempObject()
	{
	}
}