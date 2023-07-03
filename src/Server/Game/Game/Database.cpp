#include "pch.h"
#include "Database.h"

namespace game
{
	ImplementSingletone(Database);

	Database::Database()
	{
	}

	Database::~Database()
	{
	}

#pragma region [ERROR]
	/************************************************************************
	HandleDiagnosticRecord : display error/warning information

	Parameters:
		hHandle		 ODBC handle
		hType		 Type of handle (SQL_HANDLE_STMT, SQL_HANDLE_ENV, SQL_HANDLE_DBC)
		RetCode		 Return code of failing command
	*************************************************************************/
	void Database::ShowError(SQLHANDLE handle, SQLSMALLINT type)
	{
		SQLSMALLINT iRec{ 0 };
		SQLINTEGER error;
		std::wstring msg, state;

		msg.resize(1000);
		state.resize(SQL_SQLSTATE_SIZE + 1);

		if (m_ret == SQL_INVALID_HANDLE)
		{
			std::wcerr << std::format(L"Invlaid handle\n");
			return;
		}

		while (SQLGetDiagRec(type, handle, ++iRec, state.data(), &error, msg.data(),
			static_cast<SQLSMALLINT>(sizeof(msg) / sizeof(wchar_t)), reinterpret_cast<SQLSMALLINT*>(NULL)) == SQL_SUCCESS)
		{
			// Hide data truncated..
			if (wcsncmp(state.data(), L"01004", 5))
				std::wcerr << L"[" << state << "] " << msg << " (" << error << ")\n";
		}
	}
#pragma endregion

	void Database::Init(HANDLE iocp)
	{
		m_iocp = iocp;

		m_dbName = L"practice";

		// Allocate environment handle
		m_ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);

		if (IsSuccess() == false)
		{
			ShowError(m_henv, SQL_HANDLE_ENV);
			return;
		}

		// Set the ODBC version evvironment attribute
		m_ret = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER*>(SQL_OV_ODBC3), 0);

		if (IsSuccess() == false)
		{
			ShowError(m_henv, SQL_HANDLE_ENV);
			SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
			return;
		}

		// Allocate connection handle
		m_ret = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);

		if (IsSuccess() == false)
		{
			ShowError(m_hdbc, SQL_HANDLE_DBC);
			SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
			return;
		}

		// Set login timeout to 5 seconds
		SQLSetConnectAttr(m_hdbc, SQL_LOGIN_TIMEOUT, reinterpret_cast<SQLPOINTER>(5), 0);

		// Connect to data source
		m_ret = SQLConnect(m_hdbc, m_dbName.data(), SQL_NTS, reinterpret_cast<SQLWCHAR*>(NULL), 0, nullptr, 0);

		if (IsSuccess() == false)
		{
			ShowError(m_hdbc, SQL_HANDLE_DBC);
			SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
			SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
			return;
		}

	}

	void Database::Release()
	{
	}

	void Database::Update()
	{
	}

	bool Database::IsSuccess()
	{
		return m_ret == SQL_SUCCESS or m_ret == SQL_SUCCESS_WITH_INFO;
	}

	void Database::ProcessSignUp()
	{

	}

	void Database::ProcessLogin()
	{
	}

	void Database::ProcessUserSave()
	{
	}

}