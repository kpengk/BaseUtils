#pragma once
#include <QString>
#include <QStringList>
#include "MySqlDatabase.h"

class MYSQL_RES;
class MySqlQuery
{
public:
	MySqlQuery(const MySqlDatabase& db);
	~MySqlQuery();
	MySqlQuery(const MySqlQuery& other) = delete;
	MySqlQuery& operator=(const MySqlQuery& other) = delete;

	bool exec(const QString& query);
	bool isActive() const;
	int fieldSize();
	QStringList fieldsName();
	QString	lastError() const;
	bool next();
	int	size() const;
	QString	value(int index) const;

private:
	MYSQL* mysql_;
	MYSQL_RES* result_;
	char** row_;
	bool active_;
};

