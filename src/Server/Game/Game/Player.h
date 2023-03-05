#pragma once

namespace game
{
	class CObject;

	class CPlayer : public CObject
	{
	public:
		CPlayer();
		~CPlayer();

		void Init(int32_t id) override;

		constexpr int32_t GetAccessID() const { return m_accessID; }

		bool IsConnected() { return m_connected; }

	private:
		int32_t m_accessID;

		std::atomic_bool m_connected;
	};
}