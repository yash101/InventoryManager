var username = "";
var password = "";

$(document).ready(function()
{
	if(!auth(localStorage.getItem("username"), localStorage.getItem("password")))
	{
		ajax_get("/login.txt", function(x)
		{
			document.querySelectorAll("#navigation #login")[0].innerHTML = x;
			document.querySelectorAll("#navigation #login #login_button")[0].onclick = function()
			{
				var aret = auth(document.querySelectorAll("#navigation #login #username")[0].value, document.querySelectorAll("#navigation #login #password")[0].value);
				if(aret == true || aret == null)
				{
					document.querySelectorAll("#navigation #login")[0].style.backgroundColor = "#C6C6C6";
				}
				else
				{
					document.querySelectorAll("#navigation #login")[0].style.backgroundColor = "red";
				}
			};
		});
	}
});

function auth(u, p)
{
	if(u == null || p == null) return null;
	var y = "username=" + encodeURI(u) + "&password=" + encodeURI(p);
	ajax_post("/authenticate", y, function(z)
	{
		if(z == "true")
		{
			username = u;
			password = p;
			localStorage.setItem("username", username);
			localStorage.setItem("password", password);
			loadHome();
			return true;
		}
		else
		{
			console.log("Returning false after [" + u + "]->[" + p + "]");
			return false;
		}
	});
}

function loadHome()
{
	ajax_get("/loggedin.txt", function(a)
	{
		document.querySelectorAll("#navigation #login")[0].innerHTML = a;
		document.querySelectorAll("#navigation #login #welcome")[0].innerHTML = "Welcome, " + username;
		document.querySelectorAll("#navigation #login #logout")[0].onclick = function()
		{
			localStorage.removeItem("username");
			localStorage.removeItem("password");
			location.reload();
		};
		setNavigationSearchHandler();
		document.querySelectorAll("#navigation #search #query")[0].disabled = false;
		loadListing();
	});
}

function loadListing()
{
	ajax_get("loggedin_content.txt", function(a)
	{
		document.querySelectorAll("#content")[0].innerHTML = a;
		document.querySelectorAll("#content #refresh_icon")[0].onclick = function()
		{
			ajax_post("/getlist", "username=" + encodeURI(username) + "&password=" + encodeURI(password), function(q)
			{
				document.querySelectorAll("#content .table_rest #table")[0].innerHTML = q;
			});
		};
		ajax_post("/getlist", "username=" + encodeURI(username) + "&password=" + encodeURI(password), function(q)
		{
			document.querySelectorAll("#content .table_rest #table")[0].innerHTML = q;
		});

		document.querySelectorAll("#content #table_add #submit")[0].onclick = function()
		{
			ajax_post("/add", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&name=" + encodeURI(document.querySelectorAll("#content #table_add #name")[0].value), function(x)
			{
				ajax_post("/getlist", "username=" + encodeURI(username) + "&password=" + encodeURI(password), function(q)
				{
					document.querySelectorAll("#content .table_rest #table")[0].innerHTML = q;
				});
				document.querySelectorAll("#content #table_add #name")[0].value = "";
			});
		};
		document.querySelectorAll("#content #table_add #name")[0].onkeypress = function(y)
		{
			if(y.keyCode == 13)
			{
				ajax_post("/add", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&name=" + encodeURI(document.querySelectorAll("#content #table_add #name")[0].value), function(x)
				{
					ajax_post("/getlist", "username=" + encodeURI(username) + "&password=" + encodeURI(password), function(q)
					{
						document.querySelectorAll("#content .table_rest #table")[0].innerHTML = q;
					});
					document.querySelectorAll("#content #table_add #name")[0].value = "";
				});
			}
		};
	});
}

var hgjskfgskdlgjhklg = setInterval(function()
{
	document.getElementById("navigation").style.webkitTransform = "none";
	document.getElementById("navigation").style.mozTransform = "none";
	document.getElementById("navigation").style.msTransform = "none";
	document.getElementById("navigation").style.oTransform = "none";
	document.getElementById("navigation").style.transform = "none";
	document.getElementById("content").style.webkitTransform = "none";
	document.getElementById("content").style.mozTransform = "none";
	document.getElementById("content").style.msTransform = "none";
	document.getElementById("content").style.oTransform = "none";
	document.getElementById("content").style.transform = "none";
	clearInterval(hgjskfgskdlgjhklg);
}, 10);

function setNavigationSearchHandler()
{
	document.querySelectorAll("#navigation #search #query")[0].onkeyup = function()
	{
		ajax_post("/search", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&query=" + encodeURI(document.querySelectorAll("#navigation #search #query")[0].value + "&type=" + encodeURI(document.querySelectorAll("#navigation #search #SearchType")[0].value)), function(x)
		{
			document.querySelectorAll("#navigation #search #search_results")[0].innerHTML = x;
		});
	}
}

function increment(token, id)
{
	ajax_post("/increment", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&token=" + encodeURI(token), function(a)
	{
		ajax_post("/getName", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&token=" + encodeURI(token), function(name)
		{
			var x = "<td class=\"quantity\">" + (id + 1) + "</td>";
			x = x + "<td onclick=\"javascript:loadInformation('" + token + "');\">" + name + "</td>";
			x = x + "<td class=\"quantity\">" + a + "</td>";
			x = x + "<td class=\"button\" onclick=\"javascript:increment('" + token + "', " + id + ");\">+</td>";
			x = x + "<td class=\"button\" onclick=\"javascript:decrement('" + token + "', " + id + ");\">-</td>";
			document.querySelectorAll("#content .table_rest #table tr")[id].innerHTML = x;
		});
	});
}

function decrement(token, id)
{
	ajax_post("/decrement", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&token=" + encodeURI(token), function(a)
	{
		ajax_post("/getName", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&token=" + encodeURI(token), function(name)
		{
			var x = "<td class=\"quantity\">" + (id + 1) + "</td>";
			x = x + "<td onclick=\"javascript:loadInformation('" + token + "');\">" + name + "</td>";
			x = x + "<td class=\"quantity\">" + a + "</td>";
			x = x + "<td class=\"button\" onclick=\"javascript:increment('" + token + "', " + id + ");\">+</td>";
			x = x + "<td class=\"button\" onclick=\"javascript:decrement('" + token + "', " + id + ");\">-</td>";
			document.querySelectorAll("#content .table_rest #table tr")[id].innerHTML = x;
		});
	});
}

function loadInformation(token)
{
	ajax_get("/information.txt", function(x)
	{
		document.querySelectorAll("#content")[0].innerHTML = x;
		document.querySelectorAll("#content #home_btn")[0].onclick = function()
		{
			loadHome();
		}
		ajax_post("/getName", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&token=" + token, function(y)
		{
			document.querySelectorAll("#content #name")[0].innerHTML = y;
		});
		ajax_post("/getByToken", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&token=" + token + "&key=quantity", function(z)
		{
			document.querySelectorAll("#content #quantity")[0].value = z;
		});
		ajax_post("/getByToken", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&token=" + token + "&key=description", function(j)
		{
			document.querySelectorAll("#content #description")[0].innerHTML = j;
		});
		document.querySelectorAll("#content #submit")[0].onclick = function()
		{
			ajax_post("/setByToken", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&token=" + token + "&key=quantity&value=" + encodeURI(document.querySelectorAll("#content #quantity")[0].value), function(h)
			{
				document.querySelectorAll("#content #quantity")[0].value = h;
			});
			ajax_post("/setByToken", "username=" + encodeURI(username) + "&password=" + encodeURI(password) + "&token=" + token + "&key=description&value=" + encodeURI(document.querySelectorAll("#content #description")[0].innerHTML), function(k)
			{
				document.querySelectorAll("#content #description")[0].value = k;
			});
		};
		document.querySelectorAll("#content #m10")[0].onclick = function()
		{
			if(parseInt(document.querySelectorAll("#content #quantity")[0].value) <= 0 || isNaN(parseInt(document.querySelectorAll("#content #quantity")[0].value))) document.querySelectorAll("#content #quantity")[0].value = "0";
			document.querySelectorAll("#content #quantity")[0].value = parseInt(document.querySelectorAll("#content #quantity")[0].value) - 10;
			if(parseInt(document.querySelectorAll("#content #quantity")[0].value) <= 0 || isNaN(parseInt(document.querySelectorAll("#content #quantity")[0].value))) document.querySelectorAll("#content #quantity")[0].value = "0";
		};
		document.querySelectorAll("#content #m1")[0].onclick = function()
		{
			if(parseInt(document.querySelectorAll("#content #quantity")[0].value) <= 0 || isNaN(parseInt(document.querySelectorAll("#content #quantity")[0].value))) document.querySelectorAll("#content #quantity")[0].value = "0";
			document.querySelectorAll("#content #quantity")[0].value = parseInt(document.querySelectorAll("#content #quantity")[0].value) - 1;
			if(parseInt(document.querySelectorAll("#content #quantity")[0].value) <= 0 || isNaN(parseInt(document.querySelectorAll("#content #quantity")[0].value))) document.querySelectorAll("#content #quantity")[0].value = "0";
		};
		document.querySelectorAll("#content #p10")[0].onclick = function()
		{
			if(parseInt(document.querySelectorAll("#content #quantity")[0].value) <= 0 || isNaN(parseInt(document.querySelectorAll("#content #quantity")[0].value))) document.querySelectorAll("#content #quantity")[0].value = "0";
			document.querySelectorAll("#content #quantity")[0].value = parseInt(document.querySelectorAll("#content #quantity")[0].value) + 10;
			if(parseInt(document.querySelectorAll("#content #quantity")[0].value) <= 0 || isNaN(parseInt(document.querySelectorAll("#content #quantity")[0].value))) document.querySelectorAll("#content #quantity")[0].value = "0";
		};
		document.querySelectorAll("#content #p1")[0].onclick = function()
		{
			if(parseInt(document.querySelectorAll("#content #quantity")[0].value) <= 0 || isNaN(parseInt(document.querySelectorAll("#content #quantity")[0].value))) document.querySelectorAll("#content #quantity")[0].value = "0";
			document.querySelectorAll("#content #quantity")[0].value = parseInt(document.querySelectorAll("#content #quantity")[0].value) + 1;
			if(parseInt(document.querySelectorAll("#content #quantity")[0].value) <= 0 || isNaN(parseInt(document.querySelectorAll("#content #quantity")[0].value))) document.querySelectorAll("#content #quantity")[0].value = "0";
		};
		ajax_get("/getWorkingURL", function(c)
		{
			var i = 0;
			for(i = 0; i < token.length; i++) if(token[i] == '_') break;
			console.log(i);
			var url = "http://" + c + "/qedit/" + encodeURI(token.substring(i + 1, token.length));
			new QRCode(document.querySelectorAll("#content #qrcd")[0], url);
			document.querySelectorAll("#content #qrcd")[0].onclick = function() { document.open(url, "_blank"); };
		});
	});
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
