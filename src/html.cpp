
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <errno.h>
#include <tidy.h>
#include <tidybuffio.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>

#include "llvm/Support/FormatVariadic.h"

#include "common/helpers.h"
#include "html.h"

extern const char*	g_progName;
extern const char*	g_progVersion;
extern const char*	g_progCopyright;
extern string		g_dataRoot;
extern string		g_msgBoxText;

CHtml::CHtml()
{
	Init();
}

void CHtml::Init()
{
}

CHtml::~CHtml()
{
}

string CHtml::tidyRepair(string html, int displayError/*=0*/)
{
	TidyBuffer output = {0, 0, 0, 0, 0};
	TidyBuffer errbuf = {0, 0, 0, 0, 0};
	int rc = -1;

	TidyDoc tdoc = tidyCreate();

	int dt;
//	dt = TidyDoctypeAuto;		/* Keep DOCTYPE in input, set version to content */
	dt = TidyDoctypeHtml5;		/* <!DOCTYPE html> */
//	dt = TidyDoctypeOmit;		/* Omit DOCTYPE altogether */
//	dt = TidyDoctypeStrict;		/* Convert document to HTML 4 strict content model */
////	dt = TidyDoctypeLoose;		/* Convert document to HTML 4 transitional content model */
//	dt = TidyDoctypeUser;		/* Set DOCTYPE FPI explicitly */
	
	bool ok = true;
	ok &= tidyOptSetInt (tdoc, TidyDoctypeMode,	 dt); 
	ok &= tidyOptSetInt (tdoc, TidyIndentSpaces,	  2);
	ok &= tidyOptSetInt (tdoc, TidyWrapLen,		512);

	ok &= tidyOptSetBool(tdoc, TidyXhtmlOut,	yes);
	ok &= tidyOptSetBool(tdoc, TidyMark,		 no);
	ok &= tidyOptSetBool(tdoc, TidyReplaceColor,	yes);
	ok &= tidyOptSetBool(tdoc, TidySortAttributes,	 no);
	ok &= tidyOptSetBool(tdoc, TidyBreakBeforeBR,	yes);
	ok &= tidyOptSetBool(tdoc, TidyIndentContent,	yes);
	ok &= tidyOptSetBool(tdoc, TidyIndentCdata,	yes);
	ok &= tidyOptSetBool(tdoc, TidyMakeClean,	 no);

	if (!ok) {
		return llvm::formatv("\n[{0}:{1}] Error tidyOptSetXXX().\n", __func__, __LINE__);
	}

	rc = tidySetErrorBuffer(tdoc, &errbuf);			// Capture diagnostics
	if (rc >= 0)
		rc = tidyParseString(tdoc, html.c_str());	// Parse the input
	if (rc >= 0)
		rc = tidyCleanAndRepair(tdoc);			// Tidy it up!
	if (rc >= 0)
		rc = tidyRunDiagnostics(tdoc);
	if (rc > 1)						// If error, force output.
		rc = (tidyOptSetBool(tdoc, TidyForceOutput, yes) ? rc : -1);
	if (rc >= 0)
		rc = tidySaveBuffer(tdoc, &output);		// Pretty Print

	string ret, errRet;
	errRet = static_cast<string>((const char*)errbuf.bp);
	errRet = str_replace("\r", "", errRet);
	errRet = str_replace("\n", "<br />ABC", errRet);
	errRet = str_replace("ABC", "\n", errRet);

	if (rc >= 0)
		ret = static_cast<string>((const char*)output.bp);

	tidyBufFree(&output);
	tidyBufFree(&errbuf);
	tidyRelease(tdoc);

	if (displayError == 1) {
		ret += "\n<br />\n<br />\n";
		ret += errRet;
	}
	else if (displayError == 2)
		return errRet;

	return ret;
}

string CHtml::getHtmlHeader(string title, int flags, string extraHeader/*=""*/)
{
	string ret = " \
	<!DOCTYPE html>\n \
	<html><head>\n \
	<meta charset=\"utf-8\">\n";
	if ((flags & includeGenerator) == includeGenerator) {
#ifdef USE_CLANG
		string gcc_vers = llvm::formatv("CLANG {0}.{1}.{2}", __clang_major__, __clang_minor__, __clang_patchlevel__);
#else
		string gcc_vers = static_cast<string>(__VERSION__);
		size_t pos = gcc_vers.find_first_of(" ");
		if (pos != string::npos)
			gcc_vers = gcc_vers.substr(0, pos);
		gcc_vers = llvm::formatv("GCC {0}", gcc_vers);
#endif
		string sass_vers = "";
#ifdef SASS_VERSION
		sass_vers = llvm::formatv(", SASS {0}", SASS_VERSION);
#endif
		string tidy_vers = llvm::formatv(" and HTML Tidy for Linux v{0}", tidyLibraryVersion());
		ret += " \
		<meta name=\"generator\" content=\"" + gcc_vers + sass_vers + tidy_vers + "\" />\n";
	}
	if ((flags & includeCopyR) == includeCopyR) {
		ret += " \
		<meta name=\"copyright\" content=\"" + static_cast<string>(g_progCopyright) + "\" />\n";
	}
	if ((flags & includeApplication) == includeApplication) {
		ret += " \
		<meta name=\"application\" content=\"" + static_cast<string>(g_progName) + " " + g_progVersion + "\" />\n";
	}
	if ((flags & includeNoCache) == includeNoCache) {
		ret += " \
		<meta http-equiv=\"cache-control\" content=\"no-cache\" />\n \
		<meta http-equiv=\"Pragma\" content=\"no-cache\" />\n \
		<meta http-equiv=\"Expires\" content=\"0\" />\n";
	}
	if (!extraHeader.empty()) {
		ret += extraHeader;
	}
	ret += " \
	<link href=\"/images/favicon02.ico\" rel=\"icon\" type=\"image/x-icon\" />\n \
	<title>" + title + "</title>\n";
	return ret;
}

string CHtml::getHtmlFooter(string templ, string tagBefore)
{
	return tagBefore + readFile(templ);
}

string CHtml::getIndexSite()
{
	stringstream ret;
	int headerFlags = 0;
	headerFlags |= CHtml::includeCopyR;
	headerFlags |= CHtml::includeGenerator;
	headerFlags |= CHtml::includeApplication;
	string extraHeader = " \
		<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/index.css\" />\n \
		";
	ret << getHtmlHeader("Coolithek", headerFlags, extraHeader);
	string html = readFile(g_dataRoot + "/template/index.html");
	ret << html << endl;
	ret << "</body></html>" << endl;
	return ret.str();
}

string CHtml::getErrorSite(int errNum, string site)
{
	stringstream ret;
	int headerFlags = 0;
	headerFlags |= CHtml::includeCopyR;
	headerFlags |= CHtml::includeGenerator;
	headerFlags |= CHtml::includeApplication;
	string extraHeader = " \
		<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/error.css\" />\n \
		";
	ret << getHtmlHeader(llvm::formatv("Coolithek - Error {0}", errNum), headerFlags, extraHeader);
	string html = readFile(llvm::formatv("{0}/template/error.html", g_dataRoot));

	string errText;
	if (errNum == 403) {
		errText = "Verwehrt der Zugang dir ist.";
	}
	else if (errNum == 404) {
		string site_ = (site.empty()) ? "eine Seite" : "die <span class='errorText3'>" + site + ".html</span>";
		errText = llvm::formatv("Verloren {0} du hast.<br />Wie peinlich. Wie peinlich!", site_);
	}
	else {
		errText = "Unbekannt der Fehler mir ist.";
	}

	html = str_replace("@@@ERR_NUM@@@", to_string(errNum), html);
	ret << str_replace("@@@ERR_TXT@@@", errText, html) << endl;
	ret << "</body></html>" << endl;
	return ret.str();
}
