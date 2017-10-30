
#ifndef __SQL_H__
#define __SQL_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <mysql.h>

#include <string>

#include "types.h"

using namespace std;

class CSql
{
	private:
		MYSQL *mysqlCon;
		string pwFile;
		string usedDB;
		string tabChannelinfo;
		string tabVersion;
		string tabVideo;
		int resultCount;

		void Init();
		void show_error(const char* func, int line);
		char checkStringBuff[0xFFFF];
		inline string checkString(string& str, int size) {
			size_t size_ = ((size_t)size > (sizeof(checkStringBuff)-1)) ? sizeof(checkStringBuff)-1 : size;
			string str2 = (str.length() > size_) ? str.substr(0, size_) : str;
//			memset(checkStringBuff, 0, sizeof(checkStringBuff));
			memset(checkStringBuff, 0, size_+1);
			mysql_real_escape_string(mysqlCon, checkStringBuff, str2.c_str(), str2.length());
			str = (string)checkStringBuff;
			return "'" + str + "'";
		}
		inline string checkInt(int i) { return to_string(i); }
		string formatSql(string data, int id, string tagBefore="", string tagAfter="");
		int getResultCount(string where);
		double startTimer();
		string getTimer(double startTime, string txt, int preci=3);
		int row2int(MYSQL_ROW& row, uint64_t* lengths, int index);
		bool row2bool(MYSQL_ROW& row, uint64_t* lengths, int index);
		string row2string(MYSQL_ROW& row, uint64_t* lengths, int index);

		/* ########## search ########## */
		typedef struct wordsCount_t
		{
			string word;
			int count;
		} wordsCount_struct_t;

		vector<wordsCount_t> wordsFound_v;
		vector<wordsCount_t> wordsNotFound_v;
		bool s_oneWord;
		bool s_exactWords;
		bool s_title;
		bool s_theme;
		bool s_url;
		string excludeWord;

		string sqlListVideo_getSelect();
		string sqlListVideo_getWhereTimings(cmdListVideo_t* clv, time_t& now);
		bool   sqlListVideo_processingRetData(MYSQL_RES* result, vector<listVideo_t>& lv);
		bool   sqlSearchVideo_checkFound(cmdListVideo_t* clv, string where0, string whereTimings);

	public:
		CSql();
		~CSql();

		bool connectMysql();
		bool sqlSearchVideo(cmdListVideo_t* clv, listVideoHead_t* lvh, vector<listVideo_t>& lv);
		bool sqlListVideo(cmdListVideo_t* clv, listVideoHead_t* lvh, vector<listVideo_t>& lv);
		bool sqlGetProgInfo(progInfo_t* pi);
		bool sqlListLiveStreams(vector<livestreams_t>& ls);
		bool sqlListChannels(vector<channels_t>& ch);
};


#endif // __SQL_H__
