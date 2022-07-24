#ifndef HTML_FRAGMENTS_H
#define HTML_FRAGMENTS_H

/* 
 * File:   HTMLFragments.h
 * Author: raodm
 *
 * Copyright (C) 2021 raodm@miamioh.edu
 * 
 * Created on April 17, 2021, 4:56 PM
 */

#include <string>

/** \file HTMLFragments.h
 * 
 * This file contains fragments of HTML that can be used to send HTML
 * formatted response to the client when a process is being run.
 */

/** A fixed constant string that is used as the start of the response
    sent when processing a program execution request */
const std::string htmlStart =
          "<html>\n"
          "  <head>\n"
          "    <script type='text/javascript' "
          "src='https://www.gstatic.com/charts/loader.js'></script>\n"
          "    <script type='text/javascript' src='/draw_chart.js'></script>\n"
          "    <link rel='stylesheet' type='text/css' href='/mystyle.css'>\n"
          "  </head>\n\n"
          "  <body>\n"
          "    <h3>Output from program</h3>\n"
          "    <textarea style='width: 700px; height: 200px'>\n";


/** A fixed srtring to used to generate the middle part of the HTML
    document containing the HTML table. */
const std::string htmlMid1 =
          "     </textarea>\n"
          "     <h2>Runtime statistics</h2>\n"
          "     <table>\n"
          "       <tr><th>Time (sec)</th><th>User time</th><th>System time"
          "</th><th>Memory (MB)</th></tr>\n";

/** A fixed srtring to used to generate the middle part of the HTML
    document containing chart and starter code for javascript . */
const std::string htmlMid2 =
          "     </table>\n"
          "     <div id='chart' style='width: 900px; height: 500px'></div>\n"
          "  </body>\n"
          "  <script type='text/javascript'>\n"
          "    function getChartData() {\n"
          "      return google.visualization.arrayToDataTable(\n"
          "        [\n"
          "          ['Time (sec)', 'CPU Usage', 'Memory Usage']";

/** A fixed srtring to used to generate the end part of the HTML
    document containing chart and starter code for javascript . */
const std::string htmlEnd =
          "\n        ]\n"
          "      );\n"
          "    }\n"
          "  </script>\n"
          "</html>\n";

#endif /* HTML_FRAGMENTS_H */

