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
 * - Easy maintenance: ENABLE SYNTAX HIGHLIGHTING FOR CSS and ignore the C header/footer of this file
 *
\*/
#pragma once

const char* styles_css  = R"StylesCSS(

:root {
  --body-bgcolor: #1451AE;
  --body-textcolor: #ffffff;
  --body-fontsize: 14px;

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


* { box-sizing:content-box; }
body, h1, h2, h3, p, div { font-family: sans; }
body { background:var(--body-bgcolor); color:var(--body-textcolor); font-size:var(--body-fontsize); margin:0; padding:0.4em; max-width:80em; margin:auto; text-shadow: 1px 1px 1px black; }
h1, h2, h3 { text-align:center;  }
h1 { margin: 0; }
h3 { margin-top: 0; }
a { color: inherit; }
button { border-radius: 0.2rem; padding: 0.2rem 0.4rem; margin: 0 .7rem; border: 0px solid transparent; box-shadow: 1px 1px 1px black; }


/* prevent hscrollbar appearance while animating */
div.main { position: relative; overflow: hidden; }

.icon { width: 1em; height: 1em; display: inline-block; margin-left: 0; background-repeat: no-repeat; background-position: center center; background-size: contain; user-select: none; }

.icon.sdcard {
  background-image: url("data:image/svg+xml;utf8,%3csvg xmlns='http://www.w3.org/2000/svg' fill='white' width='24' height='24' viewBox='0 0 24 24'%3e%3cpath d='M18 2h-8L4.02 8 4 20c0 1.1.9 2 2 2h12c1.1 0 2-.9 2-2V4c0-1.1-.9-2-2-2zm-6 6h-2V4h2v4zm3 0h-2V4h2v4zm3 0h-2V4h2v4z' /%3e%3c/svg%3e")
}

.icon.spiffs {
  background-image: url("data:image/svg+xml;utf8,%3csvg xmlns='http://www.w3.org/2000/svg' fill='white' width='24' height='24' viewBox='0 0 24 24'%3e%3cpath d='M15 9H9v6h6V9zm-2 4h-2v-2h2v2zm8-2V9h-2V7c0-1.1-.9-2-2-2h-2V3h-2v2h-2V3H9v2H7c-1.1 0-2 .9-2 2v2H3v2h2v2H3v2h2v2c0 1.1.9 2 2 2h2v2h2v-2h2v2h2v-2h2c1.1 0 2-.9 2-2v-2h2v-2h-2v-2h2zm-4 6H7V7h10v10z'%3e%3c/path%3e%3c/svg%3e")
}
.icon.upload
{
  background-image: url("data:image/svg+xml;utf8,%3csvg xmlns='http://www.w3.org/2000/svg' fill='white' width='24' height='24' viewBox='0 0 24 24'%3e%3cpath d='M9 16h6v-6h4l-7-7-7 7h4zm-4 2h14v2H5z'%3e%3c/path%3e%3c/svg%3e")
}

.icon.espressif
{
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 512 512'%3E%3Crect width='512' height='512' rx='15%25' fill='%23ff3034'/%3E%3Cpath fill='%23fff' d='M126 120.53a188.46 188.46 0 0 0 266.53 266.52 7.69 7.69 0 1 1 10.87 10.87 203.83 203.83 0 0 1-288.26-288.26A7.69 7.69 0 0 1 126 120.53zm52.5 240.22a26 26 0 1 0 0 1M409 334a273 273 0 0 0-231.5-231.5A150 150 0 0 0 139 130v26a216 216 0 0 1 217 216h26a111 111 0 0 0 27-39M240 83l-4 12a298 298 0 0 1 181 180l11-4A172 172 0 0 0 240 83zm5 342a118 118 0 0 0-95-171 10.61 10.61 0 0 1 3-21 140 140 0 0 1 120 177l31 9 26.5-10a195 195 0 0 0-163-230 66.6 66.6 0 1 0-18 131.5 63 63 0 0 1 42 95.5l21 14z'/%3E%3C/svg%3E");
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

nav {
  width: 100%;
  max-width: 80em;
  position: fixed;
  z-index: 10000;
}


.sr-only { display:none; }

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

.explorer dd > span {  margin: 0 .5rem 0.5rem 0; }

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

/* This query will take effect for any screen smaller than 760px and also iPads specifically. */
@media  only screen and (max-width: 760px), (min-device-width: 768px) and (max-device-width: 1024px)  {
  /* prevent double-tap from selecting non tabular text */
  body, h1, h2, h3, p, div { user-select:none; }

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

nav label[for="newfile"] { position: absolute; left: 2.5rem; top: 0.4rem; width: 1.8em; height: 1.8em; }
#info { position: absolute; left: 0.4rem;  top: 0.4rem; }
#reload { position: absolute; right: 2.5rem;  top: 0.4rem; }
#settings { position: absolute; right: 0.4rem;  top: 0.4rem; }
.overflow-text { white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }

/* quack form */
.quacker-tab {
  position: fixed;
  bottom: 0px;
  background: var(--body-bgcolor);
  padding: 1em;
  width: 100%;
  max-width: 80em;
  box-sizing: border-box;
  border-top: .3em solid rgba(255,255,255,.7);
  z-index: 10000;
}
.quacker-tab #quackinput { border-radius: .5em; border-color: transparent; width: 16rem; }
label[for="quackinput"] { display:block; padding: 0.2rem; }

/* file explorer */

#uploader-tab { transition: transform 0.3s ease-out; }
#control-panel { text-align:center; }
#control-panel { background: rgba(255,255,255,.15); margin: .5rem 0; border-radius: .5rem; padding: 1.3em .7rem; }

/* make the label fit inside the button */
label[for="infos-panel-toggle"],
label[for="control-panel-toggle"],
label[for="no-reload-checkbox"] {
  position: relative;
  display: block;
  background-color: inherit;
  width: 100%;
  height: 100%;
  color: transparent;
  overflow: hidden;
}
label[for="control-panel-toggle"]::before,
label[for="infos-panel-toggle"]::before,
label[for="no-reload-checkbox"]::before {
  position: absolute;
  display: block;
  color: black;
  width: 1.8em;
  height: 1.8em;
  box-sizing: border-box;
  overflow: hidden;
  line-height: 1.8em;

  font-size: 1.3rem;
  left: 50%;
  top: 50%;
  transform: translate(-50%,-50%);
}
label[for="control-panel-toggle"]::before { content: "⚙"; line-height: 1.5em; }
label[for="infos-panel-toggle"]::before { content: "ℹ️"; }
label[for="no-reload-checkbox"]::before { content: "🔃"; }

/* div rules to hide 'other' divs */

/* hide self when option not checked */
input#control-panel-toggle:not(:checked) ~ * #control-panel,
input#infos-panel-toggle:not(:checked) ~ * .infos,
/* hide other when self is checked */
input#control-panel-toggle:checked ~ * .infos,
input#control-panel-toggle:checked ~ * .files-view,
input#infos-panel-toggle:checked ~ * .files-view,
input#infos-panel-toggle:checked ~ * #control-panel
{
  outline: 2px solid white;
  transform: scale(100%,0) translate(0, 0 );
  position: absolute;
  margin: auto;
  width: 100%;
  top: 0;
  padding: 0;
  border: 0;
}

/* nav buttons rules */
input#infos-panel-toggle:checked ~ * #settings,
input#infos-panel-toggle:checked ~ * #reload,
input#infos-panel-toggle:checked ~ * label[for="newfile"],
input#control-panel-toggle:checked ~ * #info,
input#control-panel-toggle:checked ~ * #reload,
input#control-panel-toggle:checked ~ * label[for="newfile"]
{
  pointer-events: none;
  opacity:0.5;
}

input#control-panel-toggle:checked ~ * label[for="control-panel-toggle"]::before,
input#infos-panel-toggle:checked ~ * label[for="infos-panel-toggle"]::before
{
  content: "🔙";
  line-height: 1.8em;
}

.files-view, .infos, #control-panel {
  transition: transform 0.3s linear, top 0.2s;
  opacity: 1;
  margin-bottom: 12em;
}

.explorer button.clear-logs-button { position: absolute; transform: translate(0, -110%); right: 0; }
#logs::before { display:block;content:"System Logs:"; margin-bottom: .5rem; }
#filepath::before { content: attr(value); }
#uploader { display:none; }
#upload { margin: 1em auto; display: none; width: 32em; max-width: calc( 100vw - 2em ); position:relative; }
#upload::after { content: attr(data-path); width: 100%; text-overflow: ellipsis; overflow-x: hidden; display: block; line-height: 2.6em; font-weight: bold; font-size: .9em; }
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
.infos .action-button { margin: 0; padding: 0; width: 1.8em; height: 1.8em; line-height: .1em; font-size: 1em; }
.infos dl { display: table; width:100%;  }
.infos dl > div { display: table-row; width:100%; }
.infos dl > div > dt, .infos dl > div > dd { display: table-cell; padding: 0.25em; height:auto; word-break: break-word; }
.infos dl > div > dt { width: 50%; font-weight: bold; text-align:right; }
.infos dl > div > dd { width: auto; }
.infos dl > div > dt::after { content:":"; display:inline-block;padding-left: 0.25em; height:100%; }
.infos { background: rgba(255,255,255,0.15); padding: 0.3rem 0; }
.infos h2 { background: rgba(0,0,0,0.25); }
.infos dl { background: rgba(255,255,255,.15); padding: 0.3rem 0; }
.infos .started, .infos .online { color: green; animation: glow 2s infinite alternate; }
.infos .enabled, .infos .on { color: lightgreen; }
.infos .stopped, .infos .disabled { color: darkred; }

.infos .icon.espressif { padding: 0; margin: .5em .5em 0 0; width: 100%; height: 1.5em; display: inline-block; text-indent: 2em; background-position: left; }
.infos span[data-status]::after { content: " " attr(class); color: white; text-transform: capitalize; padding-left: 0.5em; }
.infos .kbd span[data-status] { display:inline-block; width: 100%; line-height: 2.5em; }
.infos .kbd span[data-status]::after { white-space: pre; }

@keyframes glow {
  from {
    color: green;
  }
  to {
    color: lightgreen;
  }
}

/* css keyboard buttons for "capslock", "numlock" and "scrolllock" */

.infos .key__button {
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
.infos .key__button.on {
  animation: glow 2s infinite alternate;
  background: -webkit-linear-gradient(top, #666 0%, #555 80%, #333 100%);
  border-color: #666;
  text-shadow: 0 1px 1px #fff;
}

/* credits page */
body.credits { padding: 1rem }

/* disclaimer page */
body.disclaimer { padding: 1rem }

.mouse-tab-container {
  display:none;
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  min-width: 320px;
  min-height: 240px;
  background: rgba(0,0,0,.25);
  resize: both;
  overflow: auto;
  z-index: 100000;
  padding: 2em;
  box-sizing: border-box;
}

#mouse-tab { background: white; width: 100%; height: 100%; position: relative; }
#mouse-tab::before { position: absolute; top: -1.5em; left: 0; content: "Mouse: " attr(data-btn) ", x:" attr(data-x) ", y:" attr(data-y); }
#mouse-tab::after { right: 0; top: -1.5em; content: "X"; position: absolute; background: white; color: red; width: 2em; text-align: center; border-radius: .2em .2em 0 0; height: 1.5em; line-height: 1.5em; }

)StylesCSS";



