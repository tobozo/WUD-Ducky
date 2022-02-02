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

#include <WString.h>

// minimalistic template implementation, with French attitude :o

#define MOUSTACHE_OPEN  "{{"
#define MOUSTACHE_CLOSE "}}"


struct Poil
{
  const char* name;
  const char* value;
};

struct Poils
{
  Poil* poils;
  size_t count;
};


struct Moustache
{

  String *output = nullptr;
  const char* tpl = nullptr;
  Poils *barbe = nullptr;

  void setOutput( String *_output )
  {
    output = _output;
  }
  void setBarbe( Poils* _barbe )
  {
    barbe = _barbe;
  }

  void setTemplate( const char* _tpl )
  {
    tpl = _tpl;
  }

  void set( const char* _tpl, String *_output, Poils* _barbe )
  {
    tpl    = _tpl;
    output = _output;
    barbe  = _barbe;
  }

  void loadTemplate( const char* _tpl )
  {
    if( output == nullptr ) {
      // wut ?
      return;
    }
    *output = String( _tpl );
  }
/*
  void parse( Poil *poil )
  {
    String tige  = String(MOUSTACHE_OPEN) + String(poil->name) + String(MOUSTACHE_CLOSE);
    String bulbe = String( poil->value );
    output->replace( tige, bulbe );
  }

  void parse( Poil* _poils, size_t count )
  {
    if( !_poils || count == 0 ) return; // can't shave a bald beard!
    for( int i=0; i<count; i++ )
      parse( &_poils[i] );
  }

  void slowparse( Poils* _barbe )
  {
    parse( _barbe->poils, _barbe->count );
  }
*/
  void parse()
  {
    if( barbe == nullptr || output == nullptr ) return;
    if( barbe->count == 0 || barbe->poils == nullptr ) return;
    if( tpl == nullptr ) return;
    fastparse();
    //loadTemplate( tpl );
    //slowparse( barbe );
  }


  void fastparse()
  {
    #define max_marker_size 33
    size_t len = strlen(tpl);
    bool in_marker = false;
    size_t marker_size = 0;
    int marker_start = 0;
    char marker_name[max_marker_size];
    for( int i=0; i<len;i++ ) {
      /*
      if( in_marker  && i+1<len && tpl[i] == MOUSTACHE_CLOSE[0] && tpl[i+1] == MOUSTACHE_CLOSE[1] ) {
        in_marker = false;
      }*/
      if( i+1<len && tpl[i] == MOUSTACHE_OPEN[0] && tpl[i+1]  == MOUSTACHE_OPEN[1] )  {
        in_marker = true;
        marker_size = 0;
        memset( marker_name, 0, max_marker_size );
        marker_start = i;
        i += 2; // skip opening moustache
        while( in_marker && marker_size < max_marker_size && i < len+3 ) {
          marker_name[marker_size++] = tpl[i++];
          in_marker = tpl[i] != MOUSTACHE_CLOSE[0] && tpl[i+1] != MOUSTACHE_CLOSE[1];
        }
        if( !in_marker && marker_size > 0 && marker_name[0] != '\0' ) { // found marker name
          marker_name[marker_size] = tpl[i]; // fetch last letter
          bool found = false;
          for( int j=0; j<barbe->count; j++ ) { // foreach named marker
            if( strcmp( barbe->poils[j].name, marker_name) == 0) { // marker name matches?
              *output += String( barbe->poils[j].value ); // append marker value
              found = true;
              break;
            }
          }
          if( ! found ) {
            *output += "!!" + String( marker_name ) + "!!";
          }
          i += 2; // skip closing moustache
        } else {
          // false positive, append skipped chunks
          for( int j=marker_start; j<=i; j++ ) *output += String( tpl[j] );
        }
      } else {
        *output += String( tpl[i] );
      }

    }



    char str[] = "Geeks-for-Geeks";

    // Returns first token
    char* token = strtok(str, "-");

    // Keep printing tokens while one of the
    // delimiters present in str[].
    while (token != NULL) {
        printf("%s\n", token);
        token = strtok(NULL, "-");
    }


    char* token_in = strtok(str, MOUSTACHE_OPEN);
    if( token_in == NULL ) return;
    char* token_out = strtok(str, MOUSTACHE_CLOSE);
    printf("%s\n", token_out);



  }




};


typedef void (*URITemplateParser)( String *output );

const char* contentTypeHtml = "text/html; charset=utf-8";
const char* contentTypeText = "text/plain; charset=utf-8";
const char* contentTypeCss = "text/css; charset=utf-8";
const char* contentTypeJs = "text/javascript; charset=utf-8";
const char* contentTypeJson = "text/json";

struct DynamicWebContent
{
  const char* basepath;
  const char* content_type;
  URITemplateParser content;
};


struct StaticWebContent
{
  const char* basepath;
  const char* content_type;
  const char* content;
};



