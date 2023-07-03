#pragma once

#include <sqlext.h>

namespace game
{
	class Database
	{
	private:
		DeclareSingletone(Database);

	private:
		Database();
		~Database();

	public:
		void ShowError(SQLHANDLE handle, SQLSMALLINT type);

		void Init(HANDLE iocp);
		void Release();
		void Update();

		bool IsSuccess();

		void ProcessSignUp();
		void ProcessLogin();
		void ProcessUserSave();

	private:
		HANDLE m_iocp;

		SQLHENV m_henv;			// SQL enviornment handle
		SQLHDBC m_hdbc;			// ODBC handle
		SQLRETURN m_ret;

		std::wstring m_dbName;
	};
}