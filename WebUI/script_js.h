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


const sdBtn     = document.querySelector('#fs-sd');
const spiffsBtn = document.querySelector('#fs-spiffs');
const mainDiv   = document.querySelector('div.main');
const filesList = document.querySelector('#cont-files');
const fsTab     = document.querySelector('#filesystem-tab');

let isReadOnlyFS = false;


function loadLogs()
{
  const xh = new XMLHttpRequest();
  xh.onreadystatechange = () => {
    if (xh.readyState == 4 && xh.status == 200) {
      const res = xh.responseText.split("\n");
      const cont = document.querySelector('#logs');
      cont.innerText = '';
      res.forEach(str => {
        cont.innerText += str + "\n";
      });
    };
  };
  xh.open("GET", "/logs", true);
  xh.send(null);
};


function changeFS( newfs )
{
  const xh = new XMLHttpRequest();
  xh.onreadystatechange = () => {
    if (xh.readyState == 4 && xh.status == 200) {
      console.log("will reload page");
      loadPage("/");
      //document.location.reload();
    };
  };
  xh.open("GET", "/changefs?fs="+newfs, true);
  xh.send(null);
};



function onFSBtnClick( btn )
{
  if( !btn.className.match(/enabled/) ) {
    changeFS( btn.dataset.name );
  }
}


function setpath()
{
  const default_path = document.getElementById("newfile").files[0].name;
  document.getElementById("filepath").value = default_path;
  if( default_path ) {
    // show upload button
    document.getElementById("upload").style.display = 'inline-block';
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
  /* Max size of an individual file. Make sure this
    * value is same as that set in file_server.c */
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
    // status 404 or 500 will set ok to false
    if (response.ok) {
      editor.value = await response.text();
    } else {
      closeEditor();
      throw new Error(response.status + " Failed Fetch ");
    }
  }).catch(e => console.error('DUH!', e))
}



function runPayload( path )
{
  //const xh = xmlHttpReq();
  //const formData = new FormData();
  //formData.append("file", path);
  //xh.open("GET", "/runpayload?file="+encodeURI(path) );
  //xh.send();
  fetch("/runpayload?file="+encodeURI(path)).then(async(response) => {
    // status 404 or 500 will set ok to false
    if (response.ok) {
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

// const hasCompression
// const compressedReadableStream = inputReadableStream.pipeThrough(new CompressionStream('gzip'));


function loadPage(dirpath) {

  const loadFiles = dir => {
    const xh = new XMLHttpRequest();
    xh.onreadystatechange = () => {
      if (xh.readyState == 4 && xh.status == 200) {
        const res = JSON.parse(xh.responseText);
        if( res ) {
          if( fsTab ) {
            if( res.filesystem ) {
              switch( res.filesystem ) {
                case 'spiffs': isReadOnlyFS = false; sdBtn.classList.remove('enabled'); spiffsBtn.classList.add('enabled'); mainDiv.classList.remove("nodelete"); break;
                case 'sd': isReadOnlyFS = true; spiffsBtn.classList.remove('enabled'); sdBtn.classList.add('enabled'); mainDiv.classList.add("nodelete"); break;
                default: isReadOnlyFS = true;
              }
              sdBtn.onclick     = () => { onFSBtnClick(sdBtn) };
              spiffsBtn.onclick = () => { onFSBtnClick(spiffsBtn) };
              isReadOnlyFS ? filesList.classList.add('read-only') : filesList.classList.remove('read-only');
            } else fsTab.innerHTML = "No filesystem info in JSON";
          } else document.body.innerHTML += "Can't find filesystem-tab div";
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
              const typeCell  = elem.type == 'dir' ? '📁' : isQuackFile ? '🦆' : '🗎';
              const typeTitle = isQuackFile ? 'Payload' : elem.type;
              const sizeCell  = formatBytes( elem.size );
              const linkEvt   = ` onclick="loadPage(location.pathname+this.href);return false;" `
              const linkCell  = `<a href="${encodeURI(elem.name)}" ${elem.type == 'file' ? '' : linkEvt}>${elem.name.substring(1)}</a>`;
              // used to toggle compact/detailed item view with css rules
              const adjView   = `<label class="item-expander" for="toggler-${pos}">@</label><input class="item-expander" id="toggler-${pos}" type="checkbox">`;
              let actions     = `${runCell}${editCell}${delCell}`;
              let hasActions  = !(actions === '');

              hasQuackFile = isQuackFile ? true : hasQuackFile;
              if( !hasActions ) actions = 'n/a';

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
        loadLogs();
      };
    };
    xh.open("GET", "/list?dir=" + dir, true);
    xh.send(null);
  };

  loadFiles(dirpath);

}


loadPage(location.pathname);


)ScriptJS";

