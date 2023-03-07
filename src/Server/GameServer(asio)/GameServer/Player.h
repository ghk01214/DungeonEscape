#pragma once

#include "Object.h"

namespace game
{
	class CPlayer : public CObject
	{
	public:
		CPlayer();
		virtual ~CPlayer();

		void Init(uint32_t id) override;

		constexpr uint32_t GetID() { return m_id; }
		constexpr uint32_t GetAccessID() { return m_accessID; }

		bool IsConnected() { return m_connected; }

	private:
		uint32_t m_id;
		uint32_t m_accessID;

		std::atomic_bool m_connected;
	};
}