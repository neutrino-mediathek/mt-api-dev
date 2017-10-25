
//var sqlbuttonDefault = 'defaultHide';
var sqlbuttonDefault = 'defaultShow';
var sqlButtonTxtHide = 'Hide SQL';
var sqlButtonTxtShow = 'Show SQL';
var msgTxt = "<strong>Message:</strong>";

function setFirstData() {
	var text = (sqlbuttonDefault == 'defaultHide') ? sqlButtonTxtShow : sqlButtonTxtHide;
	var disp = (sqlbuttonDefault == 'defaultHide') ? 'none' : 'block';
	document.getElementById('spoiler1_txt').innerHTML = text;
	document.getElementById('sql_0').style.display = disp;
	clearMsgBox();
}

function clearMsgBox() {
	document.getElementById('msgPre').innerHTML = msgTxt;
}

function addTextMsgBox(txt, rep) {
	if (txt == "") {
		clearMsgBox();
		return;
	}
	var tmpTxt = document.getElementById('msgPre').innerHTML;
	var rep2 = new RegExp(rep, "g");
	txt = txt.replace(rep2, '"');
	document.getElementById('msgPre').innerHTML = tmpTxt + txt;
}

function jsonBox(json, rep) {
	var rep2 = new RegExp(rep, "g");
	json = json.replace(rep2, '"');
	document.getElementById("jsonContainer_inner").innerHTML = "<pre>" + json + "</pre>";
}

function sqlBox(data, id) {
	var sql = sqlFormatter.format(data);
	document.getElementById("sqlCode_" + id).innerHTML = "<pre>" + sql + "</pre>";
}

function sqlSpoiler() {
	obj1 = document.getElementById('sql_0');
	obj2 = document.getElementById('spoiler1_txt');
	if (obj1.style.display == 'block') {
		obj1.style.display = 'none';
		obj2.innerHTML = sqlButtonTxtShow;
	}
	else {
		obj1.style.display = 'block';
		obj2.innerHTML = sqlButtonTxtHide;
	}
}
