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

//  - Easy maintenance: ENABLE SYNTAX HIGHLIGHTING FOR HTML and ignore the C header/footer of this file

const char* info_html = R"infoHTML(

<!doctype html public "🦆">
<html>
<head>
  <link rel="apple-touch-icon" sizes="180x180" href="/apple-touch-icon.png">
  <link rel="icon" type="image/png" sizes="32x32" href="/favicon-32x32.png">
  <link rel="icon" type="image/png" sizes="16x16" href="/favicon-16x16.png">
  <link rel="manifest" href="/site.webmanifest">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta charset="utf-8">
  <title>{{USB_PRODUCT}}</title>
  <link rel="stylesheet" type="text/css" href="/styles.css">
</head>
<body class="info">
  <nav>
    <button id="reload" class="action-button" onClick="history.go(0)">🗘</button>
    <button id="info" class="action-button" onClick="top.location='/info'">🛈</button>
    <button id="quack" class="action-button" onClick="top.location='/'">🦆</button>
  </nav>
  <h1>WUD Ducky</h1>
  <h3>System Info</h3>
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
