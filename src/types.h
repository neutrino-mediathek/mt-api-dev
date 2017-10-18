
#ifndef __TYPES_H__
#define __TYPES_H__

#include <json/json.h>
#include <string>

using namespace std;

enum {
	apiMode_coolithek,
	apiMode_unknown
};

enum {
	timeMode_normal = 1,
	timeMode_future = 2
};

enum {
	queryMode_Info            = 1,
	queryMode_listChannels    = 2,
	queryMode_listLivestreams = 3,
	queryMode_listVideos      = 4
};

typedef struct listVideo_t
{
	string channel;
	int    timeMode;
	int    epoch;
	int    duration;
	int    limit;
	int    start;
	time_t refTime;
} listVideo_struct_t;

typedef struct query_header_t
{
	string      software;
	int         vMajor;
	int         vMinor;
	bool        isBeta;
	int         vBeta;
	int         mode;
	bool        debug;
	Json::Value data;
} query_header_struct_t;


#endif // __TYPES_H__
