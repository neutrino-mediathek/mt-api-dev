
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <iomanip>

#include "llvm/Support/FormatVariadic.h"

#include "common/helpers.h"
#include "mt-api.h"
#include "sql.h"

extern CMtApi*		g_mainInstance;
extern string		g_dataRoot;
extern bool		g_debugMode;
extern int		g_apiMode;
extern string		g_msgBoxText;

CSql::CSql()
{
	Init();
}

void CSql::Init()
{
	mysqlCon = NULL;
	pwFile   = g_dataRoot + "/.passwd/sqlpasswd";
	usedDB   = "mediathek_1_TEST";
	tabVideo = "video";
	resultCount = 0;
}

CSql::~CSql()
{
	if (mysqlCon != NULL)
		mysql_close(mysqlCon);
}

void CSql::show_error(const char* func, int line)
{
	g_msgBoxText = llvm::formatv("<span style='color: OrangeRed'>[{0}:{1}] Error({2}) [{3}] \"{4}\"\n<br /></span>",
				     func, line,
				     mysql_errno(mysqlCon), mysql_sqlstate(mysqlCon), mysql_error(mysqlCon));

	mysql_close(mysqlCon);
	mysqlCon = NULL;
}

bool CSql::connectMysql()
{
	string pw = readFile(pwFile);
	pw = trim(pw);
	vector<string> v = split(pw, ':');

	mysqlCon = mysql_init(NULL);
	unsigned long flags = 0;
//	flags |= CLIENT_MULTI_STATEMENTS;
//	flags |= CLIENT_COMPRESS;
	if (!mysql_real_connect(mysqlCon, "127.0.0.1", v[0].c_str(), v[1].c_str(), usedDB.c_str(), 3306, NULL, flags)) {
		show_error(__func__, __LINE__);
		return false;
	}

	if (mysql_set_character_set(mysqlCon, "utf8") != 0) {
		show_error(__func__, __LINE__);
		return false;
	}

	return true;
}

string CSql::formatSql(string data, int id, string tagBefore, string tagAfter)
{
	string html = readFile(g_dataRoot + "/template/sql-format.html");
	html = str_replace("@@@SQL_DATA@@@", data, html);
	html = str_replace("@@@ID@@@", to_string(id), html);

	return tagBefore + html + tagAfter;
}

double CSql::startTimer()
{
	struct timeval t1;
	gettimeofday(&t1, NULL);
	return (double)t1.tv_sec*1000ULL + ((double)t1.tv_usec)/1000ULL;
}

string CSql::getTimer(double startTime, string txt, int preci/*=3*/)
{
	struct timeval t1;
	gettimeofday(&t1, NULL);
	double workDTms = (double)t1.tv_sec*1000ULL + ((double)t1.tv_usec)/1000ULL;
	string format = "{0} {1:F" + to_string(preci) + "} sec";
	return llvm::formatv(format.c_str(), txt, (workDTms - startTime) / 1000ULL);
}

int CSql::getResultCount(string where)
{
	if (mysqlCon == NULL)
		return 0;

	string sql = "";
        sql += "SELECT COUNT(id) AS anz FROM " + tabVideo + " " + where + ";";

	double timer = startTimer();

	if (mysql_real_query(mysqlCon, sql.c_str(), sql.length()) != 0) {
		show_error(__func__, __LINE__);
		return false;
	}

	int ret = 0;
	MYSQL_RES* result = mysql_store_result(mysqlCon);
	if (result) {
		if (mysql_num_fields(result) > 0) {
			MYSQL_ROW row;
			row = mysql_fetch_row(result);
			uint64_t* lengths = mysql_fetch_lengths(result);
			if ((row != NULL) && (row[0] != NULL)) {
				string tmp_s = static_cast<string>(row[0]);
				tmp_s = tmp_s.substr(0, lengths[0]);
				ret = stoi(tmp_s);
			}
		}
		mysql_free_result(result);
	}

	string timer_s = getTimer(timer, "Duration sql query: ");

ostringstream oss;
oss << "ret: " << ret << endl;
g_msgBoxText += oss.str();
oss.str(string());
oss.clear();
oss << timer_s << endl;
g_msgBoxText += oss.str();

	if (g_debugMode)
		g_mainInstance->htmlOut << formatSql(sql, 1, "", "") << endl;

	return ret;
}

bool CSql::sqlListVideo(listVideo_t* lv)
{
	if (mysqlCon == NULL)
		return false;

	time_t now      = (lv->refTime == 0) ? time(0) : lv->refTime;
	time_t fromTime = 0;
	time_t toTime   = 0;

	time_t epoch = lv->epoch;
	/* (lv->epoch < 0) => all data */
	if (epoch == 0)
		epoch = 1;

	if (epoch > 0) {
		epoch = epoch * 3600 * 24;
		fromTime = now;
		toTime   = now - epoch;
		if (lv->timeMode == timeMode_future)
			fromTime += epoch;
	}

	string where = "";
	where += " WHERE ( channel LIKE " + checkString(lv->channel, 128);
	where += " AND duration >= " + checkInt(lv->duration);
	if (epoch > 0) {
		where += " AND date_unix < " + checkInt(fromTime);
		where += " AND date_unix > " + checkInt(toTime);
	}
	else {
		if (lv->timeMode != timeMode_future)
			where += " AND date_unix < " + checkInt(now);
	}
	where += " )";

	resultCount = getResultCount(where);

	string sql0 = "";
	sql0 += "SELECT  channel, theme, title, description, url, url_small, url_hd, date_unix, duration, geo, parse_m3u8";
	sql0 += " FROM " + tabVideo;
	sql0 += where;
	sql0 += " ORDER BY date_unix DESC";
	sql0 += " LIMIT " + checkInt(lv->limit);
	sql0 += " OFFSET " + checkInt(lv->start);

	string sql = "";
	sql += "SELECT * FROM ( ";
	sql += sql0;
	sql += " ) AS dingens";
	sql += " ORDER BY date_unix DESC, title ASC;";

	if (g_debugMode)
		g_mainInstance->htmlOut << formatSql(sql, 2, "", "") << endl;

	return true;
}
