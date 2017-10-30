
var queryMode_listVideos	= 5;
var queryMode_searchVideos	= 6;

var searchMode_allWords		= 0x1;
var searchMode_oneWord		= 0x2;
var searchMode_exactWords	= 0x4;
var searchMode_title		= 0x100;
var searchMode_theme		= 0x200;
var searchMode_url		= 0x400;

var timeMode_normal		= 1;
var timeMode_future		= 2;

function submitJson()
{
	var data1 = $id('data1').value;
	if ((data1 == "") || (data1 == "{}")) {
		return false;
	}
	else {
		var json = makeJsonString();
		$id('data1').value = json;
		return true;
	}
}

function setFormData()
{
	var data1 = $id('data1').value;
	if ((data1 == "") || (data1 == "{}")) {
		return;
	}
	var jsonValue = JSON.parse(data1);

	$id('channel').value			= jsonValue.data.channel;
	$id('duration').value			= jsonValue.data.duration;
	$id('epoch').value			= jsonValue.data.epoch;
	$id('limit').value			= jsonValue.data.limit;
	$name('future', 0).checked		= (jsonValue.data.timeMode == timeMode_future) ? true : false;
	$id('keywords_title').value		= jsonValue.data.keywords;
	$id('keywords_theme').value		= jsonValue.data.keywords;
//	$id('keywords_title').value		= jsonValue.data.keywords_title;
//	$id('keywords_theme').value		= jsonValue.data.keywords_theme;
	if (jsonValue.mode == queryMode_listVideos) {
		disableSearch(true);
		$name('listMode', 0).checked	= true;
	}
	else {
		disableSearch(false);
		$name('listMode', 1).checked	= true;
	}
	
	var searchMode   = jsonValue.data.searchMode;
	var s_allWords   = ((searchMode & searchMode_allWords) == searchMode_allWords);
	var s_oneWord    = ((searchMode & searchMode_oneWord) == searchMode_oneWord);
//	var s_exactWords = ((searchMode & searchMode_exactWords) == searchMode_exactWords);
	var s_1 = -1;
	if (s_allWords == true) {
		s_1 = 0;
	}
	else if (s_oneWord == true) {
		s_1 = 1;
	}
//	else if (s_exactWords == true) {
//		s_1 = 2;
//	}
	$name('searchMode_1', s_1).checked = true;

	var s_title   = ((searchMode & searchMode_title) == searchMode_title);
	var s_theme   = ((searchMode & searchMode_theme) == searchMode_theme);
	var s_2 = -1;
	if ((s_title == true) && (s_theme == true)) {
		s_2 = 1;
	}
	else if (s_title == true) {
		s_2 = 0;
	}
	else if (s_theme == true) {
		s_2 = 2;
	}
	$name('searchMode_2', s_2).checked	= true;

	changeSearchMode_2();
	changeListMode();
}
    
function makeJsonString()
{
	var jsonValue			= {};
	jsonValue.data			= {};

	jsonValue.software		= "Neutrino Mediathek - CST";
	jsonValue.vMajor		= 0;
	jsonValue.vMinor		= 3;
	jsonValue.isBeta		= true;
	jsonValue.vBeta			= 1;
	jsonValue.mode			= getListMode();

	jsonValue.data.channel		= $id('channel').value;
	jsonValue.data.timeMode		= ($name('future', 0).checked) ? timeMode_future : timeMode_normal;
	jsonValue.data.epoch		= $id('epoch').value;
	jsonValue.data.duration		= $id('duration').value;
	jsonValue.data.limit		= $id('limit').value;
	jsonValue.data.start		= 0;
	jsonValue.data.refTime		= 0;
	jsonValue.data.keywords		= $id('keywords_title').value;
//	jsonValue.data.keywords_title	= $id('keywords_title').value;
//	jsonValue.data.keywords_theme	= $id('keywords_theme').value;
	jsonValue.data.searchMode	= getSearchMode_1() | getSearchMode_2();

	return JSON.stringify(jsonValue, null, '\t');
}

function disableSearch(disable)
{
	if (disable == true) {
		disableSearchEdit('keywords_title', disable);
		disableSearchEdit('keywords_theme', disable);
	}
	else {
		changeSearchMode_2();
	}
	disableRadio('searchMode_1', disable);
	disableRadio('searchMode_2', disable);
}

function disableSearchEdit(id, disable)
{
	$id(id).disabled = disable;
	var elClass = (disable == true) ? 'wmfg_text_1d' : 'wmfg_text';
	$id(id).setAttribute('class', elClass);
}

function changeListMode()
{
	var obj = getRadioActive('listMode');
	if (obj.value == 'list') {
		disableSearch(true);
	}
	else {
		disableSearch(false);
	}
}

function getSearchMode_1()
{
	var obj = getRadioActive('searchMode_1');
	if (obj.value == 'allWords') {
		return searchMode_allWords;
	}
	else if (obj.value == 'oneWord') {
		return searchMode_oneWord;
	}
	else if (obj.value == 'exactWords') {
		return searchMode_exactWords;
	}
	return 0;
}

function getSearchMode_2()
{
	var obj = getRadioActive('searchMode_2');
	if (obj.value == 'title') {
		return searchMode_title;
	}
	else if (obj.value == 'title_theme') {
		return searchMode_title | searchMode_theme;
	}
	else if (obj.value == 'theme') {
		return searchMode_theme;
	}
	return 0;
}

function getListMode()
{
	var obj = getRadioActive('listMode');
	if (obj.value == 'list') {
		return queryMode_listVideos;
	}
	else if (obj.value == 'search') {
		return queryMode_searchVideos;
	}
	return 0;
}

function changeSearchMode_2()
{
	var obj = getRadioActive('searchMode_2');
	if (obj.value == 'title') {
		disableSearchEdit('keywords_title', false);
		disableSearchEdit('keywords_theme', true);
	}
	else if (obj.value == 'title_theme') {
		disableSearchEdit('keywords_title', false);
		disableSearchEdit('keywords_theme', false);
	}
	else if (obj.value == 'theme') {
		disableSearchEdit('keywords_title', true);
		disableSearchEdit('keywords_theme', false);
	}
}

function getRadioActive(id)
{
	for (i = 0; i < $nameCount(id); i++) {
		var obj = $name(id, i);
		if (obj.checked == true) {
			return obj;
		}
	}
	obj = $name(id, 0);
	obj.checked = true;
	return obj;
}

function disableRadio(id, disable)
{
	for (i = 0; i < $nameCount(id); i++) {
		$name(id, i).disabled = disable;
	}
}
