#ifndef DBSERVER_H__
#define DBSERVER_H__

class CDBServer
{
public:
	CDBServer();
	~CDBServer();
public:
	void LoadSetting(const TCHAR* Path);
	void SelectObject(const TCHAR* Path);
public:
	void ServerStart();
private:
	void DBConnection();
	void DBInsert();
	void DBSelect();
	void DBDelete();
};

#endif // !DBSERVER_H__