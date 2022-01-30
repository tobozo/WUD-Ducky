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

    button { border-radius: 0.2rem; padding: 0.2rem 0.4rem; margin: 0 .7rem; border: 0px solid transparent; box-shadow: 1px 1px 1px black; }
    table { background: rgba(0,0,0,.15); width:100%; box-shadow: 1px 1px 1px black; }
    th { background: rgba(0,0,0,.25); }
    td { background: rgba(255,255,255,.25); }
    th, td { padding: .5rem; white-space: nowrap; }
    a { color: inherit; }
    td > a, td > span { display: inline-block; white-space: nowrap; text-overflow: ellipsis; overflow: hidden; }
    td.file-name > * { float:left; }
    td.file-name a { max-width: calc( 100% - 4em ); margin-left:1em }


    label { display: inline-block; margin: 0 3rem; background: rgba(0,0,0,.12); border-top: .4rem solid transparent; border-radius: 0.3rem; box-shadow: 1px 1px 1px black; padding: 0 .5rem; user-select: none; }


    .toggle-button { display:none; } /* not needed in general rules */


    /* responsive table layout (requires data-column attribute in each td) */
    table {
      /*width: 750px;
      border-collapse: collapse;
      margin:50px auto;*/
      }

    /* Zebra striping */
    tr:nth-of-type(2n+2) {
      background: rgba(255,255,255,.15);
    }


    .text-editor {
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      display: block;
      padding-top: 3em;
      height: 100%;
      background: rgba(0,0,0,0.75);
      z-index: 1001;
    }
    .text-editor textarea {
      width: calc( 100% - 1em );
      min-height: 30vh;
      resize: both;
      margin-bottom: .5rem;
    }


/*
    th {
      background: #3498db;
      color: white;
      font-weight: bold;
      }

    td, th {
      padding: 10px;
      border: 1px solid #ccc;
      text-align: left;
      font-size: 18px;
      }
*/
    /*
    Max width before this PARTICULAR table gets nasty
    This query will take effect for any screen smaller than 760px
    and also iPads specifically.
    */
    @media  only screen and (max-width: 760px), (min-device-width: 768px) and (max-device-width: 1024px)  {

      table {
          width: 100%;
          padding-top:2em;
      }

      /* Force table to not be like tables anymore */
      table, thead, tbody, th, td, tr {
        display: block;
      }

      /* Hide table headers (but not display: none;, for accessibility) */
      thead tr {
        position: absolute;
        top: -9999px;
        left: -9999px;
      }

      tr { box-shadow: 4.5rem 0 0 rgba(0,0,0,0.05) inset; margin-bottom: 0.3rem; }

      :root {
        --td-height: 2rem;
      }

      td {
        /* Behave  like a "row" */
        position: relative;
        padding-left: 23%;
        white-space: nowrap;
        overflow: hidden;
        text-overflow: ellipsis;
        height:var(--td-height, 2rem);
        line-height:var(--td-height, 2rem);
        transition: height 0.3s ease;
      }

      td:not(:last-child) { border-bottom: 1px solid rgba(255,255,255,0.2); }

      td:before {
        position: absolute;
        top: .8em;
        left: .6rem;
        height: var(--td-height, 2rem);
        /*line-height:var(--td-height, 2rem);*/
        max-width: 25%;
        padding-right: 10px;
        white-space: nowrap;
        overflow: hidden;
        text-overflow: ellipsis;
        content: attr(data-column);
        font-weight: bold;
        font-size: .75rem;
        text-align: left;
        text-transform: capitalize;
      }




      input[type="radio"][name="compact-view"][value="on"]:checked ~ table tr td:not(:first-child):not(:target):not(:active):not(focus) {
        height: 0px;
        padding-top: 0;
        padding-bottom: 0;
        border-bottom: 0;
      }

      label.toggle-button {
        display: block;
        position: absolute;
        margin: 0;
        right: .3rem;
        font-size: 1.8rem;
        line-height: .3em;
        height: 1.2rem;
        width: .9rem;
      }
      label.toggle-button::before {
        content: "Compact view: ";
        display: inline-block;
        left: calc( -100vw + 100% + 1rem );
        position: absolute;
        line-height: 1em;
        font-size: .9rem;
      }
      #cv1:checked ~ label[for="cv1"] { outline: 3px solid orange; display:none; }
      #cv2:checked ~ label[for="cv2"] { outline: 3px solid orange; display:none; }


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

    .noaction .action-tab, .nodelete .delete-tab, .nodelete #uploader-tab { display: none; }
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
    .action-button { margin: 0; padding: 0; width: 1.8em; height: 1.8em; line-height: .1em; font-size: 1em; }
    .action-tab button { margin-right:1em; float:left; }
    .fixed { margin: 0 auto; }

    /* system information */

    dl { display: table; width:100%;  }
    dl > div { display: table-row; width:100%; }
    dl > div > dt, dl > div > dd { display: table-cell; padding: 0.25em; height:auto; word-break: break-word; }
    dl > div > dt { width: 50%; font-weight: bold; text-align:right; }
    dl > div > dd { width: auto; }
    dl > div > dt::after { content:":"; display:inline-block;padding-left: 0.25em; height:100%; }
    .infos { background: rgba(255,255,255,0.15); padding: 0.3rem 0; }
    .infos h2 { background: rgba(0,0,0,0.25); }
    .infos dl { background: rgba(255,255,255,.15); padding: 0.3rem 0; }


)StylesCSS";



