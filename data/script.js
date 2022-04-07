function toggleCheckbox(element) {
    var toggle = new XMLHttpRequest();
    if (element.checked) {
        toggle.open("GET", "/update?pumpstate=1", true);
    }
    else {
        toggle.open("GET", "/update?pumpstate=0", true);
    }
    toggle.send();
}
let x;
setInterval(function () {
    const xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            console.log(1, this.responseText)
            if (this.responseText !== x) {
                image = document.getElementById("imgDisp");
                image.src = this.responseText;
                switch (this.responseText) {
                    case "100off":
                        document.getElementById("full").innerHTML = "100%";
                        break;
                    case "75on":
                        document.getElementById("full").innerHTML = "75%";
                        break;
                    case "75off":
                        document.getElementById("full").innerHTML = "75%";
                        break;
                    case "50on":
                        document.getElementById("full").innerHTML = "50%";
                        break;
                    case "50off":
                        document.getElementById("full").innerHTML = "50%";
                        break;
                    case "25off":
                        document.getElementById("full").innerHTML = "25%";
                        break;
                    case "25on":
                        document.getElementById("full").innerHTML = "25%";
                        break;
                    default:
                        document.getElementById("full").innerHTML = "unknown%"
                        break;
                }
            }
            x = this.responseText;
        }
        console.log(2, x);
    };
    xhttp.open("GET", "/updateimage", true);
    xhttp.send();
}, 1000);
setInterval(function () {
    var phttp = new XMLHttpRequest();
    phttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("pump").innerHTML = this.responseText;
            console.log(4, this.responseText);
            if (this.responseText === 'ON') {
                document.getElementById("box").checked = true;
            }
            else if (this.responseText ===  'OFF') {
                document.getElementById("box").checked = false;
            }
        }
    };
    phttp.open("GET", "/pump", true);
    phttp.send();
}, 1000);
setInterval(function () {
    var yhttp = new XMLHttpRequest();
    yhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("reserv").innerHTML = this.responseText;
        }
    };
    yhttp.open("GET", "/reserv", true);
    yhttp.send();
}, 1000)
