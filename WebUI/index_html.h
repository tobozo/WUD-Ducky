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
<body class="explorer">
  <audio autoplay src="/Quack.wav"></audio>
  <nav>
    <button id="reload" class="action-button" onClick="history.go(0)">🗘</button>
    <button id="info" class="action-button" onClick="top.location='/info'">🛈</button>
    <button id="quack" class="action-button" onClick="top.location='/'">🦆</button>
  </nav>
  <div class="main">
    <div class="fixed">
      <h1>WUD Ducky</h1>
      <div id="uploader-tab">
        Upload file:
        <label for="newfile">
          <button class="upload-button">⬆</button><input id="newfile" type="file" onchange="setpath()"><input id="filepath" type="hidden"><button id="upload" type="button" onclick="upload()">Upload</button>
        </label>
      </div>
      <div id="filesystem-tab">
        <button id="fs-sd" data-name="sd">SD</button>
        <button id="fs-spiffs" data-name="spiffs">SPIFFS</button>
      </div>
    </div>
    <div>
      <input class="toggle-button" type="radio" name="compact-view" value="on"  id="cv1">
      <input class="toggle-button" type="radio" name="compact-view" value="off" id="cv2" checked>
      <label class="toggle-button" for="cv1">☰</label>
      <label class="toggle-button" for="cv2">☶</label>
      <dl class="fixed table" id="cont-files"></dl>
    </div>
    <div id="filesystemlogs">
      <div id="logs" style="white-space:pre"></div>
    </div>
  </div>
  <script type="text/javascript" src="/script.js"></script>
</body>
</html>



)indexHTML";
