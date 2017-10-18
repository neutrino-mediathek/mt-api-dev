
#ifndef __NET_H__
#define __NET_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <string>

using namespace std;

class CNet
{
	private:
		uint32_t postMaxData;
		
		void Init();

	public:
		CNet();
		~CNet();

		void sendContentTypeHeader(string type="text/html; charset=utf-8");

		string readGetData(string &data);
		bool splitGetInput(string get, vector<string>& get_v);
		string getGetValue(vector<string>& get_v, string key);

		string readPostData(string &data);
		bool splitPostInput(string post, vector<string>& post_v);
		string getPostValue(vector<string>& post_v, string key);
		void setPostMaxData(uint32_t val) { postMaxData = val; };
		uint32_t getPostMaxData() { return postMaxData; };

		string getEnv(string key);
};



#endif // __NET_H__
