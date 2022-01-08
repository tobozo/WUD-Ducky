/*\
 *
 * ESP32-S2 WUD-Ducky
 *
 * Copyright (c) 2021 tobozo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
\*/
#pragma once

// Storing index.html in a const char* because SPIFFS upload on esp32-s2 with Arduino IDE is a hassle:
//  - SPIFFS upload can't work with a serial console open while flashing does
//  - Annoying failed reset followind successful SPIFFS upload on WUD increments the ttyACMx device number and has Arduino IDE auto selection fail, producing a false negative
//  - Updating index.html and the webserver logic requires two flashing operations, one of which is disruptive in the development flow
//  - Static files can't do templating without ram allocation, also at the time of writing this comment, sketch uses 58% of program storage space, so there's plenty of room
//  - Easy maintenance: just enable syntax highlighting for HTML and ignore the C header/footer of this file

const char* index_html = R"indexHTML(

<!DOCTYPE html>
<html>
<head>
  <link rel="apple-touch-icon" sizes="180x180" href="/apple-touch-icon.png">
  <link rel="icon" type="image/png" sizes="32x32" href="/favicon-32x32.png">
  <link rel="icon" type="image/png" sizes="16x16" href="/favicon-16x16.png">
  <link rel="manifest" href="/site.webmanifest">
  <meta charset="utf-8">
  <style>

    #fs-name::before { display:inline-block;content:"Filesystem:"; }
    #fs-name button { color: red; }
    #fs-name button::before { display:inline-block;content:"Switch to:"; }
    #fs-name button.enabled::before { display:inline-block;content:"Using:"; }
    #fs-name button.enabled { color: green; pointer-events:none; border-color: transparent; background: transparent; }
    #logs::before { display:block;content:"System Logs:"; }

    .nodelete .delete-tab, .nodelete #uploader-tab { display:none; }

  </style>
</head>
<body>
<div class="main">
  <table width="100%" class="fixed" border="0">
  <tr><td>
    <h2>WUD Ducky Payload Runner</h2>
  </td><td id="uploader-tab">
    <table border="0">
      <tr>
        <td>
          <label for="newfile">Upload a file</label>
        </td>
        <td>
          <input id="filepath" type="hidden">
          <input id="newfile" type="file" onchange="setpath()" style="width:100%;">
        </td>
      </tr>
      <tr>
        <td></td>
        <td>
          <button id="upload" type="button" onclick="upload()">Upload</button>
        </td>
      </tr>
    </table>
  </td></tr>
  </table>
  <table width="100%" class="fixed" border="1">
    <thead>
      <tr><th>Name</th><th>Type</th><th>Size (Bytes)</th><th class="delete-tab">Delete</th></tr>
    </thead>
    <tbody id="cont-files">
    </tbody>
  </table>
  <br />
  <div>
    <div id="fs-name"> <button id="fs-sd" data-name="sd">SD</button>  <button id="fs-spiffs" data-name="spiffs">SPIFFS</button> </div>
  </div>
  <div id="logs" style="white-space:pre"></div>
</div>
</body>
<script>


var sdBtn     = document.querySelector('#fs-sd');
var spiffsBtn = document.querySelector('#fs-spiffs');
var mainDiv   = document.querySelector('div.main');


function loadLogs() {
  var xh = new XMLHttpRequest();
  xh.onreadystatechange = () => {
    if (xh.readyState == 4 && xh.status == 200) {
      var res = xh.responseText.split("\n");
      var cont = document.querySelector('#logs');
      cont.innerHTML = '';
      res.forEach(str => {
        cont.innerHTML += str + "\n";
      });
    };
  };
  xh.open("GET", "/logs", true);
  xh.send(null);
};



function changeFS( newfs ) {
  var xh = new XMLHttpRequest();
  xh.onreadystatechange = () => {
    if (xh.readyState == 4 && xh.status == 200) {
      document.location.reload();
    };
  };
  xh.open("GET", "/changefs?fs="+newfs, true);
  xh.send(null);
};



function onFSBtnClick( btn ) {
  if( !btn.className.match(/enabled/) ) {
    changeFS( btn.dataset.name );
  }
}


function setpath() {
  var default_path = document.getElementById("newfile").files[0].name;
  document.getElementById("filepath").value = default_path;
}

function upload() {
  var filePath = document.getElementById("filepath").value;
  var upload_path = "/upload";
  var fileInput = document.getElementById("newfile").files;

  /* Max size of an individual file. Make sure this
    * value is same as that set in file_server.c */
  var MAX_FILE_SIZE = 20*1024*1024;
  var MAX_FILE_SIZE_STR = "20MB";

  if (fileInput.length == 0) {
    alert("No file selected!");
  } else if (filePath.length == 0) {
    alert("File path on server is not set!");
  } else if (filePath.indexOf(' ') >= 0) {
    alert("File path on server cannot have spaces!");
  } else if (filePath[filePath.length-1] == '/') {
    alert("File name not specified after path!");
  } else if (fileInput[0].size > MAX_FILE_SIZE) {
    alert("File size must be less than " + MAX_FILE_SIZE_STR);
  } else {
    document.getElementById("newfile").disabled = true;
    document.getElementById("filepath").disabled = true;
    document.getElementById("upload").disabled = true;

    var file = fileInput[0];
    var xh = new XMLHttpRequest();
    xh.onreadystatechange = function() {
      if (xh.readyState == 4) {
        if (xh.status == 200) {
          //document.open();
          //document.write(xh.responseText);
          //document.close();
          loadLogs();
        } else if (xh.status == 0) {
          alert("Server closed the connection abruptly!");
          location.reload()
        } else {
          alert(xh.status + " Error!\n" + xh.responseText);
          location.reload()
        }
      }
    };
    var formData = new FormData();
    formData.append("filename", file);
    xh.open("POST", upload_path);
    xh.send(formData);
  }
}



(function() {

  var loadFiles = dir => {
    var xh = new XMLHttpRequest();
    xh.onreadystatechange = () => {
      if (xh.readyState == 4 && xh.status == 200) {
        var res = JSON.parse(xh.responseText);
        if( res ) {
          var cont = document.querySelector('#cont-files');
          var sta = document.querySelector('#fs-name');
          if( sta ) {
            if( res.filesystem ) {
              switch( res.filesystem ) {
                case 'spiffs': sdBtn.classList.remove('enabled'); spiffsBtn.classList.add('enabled'); break;
                case 'sd':
                  spiffsBtn.classList.remove('enabled'); sdBtn.classList.add('enabled');
                  mainDiv.classList.add("nodelete");
                break;
              }
              sdBtn.onclick     = () => { onFSBtnClick(sdBtn) };
              spiffsBtn.onclick = () => { onFSBtnClick(spiffsBtn) };
            } else sta.innerHTML = "No filesystem info in JSON";
          } else document.body.innerHTML += "Can't find fs-name div";
          if( cont ) {
            cont.innerHTML = '';
            res.files.forEach(i => {
              var links;

              if( (i.name).match(/quack/) ) {
                links = `🦆 <a href="${i.name}">${i.name.substring(1)}</a> `
                          + ` [<a class="payload-runner" href="/runpayload?file=${i.name}">Run</a>] `
                          + ` [<a class="payload-viewer" href="${i.name}">View</a>] `
                ;
              } else {
                links = ` <a href="${i.name}">${i.name.substring(1)}</a> `;
              }
              cont.innerHTML +=
                `<tr>
                  <td>${links}</td> <td>${i.type}</td> <td>${i.size}</td>
                  <td class="delete-tab"><form method="post" action="/delete"><input type="hidden" name="dir" value="${i.name}"><button type="submit">Delete</button></form></td>
                </tr>`;
            });
          } else document.body.innerHTML += "No file list container";
        } else document.body.innerHTML += "JSON PARSING FAILED";
        loadLogs();
      };
    };
    xh.open("GET", "/list?dir=" + dir, true);
    xh.send(null);
  };

  loadFiles(window.location.pathname);
})();
</script>
</html>



)indexHTML";
