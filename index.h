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
//  - Static files can't do templating without ram allocation, also at the time of writing this comment, sketch uses 60% of program storage space, so there's plenty of room
//  - Easy maintenance: just enable syntax highlighting for HTML and ignore the C header/footer of this file

const char* index_html = R"indexHTML(

<!DOCTYPE html>
<html>
<head>
  <link rel="apple-touch-icon" sizes="180x180" href="/apple-touch-icon.png">
  <link rel="icon" type="image/png" sizes="32x32" href="/favicon-32x32.png">
  <link rel="icon" type="image/png" sizes="16x16" href="/favicon-16x16.png">
  <link rel="manifest" href="/site.webmanifest">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta charset="utf-8">
  <style>

    body, h1, h2, h3, p, div { font-family: courier, courier-new, sans-serif; }

    #filesystem-tab::before { display:inline-block;content:"Filesystem:"; }
    #filesystem-tab button { color: #ff6a3d; }
    #filesystem-tab button::before { display:inline-block;content:"Switch to:"; }
    #filesystem-tab button.enabled::before { display:inline-block;content:"Using:"; }
    #filesystem-tab button.enabled { color: #f4db7d; pointer-events:none; border-color: #f4db7d; background: #585858; }
    #logs::before { display:block;content:"System Logs:"; }
    .nodelete .delete-tab, .nodelete #uploader-tab { display:none; }


    @media (max-width: 1080px) {
      body{
        width: 100%;
      }
      input[type="file"] {
        background-color: #394f8a;
        color: #ffffff;
        width: 85%;
      }

      #filesystem-tab button {
        border-bottom: #f4db7d;
        width: 120px;
        height: 30px;
      }

      button {
        background-color: #394f8a;
        color: #ff6a3d;
        width:80px;
        height:30px;
      }

      #logs {
        background:#0f213b;
        box-shadow:inset 0px 0px 3px 1px rgba(0,0,0,1);
        height: 480px;
        width: 400px;
        overflow-y: scroll;
        text-align: left;
      }

      .payload-runner .payload-viewer .delete-tab {
        width: 60%;
      }

      .fixed {
        margin: 0 auto;
        border-collapse: collapse;
      }
    }

    /* removing this directive, too wide, also getting rid of any vendor-prefixed directive */
    /*
    * {
      background-color: #1a2238;
      border-radius: 7px 10px 7px 10px;
      -webkit-border-radius: 7px 10px 7px 10px;
      -moz-border-radius: 7px 10px 7px 10px;
      -webkit-font-smoothing: antialiased;
      -moz-osx-font-smoothing: grayscale;
      border-color: #394f8a;
      color: #ff6a3d;
    }
    */

    #filesystem-tab button {
      border-bottom: #f4db7d;
      width: 120px;
      height: 30px;
    }

    input {
      background-color: #394f8a;
      color: #ffffff;
      width: 22%;
    }

    button {
      background-color: #394f8a;
      color: #ff6a3d;
      width:100px;
      height:30px;
    }

    #reload {
      background-color: #394f8a;
      color: #ff6a3d;
      width:100px;
      height:30px;
    }

    h2 {
      box-shadow:inset 0px 0px 3px 1px rgba(0,0,0,1);
      color: #ffffff;
      font-size: 35px;
      text-transform: uppercase;
      background-color:#424242;
      border-bottom: 3px solid #ff6a3d;
      text-align: center;
    }

    label {
      color: #ffffff;
      text-align: right;
    }

    td {
      text-align: center;
    }

    #logs {
      background:#0f213b;
      box-shadow:inset 0px 0px 3px 1px rgba(0,0,0,1);
      text-align: left;
      height: 480px;
      overflow-y: scroll;
    }

    #filesystemlogs {
      background: #394f8a;
      margin: 0 auto;
      border-collapse: collapse;
    }

    .fixed {
      margin: 0 auto;
      border-collapse: collapse;
    }

</style>
</head>
<body>
<div class="main">
  <div class="fixed">
    <h2>WUD Ducky Payload Runner</h2>
    <div id="filesystem-tab"> <button id="fs-sd" data-name="sd">SD</button> <button id="fs-spiffs" data-name="spiffs">SPIFFS</button></div>
    <div id="uploader-tab">
      <label for="newfile">Upload a file</label>
      <input id="filepath" type="hidden">
      <input id="newfile" type="file" onchange="setpath()">
      <button id="upload" type="button" onclick="upload()">Upload</button><input id="reload" type="button" value="Reload Page" onClick="history.go(0)">
    </div>
  </div>
  <table width="80%" class="fixed" border="1">
    <thead><tr>
      <th>Name</th>
      <th>Type</th>
      <th>Size (Bytes)</th>
      <th class="payload-run">Run Payload</th>
      <th class="payload-viewer">Payload Code</th>
      <th class="delete-tab">Delete</th>
    </tr></thead>
    <tbody id="cont-files"></tbody>
  </table>
  <div id="filesystemlogs">
    <div id="logs" style="white-space:pre"></div>
  </div>
</div>
</body>
<script>

var sdBtn     = document.querySelector('#fs-sd');
var spiffsBtn = document.querySelector('#fs-spiffs');
var mainDiv   = document.querySelector('div.main');


function loadLogs()
{
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


function changeFS( newfs )
{
  var xh = new XMLHttpRequest();
  xh.onreadystatechange = () => {
    if (xh.readyState == 4 && xh.status == 200) {
      document.location.reload();
    };
  };
  xh.open("GET", "/changefs?fs="+newfs, true);
  xh.send(null);
};



function onFSBtnClick( btn )
{
  if( !btn.className.match(/enabled/) ) {
    changeFS( btn.dataset.name );
  }
}


function setpath()
{
  var default_path = document.getElementById("newfile").files[0].name;
  document.getElementById("filepath").value = default_path;
}

function upload()
{
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
          var sta = document.querySelector('#filesystem-tab');
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
          } else document.body.innerHTML += "Can't find filesystem-tab div";
          if( cont ) {
            cont.innerHTML = '';
            res.files.forEach(i => {
              var links;

              if( (i.name).match(/quack/) ) {
                links = `🦆 <a href="${i.name}">${i.name.substring(1)}</a> `
                         /* + ` [<a class="payload-runner" href="/runpayload?file=${i.name}">Run</a>] `
                          + ` [<a class="payload-viewer" href="${i.name}">View</a>] `*/
                ;
              } else {
                links = ` <a href="${i.name}">${i.name.substring(1)}</a> `;
              }
              cont.innerHTML +=
               `<tr>
                  <td>${links}</td>
                  <td>${i.type}</td> <td>${i.size}</td>
                  <td><a class="payload-runner" href="/runpayload?file=${i.name}"><button type="submit">Run Payload</button></a></td>
                  <td><a class="payload-viewer" href="${i.name}"><button type="submit">Payload Code</button></a></td>
                  <td class="delete-tab"><form method="post" action="/delete"><input type="hidden" name="dir" value="${i.name}"><button type="submit">Delete</button></form></td>
               </tr>`
              ;
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
