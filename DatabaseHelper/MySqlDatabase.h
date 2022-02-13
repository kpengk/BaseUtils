#pragma once
#include <QString>

class MYSQL;
class MySqlDatabase
{
public:
	MySqlDatabase();
	~MySqlDatabase();
	MySqlDatabase(const MySqlDatabase& other) = delete;
	MySqlDatabase& operator=(const MySqlDatabase& other) = delete;

	bool setCharset(const QString& charset);
	void setDatabaseName(const QString& name);
	void setHostName(const QString& host);
	void setUserName(const QString& name);
	void setPassword(const QString& password);
	void setPort(int port);
	
	bool open();
	void close();

	bool transaction();
	bool commit();
	bool rollback();

	QString	databaseName() const;
	QString	hostName() const;
	QString	userName() const;
	QString	password() const;
	int	port() const;

	bool isOpen() const;
	QString	lastError() const;

	MYSQL* mysqlHand() const;

private:
	MYSQL* mysql_;
	QString db_name_;
	QString host_name_;
	QString user_name_;
	QString password_;
	int port_;
	bool is_open_;
};

