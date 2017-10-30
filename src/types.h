
#ifndef __TYPES_H__
#define __TYPES_H__

#include <json/json.h>
#include <string>

using namespace std;

enum {
	apiMode_default = 1,
	apiMode_cst     = 2
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
	queryMode_listVideos      = 5,
	queryMode_searchVideos    = 6
};

enum {
	searchMode_allWords	= 0x1,
	searchMode_oneWord	= 0x2,
	searchMode_exactWords	= 0x4,
	searchMode_title	= 0x100,
	searchMode_theme	= 0x200,
	searchMode_url		= 0x400,
	searchMode_dingens1	= 0x800,
	searchMode_dingens2	= 0x1000
};

typedef struct cmdListVideo_t
{
	string channel;
	int    timeMode;
	int    epoch;
	int    duration;
	int    limit;
	int    start;
	time_t refTime;
	string keywords;
	int    searchMode;
} cmdListVideo_struct_t;

typedef struct listVideo_t
{
	string channel;
	string theme;
	string title;
	string description;
	string website;
	string subtitle;
	string url;
	string url_small;
	string url_hd;
	string url_rtmp;
	string url_rtmp_small;
	string url_rtmp_hd;
	string url_history;
	time_t date_unix;
	int    duration;
	int    size_mb;
	string geo;
	int parse_m3u8;
} listVideo_struct_t;

typedef struct listVideoHead_t
{
	int    start;
	int    end;
	int    rows;
	int    total;
	time_t refTime;
	string wordsFound;
	string wordsNotFound;
	/* for debugging */
	string keywords;
	int    searchMode;
	string channel;
} listVideoHead_struct_t;

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

typedef struct channels_t
{
	string channel;
	int    count;
	time_t latest;
	time_t oldest;
} channels_struct_t;

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
