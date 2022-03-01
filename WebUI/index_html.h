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
//
//
//  - Easy maintenance: ENABLE SYNTAX HIGHLIGHTING FOR HTML and ignore the C header/footer of this file

const char* index_html = R"indexHTML(

<!doctype html public "🦆">
<html>
<head>
  <link rel="apple-touch-icon" sizes="180x180" href="/apple-touch-icon.png">
  <link rel="icon" type="image/png" sizes="32x32" href="/favicon-32x32.png">
  <link rel="icon" type="image/png" sizes="16x16" href="/favicon-16x16.png">
  <link rel="manifest" href="/site.webmanifest">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta charset="utf-8">
  <title>WUD-NutQuacker</title>
  <link rel="stylesheet" type="text/css" href="/styles.css">
</head>
<body class="explorer nodelete">
  <audio autoplay src="/Quack.wav"></audio>
  <input type="checkbox" id="control-panel-toggle" style="display: none;">
  <input type="checkbox" id="infos-panel-toggle" style="display: none;">
  <nav>
    <button id="reload" class="action-button" onClick="history.go(0)"><label for="no-reload-checkbox">🔃</label></button>
    <button id="info" class="action-button" onClick="showInfo();"><label for="infos-panel-toggle">🛈</label></button>
    <label for="newfile" class="action-button icon upload"></label>
    <button id="settings" class="action-button"><label for="control-panel-toggle">⚙</label></button>
  </nav>
  <div class="main">
    <div class="fixed">
      <h1>WUD Ducky</h1>
      <div id="control-panel">
        <h2>Filesystem</h2>
        <ul class="radio-switch">
          <li class="radio-switch__item">
            <input type="radio" class="radio-switch__input sr-only" id="fs1" name="fs-switcher" data-fs="sd" onclick="onFSRadioClick(this)">
            <label for="fs1" class="radio-switch__label icon sdcard"></svg>SD</label>
          </li>

          <li class="radio-switch__item">
            <input type="radio" class="radio-switch__input sr-only" id="fs3" name="fs-switcher" data-fs="spiffs" checked onclick="onFSRadioClick(this)">
            <label for="fs3" class="radio-switch__label icon spiffs">SPIFFS</label>
            <div class="radio-switch__marker" aria-hidden="true"></div>
          </li>
        </ul>

        <h2>Logging</h2>
        <ul class="radio-switch">
          <li class="radio-switch__item">
            <input type="radio" class="radio-switch__input sr-only" id="log1" name="logs-switcher" data-logs="on" checked onclick="setLogs(this)">
            <label for="log1" class="radio-switch__label"></svg>ON</label>
          </li>

          <li class="radio-switch__item">
            <input type="radio" class="radio-switch__input sr-only" id="log2" name="logs-switcher" data-logs="off" onclick="setLogs(this)">
            <label for="log2" class="radio-switch__label">OFF</label>
            <div class="radio-switch__marker" aria-hidden="true"></div>
          </li>
        </ul>

        <div class="logs-viewer">
          <input class="toggle-button" type="checkbox" id="view-logs-toggler" onclick="if(checked) loadLogs()">
          <label for="view-logs-toggler" class="">📜 View logs</label>
          <div id="filesystemlogs">
            <button class="clear-logs-button" onclick="clearLogs()">Clear logs</button>
            <div id="logs" style="white-space:pre"></div>
          </div>
        </div>

      </div>
    </div>

    <div class="mouse-tab-container"><div id="mouse-tab" data-btn data-x data-y></div></div>

    <div id="uploader-tab">
      <input id="newfile" type="file" onchange="setpath()">
      <input id="filepath" type="hidden">
      <button id="upload" type="button" onclick="upload()">Upload</button>
    </div>

    <div class="quacker-tab">
      <label for="quackinput">Who let the ducks out?</label>
      <input list="ducky-commands" id="quackinput" name="quackinput" />
      <button id="quack" onclick="quack(quackinput.value);quackinput.value=''">Quack!</button>
      <datalist id="ducky-commands"><!--  <option value="Blah"> --></datalist>
    </div>

    <div class="files-view">
      <input class="toggle-button" type="radio" name="compact-view" value="on"  id="cv1" checked>
      <input class="toggle-button" type="radio" name="compact-view" value="off" id="cv2">
      <div class="toggle-buttons">
        <label class="toggle-button" for="cv1">☰</label>
        <label class="toggle-button" for="cv2">☶</label>
      </div>
      <dl class="fixed table" id="cont-files"><h3>Loading Files List...</h3></dl>
    </div>

    <div class="infos"><h3>Loading Sytem Information...</h3></div>

  </div>


  <script type="text/template" id="locales-template">
    <div class="locales-tab">
      <input list="ducky-locales" id="localesinput" name="localesinput" style="width:6em" />
      <button id="quack-locale" onclick="quack('LOCALE ' + quackinput.value);this.parentElement.parentElement.innerText=quackinput.value">Set Locale</button>
      <datalist id="ducky-locales"></datalist>
    </div>
  </script>


  <script type="text/template" id="infos-template">

      <h2>Firmware / CPU / Flash</h2>
        <dl>
          <div><dt>Chip Id</dt><dd><span class="espressif icon">{{getChipModel}}</span></dd></div>
          <div><dt>📟 Serial debug</dt><dd><span class="{{hwserial_begun}}" data-status>◉</span> ({{SerialDebug}})</dd></div>
          <div><dt>Logging</dt><dd><span class="{{logging_enabled}}" data-status>◉</span></dd></div>
          <div><dt>OTA</dt><dd><span class="{{ota_enabled}}" data-status>◉</span></dd></div>
          <div><dt>CPU frequency</dt><dd>{{getCpuFreqMHz}} MHz</dd></div>
          <div><dt>Flash frequency</dt><dd>{{flashFreq}} MHz</dd></div>
          <div><dt>Flash chip Id</dt><dd>{{getFlashChipMode}}</dd></div>
          <div><dt>Estimated Flash size</dt><dd>{{getFlashChipSize}}</dd></div>
          <div><dt>Flash write mode</dt><dd>{{ideMode}}</dd></div>
          <div><dt>📏 Sketch size</dt><dd>{{getSketchSize}}</dd></div>
          <div><dt>#️⃣ Sketch hash</dt><dd>{{getSketchMD5}}</dd></div>
          <div><dt>SDK version</dt><dd><span class="overflow-text">{{getSdkVersion}}</span></dd></div>
          <div><dt>🆓 Space available</dt><dd>{{getFreeSketchSpace}}</dd></div>
        </dl>

      <h2>Heap</h2>
        <dl>
          <div><dt>∑ Total</dt><dd>{{getHeapSize}}</dd></div>
          <div><dt>🆓 Available</dt><dd>{{getFreeHeap}}</dd></div>
          <div><dt>🎚️ Min since boot</dt><dd>{{getMinFreeHeap}}</dd></div>
          <div><dt>↕️ Max alloc</dt><dd>{{getMaxAllocHeap}}</dd></div>
        </dl>

      <h2>USB</h2>
        <dl>
          <div><dt>🏪  Vendor ID</dt><dd>{{USB_VID}}</dd></div>
          <div><dt>📦  Product ID</dt><dd>{{USB_PID}}</dd></div>
          <div><dt>🏭  Manufacturer</dt><dd>{{USB_MANUFACTURER}}</dd></div>
          <div><dt>🏷️ Product Name</dt><dd>{{USB_PRODUCT}}</dd></div>
          <div><dt>#️⃣  Serial Number</dt><dd>{{USB_SERIAL}}</dd></div>
          <div><dt>✳️ HUB</dt><dd><span class="{{usb_begun}}" data-status>◉</span></dd></div>
          <div><dt>🔘 HID</dt><dd><span class="{{hid_ready}}" data-status>◉</span></dd></div>
          <div><dt>📟 Serial</dt><dd><span class="{{usbserial_begun}}" data-status>◉</span></dd></div>
          <div><dt>⌨ Keyboard</dt><dd class="kbd"><span class="{{keyboard_begun}}" data-status>◉</span>
            <div class="keyboard-locks">
              <span title="Caps Lock {{capslock_on}}" class="key__button {{capslock_on}}" onclick="toggleLock(this,'CAPSLOCK')">⇪</span>
              <span title="Num Lock {{numlock_on}}" class="key__button {{numlock_on}}" onclick="toggleLock(this,'NUMLOCK')">⇭</span>
              <span title="Scroll Lock {{scrolllock_on}}" class="key__button {{scrolllock_on}}" onclick="toggleLock(this,'SCROLLLOCK')">⤓</span>
            </div>
            <div class="keyboard-layout">
              <label for="ducky-locales">Layout </label>
              <select id="ducky-locales" onchange="setKbdLocale(this)"><option value="{{KEYBOARD_LOCALE}}" selected></option></select>
            </div>
          </dd></div>
          <div onclick="AbsMousePad(mouseTab)"><dt>🖱️ Mouse</dt><dd><span class="{{absmouse_begun}}" data-status>◉</span></dd></div>
          <div><dt>💾 PenDrive</dt><dd><span class="{{pendrive_begun}}" data-status>◉</span></dd></div>
        </dl>


      <h2>Network</h2>
        <dl>
          <div><dt>MAC Address</dt><dd>{{MAC_ADDR}}</dd></div>
          <div><dt>STA IP Address</dt><dd>{{STA_ADDR}}</dd></div>
          <div><dt>Hostname</dt><dd>{{HOST_NAME}} (.home / .local / .localdomain)</dd></div>
          <div><dt>HTTP/WebSocket Server</dt><dd><span class="{{webserver_begun}}" data-status>◉</span></dd></div>
          <div><dt>WiFi Access Point</dt><dd><span class="{{softap_begun}}" data-status>◉</span></dd></div>
          <div><dt>AP SSID</dt><dd>{{AP_SSID}}</dd></div>
          <div><dt>AP PASS</dt><dd>{{AP_PASSWORD}}</dd></div>
          <div><dt>WiFi Station</dt><dd><span class="{{wifista_begun}}" data-status>◉</span></dd></div>
          <div><dt>STA SSID</dt><dd>{{STA_SSID}}</dd></div>
          <div><dt>STA PASS</dt><dd>{{STA_PASSWORD}}</dd></div>
          <div><dt>NTP Client</dt><dd><span class="{{ntp_enabled}}" data-status>◉</span></dd></div>
          <div><dt>NTP Zone</dt><dd>{{NTP_ZONE}}</dd></div>
          <div><dt>NTP Server address</dt><dd>{{NTP_SERVERADDR}}</dd></div>
        </dl>

      <h2>Storage</h2>
        <dl>
          <div><dt><span class="icon sdcard {{sd_begun}}"></span> SD</dt><dd><span class="{{sd_begun}}" data-status>◉</span></dd></div>
          <div><dt><span class="icon spiffs {{spiffs_begun}}"></span> SPIFFS</dt><dd><span class="{{spiffs_begun}}" data-status>◉</span></dd></div>
        </dl>
  </script>


  <script type="text/javascript" src="/script.js"></script>
</body>
</html>



)indexHTML";
