/*
	mt-api - Deliver json data based on a html request
	Copyright (C) 2017, M. Liebmann 'micha-bbg'

	License: GPL

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public
	License along with this program; if not, write to the
	Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
	Boston, MA  02110-1301, USA.
*/

#define PROGVERSION "0.1.0"
#define PROGNAME "mediathek-api"
#define COPYRIGHT "2015-2017© M. Liebmann (micha-bbg)"

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

#include <json/json.h>

#include "mt-api.h"
#include "net.h"
#include "html.h"
#include "json.h"
#include "sql.h"
#include "common/helpers.h"

CMtApi*			g_mainInstance;
const char*		g_progName;
const char*		g_progVersion;
const char*		g_progCopyright;
string			g_documentRoot;
string			g_dataRoot;
bool			g_debugMode;
int			g_apiMode;
string			g_msgBoxText;

void myExit(int val);

CMtApi::CMtApi()
{
	cnet		= NULL;
	chtml		= NULL;
	cjson		= NULL;
	csql		= NULL;
	g_debugMode	= false;
	g_apiMode	= apiMode_unknown;
	g_msgBoxText	= "";
	indexMode	= false;
	Init();
}

string CMtApi::addTextMsgBox(bool clear/*=false*/)
{
	g_msgBoxText = "\\n" + str_replace("\n", "\\n", g_msgBoxText);
	string rep = "@X@";
	g_msgBoxText = str_replace("\"", rep, g_msgBoxText);
	string html = readFile(g_dataRoot + "/template/msgbox.html");
	html = str_replace("@@@MSGTXT@@@", (clear)?"":g_msgBoxText, html);
	html = str_replace("@@@REP@@@", rep, html);
	g_msgBoxText = "";
	return html;
}

void CMtApi::Init()
{
	/* read GET data */
	string inData;
	cnet->readGetData(inData);
	cnet->splitGetInput(inData, getData);
	queryString = cnet->getGetValue(getData, "mode");
	if (queryString.empty() || (queryString == "index")) {
		indexMode = true;
	}
	string tmp_s = cnet->getEnv("SERVER_NAME");
	g_debugMode = ((tmp_s.find(".debug.coolithek.") != string::npos) ||
		       (tmp_s.find("coolithek.slknet.de") == 0) ||
		       (indexMode == true));
	string cth = (g_debugMode) ? "text/html; charset=utf-8" : "application/json; charset=utf-8";
	cnet->sendContentTypeHeader(cth);
//#ifdef SANITIZER
	if (g_debugMode) {
		dup2(STDOUT_FILENO, STDERR_FILENO);
	}
//#endif

	g_documentRoot  = cnet->getEnv("DOCUMENT_ROOT");
	g_dataRoot	= getPathName(g_documentRoot) + "/data";
	g_progName	= PROGNAME;
	g_progCopyright	= COPYRIGHT;
	g_progVersion	= "v" PROGVERSION;

	cnet		= new CNet();
	chtml		= new CHtml();
	cjson		= new CJson();
	csql		= new CSql();
}

CMtApi::~CMtApi()
{
	if (cnet != NULL)
		delete cnet;
	if (chtml != NULL)
		delete chtml;
	if (cjson != NULL)
		delete cjson;
	if (csql != NULL)
		delete csql;
}

string CMtApi::formatJson(string data, string tagBefore, string tagAfter)
{
	string html = readFile(g_dataRoot + "/template/json-format.html");
	html = str_replace("@@@JSON_DATA@@@", data, html);

	return tagBefore + html + tagAfter;
}

int CMtApi::run(int, char**)
{
	if (indexMode) {
		htmlOut << chtml->getIndexSite();
		cout << chtml->tidyRepair(htmlOut.str(), 0) << endl;
		return 0;
	}
	else if (queryString == "api") {
	}
	else if (queryString == "coolithek") {
	}
	else if ((queryString.find("page") == 3) && (queryString.length() == 7)) {
		/* 000page */
		htmlOut << chtml->getErrorSite(atoi(queryString.c_str()), "");
		cout << chtml->tidyRepair(htmlOut.str(), 0) << endl;
		return 0;
	}
	else {
		htmlOut << chtml->getErrorSite(404, queryString);
		cout << chtml->tidyRepair(htmlOut.str(), 0) << endl;
		return 0;
	}

	/* read POST data */
	string inData;
	cnet->readPostData(inData);
	cnet->splitPostInput(inData, postData);
	inJsonData = cnet->getPostValue(postData, "data1");
	if (inJsonData.empty())
		inJsonData = readFile(g_dataRoot + "/template/test_1.json");

	csql->connectMysql();

	if (g_debugMode) {
		int headerFlags = 0;
		headerFlags |= CHtml::includeCopyR;
		headerFlags |= CHtml::includeGenerator;
		headerFlags |= CHtml::includeApplication;
		htmlOut << chtml->getHtmlHeader("Coolithek API", headerFlags);

		string tmp1;
		tmp1 = readFile(g_dataRoot + "/template/main-body.html");
		inJsonData = cjson->styledJson(inJsonData);
		tmp1 = str_replace("@@@JSON_TEXTAREA@@@", inJsonData, tmp1);
		tmp1 = str_replace("@@@DEBUG@@@", (g_debugMode)?"?d=true":"", tmp1);
		htmlOut << tmp1;

///////
		bool parseIO = cjson->parsePostData(inJsonData);
		if (parseIO) {
//			if (!inJsonData.empty())
//				htmlOut << formatJson(inJsonData) << endl;
		}
///////

		if (!g_msgBoxText.empty())
			htmlOut << addTextMsgBox();

		htmlOut << chtml->getHtmlFooter(g_dataRoot + "/template/footer.html", "<hr style='width: 80%;'>") << endl;

		/* Output data repaired by tidy */
		cout << chtml->tidyRepair(htmlOut.str(), 0) << endl;
	}
	else {
		string json = "{ \"error\": 1, \"head\": [], \"entry\": \"Unsupported parameter.\" }";
		json = cjson->styledJson(json);
		cout << json << endl;
	}

	return 0;
}

void myExit(int val)
{
	exit(val);
}

int main(int argc, char *argv[])
{
	g_mainInstance = NULL;

	/* main prog */
	g_mainInstance = new CMtApi();
	int ret = g_mainInstance->run(argc, argv);
	delete g_mainInstance;

	return ret;
}
