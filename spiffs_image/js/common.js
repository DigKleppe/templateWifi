//var SIMULATE = true;
var SIMULATE = false;

function sendItem(item) {
	console.log("sendItem: " + item);
	if (SIMULATE)
		return "OK";

	var str;
	var req = new XMLHttpRequest();
	req.open("POST", "/upload/cgi-bin/", false);
	req.send(item);
	if (req.readyState == 4) {
		if (req.status == 200) {
			str = req.responseText.toString();
			return str;
		}
	}
}

function getItem(item) {
	console.log("getItem " + item);
	if (SIMULATE)
		return "OK";

	var str;
	var req = new XMLHttpRequest();
	req.open("GET", "cgi-bin/" + item, false);
	req.send();
	if (req.readyState == 4) {
		if (req.status == 200) {
			str = req.responseText.toString();
			return str;
		}
	}
}

function makeInfoTable(descriptorData , tableName) {
	var tbl = document.getElementById(tableName);
	var x = tbl.rows.length;
	if ( x == 0 ) // new table 
	{
		tblBody = document.createElement("tbody");
	}
	else {
		for (var r = 0; r < x; r++) 
			tbl.deleteRow(-1);
	}

//	var rows = descriptorData.split("\n");
	var rows = descriptorData.split("\r");   
	for (var i = 0; i < rows.length ; i++) {
		var row = document.createElement("tr");
		if (i == 0) {
			colls = rows[i].split(",");
			for (var j = 0; j < colls.length; j++) {
				var cell = document.createElement("th");
				var cellText = document.createTextNode(colls[j]);
				cell.appendChild(cellText);
				row.appendChild(cell);
			}
		}
		else {
			colls = rows[i].split("=");
			for (var j = 0; j < colls.length; j++) {
				var cell = document.createElement("td");
				var cellText = document.createTextNode(colls[j]);
				cell.appendChild(cellText);
				row.appendChild(cell);
			}
		}
		tblBody.appendChild(row);
	}
	tbl.appendChild(tblBody);
}

function updateInfoTable ( descriptorData ,tableName) {
	var tbl = document.getElementById(tableName);
	var x = tbl.rows.length;
	if ( x == 0 ) // new table 
		makeInfoTable (descriptorData,tableName);
	else {
		var rows = descriptorData.split("\r");
		if ( rows.length == 1) {  // only header 
			colls = rows[0].split(",");
			tbl.rows[0].cells[1].innerHTML = colls[1];
		}
		else
			for (var i = 1; i < rows.length-1 ; i++) {   // update data fields
				colls = rows[i].split("=");
				tbl.rows[i].cells[1].innerHTML = colls[1];
			}
	}
}
	

function makeSettingsTable(descriptorData, settingsTbl) {

	var colls;
	var x = settingsTbl.rows.length;
	for (var r = 0; r < x; r++) {
		settingsTbl.deleteRow(-1);
	}
	tblBody = document.createElement("tbody");

	var rows = descriptorData.split("\n");

	for (var i = 0; i < rows.length - 1; i++) {
		var row = document.createElement("tr");
		if (i == 0) {
			colls = rows[i].split(",");
			for (var j = 0; j < colls.length - 1; j++) {
				var cell = document.createElement("th");
				var cellText = document.createTextNode(colls[j]);
				cell.appendChild(cellText);
				row.appendChild(cell);
			}
		}
		else {
			colls = rows[i].split(",");
			var cell = document.createElement("td");  // name
			var cellText = document.createTextNode(colls[0]);
			cell.appendChild(cellText);
			row.appendChild(cell);

			cell = document.createElement("td");  // value
			var input = document.createElement("input");
			input.value = colls[1];
			input.setAttribute("type", "number");
			cell.appendChild(input);
			row.appendChild(cell);

			cell = document.createElement("td");
			var button = document.createElement("button");
			button.innerHTML = "Stel in";
			button.className = "button-3";
			cell.appendChild(button);
			cell.setAttribute("settingsItem", i);
			row.appendChild(cell);
		}
		tblBody.appendChild(row);
	}
	settingsTbl.appendChild(tblBody);

	const cells = document.querySelectorAll("td[settingsItem]");
	cells.forEach(cell => {
		cell.addEventListener('click', function() { setSettingFunction(settingsTbl,  cell.closest('tr').rowIndex, cell.cellIndex) });
	});
}

function setSettingFunction( settingsTbl, row, coll) {
	var item = settingsTbl.rows[row].cells[0].innerText;
	var value = settingsTbl.rows[row].cells[1].firstChild.value;
	console.log(item + value);
	sendItem("setVal:" + item + '=' + value);
}


function getSettings ( command, tableName) {
	var str;
	var	settingsTbl = document.getElementById(tableName);
	str = getItem(command);
	makeSettingsTable(str, settingsTbl);	
}


function getInfo( command, tableName) {
	var str;
   	str = getItem(command);
	updateInfoTable( str, tableName);

	// if (firstTime) {
	// 	makeInfoTable(str, tableName);
	// }
	// else {
	// 	var rows = str.split("\n");
	// 	for (var i = 1; i < rows.length - 1; i++) {
	// 		var colls = rows[i].split(",");
	// 		for (var j = 1; j < colls.length; j++) {
	// 			table.rows[i].cells[j].innerHTML = colls[j];
	// 		}
	// 	}
	// }
	return str;
}

