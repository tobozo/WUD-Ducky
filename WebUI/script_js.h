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

//  - Easy maintenance: ENABLE SYNTAX HIGHLIGHTING FOR JavaScript and ignore the C header/footer of this file

const char* script_js = R"ScriptJS(


const mainDiv   = document.querySelector('div.main');
const infoDiv   = document.querySelector('.infos');
const filesList = document.getElementById('cont-files');
const infoTpl   = document.getElementById('infos-template');
const mouseTab  = document.getElementById("mouse-tab");

let logsEnabled =  document.querySelector('[name="logs-switcher"]:checked').dataset["logs"] === "on";
let isReadOnlyFS = false;
let sysinfo      = {};
let kbdLocales   = [];


function tplparse( tpl, domtarget, markers )
{
  const initialdisplay = domtarget.style.display;
  domtarget.style.display = 'none';
  domtarget.innerHTML = tpl.innerHTML;

  Object.entries(markers).forEach(([key, value]) => {
    //console.log(`${key} ${value}`);
    rx = new RegExp('{{' + key +'}}', 'g');
    domtarget.innerHTML = domtarget.innerHTML.replace(rx, value );
  });
  domtarget.style.display = initialdisplay;
}



function populateLocaleChooser()
{
  const datalist = document.getElementById("ducky-locales");
  if( !datalist ) {
    log_e("Locales can't be set :(");
    return;
  }
  const selectedLocale = sysinfo && sysinfo.KEYBOARD_LOCALE ? sysinfo.KEYBOARD_LOCALE : kbdLocales[0];
  datalist.innerHTML = '';
  kbdLocales.forEach( function( locale ) {
    const selected = ( locale === selectedLocale ) ? ' selected' : '';
    datalist.innerHTML += `<option value="${locale}"${selected}>${locale}</option>`;
  });
};


function showInfo()
{
  tplparse( infoTpl, infoDiv, sysinfo);
  populateLocaleChooser();
  infoDiv.style.display = 'block';
  infoDiv.focus();
}


function quack( msg, onsuccess, onerror )
{
  const url = "/quack?cmd="+encodeURI(msg);
  fetch(url).then(async(response) => {
    if (response.ok) { // status 404 or 500 will set "ok" property to false
      if( onsuccess ) onsuccess();
    } else {
      if( onerror ) onerror();
      throw new Error(response.status + " Failed Fetch ");
    }
  }).catch(e => console.error('DUH!', e))
}


function setLogs( target )
{
  const targetEnable = target.dataset["logs"] === "on";
  if( targetEnable != logsEnabled ) {
    document.querySelector(".logs-viewer").style.display = targetEnable ? 'block' : 'none';
    switch( targetEnable ) {
      case true: quack("logs-enable",  () => logsEnabled = true ); break;
      case false:quack("logs-disable", () => logsEnabled = false ); break;
    }
  } //else console.log("no change");
}


function clearLogs()
{
  quack("logs-clear");
  document.getElementById("logs").innerHTML = '';
}


function loadLogs()
{
  const xh = new XMLHttpRequest();
  xh.onreadystatechange = () => {
    if (xh.readyState == 4 && xh.status == 200) {
      const res = xh.responseText.split("\n");
      const cont = document.querySelector('#logs');
      cont.innerText = '';
      res.forEach(str => {
        cont.innerText += str;
        cont.innerHTML += '<br>';
      });
    };
  };
  xh.open("GET", "/logs", true);
  xh.send(null);
};


function toggleLock( elem, lock )
{
  let success = () =>
  {
    elem.classList.toggle('on');
    elem.classList.toggle('off');
  };
  quack(lock, success);
}


function setKbdLocale(selectItem)
{
  const newLocale = selectItem.options[selectItem.selectedIndex].value;
  let success = () =>
  {
    sysinfo.KEYBOARD_LOCALE = newLocale;
  };
  quack('LOCALE ' + newLocale, success );
}


function changeFS( newfs )
{
  const xh = new XMLHttpRequest();
  xh.onreadystatechange = () => {
    if (xh.readyState == 4 && xh.status == 200) {
      loadPage("/");
    };
  };
  xh.open("GET", "/changefs?fs="+newfs, true);
  xh.send(null);
};


function UIselectFS( selectedFS )
{
  console.log("selecting fs: ", selectedFS );
  switch( selectedFS ) {
    case 'spiffs': document.body.classList.remove("nodelete"); break;
    case 'sd': document.body.classList.add("nodelete"); break;
  }
}


function onFSRadioClick(selectedRadio)
{
  if( !selectedRadio.checked ) {
    console.log("filesystem already selected");
    return;
  }
  const selectedFS = selectedRadio.dataset.fs;
  if( selectedFS ) {
    UIselectFS( selectedFS );
    if( !document.body.classList.contains(selectedFS) ) {
      changeFS( selectedFS );
    }
  } else {
    console.error("target radio has no fs dataset");
  }
}


function setpath()
{
  const default_path = document.getElementById("newfile").files[0].name;
  document.getElementById("filepath").value = default_path;

  if( default_path ) {
    // show upload button
    document.getElementById("uploader-tab").style.display = 'block';
    document.getElementById("upload").style.display = 'block';
    document.getElementById("upload").dataset["path"] = default_path;
  }
}


function xmlHttpReq( success_cb, close_cb, error_cb )
{
  const xh = new XMLHttpRequest();
  xh.onreadystatechange = function() {
    if (xh.readyState == 4) {
      if (xh.status == 200) {
        if( success_cb ) success_cb();
        location.reload()
      } else if (xh.status == 0) {
        alert("Server closed the connection abruptly!");
        if( close_cb ) close_cb();
        location.reload()
      } else {
        alert(xh.status + " Error!\n" + xh.responseText);
        if( error_cb ) error_cb();
        location.reload()
      }
    }
  };
  return xh;
}


function upload()
{
  const filePath = document.getElementById("filepath").value;
  const fileInput = document.getElementById("newfile").files;
  const MAX_FILE_SIZE = 20*1024*1024;
  const MAX_FILE_SIZE_STR = "20MB";

  if (fileInput.length == 0) {
    alert("No file selected!");
  } else if (filePath.length == 0) {
    alert("File path on server is not set!");
  } else if (filePath.indexOf(' ') >= 0) {
    alert("File path on server cannot have spaces!");
  } else if (filePath[filePath.length-1] == '/') {
    alert("File name not specified after path!");
  } else if (fileInput[0].size > MAX_FILE_SIZE) {
    alert("File size must be less than " + MAX_FILE_SIZE_STR);
  } else {
    document.getElementById("newfile").disabled = true;
    document.getElementById("filepath").disabled = true;
    document.getElementById("upload").disabled = true;

    const file = fileInput[0];
    const xh = xmlHttpReq();
    const formData = new FormData();
    formData.append("filename", file);
    xh.open("POST", "/upload");
    xh.send(formData);
  }
}


function closeEditor()
{
  editor.remove();
  document.querySelector('.text-editor').remove();
}


function deleteFile( path )
{
  if( !confirm("Really delete " + path + "?" ) ) return;
  const xh = xmlHttpReq();
  const formData = new FormData();
  formData.append("path", path);
  xh.open("POST", "/delete");
  xh.send(formData);
}


function saveFile( path )
{
  const file = new File([editor.value], path, { type: "text/plain", });
  const xh = xmlHttpReq();
  const formData = new FormData();
  formData.append("filename", file);
  xh.open("POST", "/upload");
  xh.send(formData);
}


function editFile( path )
{
  mainDiv.innerHTML += `<div class="text-editor"><div>Editing <b>${path}</b></div><textarea id=editor></textarea><button id="save" onclick=saveFile("${path}")>Save</button><button id="cancel" onclick=closeEditor()>Cancel</button></div>`;
  fetch(path).then(async(response) => {
    if (response.ok) { // status 404 or 500 will set ok to false
      editor.value = await response.text();
    } else {
      closeEditor();
      throw new Error(response.status + " Failed Fetch ");
    }
  }).catch(e => console.error('DUH!', e))
}


function runPayload( path )
{
  fetch("/runpayload?file="+encodeURI(path)).then(async(response) => {
    if (response.ok) { // status 404 or 500 will set ok to false
      // yay !
    } else {
      throw new Error(response.status + " Failed Fetch ");
    }
  }).catch(e => console.error('DUH!', e))
}


function formatBytes(bytes, decimals = 2)
{
  if (bytes === 0) return '0 Bytes';
  const k = 1024;
  const dm = decimals < 0 ? 0 : decimals;
  const sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
  const i = Math.floor(Math.log(bytes) / Math.log(k));
  return parseFloat((bytes / Math.pow(k, i)).toFixed(dm)) + ' ' + sizes[i];
}


const AbsMouseReport = ( posX_int16_t, posY_int16_t, btn_state, wheel ) =>
{
  let asUint8 = new Uint8Array(7);

  asUint8[0] = btn_state & 0xff;

  // posX [0 - 32767] as int16_t
  asUint8[1] = posX_int16_t & 0xff;
  asUint8[2] = (posX_int16_t >> 8) & 0xff;

  // posY [0 - 32767] as int16_t
  asUint8[3] = posY_int16_t & 0xff;
  asUint8[4] = (posY_int16_t >> 8) & 0xff;

  // vertical wheel [-127 to 127]
  asUint8[5] = wheel;

  // horizontal wheel [-127 to 127]
  asUint8[6] = 0x00;

  return asUint8;
}


const AbsMousePad = (div) =>
{
  let socket         = null;
  let socketEnabled  = false;
  let eventsAttached = false;
  let lastbtn        = 0;
  let lastx          = 0;
  let lasty          = 0;
  let lastwheel      = 0;

  const MouseReporter = (e) =>
  {
    if( div.offsetWidth === 0 || div.offsetHeight === 0 ) return false;
    if( e.offsetY < 0 || e.offsetX < 0 ) return false;
    const x     = div.dataset['x'];
    const y     = div.dataset['y'];
    const btn   = div.dataset['btn'];
    const wheel = div.dataset['wheel'];
    if( x !== lastx || y !== lasty || btn !== lastbtn || lastwheel !== wheel ) {
      const absX   = (x/(div.offsetWidth/32768)) & 0xffff;
      const absY   = (y/(div.offsetHeight/32768)) & 0xffff;
      const report = AbsMouseReport( absX, absY, btn, wheel );
      if( socketEnabled ) socket.send( report );
      div.dataset['wheel'] = 0;
    }
    lastx     = x;
    lasty     = y;
    lastbtn   = btn;
    lastwheel = wheel;
    e.stopPropagation();
    e.cancelBubble = true;
    return false;
  };

  const setBtn = ( val ) =>
  {
    div.dataset['btn'] = val;
  };

  const setXY = ( x, y ) =>
  {
    div.dataset['x'] = x;
    div.dataset['y'] = y;
  };

  const setWheel = ( e ) =>
  {
    div.dataset['wheel'] = e.deltaY >= 0 ? Math.min( e.deltaY, 127 ) : Math.max( e.deltaY, -127 );
  };

  const setupSocket = () =>
  {
    socket = new WebSocket('ws://'+location.host+'/ws');
    socket.onmessage = (e) => console.log('Message from server ', e.data);
    socket.onopen    = (e) => { socket.send('Hello Server!'); socketEnabled = true; };
    socket.onclose   = (e) => { if (e.wasClean) console.log(`[ws:closed][code=${e.code} reason=${e.reason}]`); else console.log('[ws:died]'); socketEnabled = false; }
    socket.onerror   = (e) => console.log(`[ws:error] ${e.message}`);
  };

  const setupMouseEvents = () =>
  {
    div.oncontextmenu = (e) => { return false; }
    div.onmouseup     = (e) => { setBtn( 0x00 ); return MouseReporter(e);  }
    div.onmousedown   = (e) => { switch (e.button) { case 0:setBtn(0x01);break; case 1:setBtn(0x04);break; case 2:setBtn(0x02);break; } return MouseReporter(e); }
    div.onmousemove   = (e) => { setXY( e.offsetX, e.offsetY ); return MouseReporter(e); }
    div.onwheel       = (e) => { setWheel( e ); return MouseReporter(e); }
    div.onclick       = (e) => { if( e.offsetY < 0 ) div.parentElement.style.display = 'none'; return false; }
  };

  if( !eventsAttached ) {
    setupSocket();
    setupMouseEvents();
    eventsAttached = true;
  }

  div.parentElement.style.display = 'block';
};








function loadPage(dirpath) {

  const loadFiles = dir => {
    const xh = new XMLHttpRequest();
    xh.onreadystatechange = () => {
      if (xh.readyState == 4 && xh.status == 200) {
        const res = JSON.parse(xh.responseText);
        if( res ) {

          if( res.filesystem ) {
            switch( res.filesystem ) {
              case 'spiffs': isReadOnlyFS = false; document.body.classList.remove("nodelete"); break;
              case 'sd':     isReadOnlyFS = true;  document.body.classList.add("nodelete");    break;
              default: isReadOnlyFS = true;
            }
            document.body.classList.remove("spiffs");
            document.body.classList.remove("sd");
            document.body.classList.add( res.filesystem );

            const selectedRadio = document.querySelector('[name="fs-switcher"]:checked');
            const selectedFS = selectedRadio.dataset.fs;
            if( !document.body.classList.contains(selectedFS) ) {
              selectedRadio.onclick();
            }

            isReadOnlyFS ? filesList.classList.add('read-only') : filesList.classList.remove('read-only');
          } else fsTab.innerHTML = "No filesystem info in JSON";

          // set the control panel "logging enabled" radio state
          if( res.sysinfo ) {
            sysinfo = res.sysinfo;
            if( res.sysinfo.logging_enabled ) {
              logsEnabled = ( res.sysinfo.logging_enabled === "enabled" );
              document.querySelector(".logs-viewer").style.display = logsEnabled ? 'block' : 'none';
              if( logsEnabled ) document.querySelector('[name="logs-switcher"][data-logs="on"]').click();
              else  document.querySelector('[name="logs-switcher"][data-logs="off"]').click();
            }
            if( document.querySelector('input#infos-panel-toggle').checked ) {
              showInfo();
            }
          }

          if( res.locales ) {
            kbdLocales = res.locales;
          }

          if( res.commands ) {
            //console.log( res.commands );
            const duckyOptions = document.getElementById("ducky-commands");
            duckyOptions.innerHTML = '';

            Object.entries( res.commands ).forEach(([key, value]) => {
              value.forEach( function( cmd ) {
                duckyOptions.innerHTML += `<option value="${cmd}">`;
              });
            });
          }

          if( filesList ) {
            const tableHeaders = {
              "name":`<dt class="th file-name">Name</dt>`,
              "size":`<dt class="th file-size">Size</dt>`,
              'action':`<dt class="th file-action">Actions</dt>`
            };
            const tableHeader = `
              ${tableHeaders.name}
              ${tableHeaders.size}
              ${tableHeaders.action}
            `;
            filesList.innerHTML = `<div class="tr thead">${tableHeader}</div>`;
            let hasQuackFile = false; // not used yet but may be useful to raise a status icon
            res.files.forEach( (elem, pos) => {
              const isQuackFile = (elem.name).match(/quack/);
              const ext = elem.name.split('.').pop();
              const isplaintext = elem.type == 'file' && ext.match(/svg|js|css|txt|html?/);
              const isWritable  = !elem.readonly;
              // using inline 'onclick' instead of 'addEventListener' to avoid memory leaks
              const runCell   = isQuackFile ? `<button class="action-button" onclick=runPayload("${encodeURI(elem.name)}")>⏻<span>Run</span></button>` : '';
              const delCell   = (isReadOnlyFS || !isWritable) ? '' : `<button type="submit" class="action-button" onclick="deleteFile('${encodeURI(elem.name)}')">❌<span>Delete</span></button>`;
              const editCell  = isReadOnlyFS ? '' : isplaintext ? `<button type="submit" class="action-button" onclick="editFile('${encodeURI(elem.name)}')">🖉<span>Edit</span></button>` : '';
              const typeCell  = elem.type == 'dir' ? '📁' : isQuackFile ? '🦆' : '📜';
              const typeTitle = isQuackFile ? 'Payload' : elem.type;
              const sizeCell  = formatBytes( elem.size );
              const linkEvt   = ` onclick="loadPage(location.pathname+this.href);return false;" `
              const linkCell  = `<a href="${encodeURI(elem.name)}" ${elem.type == 'file' ? '' : linkEvt}>${elem.name.substring(1)}</a>`;
              // used to toggle compact/detailed item view with css rules
              const adjView   = `<input class="item-expander" id="toggler-${pos}" type="checkbox"><label class="item-expander" for="toggler-${pos}">⋯</label>`;
              let actions     = `${runCell}${editCell}${delCell}`;
              let hasActions  = !(actions === '');

              hasQuackFile = isQuackFile ? true : hasQuackFile;
              if( !hasActions ) actions = 'n/a';

              // TODO: build from DOM Elements, grid layout can glitch when built with innerHTML
              filesList.innerHTML +=
                `<div class="tr tbody ${hasActions?'has-actions':''}">
                    ${adjView}
                    ${tableHeaders.name}
                    <dd class="file-name" title="${typeTitle}"><span>${typeCell}</span>${linkCell}</dd>
                    ${tableHeaders.size}
                    <dd class="file-size" title="${elem.size} Bytes">${sizeCell}</dd>
                    ${tableHeaders.action}
                    <dd class="file-action">${actions}</dd>
                </div>`
              ;
            });
          } else document.body.innerHTML += "No file list container";
        } else document.body.innerHTML += "JSON PARSING FAILED";
      };
    };
    xh.open("GET", "/list?dir=" + dir, true);
    xh.send(null);
  };

  loadFiles(dirpath);
}


loadPage(location.pathname);


)ScriptJS";

