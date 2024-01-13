#ifndef SCRIPT_H
#define SCRIPT_H

char webScript[] PROGMEM = R"=====(
document.getElementById('channel=up').addEventListener('click', function() { sendRequest('channel=up'); });
document.getElementById('channel=down').addEventListener('click', function() { sendRequest('channel=down'); });
document.getElementById('switch=1').addEventListener('click', function() { this.classList.toggle('mirrored'); sendRequest('switch=1'); });
document.getElementById('switch=2').addEventListener('click', function() { this.classList.toggle('mirrored'); sendRequest('switch=2'); });

function addClickListeners() {
    var buttons = document.getElementsByClassName('custom-button');
    for (var i = 0; i < buttons.length; i++) {
        buttons[i].addEventListener('click', function() { sendRequest(this.id); });
    }
}
addClickListeners();

function sendRequest(command) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            if (command == 'systemList') {
                document.getElementById('systemTable').innerHTML = this.responseText;
                document.getElementById('systemList').disabled = true;
            }
            console.log('Button ' + command + ' pressed');
        }
    };
    xhttp.open('GET', '/?' + command, true);
    xhttp.send();
}

function getDynamicData() {
    nocache = '&nocache=' + Math.random() * 1000000;
    var request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200 && this.responseText != null) {
            document.getElementById('sw_an_data').innerHTML = this.responseText;
        }
    };
    request.open('GET', 'ajax_switch' + nocache, true);
    request.send();
    setTimeout(getDynamicData, 2000); // timeout
}

function logger() {
    var request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200 && this.responseText != null) {
            document.getElementById('webLogger').innerHTML += this.responseText;
        }
    };
    request.open('GET', 'web_logger', true);
    request.send();
    setTimeout(logger, 2500); // timeout
}

function initPage() {
    getDynamicData();
    logger();
}

var loggerBox = document.getElementById("webLogger");
var scrollToBottomBtn = document.getElementById("goToBottomBtn");

loggerBox.onscroll = function() {
    var isScrolledToBottom = loggerBox.scrollHeight - loggerBox.clientHeight <= loggerBox.scrollTop + 1;
    if (isScrolledToBottom) {
        scrollToBottomBtn.style.display = "none";
    } else {
        scrollToBottomBtn.style.display = "block";
    }
};

function goToBottom() {
    loggerBox.scrollTop = loggerBox.scrollHeight;
    scrollToBottomBtn.style.display = "none";
}

)=====";
#endif
