#include "MySqlQuery.h"
#include "mysql.h"

#pragma comment(lib,"libmysql.lib")

MySqlQuery::MySqlQuery(const MySqlDatabase& db)
{
	mysql_ = db.mysqlHand();
	result_ = nullptr;
	row_ = nullptr;
	active_ = false;
}

MySqlQuery::~MySqlQuery()
{
	if (result_ != nullptr) {
		mysql_free_result(result_);
		result_ = nullptr;
	}
}

bool MySqlQuery::exec(const QString& query)
{
	if (result_ != nullptr) {
		mysql_free_result(result_);
		result_ = nullptr;
	}

	if (mysql_ == nullptr) {
		return false;
	}

	row_ = nullptr;
	active_ = false;

	const QString& query_sql = query.trimmed();
	if (query_sql.isEmpty()) {
		return false;
	}

	QByteArray data = query_sql.toUtf8();
	data.push_back('\0');
	const char* sql = data.constData();
	if (mysql_query(mysql_, sql) == 0) {
		active_ = true;
		if (query_sql.startsWith("SELECT", Qt::CaseInsensitive)) {
			result_ = mysql_store_result(mysql_);
		}
		return true;
	}
	else {
		return false;
	}
}

bool MySqlQuery::isActive() const
{
	return active_;
}

int MySqlQuery::fieldSize()
{
	if (result_ == nullptr) {
		return 0;
	}
	return mysql_num_fields(result_);
}

QStringList MySqlQuery::fieldsName()
{
	if (result_ == nullptr) {
		return QStringList();
	}

	QStringList names;
	const unsigned int feildcount = mysql_num_fields(result_);

	for (unsigned int i = 0; i < feildcount; ++i) {
		MYSQL_FIELD* feild = mysql_fetch_field_direct(result_, i);
		names << QString::fromUtf8(feild->name);
	}

	return names;
}

QString MySqlQuery::lastError() const
{
	const int code = mysql_errno(mysql_);
	return QString("ERROR %1: %2").arg(code).arg(mysql_error(mysql_));
}

bool MySqlQuery::next()
{
	if (result_ == nullptr) {
		return false;
	}
	row_ = mysql_fetch_row(result_);
	return (row_ != nullptr);
}

int MySqlQuery::size() const
{
	if (result_ == nullptr) {
		return 0;
	}
	return mysql_num_rows(result_);
}

QString MySqlQuery::value(int index) const
{
	if (row_ == nullptr || index < 0) {
		return QString();
	}

	char* str_val = row_[index];
	if (str_val == nullptr) {
		return "NULL";
	}
	else {
		return QString::fromUtf8(str_val);
	}
}
