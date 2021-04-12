	// function name   AaaBbbCcc
	// variable name   aaaBbbCcc
	

	var nameInput = document.getElementsByClassName("input_name");
	var nameLabel = document.getElementById("input_error_name");
	var emailInput = document.getElementsByClassName("input_email");
	var emailLabel = document.getElementById("input_error_email");
	var passwdInput = document.getElementsByClassName("input_passwd");
	var passwdLabel = document.getElementById("input_error_passwd");
	
	
	nameInput[0].addEventListener("keydown", NameDetect);
	emailInput[0].addEventListener("keydown", EmailDetect);
	passwdInput[0].addEventListener("keydown", PasswdDetect);
	
	function NameDetect()
	{
		nameLabel.style.visibility="hidden";
		nameInput[0].style.backgroundImage = "url('../Image/gif/delay-16px.gif')";
		setTimeout(inline, 500);
		function inline() {
			if (8 <= nameInput[0].value.length) {
				nameLabel.style.visibility = "visible";
				nameLabel.innerHTML = "用户名 " + nameInput[0].value + " 可以使用";
				nameLabel.className = "input_success";
				nameInput[0].style.borderColor = "#34d058";
				nameInput[0].style.backgroundImage = "url('../Image/png/success.png')";
			} 
			else if (nameInput[0].value.length == 0) {
				nameLabel.style.visibility = "hidden";
				nameInput[0].style.backgroundImage = "url('none')";
				nameInput[0].style.borderColor = "#d8dee2";
			}
			else {
				nameLabel.style.visibility="visible";
				nameLabel.innerHTML = "用户名 " + nameInput[0].value + " 不合法或已被使用";
				nameLabel.className = "input_error";
				nameInput[0].style.borderColor = "#F97583";
				nameInput[0].style.backgroundImage = "url('../Image/png/error.png')";
			}
		}
	}
	function EmailDetect()
	{
		emailLabel.style.visibility="hidden";
		emailInput[0].style.backgroundImage = "url('../Image/gif/delay-16px.gif')";
		setTimeout(inline, 500);
		var STATE="none";
		var pattern = /^([A-Za-z0-9_\-\.])+\@([A-Za-z0-9_\-\.])+\.([A-Za-z]{2,4})$/;
		function inline() {
			if (true == pattern.test(emailInput[0].value)) {
				emailLabel.style.visibility="hidden";
				emailLabel.className = "input_success"
				emailInput[0].style.borderColor = "#34d058";
				emailInput[0].style.backgroundImage = "url('../Image/png/success.png')";
			} 
			else if (0 == emailLabel.value) {
				emailLabel.style.visibility="hidden";
				emailInput[0].style.backgroundImage = "url('none')";
				emailInput[0].style.borderColor = "#d8dee2";
			}
			else {
				emailLabel.style.visibility="visible";
				emailLabel.className = "input_error";
				emailLabel.innerHTML ="邮箱不合法或已被使用";
				emailInput[0].style.borderColor = "#F97583";
				emailInput[0].style.backgroundImage = "url('../Image/png/error.png')";
			}
		}
	}
	function PasswdDetect() {
		passwdInput[0].style.backgroundImage = "url('../Image/delay-16px.gif')";
		setTimeout(PassWordInput, 500);
		function PassWordInput() {
			if (8 <= passwdInput[0].value.length) {
				passwdLabel.style.color = "#34d058";
				passwdInput[0].style.borderColor = "#34d058";
				passwdInput[0].style.backgroundImage = "url('../Image/png/success.png')";
			} 
			else if (passwdInput[0].value.length == 0) {
				passwdInput[0].style.backgroundImage = "url('none')";
				passwdInput[0].style.borderColor = "#d8dee2";
			}
			else {
				passwdLabel.style.color = "#86181D";
				passwdInput[0].style.borderColor = "#F97583";
				passwdInput[0].style.backgroundImage = "url('../Image/png/error.png')";
			}
		}
	}