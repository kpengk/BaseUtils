#include "MySqlDatabase.h"
#include "mysql.h"

#pragma comment(lib,"libmysql.lib")

MySqlDatabase::MySqlDatabase()
{
	is_open_ = false;
	port_ = 3306;
	mysql_ = nullptr;
}

MySqlDatabase::~MySqlDatabase()
{
	this->close();
}

bool MySqlDatabase::setCharset(const QString& charset)
{
	if (mysql_ == nullptr) {
		return false;
	}
	const bool ret = mysql_options(mysql_, MYSQL_SET_CHARSET_NAME, charset.toStdString().c_str());
	return ret == 0;
}

void MySqlDatabase::setDatabaseName(const QString& name)
{
	db_name_ = name;
}

void MySqlDatabase::setHostName(const QString& host)
{
	host_name_ = host;
}

void MySqlDatabase::setUserName(const QString& name)
{
	user_name_ = name;
}

void MySqlDatabase::setPassword(const QString& password)
{
	password_ = password;
}

void MySqlDatabase::setPort(int port)
{
	port_ = port;
}

bool MySqlDatabase::open()
{
	mysql_ = mysql_init(nullptr);
	if (mysql_ == nullptr) {
		return false;
	}

	this->setCharset("utf8");
	mysql_ = mysql_real_connect(mysql_, host_name_.toStdString().c_str(), 
		user_name_.toStdString().c_str(), password_.toStdString().c_str(), 
		db_name_.toStdString().c_str(), port_, nullptr, 0);

	is_open_ = (mysql_ != nullptr);
	return is_open_;
}

void MySqlDatabase::close()
{
	if (mysql_ != nullptr && is_open_) {
		mysql_close(mysql_);
		mysql_ = nullptr;
		is_open_ = false;
	}
}

bool MySqlDatabase::transaction()
{
	return (mysql_query(mysql_, "begin") == 0);
}

bool MySqlDatabase::commit()
{
	return (mysql_query(mysql_, "commit") == 0);
}

bool MySqlDatabase::rollback()
{
	return (mysql_query(mysql_, "rollback") == 0);
}

QString MySqlDatabase::databaseName() const
{
	return db_name_;
}

QString MySqlDatabase::hostName() const
{
	return host_name_;
}

QString MySqlDatabase::userName() const
{
	return user_name_;
}

QString MySqlDatabase::password() const
{
	return password_;
}

int MySqlDatabase::port() const
{
	return port_;
}

bool MySqlDatabase::isOpen() const
{
	return is_open_;
}

QString MySqlDatabase::lastError() const
{
	const int code = mysql_errno(mysql_);
	return QString("ERROR %1: %2").arg(code).arg(mysql_error(mysql_));
}

MYSQL* MySqlDatabase::mysqlHand() const
{
	return mysql_;
}
