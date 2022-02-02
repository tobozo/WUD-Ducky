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
    <button id="quack" class="action-button" onClick="top.location='/'">🔙</button>
  </nav>
  <h1>WUD Ducky</h1>
  <h3>System Info</h3>
  <div class="infos">
    <h2>Software</h2>
      <dl>
        <div><dt>SDK version</dt><dd><span class="overflow-text">{{getSdkVersion}}</span></dd></div>
        <div><dt>Chip Id</dt><dd>{{getChipModel}}</dd></div>
        <div><dt>WebServer</dt><dd><span class="{{webserver_begun}}">◉</span></dd></div>
        <div><dt>Logging</dt><dd><span class="{{logging_enabled}}">◉</span></dd></div>
        <div><dt>📟 Serial debug</dt><dd>{{SerialDebug}} <span class="{{hwserial_begun}}">◉</span></dd></div>
      </dl>
    <h2>CPU / Flash</h2>
      <dl>
        <div><dt>CPU frequency</dt><dd>{{getCpuFreqMHz}} MHz</dd></div>
        <div><dt>Flash frequency</dt><dd>{{flashFreq}} MHz</dd></div>
        <div><dt>Flash chip Id</dt><dd>{{getFlashChipMode}}</dd></div>
        <div><dt>Estimated Flash size</dt><dd>{{getFlashChipSize}}</dd></div>
        <div><dt>Flash write mode</dt><dd>{{ideMode}}</dd></div>
      </dl>
    <h2>Sketch</h2>
      <dl>
        <div><dt>#️⃣ Sketch hash</dt><dd>{{getSketchMD5}}</dd></div>
        <div><dt>📏 Sketch size</dt><dd>{{getSketchSize}}</dd></div>
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
        <div><dt>✳️ HUB</dt><dd><span class="{{usb_begun}}">◉</span></dd></div>
        <div><dt>🔘 HID</dt><dd><span class="{{hid_ready}}">◉</span></dd></div>
        <div><dt>📟 Serial</dt><dd><span class="{{usbserial_begun}}">◉</span></dd></div>
        <div><dt>⌨ Keyboard</dt><dd class="kbd"><span class="{{keyboard_begun}}">◉</span> <span title="Caps Lock {{capslock_on}}" class="key__button {{capslock_on}}">⇪</span><span title="Num Lock {{numlock_on}}" class="key__button {{numlock_on}}">⇭</span><span  title="Scroll Lock {{scrolllock_on}}" class="key__button {{scrolllock_on}}">⤓</span></dd></div>
        <div><dt>🖱️ Mouse</dt><dd><span class="{{absmouse_begun}}">◉</span></dd></div>
        <div><dt>💾 PenDrive</dt><dd><span class="{{pendrive_begun}}">◉</span></dd></div>
      </dl>
    <h2>Storage</h2>
      <dl>
        <div><dt><span class="icon sdcard {{sd_begun}}" style=""></span> SD</dt><dd><span class="{{sd_begun}}">◉</span></dd></div>
        <div><dt><span class="icon spiffs {{spiffs_begun}}" style=""></span> SPIFFS</dt><dd><span class="{{spiffs_begun}}">◉</span></dd></div>
      </dl>
  </div>
</body>
</html>


)infoHTML";
