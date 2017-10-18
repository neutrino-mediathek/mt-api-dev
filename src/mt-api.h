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

#ifndef __MT_API_H__
#define __MT_API_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <string>

#include "types.h"

using namespace std;

class CNet;
class CHtml;
class CJson;
class CSql;

class CMtApi
{
	private:

		string queryString;
		bool indexMode;

		void Init();
		string addTextMsgBox(bool clear=false);
		string formatJson(string data, string tagBefore="", string tagAfter="");

	public:
		CNet* cnet;
		CHtml* chtml;
		CJson* cjson;
		CSql* csql;
		stringstream htmlOut;
		string inJsonData;

		vector<string> getData;
		vector<string> postData;

		CMtApi();
		~CMtApi();
		int run(int argc, char *argv[]);

};


#endif // __MT_API_H__
