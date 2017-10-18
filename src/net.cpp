
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <errno.h>

#include <boost/network/uri.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>

#include "common/helpers.h"
#include "net.h"

namespace uri = boost::network::uri;

CNet::CNet()
{
	Init();
}

void CNet::Init()
{
	postMaxData = 1024 * 32; /* 32KB */
}

CNet::~CNet()
{
}

void CNet::sendContentTypeHeader(string type/*="text/html; charset=utf-8"*/)
{
	cout << "Content-Type: " << type << "\n\n";
#ifdef SANITIZER
	cerr << "Content-Type: " << type << "\n\n";
#endif
}

string CNet::readGetData(string &data)
{
	data = "";
	char* get = getenv("QUERY_STRING");
	if (get != NULL)
		data = (string)get;

	return data;
}

bool CNet::splitGetInput(string get, vector<string>& get_v)
{
	return splitPostInput(get, get_v);
}

string CNet::getGetValue(vector<string>& get_v, string key)
{
	return getPostValue(get_v, key);
}

string CNet::readPostData(string &data)
{
	/* test whether stdin is associated with a terminal */
	if (isatty(fileno(stdin))) {
		data = "";
		return data;
	}

	/* read POST data */
	data = "";
	for (string line; getline(cin, line);) {
		if ((data.length() + line.length()) > postMaxData) {
			data += line;
			data = data.substr(0, postMaxData-1);
			break;
		}
		data += line;
	}

	return data;
}

bool CNet::splitPostInput(string post, vector<string>& post_v)
{
	stringstream ss;
	ss << post;
	string item;
	while (getline(ss, item, '&'))
		post_v.push_back(item);

	return true;
}

string CNet::getPostValue(vector<string>& post_v, string key)
{
	for (size_t i = 0; i < post_v.size(); i++) {
		vector<string> v = split(post_v[i], '=');
		if (!v.empty() && (v[0] == key))
			return uri::decoded(v[1]);
	}
	return "";
}

/*
cgi environment variables:
--------------------------
AUTH_TYPE
CONTENT_LENGTH
CONTENT_TYPE
DOCUMENT_ROOT
GATEWAY_INTERFACE
HTTP_ACCEPT
HTTP_USER_AGENT
PATH
PATH_INFO
PATH_TRANSLATED
QUERY_STRING
REMOTE_ADDR
REMOTE_HOST
REMOTE_IDENT
REMOTE_USER
REQUEST_METHOD
REQUEST_SCHEME
SCRIPT_FILENAME
SCRIPT_NAME
SERVER_ADDR
SERVER_NAME
SERVER_PORT
SERVER_PROTOCOL
SERVER_SIGNATURE
SERVER_SOFTWARE
*/
string CNet::getEnv(string key)
{
	string ret = "";
	char* tmp = getenv(key.c_str());
	if (tmp != NULL)
		ret = (string)tmp;
	
	return ret;
}
