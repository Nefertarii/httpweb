	// function name   AaaBbbCcc
	// variable name   aaaBbbCcc
	
	var passwdLabel = document.getElementById("passwd_label");
	var passwdInput = document.getElementById("passwd_input");
	var userNameInput = document.getElementById("username_input");
	var userNameError = document.getElementById("username_error");
	var emailInput = document.getElementById("email_input");
	var emailError = document.getElementById("email_error");
	passwdInput.addEventListener("keydown", PasswdDetect);
	userNameInput.addEventListener("keydown", UserNameDetect);
	emailInput.addEventListener("keydown", EmailDetect);
	
	function UserNameDetect()
	{
		emailError.style.visibility="hidden";
		userNameInput.style.backgroundImage = "url('../Image/gif/delay-16px.gif')";
		setTimeout(UserNameInput, 500);
		function UserNameInput() {
			if (8 <= userNameInput.value.length) {
				userNameError.style.visibility="visible";
				userNameError.className = "success"
				userNameError.innerHTML = userNameError.innerHTML=
				"Username " + userNameInput.value + " is a available";
				userNameInput.style.borderColor = "green";
				userNameInput.style.backgroundImage = "url('../Image/png/success.png')";
			} 
			else if (userNameInput.value.length == 0) {
				userNameError.style.visibility="hidden";
				userNameInput.style.backgroundImage = "url('none')";
				userNameInput.style.borderColor = "#d8dee2";
			}
			else {
				userNameError.style.visibility="visible";
				userNameError.className = "error"
				userNameError.innerHTML = userNameError.innerHTML=
				"Username " + userNameInput.value + " is not a available";
				userNameInput.style.borderColor = "red";
				userNameInput.style.backgroundImage = "url('../Image/png/error.png')";
			}
		}
	}
	function EmailDetect()
	{
		emailError.style.visibility="hidden";
		emailInput.style.backgroundImage = "url('../Image/gif/delay-16px.gif')";
		setTimeout(EmailInput, 500);
		var STATE="none";
		var pattern = /^([A-Za-z0-9_\-\.])+\@([A-Za-z0-9_\-\.])+\.([A-Za-z]{2,4})$/;
		function EmailInput() {
			if (true == pattern.test(emailInput.value)) {
				emailError.style.visibility="hidden";
				emailError.className = "success"
				emailInput.style.borderColor = "green";
				emailInput.style.backgroundImage = "url('../Image/png/success.png')";
			} 
			else if (0 == emailError.value) {
				emailError.style.visibility="hidden";
				emailInput.style.backgroundImage = "url('none')";
				emailInput.style.borderColor = "#d8dee2";
			}
			else {
				emailError.style.visibility="visible";
				emailError.className = "error"
				emailError.innerHTML = emailError.innerHTML=
				"Email is invalid or already taken"
				emailInput.style.borderColor = "red";
				emailInput.style.backgroundImage = "url('../Image/png/error.png')";
			}
		}
	}
	
	function PasswdDetect() {
		passwdInput.style.backgroundImage = "url('../Image/delay-16px.gif')";
		setTimeout(PassWordInput, 500);
		function PassWordInput() {
			if (8 <= passwdInput.value.length) {
				passwdLabel.style.color = "green";
				passwdInput.style.borderColor = "green";
				passwdInput.style.backgroundImage = "url('../Image/png/success.png')";
			} else {
				passwdLabel.style.color = "red";
				passwdInput.style.borderColor = "red";
				passwdInput.style.backgroundImage = "url('../Image/png/error.png')";
			}
		}
	}