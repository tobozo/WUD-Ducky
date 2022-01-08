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
#include "index.h" // can be edited in place, or overriden with "index.html" file on SPIFFS

WebServer server(80);
fs::File fsUploadFile;
extern fs::FS* duckyFS;
bool webserver_begun = false;
static String logoutput;

typedef void(*wslogprintercb)( String msg );
void (*WebServerLogsPrinter)( wslogprintercb cb, bool clear_after );
wslogprintercb WebServerLogger;
void (*HIDKeySender)( String str );
void runpayload( fs::FS *fs, const char* payload);

void startWebServer();
void handleIndex();
void handleFileUpload();
bool handleFileRead(String path);
void handleFileDelete();
void handleFileList();
void handleKeySend();
void handleRunPayload();
void handleGetLogs();
void handleChangeFS();

void WebServerLogMsg( String msg );
void logprinter(String msg);
String getContentType(String filename);


void startWebServer()
{
  if( webserver_begun ) {
    WebServerLogMsg("WebServer is already started");
    return;
  }
  server.on("/list", HTTP_GET, handleFileList);
  server.on("/logs", HTTP_GET, handleGetLogs);
  server.on("/changefs", HTTP_GET, handleChangeFS );
  server.on("/delete", HTTP_POST, handleFileDelete);
  server.on("/key", HTTP_GET, handleKeySend);
  server.on("/runpayload", HTTP_GET, handleRunPayload);
  server.on("/upload", HTTP_POST, []() {
    server.sendHeader("Location", String("/"), true);
    server.send(302, "text/plain", "");
  }, handleFileUpload);
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });
  server.begin();
  WebServerLogMsg("WebServer started");
  webserver_begun = true;
}


void handleIndex()
{
  String path = "/index.html";
  String contentType = "text/html";
  if( SPIFFS.exists( path ) ) {
    fs::File file = SPIFFS.open(path.c_str());
    if( file ) {
      server.streamFile(file, contentType);
      file.close();
      return;
    }
  }
  WebServerLogMsg("Using built-in index, upload 'index.html' to override.");
  server.send(200, contentType, index_html );
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
    log_d("Receiving file: %s", filename.c_str() );
    fsUploadFile = duckyFS->open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
    }
    log_d("File Size: %d", upload.totalSize);
    WebServerLogMsg("File uploaded successfully");
  }
}


bool handleFileRead(String path)
{
  if (path.endsWith("/")) {
    handleIndex();
    log_d("Serving file: %s", path.c_str() );
    return true;
  } else {
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    bool fs_exists = duckyFS->exists(path);
    bool gz_exists = duckyFS->exists(pathWithGz);
    if ( gz_exists || fs_exists ) { // is it present on the ducky filesystem ?
      if ( gz_exists ) {
        path = pathWithGz;
      }
      fs::File file = duckyFS->open(path, "r");
      if (file.isDirectory()) {
        handleIndex();
      } else {
        server.streamFile(file, contentType);
      }
      file.close();
      log_d("Served path from %s: %s", duckyFS != &SPIFFS ? "SD":"SPIFFS", path.c_str() );
      return true;
    } else { // not on the ducky filesystem
      if( duckyFS != &SPIFFS ) { // ducky filesystem was SD, now check SPIFFS
        fs_exists = SPIFFS.exists(path);
        gz_exists = SPIFFS.exists(pathWithGz);
        if ( gz_exists || fs_exists ) {
          if ( gz_exists ) {
            path = pathWithGz;
          }
          fs::File file = SPIFFS.open(path.c_str());
          server.streamFile(file, contentType);
          file.close();
          log_d("Served path from SPIFFS: %s", path.c_str() );
          return true;
        }
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
  log_d("Deleting file: %s", path.c_str() );
  if (path == "/") {
    return server.send(500, "text/plain", "BAD PATH");
  }
  if (!duckyFS->exists(path)) {
    return server.send(404, "text/plain", "FileNotFound");
  }
  duckyFS->remove(path);
  WebServerLogMsg("File deleted successfully");
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

  log_d("Listing files from: %s", path);

  fs::File root = duckyFS->open(path);

  String fsname = duckyFS==&SPIFFS ? "spiffs" : "sd";

  String output = "{\"filesystem\":\""+fsname+"\", \"files\":[";

  if (root && root.isDirectory()) {
    fs::File file = root.openNextFile();
    while (file) {
      char lastchar = output[output.length()-1];
      if (lastchar == '}' || lastchar == ']' ) {
        // last char was a closing bracket, insert comma
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
  output += "]}";
  server.send(200, "text/json", output);
  log_d("SENT JSON: %s\n", output.c_str() );
}


void handleKeySend()
{
  if (server.args() == 0) {
    return server.send(500, "text/plain", "MISSING ARGS");
  }
  String path = server.arg(0);
  log_d("Sending Keys: %s\\n", path.c_str() );
  if( HIDKeySender) HIDKeySender( path+"\n");
  server.send(200, "text/plain", path);
  path = String();
}


void handleRunPayload()
{
  if (server.args() == 0) {
    return server.send(500, "text/plain", "MISSING ARGS");
  }
  if (!server.hasArg("file")) {
    return server.send(500, "text/plain", "BAD ARGS");
  }
  String path = server.arg("file");
  if( !duckyFS->exists( path ) ) {
    return server.send(500, "text/plain", "FILE NOEXISTS");
  }
  server.send(200, "text/plain", path);
  runpayload( duckyFS, path.c_str() );
}


void handleGetLogs()
{
  if(WebServerLogsPrinter) {
    logoutput = "";
    WebServerLogsPrinter( logprinter, false );
    server.send(200, "text/plain", logoutput);
  }
}


void handleChangeFS()
{
  if (server.args() == 0) {
    return server.send(500, "text/plain", "MISSING ARGS");
  }
  String fsname = server.arg(0);
  String response = "Filesystem changed to " + fsname + " successfully";
  if( fsname == "spiffs" ) {
    duckyFS = &SPIFFS;
  } else  if( fsname == "sd" ) {
    duckyFS = &SD;
  } else {
    response = "Unknown filesystem";
  }
  WebServerLogMsg(response);
  server.sendHeader("Location", String("/"), true);
  server.send(302, "text/plain", "");
}


void WebServerLogMsg( String msg )
{
  if( WebServerLogger ) WebServerLogger(msg);
  else Serial.println( msg );
}


void logprinter(String msg)
{
  logoutput += msg + "\n";
}


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
