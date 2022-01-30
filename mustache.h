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


struct Moustache
{

  String *_tpl = nullptr;

  void set( String *tpl )
  {
    _tpl = tpl;
  }

  void load( const char* tpl )
  {
    if( _tpl == nullptr ) {
      // wut ?
      return;
    }
    *_tpl = String( tpl );
  }

  void parse( Poil *poil )
  {
    String tige  = String(MOUSTACHE_OPEN) + String(poil->name) + String(MOUSTACHE_CLOSE);
    String bulbe = String( poil->value );
    _tpl->replace( tige, bulbe );
  }

  void parse( Poil* poils, size_t count )
  {
    if( !poils || count == 0 ) return; // can't shave a bald beard!
    for( int i=0; i<count; i++ )
      parse( &poils[i] );
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



