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

const char* disclaimer_html  = R"DisclaimerHTML(

<!doctype html public "🦆">
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=0.8, minimal-ui">
  <meta name="theme-color" content="#36393E">
  <link rel="stylesheet" type="text/css" href="/styles.css">
  <meta name="description" content="WUD-Ducky">
  <title>WUD-Ducky</title>
</head>
<body class="disclaimer">
  <h1>WUD Ducky</h1>
  <div class="container">
    <div class="row">
      <div class="col-12">
        <h1 class="header" data-translate="warning">WARNING</h1>
        <p class="centered" data-translate="disclaimer">
          This project is a proof of concept for testing and educational purposes.<br>
          <br>
          Use it only against your own networks and devices!<br>
          <br>
          The purpose of this application is to use the ESP32-S2 internal USB chip to mock devices and deliver HID payloads.<br>
          Please check the legal regulations in your country before using it.<br>
          <br>
          <br>
          For more information visit:<br>
          <a
            href="https://github.com/tobozo/WUD-Ducky">github.com/tobozo/WUD-Ducky</a>
        </p>
        <p class="centered bold">
          <a class="button" href="/" data-translate="disclaimer-button">I have read and understood the
            notice above</a>
        </p>
      </div>
    </div>
  </div>
  <footer>
    <span id="version">Version 0.0.1</span>
    <br>
    <br>
    <a href="https://github.com/tobozo/WUD-Ducky" target="_blank">Repository</a> | <a href="/credits.html">Credits</a>
  </footer>
</body>

</html>

)DisclaimerHTML";
