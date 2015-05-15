//Configuration Parser :D
var ConfigurationParser = function()
{
	this.data = "";
}

ConfigurationParser.prototype.setFile = function(data) { this.file = data; };
ConfigurationParser.prototype.getFile = function() { return data; };
ConfigurationParser.prototype.get = function(key) { return this.map.get(key); };
ConfigurationParser.prototype.set = function(key, value) { this.map.set(key, value); return value; };
ConfigurationParser.prototype.refresh = function()
{
	mp = new Map();
	var lines = this.file.split('\n');
	for(var i = 0; i < lines.length; i++)
	{
		lines[i] = lines[i].split('#')[0];
		lines[i].trim();
		var vals = lines[i].split('=');
		if(vals.length == 1)
		{
			mp.set(decodeURI(vals[0]), "");
		}
		else
		{
			mp.set(decodeURI(vals[0]), decodeURI(vals[1]));
		}
	}
	this.map = mp;
};
ConfigurationParser.prototype.flush = function()
{
	var f = "";
	this.map.forEach(function(value, key)
	{
		f += encodeURI(key) + "=" + encodeURI(value) + "\n";
	});
	this.file = f;
};
ConfigurationParser.prototype.getMap = function() { return this.map; };

//Does boot stuff
$(document).ready(function()
{
	ajax_post("/authenticate", "username=" + encodeURI(localStorage.getItem("username")) + "&password=" + encodeURI(localStorage.getItem("password")), function(x)
	{
		if(x == "true" || (username != undefined && password != undefined))
		{
			username = localStorage.getItem("username");
			password = localStorage.getItem("password");
			setPage();
		}
		document.querySelectorAll("body")[0].style.display = "block";
	});
});

//Logs in
function login()
{
	ajax_post("/authenticate", "username=" + encodeURI(document.querySelectorAll("#LoginFields #username")[0].value) + "&password=" + encodeURI(document.querySelectorAll("#LoginFields #password")[0].value), function(x)
	{
		if(x == "true")
		{
			username = document.querySelectorAll("#LoginFields #username")[0].value;
			password = document.querySelectorAll("#LoginFields #password")[0].value;
			localStorage.setItem("username", username);
			localStorage.setItem("password", password);
			setPage();
		}
		else
		{
			document.querySelectorAll("#LoginFields #username")[0].style.backgroundColor = "red";
			document.querySelectorAll("#LoginFields #username")[0].style.color = "blue";
			document.querySelectorAll("#LoginFields #password")[0].style.backgroundColor = "red";
			document.querySelectorAll("#LoginFields #password")[0].style.color = "blue";
		}
	});
}

var cfg = new ConfigurationParser();

function setPage()
{
	document.querySelectorAll("#LoginFields")[0].innerHTML = "";
	document.querySelectorAll("#LoginFields")[0].style.top = "-100px";
	document.querySelectorAll("#LoginFields")[0].style.left = "-100px";
	document.querySelectorAll("#LoginFields")[0].style.bottom = "210%";
	document.querySelectorAll("#LoginFields")[0].style.right = "210%";
	document.querySelectorAll("#Form")[0].style.top = "0px";
	document.querySelectorAll("#Form")[0].style.left = "0px";
	document.querySelectorAll("#Form")[0].style.right = "0px";
	document.querySelectorAll("#Form")[0].style.bottom = "0px";
	document.querySelectorAll("#Form #logout")[0].onclick = logout;
	ajax_post("/getName", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&token=" + encodeURI(token), function(x)
	{
		document.querySelectorAll("#Form #Holder #name")[0].innerHTML = "Name: " + x;
	});
	document.querySelectorAll("#Form #Holder #tbl-qty #m10")[0].onclick = function() { document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value = parseInt(document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value) - 10; if(parseInt(document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value) < 0) document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value = "0"; }
	document.querySelectorAll("#Form #Holder #tbl-qty #m1")[0].onclick = function() { document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value = parseInt(document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value) - 1; if(parseInt(document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value) < 0) document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value = "0"; }
	document.querySelectorAll("#Form #Holder #tbl-qty #p10")[0].onclick = function() { document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value = parseInt(document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value) + 10; if(parseInt(document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value) < 0) document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value = "0"; }
	document.querySelectorAll("#Form #Holder #tbl-qty #p1")[0].onclick = function() { document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value = parseInt(document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value) + 1; if(parseInt(document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value) < 0) document.querySelectorAll("#Form #Holder #tbl-qty #quantity")[0].value = "0"; }
	ajax_post("/getDB", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&token=" + token, function(a)
	{
		cfg.setFile(a.trim());
		cfg.refresh();
		loadElements();
	});
	document.querySelectorAll("#Form #Holder #Submit").onclick = function()
	{
		cfg.flush();
		ajax_post("/setDB", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&token=" + token + "&data=" + encodeURI(cfg.getFile), function(b)
		{
			if(a != "true") alert("Submit Failed! Data not saved!");
		});
	};
}

var dict = new Map();

function loadElements()
{
	dict.clear();
	var i = 0;
	var t = "<tr><th class=\"name\">Name</th><th>Value</th></tr>";
	cfg.getMap().forEach(function(value, key)
	{
		i++;
		dict[i] = key;
		t += "<tr onclick=\"javascript:update(" + this + ");\">";
		t += "<td class=\"name\">" + key + "</td>";
		t += "<td><input onclick=\"javascript:update(" + i + ");\" type=\"text\" class=\"login_fields\" value=\"" + value + "\"></td>";
		t += "</tr>";
		document.querySelectorAll("#Form #Holder #fields")[0].innerHTML = t;
	});
}

function update(index)
{
}

function logout()
{
	localStorage.removeItem("username");
	localStorage.removeItem("password");
	location.reload();
}

//Stores the current credentials
if(username != null && username != undefined) var username = "";
if(password != null && password != undefined) var password = "";

//Communication with the server
function ajax_get(location, callback)
{
	var xhr = new XMLHttpRequest();
	xhr.onreadystatechange = function()
	{
		if(xhr.readyState == 4)
		{
			callback(xhr.responseText);
		}
	};
	xhr.timeout = 4000;
	xhr.open("GET", location, true);
	xhr.send();
}

function ajax_post(location, data, callback)
{
	var xhr = new XMLHttpRequest();
	xhr.onreadystatechange = function()
	{
		if(xhr.readyState == 4)
		{
			callback(xhr.responseText);
		}
	};
	xhr.timeout = 4000;
	xhr.open("POST", location, true);
	xhr.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xhr.send(data);
}