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
    if( WebServerLogger ) WebServerLogger("[HTTP] " + msg );
    //else HWSerial.println( msg );
  }


  void logprinter(String msg)
  {
    logoutput += msg + "\n";
  }


  String getContentType(String filename)
  {
    /*if (server.hasArg("download")) {
      return "application/octet-stream";
    } else*/ if (filename.endsWith(".htm")) {
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


  void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
  {
    if(type == WS_EVT_CONNECT){
      Logger::logsprintf("ws[%s][%u] connect", server->url(), client->id());
      client->printf("Hello Client %u :)", client->id());
      client->ping();
    } else if(type == WS_EVT_DISCONNECT){
      Logger::logsprintf("ws[%s][%u] disconnect", server->url(), client->id());
    } else if(type == WS_EVT_ERROR){
      Logger::logsprintf("ws[%s][%u] error(%u): %s", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
    } else if(type == WS_EVT_PONG){
      Logger::logsprintf("ws[%s][%u] pong[%u]: %s", server->url(), client->id(), len, (len)?(char*)data:"");
    } else if(type == WS_EVT_DATA){
      AwsFrameInfo * info = (AwsFrameInfo*)arg;
      String msg = "";
      if(info->final && info->index == 0 && info->len == len){
        //the whole message is in a single frame and we got all of it's data
        // Logger::logsprintf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
        int elemSize = sizeof(hid_abs_mouse_report_t);
        int elements = len / elemSize;
        if( elements>0 && len%elemSize == 0 ) {
          // is valid abs_mouse_report packet
          for( int i = 0; i < elements; i++ ) {
            hid_abs_mouse_report_t* report = (hid_abs_mouse_report_t*)(&data[i*elemSize]);
            //Logger::logsprintf("ws[%s][%u] Btn: 0x%02x Wheel: %d Coords: [0x%04x:0x%04x]", server->url(), client->id(), report->buttons, report->wheel, report->x, report->y );
            AbsMouse.sendReport( *report );
          }
        } else {
          // not a mouse message
          if(info->opcode == WS_TEXT){
            for(size_t i=0; i < info->len; i++) {
              msg += (char) data[i];
            }
          } else {
            char buff[3];
            for(size_t i=0; i < info->len; i++) {
              sprintf(buff, "%02x ", (uint8_t) data[i]);
              msg += buff ;
            }
          }

          Logger::logsprintf("%s",msg.c_str());

          if(info->opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");

          // Logger::logsprintf("ws[%s][%u] mouse invalid msg len: %d (expecting multiple of %d)", server->url(), client->id(), len, elemSize );
        }

      } else {
        //message is comprised of multiple frames or the frame is split into multiple packets
        if(info->index == 0){
          if(info->num == 0)
            Logger::logsprintf("ws[%s][%u] %s-message start", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          Logger::logsprintf("ws[%s][%u] frame[%u] start[%llu]", server->url(), client->id(), info->num, info->len);
        }

        Logger::logsprintf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

        if(info->opcode == WS_TEXT){
          for(size_t i=0; i < len; i++) {
            msg += (char) data[i];
          }
        } else {
          char buff[3];
          for(size_t i=0; i < len; i++) {
            sprintf(buff, "%02x ", (uint8_t) data[i]);
            msg += buff ;
          }
        }
        //Logger::logsprintf("%s",msg.c_str());

        if((info->index + len) == info->len){
          Logger::logsprintf("ws[%s][%u] frame[%u] end[%llu]", server->url(), client->id(), info->num, info->len);
          if(info->final){
            Logger::logsprintf("ws[%s][%u] %s-message end", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
            if(info->message_opcode == WS_TEXT)
              client->text("I got your multiple text message");
            else
              client->binary("I got your multiple binary message");
          }
        }
      }
    }
  }


  void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
  {
    if(!index){
      WebServerLogMsg("Receiving file: "+filename );
      fsUploadFile = LittleFS.open( "/" + filename, "w");
    }
    fsUploadFile.write(data, len);
    if(final){
      if (fsUploadFile) {
        fsUploadFile.close();
      }
      WebServerLogMsg("File Size: " + String( index+len ));
      WebServerLogMsg("File "+filename+" uploaded successfully");

      request->redirect("/");
      //server.sendHeader("Location", String("/"), true);
      //server.send(302, contentTypeText, "");

    }
  }
/*

  void handleFileUpload(AsyncWebServerRequest *request)
  {
    if (request->url() != "/upload") {
      return;
    }

    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      String filename = upload.filename;
      if (!filename.startsWith("/")) {
        filename = "/" + filename;
      }
      WebServerLogMsg("Receiving file: "+filename );
      fsUploadFile = LittleFS.open(filename, "w");
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
*/

  bool contentFound(AsyncWebServerRequest *request)
  {
    using namespace WebUI;
    String path = request->url();
    if (path.endsWith("/")) { // requesting directory listing
      WebServerLogMsg("Rewriting root path to index.html");
      path = "/index.html";
    }
    if( LittleFS.exists( path ) ) {
      WebServerLogMsg("Serving '"+path+"' from flash filesystem");
      fs::File file = LittleFS.open(path.c_str());
      bool streamable =  file && !file.isDirectory();
      file.close();
      if( streamable ) {
        //request->streamFile(file, getContentType(path) );
        request->send( LittleFS, path.c_str(), getContentType(path) );
        return true;
      }
      //if( streamable )
    }
    // basename search in static documents
    const char* bnameCStr = basename( path.c_str() );
    if( bnameCStr == NULL ) return false;
    size_t docs_count = sizeof(StaticDocuments)/sizeof(StaticWebContent);
    for( int i=0; i<docs_count; i++ ) {
      if( strcmp( StaticDocuments[i].basepath, bnameCStr ) == 0 ) {
        WebServerLogMsg("Serving '"+String(bnameCStr)+"' from flash memory");
        //server.send(200, WebUI::StaticDocuments[i].content_type, WebUI::StaticDocuments[i].content );
        AsyncWebServerResponse *response = request->beginResponse_P(200, String(StaticDocuments[i].content_type), (const uint8_t*)StaticDocuments[i].content, strlen(StaticDocuments[i].content) );
        request->send(response);
        return true;
      }
    }
    // basename search in dynamic documents
    docs_count = sizeof(WebUI::DynamicDocuments)/sizeof(StaticWebContent);
    for( int i=0; i<docs_count; i++ ) {
      if( strcmp( DynamicDocuments[i].basepath, bnameCStr ) == 0 ) {
        WebServerLogMsg("Serving '"+String(bnameCStr)+"' from job");
        String output;
        DynamicDocuments[i].content(&output);
        //server.send(200, WebUI::DynamicDocuments[i].content_type, output );
        AsyncWebServerResponse *response = request->beginResponse_P(200, String(DynamicDocuments[i].content_type), (const uint8_t*)output.c_str(), output.length() );
        request->send(response);
        return true;
      }
    }
    bool ret = false;
    WebServerLogMsg("Internal path '"+String(bnameCStr)+"' not found");
    return ret;
  }


  void handleFileDelete(AsyncWebServerRequest *request)
  {

    if (request->params() == 0) {
      return request->send(500, contentTypeText, "BAD ARGS");
    }
    String path = request->getParam(0)->value();
    //log_d("Deleting file: %s", path.c_str() );
    if (path == "/") {
      return request->send(500, contentTypeText, "BAD PATH");
    }
    if (!LittleFS.exists(path)) {
      String responseText = "File Not Found: " + String( request->url() );
      return request->send(404, contentTypeText, responseText);
    }
    LittleFS.remove(path);
    WebServerLogMsg("File "+path+" deleted successfully");
    request->redirect("/");
    //server.sendHeader("Location", String("/"), true);
    //server.send(302, contentTypeText, "");
    path = String();
  }


  void handleFileList(AsyncWebServerRequest *request)
  {
    String path;
    if (!request->hasArg("dir")) {
      path = "/";
    } else {
      path = request->arg("dir");
    }
    String output;
    WebUI::lsJson( path.c_str(), output );
    AsyncWebServerResponse *response = request->beginResponse_P(200, contentTypeJson, (const uint8_t*)output.c_str(), output.length()  );
    request->send(response);
    output = String();
  }


  void handleKeySend(AsyncWebServerRequest *request)
  {
    if (request->params() == 0) {
      return request->send(500, contentTypeText, "MISSING ARGS");
    }
    String arg = request->getParam(0)->value();
    if( HIDKeySender ) {
      HIDKeySender( arg );
    }
    request->send(200, contentTypeText, HIDKeySender ? arg : "No HID Attached" );
    arg = String();
  }


  void handleRunPayload(AsyncWebServerRequest *request)
  {
    if (request->params() == 0) {
      return request->send(500, contentTypeText, "MISSING ARGS");
    }
    if (!request->hasArg("file")) {
      return request->send(500, contentTypeText, "BAD ARGS");
    }
    static String path = request->arg("file");
    if( !duckyFS->exists( path ) ) {
      return request->send(500, contentTypeText, "FILE NOEXISTS");
    }
    request->send(200, contentTypeText, path);
    // TODO: make this async
    //runpayload( duckyFS, path.c_str() );
    runPayloadAsync( duckyFS, path.c_str() );
  }



  void handleInfo(AsyncWebServerRequest *request)
  {
    String sysInfo;
    WebUI::getSystemInfo( &sysInfo, WebUI::SYSINFO_HTML );
    AsyncWebServerResponse *response = request->beginResponse_P(200, contentTypeHtml, (const uint8_t*)sysInfo.c_str(), sysInfo.length() );
    request->send(response);
    sysInfo = String();
  }


  void handleGetLogs(AsyncWebServerRequest *request)
  {
    if(WebServerLogsPrinter) {
      logoutput = "";
      WebServerLogsPrinter( logprinter, false );
      AsyncWebServerResponse *response = request->beginResponse_P(200, contentTypeText, (const uint8_t*)logoutput.c_str(), logoutput.length() );
      request->send(response);
    }
  }


  void handleChangeFS(AsyncWebServerRequest *request)
  {
    if (request->params() == 0) {
      return request->send(500, contentTypeText, "MISSING ARGS");
    }
    String fsname = request->getParam(0)->value();
    String response = "Filesystem changed to " + fsname + " successfully";
    int responseCode = 200;
    if( fsname == "spiffs" ) {
      duckyFS = &LittleFS;
  #ifdef HAS_PENDRIVE
    } else  if( fsname == "sd" ) {
      duckyFS = &SD;
  #endif
    } else {
      response = "Unknown filesystem";
      responseCode = 500;
    }
    WebServerLogMsg(response);
    request->send( responseCode, contentTypeText, response );
  }


  void sendWSLogEntry( String logEntry )
  {
    ws.textAll( logEntry );
  }





  void startWebServer()
  {
    if( WUDStatus::webserver_begun ) {
      WebServerLogMsg("WebServer is already started");
      return;
    }

    //Send OTA events to the browser
    ArduinoOTA.onStart([]() { events.send("Update Start", "ota"); });
    ArduinoOTA.onEnd([]() { events.send("Update End", "ota"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      char p[32];
      sprintf(p, "Progress: %u%%\n", (progress/(total/100)));
      events.send(p, "ota");
    });
    ArduinoOTA.onError([](ota_error_t error) {
      if(error == OTA_AUTH_ERROR) events.send("Auth Failed", "ota");
      else if(error == OTA_BEGIN_ERROR) events.send("Begin Failed", "ota");
      else if(error == OTA_CONNECT_ERROR) events.send("Connect Failed", "ota");
      else if(error == OTA_RECEIVE_ERROR) events.send("Recieve Failed", "ota");
      else if(error == OTA_END_ERROR) events.send("End Failed", "ota");
    });
    ArduinoOTA.setHostname( USB.productName() );
    ArduinoOTA.begin();

    MDNS.addService("http","tcp",80);

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    events.onConnect([](AsyncEventSourceClient *client){
      client->send("hello!",NULL,millis(),1000);
    });

    server.addHandler(&events);

    server.onNotFound([](AsyncWebServerRequest *request) {
      if ( !contentFound(request) ) {
        // TODO: fancy 404 page
        String responseText = "<!doctype html><h1>Error 404</h1><h2>File Not Found</h2><p>The following file was not found on the server: <b>" + String( request->url() + "</b></p>");
        request->send(404, contentTypeHtml, responseText);
      }
    });

    // serving static files from LittleFS is fucked up and unregisters all the other handlers
    // server.serveStatic("/", LittleFS, "/" );

    // action endpoints with status response
    server.on("/changefs", HTTP_GET, handleChangeFS );
    server.on("/delete", HTTP_POST, handleFileDelete);
    server.on("/quack", HTTP_GET, handleKeySend);
    server.on("/runpayload", HTTP_GET, handleRunPayload);


    // handleFileUpload
    server.onFileUpload( handleFileUpload );

    /*
    server.on("/upload", HTTP_POST, []() {
      server.sendHeader("Location", String("/"), true);
      server.send(302, contentTypeText, "");
    }, handleFileUpload);
*/
    server.begin();
    WebServerLogMsg("WebServer started");
    WUDStatus::webserver_begun = true;
  }

}

