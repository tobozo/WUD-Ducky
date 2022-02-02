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
  <nav>
    <button id="reload" class="action-button" onClick="history.go(0)">🗘</button>
    <button id="info" class="action-button" onClick="top.location='/info'">🛈</button>
    <button id="quack" class="action-button" onClick="top.location='/'">🦆</button>
    <label for="newfile" class="action-button action-button icon upload"></label>
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
            <input type="radio" class="radio-switch__input sr-only" id="log1" name="logs-switcher" data-logs="on" checked>
            <label for="log1" class="radio-switch__label"></svg>ON</label>
          </li>

          <li class="radio-switch__item">
            <input type="radio" class="radio-switch__input sr-only" id="log2" name="logs-switcher" data-logs="off">
            <label for="log2" class="radio-switch__label">OFF</label>
            <div class="radio-switch__marker" aria-hidden="true"></div>
          </li>
        </ul>

        <div>
          <input class="toggle-button" type="checkbox" id="view-logs-toggler" onclick="if(checked) loadLogs()">
          <label for="view-logs-toggler" class="">🗎 View logs</label>
          <div id="filesystemlogs">
            <button class="clear-logs-button">Clear logs</button>
            <div id="logs" style="white-space:pre"></div>
          </div>
        </div>

      </div>
    </div>

    <div id="uploader-tab">
      <input id="newfile" type="file" onchange="setpath()">
      <input id="filepath" type="hidden">
      <button id="upload" type="button" onclick="upload()">Upload</button>
    </div>

    <div class="files-view">
      <input class="toggle-button" type="radio" name="compact-view" value="on"  id="cv1" checked>
      <input class="toggle-button" type="radio" name="compact-view" value="off" id="cv2">
      <div class="toggle-buttons">
        <label class="toggle-button" for="cv1">☰</label>
        <label class="toggle-button" for="cv2">☶</label>
      </div>
      <dl class="fixed table" id="cont-files"></dl>
    </div>
  </div>
  <script type="text/javascript" src="/script.js"></script>
</body>
</html>



)indexHTML";
