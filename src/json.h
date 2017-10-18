
#ifndef __JSON_H__
#define __JSON_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <json/json.h>

#include <string>

#include "types.h"

using namespace std;

class CJson
{
	private:

		string cooliSig;

		void Init();
		void errorMsg(const char* func, int line, string msg="");
		void parseError(const char* func, int line, string msg="");
		void resetQueryHeaderStruct(query_header_t* qh);
		void resetListVideoStruct(listVideo_t* lv);
		bool parseListVideo(Json::Value root);
		bool asBool(Json::Value::iterator it);

	public:

		CJson();
		~CJson();

		bool parsePostData(string jData);
		string styledJson(string json);
		string styledJson(Json::Value json);
};



#endif // __JSON_H__
