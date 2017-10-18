
#ifndef __HTML_H__
#define __HTML_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <string>

using namespace std;

class CHtml
{
	private:

		void Init();

	public:
		enum {
			includeCopyR		= 1,
			includeGenerator	= 2,
			includeApplication	= 4,
			includeNoCache		= 8
		};

		CHtml();
		~CHtml();

		string tidyRepair(string html, int displayError=0);
		string getHtmlHeader(string title, int flags, string extraHeader="");
		string getHtmlFooter(string templ, string tagBefore="");
		string getIndexSite();
		string getErrorSite(int errNum, string site);

};



#endif // __HTML_H__
