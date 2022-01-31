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



    body, h1, h2, h3, p, div { font-family: sans; }
    body { background:#1451AE; color:#ffffff; font-size:14px; margin:0; padding:0.4em; max-width:80em; margin:auto; text-shadow: 1px 1px 1px black; }
    h1, h2, h3 { text-align:center;  }
    h1 { margin: 0; }
    h3 { margin-top: 0; }
    nav { position: fixed; top: 0; left: 0;  width: 100%; z-index: 1000; }
    a { color: inherit; }
    button { border-radius: 0.2rem; padding: 0.2rem 0.4rem; margin: 0 .7rem; border: 0px solid transparent; box-shadow: 1px 1px 1px black; }


    .explorer label[for="newfile"] { display: inline-block; margin: 0 3rem; background: rgba(0,0,0,.12); border-top: .4rem solid transparent; border-radius: 0.3rem; box-shadow: 1px 1px 1px black; padding: 0 .5rem; user-select: none; }


    /*
     * .explorer .table { background: rgba(0,0,0,.15); width:100%; box-shadow: 1px 1px 1px black; }
    .explorer .th { background: rgba(0,0,0,.25); }
    .explorer dd { background: rgba(255,255,255,.25); }
    .explorer .th, .explorer dd { padding: .5rem; white-space: nowrap; }
    .explorer dd > a, .explorer dd > span { display: inline-block; white-space: nowrap; text-overflow: ellipsis; overflow: hidden; }
    .explorer dd.file-name > * { float:left; }
    .explorer dd.file-name a { max-width: calc( 100% - 4em ); margin-left:1em }
    .explorer .tr:nth-of-type(2n+2) { background: rgba(255,255,255,.15); }
    */

    :root {
      --grid-auto-flow: column;
      --grid-auto-columns: 1fr;
      --grid-template-rows: repeat(50, min-content);
      --grid-row-start:1;
      --table-padding-top: 0;
      --table-display: grid;
      --table-row-display:inline-grid;
      --table-row-height:1.7rem;
      --table-row-margin:0;
      --header-visibility: visible;
      --header-title-cells-display: block;
      --title-cells-text-align: center;
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

    .explorer .action-button { margin: 0; padding: 0; width: 1.8em; height: 1.8em; line-height: .1em; font-size: 1em; }
    .explorer dd .action-button {  width: var(--action-button-width); margin-right: 1em; }


    .explorer .action-button > span {
        margin-left: 0.3rem;
        display:var(--action-button-text-display);
    }

    .explorer .file-name { overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }

    .explorer .table .tr.thead { visibility:var(--header-visibility); }

    .explorer dl > div:nth-of-type(2n+2) { background: rgba(255,255,255,.15); }

    .explorer dl.table {
        display: var(--table-display);
        padding-top: var(--table-padding-top);
    }

    .explorer .tr.thead, .explorer .tr.tbody {
        display: var(--table-row-display);
        grid-auto-flow: var(--grid-auto-columns);
        grid-auto-columns: var(--grid-auto-columns);
        grid-template-rows: var(--grid-template-rows);
    }

    .explorer .tr.tbody { position:relative; padding: 0.6rem 0.3rem; border-radius: .3rem; height:var(--table-row-height); line-height:var(--table-row-height); }

    .explorer .tr.tbody dt { display: var(--body-title-cells-display); margin: .5rem 0; }
    .explorer .tr.thead dt { display: var(--header-title-cells-display); }

    .explorer .tr.tbody dd { margin-bottom: var(--table-row-margin); margin-top: var(--table-row-margin); }

    .explorer .tr.tbody dd.file-size { display: var(--file-size-display); }
    .explorer .tr.thead dt.file-size { display: var(--file-size-display); }

    .explorer .tr.tbody dd.file-action { display: var(--file-action-display); }
    .explorer .tr.thead dt.file-action { display: var(--file-action-display); }

    .explorer dd > span {  margin-right: .5rem; }

    .explorer dd, .explorer dt {
        grid-row-start: var(--grid-row-start); /* reset, next column */
    }

    .explorer dt {
      text-align: var(--title-cells-text-align);
      font-weight: var(--title-cells-font-weight);
      font-size: var(--title-cells-font-size);
    }

    .explorer input[type="radio"].toggle-button { display: none; }

    .explorer label.toggle-button {
      display: var(--toggle-button-display);
      position: absolute;
      margin: 0;
      right: .6rem;
      font-size: 1.8rem;
      line-height: .3em;
      height: 1.2rem;
      width: .9rem;
    }
    .explorer label.toggle-button::before {
      content: var(--toggle-button-content);
      display: inline-block;
      left: calc( -100vw + 100% + 1rem );
      position: absolute;
      line-height: 1em;
      font-size: .9rem;
    }

    .explorer .tr.tbody input.item-expander { display: none; }
    .explorer .tr.tbody label.item-expander { display: var(--item-expander-display); position: absolute; left: 0.3em; top: 50%; transform: translate(50%,-50%); }

    .text-editor { position: fixed; top: 0; left: 0; width: 100%; display: block; padding-top: 3em; height: 100%; background: rgba(0,0,0,0.75); z-index: 1001; }
    .text-editor textarea { width: calc( 100% - 1em ); min-height: 30vh; resize: both; margin-bottom: .5rem; }

    /*
    Max width before this PARTICULAR table gets nasty
    This query will take effect for any screen smaller than 760px
    and also iPads specifically.
    */
    @media  only screen and (max-width: 760px), (min-device-width: 768px) and (max-device-width: 1024px)  {

      body {
        padding:0;
      }


      :root {
        --table-padding-top: 2rem;
        --header-visibility: hidden;
        --toggle-button-display: block;
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

      .explorer #cv2:checked ~ label[for="cv2"] { display:none; }
      .explorer #cv1:checked ~ label[for="cv1"] { display:none; }


/*      .explorer .table { width: 100%; padding-top:2em; }
      .explorer .table, .explorer .thead, .explorer .tbody, .explorer .th, .explorer dd, explorer dl, explorer dt, .explorer .tr { display: block; }
      .explorer .thead { position: absolute; top: -9999px; left: -9999px; }
      .explorer .tr { box-shadow: 4.5rem 0 0 rgba(0,0,0,0.05) inset; margin-bottom: 0.3rem; }
      :root { --td-height: 2rem; }
      .explorer dd { position: relative; padding-left: 23%; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; height:var(--td-height, 2rem); line-height:var(--td-height, 2rem); transition: height 0.3s ease; }
      .explorer dd:not(:last-child) { border-bottom: 1px solid rgba(255,255,255,0.2); }
      .explorer dd:before { position: absolute; top: .8em; left: .6rem; height: var(--td-height, 2rem); max-width: 25%; padding-right: 10px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; content: attr(data-column); font-weight: bold; font-size: .75rem; text-align: left; text-transform: capitalize; }
      .explorer .tr label { position: relative; right: .3rem; margin: 0; padding: 0; height: 100%; top: 0; width: 2rem; }
      .explorer .tr label input[type="checkbox"] { opacity:0.5; position: absolute; }
      .explorer .tr label+input[type="checkbox"]:not(:checked) ~ dd:not(:nth-child(2)) { height: 0px; padding-top: 0; padding-bottom: 0; border-bottom: 0; }
      input[type="radio"][name="compact-view"][value="on"]:checked ~ .table .tr:not(:target):not(:active):not(focus) dd:not(:nth-child(2)) { height: 0px; padding-top: 0; padding-bottom: 0; border-bottom: 0; }*/



    }

    /* general rules */
    .explorer #quack, .info #info { display:none; }
    #reload { position: absolute; right: 0.4em;  top: 0.4em; }
    #info, #quack { position: absolute; left: 0.4em;  top: 0.4em; }

    /* file explorer */

    #filesystem-tab, #uploader-tab { background: rgba(255,255,255,.15); margin: .5rem 0; border-radius: .5rem; text-align: left; padding: 1.3em .7rem; box-shadow: 1px 1px 1px black; }
    #filesystem-tab::before { display:inline-block;content:"Filesystem:"; }
    #filesystem-tab button { color: white; background: rgba(0,0,0,0.25); }
    #filesystem-tab button::before { display:inline-block;content:"Switch to:"; }
    #filesystem-tab button.enabled::before { display:inline-block;content:"Using:"; }
    #filesystem-tab button.enabled { color: lightgreen; background: green; border-color: darkgreen; pointer-events:none; }
    #logs::before { display:block;content:"System Logs:"; margin-bottom: .5rem; }
    #newfile { opacity: 0; width: 3rem; height: 2rem; }

    #upload { margin-right: 2rem; display:none; }
    #logs { text-align: left; max-height: 480px; overflow-y: scroll; font-family: courier new; color: lightgreen; background: black; padding: 0.5em 1em; }
    #filesystemlogs { margin: 0 auto; }

    .noaction .file-action, .nodelete .delete-tab, .nodelete #uploader-tab { display: none; }
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

    .file-action button { margin-right:1em; }
    .fixed { margin: 0 auto; }

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





)StylesCSS";



