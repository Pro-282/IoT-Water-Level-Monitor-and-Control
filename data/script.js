// XML function to send url with new check state of the checkbox to the ESP 32
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

// XML function in a set Interval function of 1s to get current image corresponding to the pump-state and water level
let x;
setInterval(function () {
    const xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            console.log(1, this.responseText)
            // check if this.responseText is equal to the cached value
            if (this.responseText !== x) {
                image = document.getElementById("imgDisp");
                image.src = this.responseText;

                switch (this.responseText) { // this change the tank-level statement at the bottom of the page
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
            x = this.responseText; // let x cache the value of this.responseText 
            // the reason for cacheing is mainly because of the GIF images, since it average duration is 2.2s
            // and the setInterval is 1s, changing the image every 1s makes the gif break after 1s, which is not a nice UX
            // also since the image won't change until the responseText changes, the page will be more responsive.
        }
        console.log(2, x);
    };
    xhttp.open("GET", "/updateimage", true);
    xhttp.send();
}, 1000);

// XML function inside a setInterval funtion to update pump state on the page
setInterval(function () {
    var phttp = new XMLHttpRequest();
    phttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("pump").innerHTML = this.responseText;
            console.log(4, this.responseText);
            //update the check state of check box based on the responseText value.
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

// same for reserviour state on the page
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
