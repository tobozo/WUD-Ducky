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

#include "./WebServer.hpp"
#include "WebUI.cpp"
#include "NTP.cpp"

extern fs::FS* duckyFS;

namespace WS
{

  void WebServerLogMsg( String msg )
  {
    if( WebServerLogger ) WebServerLogger(msg);
    //else HWSerial.println( msg );
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
    return contentTypeText;
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
      WebServerLogMsg("Receiving file: "+filename );
      fsUploadFile = SPIFFS.open(filename, "w");
      filename = String();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (fsUploadFile) {
        fsUploadFile.write(upload.buf, upload.currentSize);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (fsUploadFile) {
        fsUploadFile.close();
      }
      WebServerLogMsg("File Size: " + String( upload.totalSize ));
      WebServerLogMsg("File uploaded successfully");
    }
  }


  bool contentFound(String path)
  {
    if (path.endsWith("/")) { // requesting directory listing
      WebServerLogMsg("Rewriting root path to index.html");
      path = "/index.html";
    }
    if( SPIFFS.exists( path ) ) {
      WebServerLogMsg("Serving '"+path+"' from flash filesystem");
      fs::File file = SPIFFS.open(path.c_str());
      bool streamable =  file && !file.isDirectory();
      if( streamable ) {
        server.streamFile(file, getContentType(path) );
      }
      file.close();
      if( streamable ) return true;
    }
    // basename search in static documents
    const char* bnameCStr = basename( path.c_str() );
    if( bnameCStr == NULL ) return false;
    size_t docs_count = sizeof(WebUI::StaticDocuments)/sizeof(StaticWebContent);
    for( int i=0; i<docs_count; i++ ) {
      if( strcmp( WebUI::StaticDocuments[i].basepath, bnameCStr ) == 0 ) {
        WebServerLogMsg("Serving '"+String(bnameCStr)+"' from flash memory");
        server.send(200, WebUI::StaticDocuments[i].content_type, WebUI::StaticDocuments[i].content );
        return true;
      }
    }
    // basename search in dynamic documents
    docs_count = sizeof(WebUI::DynamicDocuments)/sizeof(StaticWebContent);
    for( int i=0; i<docs_count; i++ ) {
      if( strcmp( WebUI::DynamicDocuments[i].basepath, bnameCStr ) == 0 ) {
        WebServerLogMsg("Serving '"+String(bnameCStr)+"' from job");
        String output;
        WebUI::DynamicDocuments[i].content(&output);
        server.send(200, WebUI::DynamicDocuments[i].content_type, output );
        return true;
      }
    }
    bool ret = false;
    WebServerLogMsg("Internal path '"+String(bnameCStr)+"' not found");
    return ret;
  }


  void handleFileDelete()
  {
    if (server.args() == 0) {
      return server.send(500, contentTypeText, "BAD ARGS");
    }
    String path = server.arg(0);
    //log_d("Deleting file: %s", path.c_str() );
    if (path == "/") {
      return server.send(500, contentTypeText, "BAD PATH");
    }
    if (!SPIFFS.exists(path)) {
      String responseText = "File Not Found: " + String( server.uri() );
      return server.send(404, contentTypeText, responseText);
    }
    SPIFFS.remove(path);
    WebServerLogMsg("File "+path+" deleted successfully");
    server.sendHeader("Location", String("/"), true);
    server.send(302, contentTypeText, "");
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
    String output;
    WebUI::lsJson( path.c_str(), output );
    server.send(200, contentTypeJson, output);
  }


  void handleKeySend()
  {
    if (server.args() == 0) {
      return server.send(500, contentTypeText, "MISSING ARGS");
    }
    String arg = server.arg(0);
    if( HIDKeySender ) {
      HIDKeySender( arg );
    }
    server.send(200, contentTypeText, HIDKeySender ? arg : "No HID Attached" );
    arg = String();
  }


  void handleRunPayload()
  {
    if (server.args() == 0) {
      return server.send(500, contentTypeText, "MISSING ARGS");
    }
    if (!server.hasArg("file")) {
      return server.send(500, contentTypeText, "BAD ARGS");
    }
    String path = server.arg("file");
    if( !duckyFS->exists( path ) ) {
      return server.send(500, contentTypeText, "FILE NOEXISTS");
    }
    server.send(200, contentTypeText, path);
    runpayload( duckyFS, path.c_str() );
  }



  void handleInfo()
  {
    String sysInfo;
    WebUI::getSystemInfo( &sysInfo, WebUI::SYSINFO_HTML );
    server.send(200, contentTypeHtml, sysInfo );
    sysInfo = String();
  }


  void handleGetLogs()
  {
    if(WebServerLogsPrinter) {
      logoutput = "";
      WebServerLogsPrinter( logprinter, false );
      server.send(200, contentTypeText, logoutput);
    }
  }


  void handleChangeFS()
  {
    if (server.args() == 0) {
      return server.send(500, contentTypeText, "MISSING ARGS");
    }
    String fsname = server.arg(0);
    String response = "Filesystem changed to " + fsname + " successfully";
    int responseCode = 200;
    if( fsname == "spiffs" ) {
      duckyFS = &SPIFFS;
  #ifdef HAS_PENDRIVE
    } else  if( fsname == "sd" ) {
      duckyFS = &SD;
  #endif
    } else {
      response = "Unknown filesystem";
      responseCode = 500;
    }
    WebServerLogMsg(response);
    server.send( responseCode, contentTypeText, response );
  }






  void startWebServer()
  {
    if( WUDStatus::webserver_begun ) {
      WebServerLogMsg("WebServer is already started");
      return;
    }

    server.onNotFound([]() {
      if (!contentFound(server.uri())) {
        // TODO: fancy 404 page
        String responseText = "<!doctype html><h1>Error 404</h1><h2>File Not Found</h2><p>The following file was not found on the server: <b>" + String( server.uri() + "</b></p>");
        server.send(404, contentTypeHtml, responseText);
      }
    });

    // serving static files from SPIFFS is fucked up and unregisters all the other handlers
    // server.serveStatic("/", SPIFFS, "/" );

    // action endpoints with status response
    server.on("/changefs", HTTP_GET, handleChangeFS );
    server.on("/delete", HTTP_POST, handleFileDelete);
    server.on("/quack", HTTP_GET, handleKeySend);
    server.on("/runpayload", HTTP_GET, handleRunPayload);

    server.on("/upload", HTTP_POST, []() {
      server.sendHeader("Location", String("/"), true);
      server.send(302, contentTypeText, "");
    }, handleFileUpload);

    server.begin();
    WebServerLogMsg("WebServer started");
    WUDStatus::webserver_begun = true;
  }

}

