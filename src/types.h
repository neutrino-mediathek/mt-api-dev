
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
	queryMode_None            = 0,
	queryMode_Info            = 1,
	queryMode_listChannels    = 2,
	queryMode_listLivestreams = 3,
	queryMode_beginPOSTmode   = 4,
	queryMode_listVideos      = 5
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

typedef struct progInfo_t
{
	string version;
	time_t vdate;
	string mvversion;
	time_t mvdate;
	int    mventrys;
	string progname;
	string progversion;
	string api;
	string apiversion;
} progInfo_struct_t;

typedef struct livestreams_t
{
	string title;
	string url;
	int    parse_m3u8;
} livestreams_struct_t;

typedef struct query_header_t
{
	string      software;
	int         vMajor;
	int         vMinor;
	bool        isBeta;
	int         vBeta;
	int         mode;
	Json::Value data;
} query_header_struct_t;


#endif // __TYPES_H__
