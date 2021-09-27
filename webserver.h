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

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <SPIFFS.h>


WebServer server(80);
fs::File fsUploadFile;


void HIDKeySend( String str );
void runpayload( fs::FS &fs, const char* payload);


String getContentType(String filename)
{
  if (server.hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}


void handleIndex()
{
  String path = "/index.htm";
  String contentType = "text/html";
  fs::File file = SPIFFS.open(path.c_str());
  server.streamFile(file, contentType);
  file.close();
}


void handleFileUpload()
{
  if (server.uri() != "/upload") {
    return;
  }

  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    Serial.print("handleFileUpload Name: ");
    Serial.println(filename);
    fsUploadFile = SD.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
    }
    Serial.print("handleFileUpload Size: ");
    Serial.println(upload.totalSize);
  }
}


bool handleFileRead(String path)
{
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) {
    handleIndex();
    return true;
  } else {
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    bool fs_exists = SD.exists(path);
    bool gz_exists = SD.exists(pathWithGz);
    if ( gz_exists || fs_exists ) { // is it present on the SD Card?
      if ( gz_exists ) {
        path = pathWithGz;
      }
      fs::File file = SD.open(path, "r");
      if (file.isDirectory()) {
        handleIndex();
      } else {
        server.streamFile(file, contentType);
      }
      file.close();
      return true;
    } else { // not on the SD Card, is it present on SPIFFS?
      fs_exists = SPIFFS.exists(path);
      gz_exists = SPIFFS.exists(pathWithGz);
      if ( gz_exists || fs_exists ) {
        if ( gz_exists ) {
          path = pathWithGz;
        }
        fs::File file = SPIFFS.open(path.c_str());
        server.streamFile(file, contentType);
        file.close();
        return true;
      }
    }
  }
  return false;
}


void handleFileDelete()
{
  if (server.args() == 0) {
    return server.send(500, "text/plain", "BAD ARGS");
  }
  String path = server.arg(0);
  Serial.println("handleFileDelete: " + path);
  if (path == "/") {
    return server.send(500, "text/plain", "BAD PATH");
  }
  if (!SD.exists(path)) {
    return server.send(404, "text/plain", "FileNotFound");
  }
  SD.remove(path);
  server.sendHeader("Location", String("/"), true);
  server.send(302, "text/plain", "");
  path = String();
}


void handleFileList()
{
  String path;
  if (!server.hasArg("dir")) {
    path = "/";
  } else {
    path = server.arg("dir");
  }

  Serial.println("handleFileList: " + path);

  fs::File root = SD.open(path);
  String output = "[";

  if (root && root.isDirectory()) {
    fs::File file = root.openNextFile();
    while (file) {
      if (output != "[") {
        output += ',';
      }

      output += "{\"type\":\"";
      output += (file.isDirectory()) ? "dir" : "file";
      output += "\",\"name\":\"";
      output += String(file.path());
      output += "\",\"size\":";
      output += String(file.size());
      output += "}";
      file = root.openNextFile();
    }
  }
  output += "]";
  server.send(200, "text/json", output);
}



void handleKeySend()
{
  if (server.args() == 0) {
    return server.send(500, "text/plain", "MISSING ARGS");
  }
  String path = server.arg(0);
  Serial.println("handleKeySend: " + path);
  HIDKeySend(path+"\n");
  server.send(200, "text/plain", path);
  path = String();
}


void handleRunPayload()
{
  if (server.args() == 0) {
    return server.send(500, "text/plain", "MISSING ARGS");
  }

  String path;

  if (!server.hasArg("file")) {
    return server.send(500, "text/plain", "BAD ARGS");
  }
  path = server.arg("file");

  if( !SD.exists( path ) ) {
    return server.send(500, "text/plain", "FILE NOEXISTS");
  }

  server.send(200, "text/plain", path);

  runpayload( SD, path.c_str() );
}


void startFileServer()
{
  server.on("/list", HTTP_GET, handleFileList);
  server.on("/delete", HTTP_POST, handleFileDelete);
  server.on("/upload", HTTP_POST, []() {
    server.sendHeader("Location", String("/"), true);
    server.send(302, "text/plain", "");
  }, handleFileUpload);

  server.on("/key", HTTP_GET, handleKeySend);
  server.on("/runpayload", HTTP_GET, handleRunPayload);

  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });
  server.begin();
}

