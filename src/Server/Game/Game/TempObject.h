#pragma once

namespace game
{
	struct TempVec3
	{
		float x;
		float y;
		float z;
	};

	struct TempVec4
	{
		float x;
		float y;
		float z;
		float w;
	};

	using TempPos = TempVec3;
	using TempQuat = TempVec4;
	using TempScale = TempVec3;

	struct TempTrans
	{
		TempPos p;
		TempQuat q;
		TempScale s;
	};

	class CTempObject
	{
	public:
		CTempObject();
		CTempObject(const std::wstring& name, TempTrans& trans);
		~CTempObject();

		//void Transform()

		constexpr std::wstring& GetName() { return m_fbxName; }
		const TempTrans& GetTrans() const { return m_trans; }
		constexpr int32_t GetID() const { return m_id; }

		void SetID(int32_t id) { m_id = id; }

	private:
		std::wstring m_fbxName;
		TempTrans m_trans;

		int32_t m_id;
	};
}