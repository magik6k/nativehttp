/*
Copyright (c) 2013 Lukasz Magiera

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/
#include "assets.h"



string html_on = "<span style='color: #0a0'>on</span>";
string html_off = "<span style='color: #f00'>off</span>";

string ddbase = "<div style='width: 100%'>"
"<div style='[[csslftu]]'>[[c1]]</div>"
"<div style='[[cssrgtu]]'>[[c2]]</div>"
"</div>"
"<div style='width: 100%'>"
"<div style='[[csslft]]'></div>"
"<div style='[[cssrgt]]'></div>"
"</div>";

string html_top = "<html>"
"<head>"
"<title>NativeStat - NativeHTTP server stats</title>"
"<link href='http://fonts.googleapis.com/css?family=Basic' rel='stylesheet' type='text/css'>"
"<style>"
"[[style]]"
"</style>"
"</head>"
"<body>"
"<div id='top'>"
"<div id='am'>"
"<div id='main'>"
"[[content]]"
"</div>"
"</div>"
"<div id='bot'>"
"Running NativeStat Devel<br/>"
"Server version: [[srvVer]]</div>"
"</div>"
"</div>"
"</body>"
"</html>";

string logform =
"<form method='POST' action='#'>"
"Security Password:<br/>"
"<input type='password' name='scky' />"
"<input type='submit' value='enter' />"
"</form>";

string logformbad =
"<span style='color: red;'>Wrong Password!</span> Retry:<br/>"
"<form method='POST' action='#'>"
"Security Password:<br/>"
"<input type='password' name='scky' />"
"<input type='submit' value='enter' />"
"</form>";

string page_style = "body{background-color: #eee;}#top{margin-left: 5%;margin-right: 5%;margin-top: 10px;}#am{background-color: #fff;}#main{"
"font-family: 'Basic', sans-serif;margin-left: 5px;}#bot{font-family: 'Basic', sans-serif;background-color: #fff;margin-top: 10px;font-size: 10px;"
"color: #777;text-align: center;}code{background-color: #ddd;margin-left: 10px;margin-top: 3px;margin-bottom: 3px;}pre{background-color: #ddd;"
"margin-left: 10px;margin-top: 3px;margin-bottom: 3px;overflow: auto;}ul, ul li{display: block;list-style: none;margin-left: 10px;padding: 0;"
"}a:link{text-decoration: none;color: #00BF1D;}a:visited{text-decoration: none;color: #00BF1D;}a:active{text-decoration: none;color: #00BF1D;"
"}a:hover{text-decoration: none;color: #009E18;}.lr{stroke:rgb(187,17,17);stroke-width:1;}.lo{stroke:rgb(220,80,0);stroke-width:1;}.lg{"
"stroke:rgb(17,187,17);stroke-width:1;}";


