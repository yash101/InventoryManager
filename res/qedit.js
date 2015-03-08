//Does boot stuff
$(document).ready(function()
{
	ajax_post("/authenticate", "username=" + encodeURI(localStorage.getItem("username")) + "&password=" + encodeURI(localStorage.getItem("password")), function(x)
	{
		if(x == "true")
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
}

function logout()
{
	localStorage.removeItem("username");
	localStorage.removeItem("password");
	location.reload();
}

//Stores the current credentials
var username = "";
var password = "";

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

//Configuration Parser :D
var ConfigurationParser = function(data)
{
	this.file = data;
	this.refresh();
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