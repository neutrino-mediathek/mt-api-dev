
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

#include "llvm/Support/FormatVariadic.h"

#include "common/helpers.h"
#include "json.h"
#include "sql.h"
#include "mt-api.h"

extern CMtApi*		g_mainInstance;
extern bool		g_debugMode;
extern int		g_apiMode;
extern string		g_msgBoxText;

CJson::CJson()
{
	Init();
}

void CJson::Init()
{
	cooliSig = "coolithek NeutrinoHD";
}

CJson::~CJson()
{
}

string CJson::styledJson(string json)
{
	string ret = json;
	Json::Value root;
	bool ok = parseJsonFromString(json, &root, NULL);
	if (ok)
		ret = root.toStyledString();

	return ret;
}

string CJson::styledJson(Json::Value json)
{
	return json.toStyledString();
}

void CJson::errorMsg(const char* func, int line, string msg)
{
	g_msgBoxText = llvm::formatv("<span style='color: OrangeRed'>[{0}:{1}] {2}</span>",
				     func, line, ((msg.empty())?"Error":msg));

}

void CJson::parseError(const char* func, int line, string msg)
{
	string msg_ = (msg.empty()) ? "" :  + " (" + msg + ")";
	errorMsg(func, line, "Error parsing json data" + msg_);
}

void CJson::resetQueryHeaderStruct(query_header_t* qh)
{
	qh->software  = "";
	qh->vMajor    = 0;
	qh->vMinor    = 0;
	qh->isBeta    = 0;
	qh->vBeta     = false;
	qh->mode      = 0;
	qh->debug     = false;
	qh->data.clear();
}

void CJson::resetListVideoStruct(listVideo_t* lv)
{
	lv->channel  = "";
	lv->timeMode = 0;
	lv->epoch    = 0;
	lv->duration = 0;
	lv->limit    = 0;
	lv->start    = 0;
	lv->refTime  = 0;
}

bool CJson::asBool(Json::Value::iterator it)
{
	string tmp_s = it->asString();
	tmp_s = trim(tmp_s);
	return ((str_tolower(tmp_s) == "false") || (tmp_s == "0")) ? false : true;
}

bool CJson::parseListVideo(Json::Value root)
{
	listVideo_t lv;
	resetListVideoStruct(&lv);
	for (Json::Value::iterator it = root.begin(); it != root.end(); ++it) {
		string name = it.name();
		if (name == "channel") {
			lv.channel = it->asString();
		}
		else if (name == "timeMode") {
				lv.timeMode = safeStrToInt(it->asString());
		}
		else if (name == "epoch") {
			lv.epoch = safeStrToInt(it->asString());
		}
		else if (name == "duration") {
			lv.duration = safeStrToInt(it->asString());
		}
		else if (name == "limit") {
			lv.limit = safeStrToInt(it->asString());
			}
		else if (name == "start") {
			lv.start = safeStrToInt(it->asString());
		}
		else if (name == "refTime") {
			lv.refTime = safeStrToInt(it->asString());
		}
	}

	g_mainInstance->csql->sqlListVideo(&lv);

	return true;
}

bool CJson::parsePostData(string jData)
{
	string errMsg = "";
	Json::Value root;
	bool ok = parseJsonFromString(jData, &root, &errMsg);
	if (!ok) {
		parseError(__func__, __LINE__, errMsg);
		return false;
	}

	query_header_t qh;
	resetQueryHeaderStruct(&qh);
	if (root.isObject()) {
		for (Json::Value::iterator it = root.begin(); it != root.end(); ++it) {
			string name = it.name();
			if (name == "software") {
				qh.software = it->asString();
			}
			else if (name == "vMajor") {
				qh.vMajor = safeStrToInt(it->asString());
			}
			else if (name == "vMinor") {
				qh.vMinor = safeStrToInt(it->asString());
			}
			else if (name == "isBeta") {
				qh.isBeta = asBool(it);
			}
			else if (name == "vBeta") {
				qh.vBeta = safeStrToInt(it->asString());
			}
			else if (name == "mode") {
				qh.mode = safeStrToInt(it->asString());
			}
			else if (name == "debug") {
				g_debugMode = asBool(it);
			}
			else if (name == "data") {
				qh.data = *it;
			}
		}
	}
	else {
		parseError(__func__, __LINE__);
		return false;
	}
	if (qh.data.isObject()) {
		if (qh.software != cooliSig) {
			errorMsg(__func__, __LINE__, "The given signature is ["+qh.software+"], but ["+cooliSig+"] is expected.");
			return false;
		}
		if (qh.mode == queryMode_Info) {
			errorMsg(__func__, __LINE__, "Function not yet available.");
			return false;
		}
		else if (qh.mode == queryMode_listChannels) {
			errorMsg(__func__, __LINE__, "Function not yet available.");
			return false;
		}
		else if (qh.mode == queryMode_listLivestreams) {
			errorMsg(__func__, __LINE__, "Function not yet available.");
			return false;
		}
		else if (qh.mode == queryMode_listVideos) {
			return parseListVideo(qh.data);
		}
		else {
			errorMsg(__func__, __LINE__, "Unknown function.");
			return false;
		}
	}
	else {
		parseError(__func__, __LINE__);
		return false;
	}

	return true;
}

