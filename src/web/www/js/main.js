
var sqlbuttonDefault = 'defaultHide';
//var sqlbuttonDefault = 'defaultShow';
var sqlButtonTxtHide = 'Hide SQL';
var sqlButtonTxtShow = 'Show SQL';
var msgTxt = "<strong>Message:</strong>";

function $id(id) { return document.getElementById(id); }
function $name(id, index) { return document.getElementsByName(id)[index]; }
function $nameCount(id) { return document.getElementsByName(id).length; }

function setFirstData() {
	var text = (sqlbuttonDefault == 'defaultHide') ? sqlButtonTxtShow : sqlButtonTxtHide;
	var disp = (sqlbuttonDefault == 'defaultHide') ? 'none' : 'block';
	$id('spoiler1_txt').innerHTML = text;
	$id('sql_0').style.display = disp;
	clearMsgBox();
}

function clearMsgBox() {
	$id('msgPre').innerHTML = msgTxt;
}

function addTextMsgBox(txt) {
	if (txt == "") {
		clearMsgBox();
		return;
	}
	var tmpTxt = $id('msgPre').innerHTML;
	txt = Base64.decode(txt)
	$id('msgPre').innerHTML = tmpTxt + txt;
}

function jsonBox(json) {
	json = Base64.decode(json)
	$id("jsonContainer_inner").innerHTML = "<pre>" + json + "</pre>";
}

function sqlBox(data, id) {
	data = Base64.decode(data)
	var sql = sqlFormatter.format(data);
	$id("sqlCode_" + id).innerHTML = "<pre>" + sql + "</pre>";
}

function sqlSpoiler() {
	obj1 = $id('sql_0');
	obj2 = $id('spoiler1_txt');
	if (obj1.style.display == 'block') {
		obj1.style.display = 'none';
		obj2.innerHTML = sqlButtonTxtShow;
	}
	else {
		obj1.style.display = 'block';
		obj2.innerHTML = sqlButtonTxtHide;
	}
}
