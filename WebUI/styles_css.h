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

const char* styles_css  = R"StylesCSS(


:root {
  --radio-switch-width: 13rem;
  --radio-switch-height: 2.7rem;
  --radio-switch-padding: .1rem;
  --radio-switch-radius: 50em;
  --radio-switch-animation-duration: 0.3s;
  --color-primary:rgba( 0,255,0,.15 );
  --color-contrast-low: rgba( 0,0,0,.15 );
  --color-contrast-lower: rgba( 0,0,0,.25 );

  --grid-auto-flow: column;
  --grid-auto-columns: 1fr;
  --grid-template-rows: repeat(50, min-content);
  --grid-row-start:1;
  --table-padding-top: 2rem;
  --table-display: grid;
  --table-row-display:inline-grid;
  --table-row-height:1.7rem;
  --table-row-margin:0;
  --header-visibility: visible;
  --header-title-cells-display: block;
  --title-cells-text-align: left;
  --title-cells-font-weight: bold;
  --title-cells-font-size: 1em;
  --body-title-cells-display: none;
  --file-size-display: block;
  --file-action-display: block;
  --item-expander-display: none;
  --toggle-button-display: none;
  --toggle-button-content:"";
  --action-button-width:5rem;
  --action-button-text-display:inline;

}



.icon {
  width: 1em;
  height: 1em;
  display: inline-block;
  margin-left: 0;
  background-repeat: no-repeat;
  background-position: center center;
  background-size: contain;
  user-select: none;
}

.icon.sdcard {
  background-image: url("data:image/svg+xml;utf8,%3csvg xmlns='http://www.w3.org/2000/svg' fill='white' width='24' height='24' viewBox='0 0 24 24'%3e%3cpath d='M18 2h-8L4.02 8 4 20c0 1.1.9 2 2 2h12c1.1 0 2-.9 2-2V4c0-1.1-.9-2-2-2zm-6 6h-2V4h2v4zm3 0h-2V4h2v4zm3 0h-2V4h2v4z' /%3e%3c/svg%3e");
}

.icon.spiffs {
  background-image: url("data:image/svg+xml;utf8,%3csvg xmlns='http://www.w3.org/2000/svg' fill='white' width='24' height='24' viewBox='0 0 24 24'%3e%3cpath d='M15 9H9v6h6V9zm-2 4h-2v-2h2v2zm8-2V9h-2V7c0-1.1-.9-2-2-2h-2V3h-2v2h-2V3H9v2H7c-1.1 0-2 .9-2 2v2H3v2h2v2H3v2h2v2c0 1.1.9 2 2 2h2v2h2v-2h2v2h2v-2h2c1.1 0 2-.9 2-2v-2h2v-2h-2v-2h2zm-4 6H7V7h10v10z'%3e%3c/path%3e%3c/svg%3e");
}
.icon.upload
{
  background-image: url("data:image/svg+xml;utf8,%3csvg xmlns='http://www.w3.org/2000/svg' fill='white' width='24' height='24' viewBox='0 0 24 24'%3e%3cpath d='M9 16h6v-6h4l-7-7-7 7h4zm-4 2h14v2H5z'%3e%3c/path%3e%3c/svg%3e")
}


.radio-switch {
  display: inline-flex;
  padding: var(--radio-switch-padding);
  border-radius: var(--radio-switch-radius);
  border: 1px solid var(--color-contrast-low);
}
.radio-switch__item {
  height: calc(var(--radio-switch-height) - 2*var(--radio-switch-padding));
  width: calc(var(--radio-switch-width)*0.5 - var(--radio-switch-padding));
  position: relative;
  display: inline-block;
}
.radio-switch__label {
  display: block;
  line-height: calc(var(--radio-switch-height) - 2*var(--radio-switch-padding));
  text-align: center;
  border-radius: var(--radio-switch-radius);
  font-weight: bold;
  user-select: none;
}

.radio-switch__marker {
  position: absolute;
  top: 0;
  left: -100%;
  height: 100%;
  width: 100%;
  background-color: var(--color-primary);
  border-radius: var(--radio-switch-radius);
  transition: transform var(--radio-switch-animation-duration);
}
.radio-switch__input:checked ~ .radio-switch__marker {
  transform: translateX(100%);
}
.radio-switch:focus-within {
  box-shadow: 0 0 0 3px var(--color-contrast-lower);
}

.radio-switch__input:not(:checked) + .radio-switch__label {
  text-shadow:none;
  opacity:0.75;
  font-weight: normal;
}

input[data-fs="sd"]+label, input[data-fs="spiffs"]+label {
  padding-left: 2rem;
  margin-left: .5rem;
  user-select: none;
  height: calc(var(--radio-switch-height) - 2*var(--radio-switch-padding));
  background-size: 60% auto;
  background-position: left;
  user-select: none;
}


.explorer label[for="newfile"] {
  user-select: none;
  background-size: auto 100%;
  width: 1.8em;
  height: 1.8em;
}

nav {
  width: 100%;
  max-width: 80em;
  position: fixed;
  z-index: 10000;
}

nav label[for="newfile"] {
  position: absolute;
  left: 3em;
  top: 0.4em;
}


.sr-only { display:none; }


body, h1, h2, h3, p, div { font-family: sans; }
body { background:#1451AE; color:#ffffff; font-size:14px; margin:0; padding:0.4em; max-width:80em; margin:auto; text-shadow: 1px 1px 1px black; }
h1, h2, h3 { text-align:center;  }
h1 { margin: 0; }
h3 { margin-top: 0; }
a { color: inherit; }
button { border-radius: 0.2rem; padding: 0.2rem 0.4rem; margin: 0 .7rem; border: 0px solid transparent; box-shadow: 1px 1px 1px black; }




:root {

}

.explorer .action-button { margin: 0; padding: 0; width: 1.8em; height: 1.8em; line-height: .1em; font-size: 1em; }
.explorer dd .action-button {  width: var(--action-button-width); margin-right: 1em; }

.explorer .action-button > span { margin-left: 0.3rem; display:var(--action-button-text-display); }

.explorer .file-name { overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }

.explorer .table .tr.thead { visibility:var(--header-visibility); height: var(--table-row-height); }

.explorer dl > div:nth-of-type(2n+2) { background: rgba(255,255,255,.15); }

.explorer dl.table { display: var(--table-display); padding-top: var(--table-padding-top); }

.explorer .tr.thead, .explorer .tr.tbody { display: var(--table-row-display); grid-auto-flow: var(--grid-auto-columns); grid-auto-columns: var(--grid-auto-columns); grid-template-rows: var(--grid-template-rows); }

.explorer .tr.tbody { position:relative; padding: 0.6rem 0.3rem; border-radius: .3rem; height:var(--table-row-height); line-height:var(--table-row-height); }

.explorer .tr.tbody dt { display: var(--body-title-cells-display); margin: .5rem 0; }
.explorer .tr.thead dt { display: var(--header-title-cells-display); }

.explorer .tr.tbody dd { margin-bottom: var(--table-row-margin); margin-top: var(--table-row-margin); }

.explorer .tr.tbody dd.file-size { display: var(--file-size-display); }
.explorer .tr.thead dt.file-size { display: var(--file-size-display); }

.explorer .tr.tbody dd.file-action { display: var(--file-action-display); }
.explorer .tr.thead dt.file-action { display: var(--file-action-display); }

.explorer dd > span {  margin-right: .5rem; }

.explorer dd, .explorer dt { grid-row-start: var(--grid-row-start); /* reset, next column */ }

.explorer dt { text-align: var(--title-cells-text-align); font-weight: var(--title-cells-font-weight); font-size: var(--title-cells-font-size); margin-left:3rem; }

.explorer input.toggle-button { display: none; }

.explorer label.toggle-button { display: var(--toggle-button-display); /*! position: absolute; */ margin: 0; right: .6rem; font-size: 1.8rem; line-height: .3em; height: 1.2rem; width: 0.9rem; /*! float: right; */margin: .5rem;}
.explorer label.toggle-button::after { content: var(--toggle-button-content); display: inline-block; margin-left: .5rem; position: absolute; line-height: 1em; font-size: .9rem; }
.explorer div.toggle-buttons { padding-top: 2rem; }

.explorer #view-logs-toggler:not(:checked)~div { display:none }

.explorer .tr.tbody input.item-expander { display: none; }
.explorer .tr.tbody label.item-expander { display: var(--item-expander-display); position: absolute; left: .3rem; top: 50%; transform: translate(0,-50%); user-select: none; line-height: 1em; padding:0.3em 0.5em 0.7em 0.5em;}
.explorer input.item-expander:checked ~ label.item-expander { color: rgba(0,0,0,.15); }

.text-editor { position: fixed; top: 0; left: 0; width: 100%; display: block; padding-top: 3em; height: 100%; background: rgba(0,0,0,0.75); z-index: 1001; }
.text-editor textarea { width: calc( 100% - 1em ); min-height: 30vh; resize: both; margin-bottom: .5rem; }

/*
Max width before this PARTICULAR table gets nasty
This query will take effect for any screen smaller than 760px
and also iPads specifically.
*/
@media  only screen and (max-width: 760px), (min-device-width: 768px) and (max-device-width: 1024px)  {

  :root {
    --header-visibility: hidden;
    --toggle-button-display: block;
    --table-padding-top: 0;
  }

  .explorer input[type="radio"][name="compact-view"][value="off"]:checked ~ * {
    --toggle-button-content:"Detailed view";
    --grid-auto-flow: unset;
    --grid-auto-columns: unset;
    --grid-template-rows: unset;
    --grid-row-start:unset;
    --table-display: block;
    --table-row-display:block;
    --table-row-height:auto;
    --table-row-margin:1rem;
    --body-title-cells-display: block;
    --header-title-cells-display: none;
    --title-cells-text-align: left;
    --title-cells-font-weight: bold;
    --title-cells-font-size: .75rem;
    --file-action-display:block;
  }

  .explorer input[type="radio"][name="compact-view"][value="on"]:checked ~ * {
    --toggle-button-content:"List view";
    --file-size-display: none;
    --file-action-display:none;
    --item-expander-display:block;
    --action-button-text-display:none;
    --action-button-width:1.8em;
  }

  .explorer .tr.tbody:not(.has-actions) label.item-expander { display: none; }
  .explorer .tr.tbody.has-actions input.item-expander:checked ~ dd.file-action { display: block; }

  .explorer #cv1:checked ~ div label[for="cv1"] { display:none; }
  .explorer #cv2:checked ~ div label[for="cv2"] { display:none; }

  body { padding:0; }

}

/* general rules */
.explorer #quack, .info #info { display:none; }
#info, #quack { position: absolute; left: 0.4em;  top: 0.4em; }
#reload { position: absolute; right: 3em;  top: 0.4em; }
#settings { position: absolute; right: 0.4em;  top: 0.4em; }
.overflow-text { white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }

/* file explorer */

#uploader-tab { transition: transform 0.3s ease-out; }
#control-panel { text-align:center; }
#control-panel { background: rgba(255,255,255,.15); margin: .5rem 0; border-radius: .5rem; padding: 1.3em .7rem; transition: transform 0.3s linear, top 0.2s; }
label[for="control-panel-toggle"] {
  padding: 0.2em .5em;
}
input#control-panel-toggle:not(:checked) ~ * #control-panel, input#control-panel-toggle:checked ~ * .files-view  {
  outline: 2px solid white;
  transform: scale(100%,0) translate(0, 0 );
  position: absolute;
  margin: auto;
  width: 100%;
  top: 0;
  padding: 0;
  border: 0;
}

.explorer .files-view {
  transition: transform 0.3s linear, top 0.2s;
  opacity: 1;
}

.explorer button.clear-logs-button { position: absolute; transform: translate(0, -110%); right: 0; }
#logs::before { display:block;content:"System Logs:"; margin-bottom: .5rem; }
#filepath::before { content: attr(value); }


#uploader { display:none; }

#upload {
  margin: 1em auto;
  display: none;
  width: 32em;
  max-width: calc( 100vw - 2em );
}

#upload::after {
  content: attr(data-path);
  width: 90vw;
  display: iblock;
  text-overflow: ellipsis;
  overflow-x: hidden;
  display: block;
  line-height: 2.6em;
  font-weight: bold;
  font-size: .9em;
}
#newfile { display:none; }

#logs { text-align: left; max-height: 480px; overflow-y: scroll; font-family: courier new; color: lightgreen; background: black; padding: 0.5em 1em; }
#filesystemlogs { margin: 0 auto; }

.noaction .file-action, .nodelete .delete-tab, .nodelete label[for="newfile"].action-button, .nodelete #uploader-tab {
  /*display: none; */
  padding: 0;
  margin: 0;
  position: absolute;
  width: 100%;
  transform: scale(0,0) translate(0,-200%);
  height: 0;
  overflow: hidden;
  outline: 2px solid rgba(255,255,255,.5); /* cylon effect on disappear */
}
.upload-button {
  content: "⬆";
  border-bottom: .2rem solid;
  display: inline-block;
  box-shadow: 0 .2rem 0 white, .2rem .2rem 0 white, -.25rem .2rem 0 white;
  height: 0.8rem;
  line-height: .2rem;
  text-align: center;
  border-radius: 0;
  border-top: 0;
  border-left: 0;
  border-right: 0;
  font-size: 1.5rem;
  box-sizing: initial;
  text-shadow: 0 -.2rem 0 white, .1rem 0 1px rgba(0,0,0,.5);
  color: transparent;
  background-color: transparent;
  padding: 0.2rem;
  position: absolute;
}

.fixed { margin: 0 auto; position: relative; }

/* system information */
.info .action-button { margin: 0; padding: 0; width: 1.8em; height: 1.8em; line-height: .1em; font-size: 1em; }
.info dl { display: table; width:100%;  }
.info dl > div { display: table-row; width:100%; }
.info dl > div > dt, .info dl > div > dd { display: table-cell; padding: 0.25em; height:auto; word-break: break-word; }
.info dl > div > dt { width: 50%; font-weight: bold; text-align:right; }
.info dl > div > dd { width: auto; }
.info dl > div > dt::after { content:":"; display:inline-block;padding-left: 0.25em; height:100%; }
.infos { background: rgba(255,255,255,0.15); padding: 0.3rem 0; }
.infos h2 { background: rgba(0,0,0,0.25); }
.infos dl { background: rgba(255,255,255,.15); padding: 0.3rem 0; }
.infos .started, .infos .online { color: green; animation: glow 2s infinite alternate; }
.infos .enabled, .infos .on { color: lightgreen; }
.infos .stopped, .infos .disabled { color: darkred; }
@keyframes glow {
  from {
    color: green;
  }
  to {
    color: lightgreen;
  }
}

.key__button {
  width: 2rem;
  height: 2rem;
  line-height: 2rem;
  box-sizing: border-box;
  font-size: .80rem;
  text-align: center;
  color: rgba(0,0,0,.75);
  cursor: pointer;
  margin: 0 .5rem;
  border-color: #f2f2f2;
  border-style: solid;
  text-shadow: 0 0.5px 1px #777, 0 2px 6px #f2f2f2;
  border-width: .1rem;
  border-radius: .5rem;
  background: -webkit-linear-gradient(top, #f9f9f9 0%, #D2D2D2 80%, #c0c0c0 100%);
  display: inline-block;
  box-shadow: 0 0 1px #888,0 1px 0 #fff, 0 6px 0 #C0C0C0, 0 8px 17px rgba(#444, 0.4), 2px 1px 4px rgba(#444, 0.25), -2px 1px 4px rgba(#444, 0.25), 0 9px 16px rgba(#444, 0.1);
}
.key__button.on {
  animation: glow 2s infinite alternate;
  background: -webkit-linear-gradient(top, #666 0%, #555 80%, #333 100%);
  border-color: #666;
  text-shadow: 0 1px 1px #fff;
}

/* credits page */
body.credits { padding: 1rem }

/* disclaimer page */
body.disclaimer { padding: 1rem }




)StylesCSS";



