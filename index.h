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
// #include "mustache.h"

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
  <!-- link rel="apple-touch-icon" sizes="180x180" href="/apple-touch-icon.png">
  <link rel="icon" type="image/png" sizes="32x32" href="/favicon-32x32.png">
  <link rel="icon" type="image/png" sizes="16x16" href="/favicon-16x16.png">
  <link rel="manifest" href="/site.webmanifest" -->
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta charset="utf-8">
  <title>{{USB_PRODUCT}}</title>
  <style>
    body, h1, h2, h3, p, div { font-family: sans; }
    body { background:#1451AE; color:#ffffff; font-size:14px; margin:0; padding:0.4em; max-width:80em; margin:auto; }
    h1, h2 { text-align:center;  }
    button { border-radius: 0.2rem; }
    table { background-color: rgba(0,0,0,.15); width:100%; }
    th { background-color: rgba(0,0,0,.25); }
    td { background-color: rgba(255,255,255,.25); text-align: center; }
    a { color: inherit; }

    #filesystem-tab, #uploader-tab { padding: 1rem 0; text-align:center; }
    #filesystem-tab::before { display:inline-block;content:"Filesystem:"; }
    #filesystem-tab button { color: white; background: rgba(0,0,0,0.25); }
    #filesystem-tab button::before { display:inline-block;content:"Switch to:"; }
    #filesystem-tab button.enabled::before { display:inline-block;content:"Using:"; }
    #filesystem-tab button.enabled { color: lightgreen; background: green; border-color: darkgreen; pointer-events:none; }
    #logs::before { display:block;content:"System Logs:"; margin-bottom: .5rem; }
    .nodelete .delete-tab, .nodelete #uploader-tab { display:none; }
    #upload { margin-right: 2rem; }
    #upload label { display: inline-block; padding: 0.2rem 1rem; background: rgba(0,0,0,.12); }
    #reload { margin: 0; padding: 0; width: 1.8em; height: 1.8em; line-height: .1em; font-size: 1em; }
    #logs { text-align: left; height: 480px; overflow-y: scroll; font-family: courier new; color: lightgreen; background: black; padding: 0.5em 1em; }
    #filesystemlogs { margin: 0 auto; border-collapse: collapse; }
    .fixed { margin: 0 auto; border-collapse: collapse; }

</style>
</head>
<body>
<div class="main">
  <div class="fixed">
    <h1>WUD Ducky Payload Runner</h1>
    <div id="filesystem-tab"> <button id="fs-sd" data-name="sd">SD</button> <button id="fs-spiffs" data-name="spiffs">SPIFFS</button></div>
    <div id="uploader-tab">
      <label for="newfile">Upload a file
        <input id="filepath" type="hidden">
        <input id="newfile" type="file" onchange="setpath()">
      </label>
      <button id="upload" type="button" onclick="upload()">Upload</button><input id="reload" type="button" value="🗘" onClick="history.go(0)">
    </div>
  </div>
  <table width="80%" class="fixed" border="1">
    <thead><tr>
      <th>Name</th>
      <th>Type</th>
      <th>Size (Bytes)</th>
      <th class="payload-run">Action</th>
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
              var isQuackFile = (i.name).match(/quack/);

              var nameCell  = (isQuackFile ? '🦆 ' : '') + `<a href="${i.name}">${i.name.substring(1)}</a> `;
              var runCell   = isQuackFile ? `<a class="payload-runner" href="/runpayload?file=${i.name}"><button type="submit">Run Payload</button></a>` : '';

              cont.innerHTML +=
                `<tr>
                    <td>${nameCell}</td>
                    <td>${i.type}</td> <td>${i.size}</td>
                    <td>${runCell}</td>
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


const char* info_html = R"infoHTML(

<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>WUD-NutQuacker</title>
  <style>
    body { background:#1451AE; color:#ffffff; font-size:14px; font-family:sans; margin:0; padding:0.4em; max-width:80em; margin:auto; }
    h1, h2 { text-align:center;  }
    dl { display: table; width:100%;  }
    dl > div { display: table-row; width:100%; }
    dl > div > dt, dl > div > dd { display: table-cell; padding: 0.25em; height:auto; word-break: break-word; }
    dl > div > dt { width: 50%; font-weight: bold; text-align:right; background-color:rbga(255,255,255,0.5); }
    dl > div > dd { width: auto; background-color:rbga(0,0,0,0.5); }
    dl > div > dt::after { content:":"; display:inline-block;padding-left: 0.25em; height:100%; }
    .infos { background-color: rgba(255,255,255,0.15); padding: 0.3rem 0; }
    .infos h2 { background-color: rgba(0,0,0,0.25); }
    .infos dl { background-color: rgba(255,255,255,.15); padding: 0.3rem 0; }
  </style>
</head>
<body>
  <h1>WUD Ducky 📶💾🦆 System Info</h1>
  <div class="infos">
    <h2>Software</h2>
      <dl>
        <div><dt>SDK version</dt><dd>{{getSdkVersion}}</dd></div>
        <div><dt>Chip Id</dt><dd>{{getChipModel}}</dd></div>
        <div><dt>WebServer</dt><dd>{{webserver_begun}}</dd></div>
      </dl>
    <h2>CPU</h2>
      <dl>
        <div><dt>CPU frequency</dt><dd>{{getCpuFreqMHz}} MHz</dd></div>
      </dl>
    <h2>Flash chip information</h2>
      <dl>
        <div><dt>Flash chip Id</dt><dd>{{getFlashChipMode}}</dd></div>
        <div><dt>Estimated Flash size</dt><dd>{{getFlashChipSize}}</dd></div>
        <div><dt>Flash frequency</dt><dd>{{flashFreq}} MHz</dd></div>
        <div><dt>Flash write mode</dt><dd>{{ideMode}}</dd></div>
      </dl>
    <h2>Sketch information</h2>
      <dl>
        <div><dt>Sketch hash</dt><dd>{{getSketchMD5}}</dd></div>
        <div><dt>Sketch size</dt><dd>{{getSketchSize}}</dd></div>
        <div><dt>Free space available</dt><dd>{{getFreeSketchSpace}}</dd></div>
      </dl>
    <h2>Heap information</h2>
      <dl>
        <div><dt>Total size</dt><dd>{{getHeapSize}}</dd></div>
        <div><dt>Free available</dt><dd>{{getFreeHeap}}</dd></div>
        <div><dt>Lowest level since boot</dt><dd>{{getMinFreeHeap}}</dd></div>
        <div><dt>Largest allocatable block</dt><dd>{{getMaxAllocHeap}}</dd></div>
      </dl>
    <h2>USB information</h2>
      <dl>
        <div><dt>Vendor ID</dt><dd>{{USB_VID}}</dd></div>
        <div><dt>Product ID</dt><dd>{{USB_PID}}</dd></div>
        <div><dt>Manufacturer</dt><dd>{{USB_MANUFACTURER}}</dd></div>
        <div><dt>Product Name</dt><dd>{{USB_PRODUCT}}</dd></div>
        <div><dt>Serial Number</dt><dd>{{USB_SERIAL}}</dd></div>
        <div><dt>HUB</dt><dd>{{usb_begun}}</dd></div>
        <div><dt>HID</dt><dd>{{hid_ready}}</dd></div>
        <div><dt>Serial (user port)</dt><dd>{{usbserial_begun}}</dd></div>
        <div><dt>Serial (debug port)</dt><dd>{{hwserial_begun}}</dd></div>
        <div><dt>Keyboard</dt><dd>{{keyboard_begun}}</dd></div>
        <div><dt>Mouse</dt><dd>{{absmouse_begun}}</dd></div>
        <div><dt>PenDrive</dt><dd>{{pendrive_begun}}</dd></div>
      </dl>
    <h2>Storage</h2>
      <dl>
        <div><dt>SD</dt><dd>{{sd_begun}}</dd></div>
        <div><dt>SPIFFS</dt><dd>{{spiffs_begun}}</dd></div>
      </dl>
  </div>
</body>
</html>


)infoHTML";


const char* info_txt = R"infoTXT(

[ WUD Ducky 🦆 System Information ]
----------------------

  == Software ==
     SDK version: {{getSdkVersion}}
     Chip Id: {{getChipModel}}
     WebServer: {{webserver_begun}}

  == CPU ==
    Frequency: {{getCpuFreqMHz}} MHz

  == Flash chip information ==
    Chip Id: {{getFlashChipMode}}
    Estimated size: {{getFlashChipSize}}
    Frequency: {{flashFreq}} MHz
    Write mode: {{ideMode}}

  == Sketch information ==
    Hash: {{getSketchMD5}}
    Size: {{getSketchSize}}
    Free space available: {{getFreeSketchSpace}}

  == Heap information ==
    Total size: {{getHeapSize}}
    Free available: {{getFreeHeap}}
    Lowest level since boot: {{getMinFreeHeap}}
    Largest allocatable block: {{getMaxAllocHeap}}

  == USB information ==
    Vendor ID: {{USB_VID}}
    Product ID: {{USB_PID}}
    Manufacturer: {{USB_MANUFACTURER}}
    Product Name: {{USB_PRODUCT}}
    Serial Number: {{USB_SERIAL}}
    HUB: {{usb_begun}}
    HID: {{hid_ready}}
    Serial (user port) : {{usbserial_begun}}
    Serial (debug port): {{hwserial_begun}}
    Keyboard: {{keyboard_begun}}
    Mouse: {{absmouse_begun}}
    PenDrive: {{pendrive_begun}}

  == Storage ==
    SD: {{sd_begun}}
    SPIFFS: {{spiffs_begun}}

  == Network ==
    MAC Address: {{MAC_ADDR}}
    STA IP Address: {{STA_ADDR}}
    WiFi Access Point: {{softap_begun}} (SSID={{AP_SSID}} PASS={{AP_PASSWORD}})
    WiFi Station: {{wifista_begun}} (SSID={{STA_SSID}} PASS={{STA_PASSWORD}})


)infoTXT";



