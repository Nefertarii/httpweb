var login = new XMLHttpRequest();
var login_name = document.getElementById("username").value;
var login_passwd = document.getElementById("password").value;
var login_button = document.getElementById("login_button");
login_button.onclick = function() 
{
	let login_name = document.getElementById("username").value;
	let login_passwd = document.getElementById("password").value;
	var tmp = login_name + "&" + login_passwd;
	login.open("POST", "login", true);
	login.send(tmp);
}


function state_Change() 
{
	if (login.readyState == 4) 
	{ // 4 = "loaded"
		if (login.status == 200) 
		{ // 200 = "OK"
			document.getElementById('p1').innerHTML = login.getAllResponseHeaders();
		} else 
		{
			alert("Problem retrieving data:" + login.statusText);
		}
	}
}
