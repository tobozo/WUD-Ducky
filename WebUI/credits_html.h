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

const char* credits_html  = R"CreditsHTML(

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

<body class="about">
  <nav>
    <ul class="menu">
      <li><a href="/info">System information</a></li>
      <li><a href="/explorer">Filesystem explorer</a></li>
      <li><a href="/nutquacker">Live payload sender</a></li>
    </ul>
  </nav>

  <div id="status"></div>

  <div class="container">
    <div class="row">
      <div class="col-12">
        <h1 class="header">Credits</h1>

        <h2>Disclaimer</h2>
        <p class="centered" data-translate="disclaimer">
          This project is a proof of concept for testing and educational purposes.<br>
          <br>
          Use it only against your own networks and devices!<br>
          <br>
          It uses USB to mock devices and deliver payloads on a victim device.<br>
          Please check the legal regulations in your country before using it.<br>
          <br>
          <br>
          For more information visit:<br>
          <a
            href="https://github.com/tobozo/WUD-Ducky">github.com/tobozo/WUD-Ducky</a>
        </p>

        <h2>Acknowledgements</h2>
        <p>
          A huge thanks to:<br>
        <ul>
          <li><a href="http://github.com/spacehuhn" target="_blank">@spacehuhn</a></li>
        </ul>
        for letting his WiFiDuck project code be partially recycled into this application.<br>
        <br>
        We also thank Espressif and their community for this awesome chip and all the software and
        hardware projects around it and the countless tutorials you can find online!<br>
        </p>

        <h2>License</h2>
        <p>
          <b>In regards to the firmware:</b><br>
          <br>
          MIT License<br>
          Copyright (c) 2022 tobozo<br>
          <br>
          Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
          associated documentation files (the "Software"), to deal in the Software without restriction,
          including without limitation the rights to use, copy, modify, merge, publish, distribute,
          sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
          furnished to do so, subject to the following conditions:<br>
          <br>
          The above copyright notice and this permission notice shall be included in all copies or substantial
          portions of the Software.<br>
          <br>
          THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
          NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
          NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
          OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
          CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
        </p>
      </div>
    </div>
  </div>
  <footer>
    <span id="version">Version 0.0.1</span>
    <br>
    <br>
    <a href="https://github.com/tobozo/WUD-Ducky" target="_blank">Repository</a> | <a href="/about.html">Credits</a>
  </footer>
</body>

</html>

)CreditsHTML";
