## Structure of the converters.json file

This document briefly outlines the structure of the **converters.json** configuration file, which is where Ferocious stores the configuration for external command-line converters (other than ReSampler).

Within the context of the **converters.json** file, a *converter configuration* represents a description of a given command-line converter tool, the parameters it is to be invoked with, and various metadata associated with it.

The JSON structure contains an array of converter configurations. 

| JSON  | Comments |
| ------------- | ------------- |
|[|Array of converter configurations|
|&nbsp;&nbsp;     {
|&nbsp;&nbsp;&nbsp;&nbsp;         "commandline": "--decode --silent {i} {o}", | command line **{i}** = substitute input file name, **{o}** = substitute output file name |
|&nbsp;&nbsp;&nbsp;&nbsp;         "comment": "Decode mp3 to wav using Lame", | comment describing what this converter (and the given command line) achieves. |
|&nbsp;&nbsp;&nbsp;&nbsp;         "downloadlocations": [|List of possible download locations for the converter|
|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;   "http://lame.sourceforge.net/download.php"
|&nbsp;&nbsp;&nbsp;&nbsp;         ], 
|&nbsp;&nbsp;&nbsp;&nbsp;         "enabled": false,
|&nbsp;&nbsp;&nbsp;&nbsp;         "executablename": "lame.exe",| *expected* filename of converter (gui uses this when asking user for the path)|
|&nbsp;&nbsp;&nbsp;&nbsp;         "executablepath": "",|path of converter|
|&nbsp;&nbsp;&nbsp;&nbsp;         "inputfileext": "mp3",|the type of file the converter accepts|
|&nbsp;&nbsp;&nbsp;&nbsp;         "outputfileext": "wav",|the type of file the converter produces|
|&nbsp;&nbsp;&nbsp;&nbsp;         "name": "Decode mp3 with Lame",|name of this converter configuration|
|&nbsp;&nbsp;&nbsp;&nbsp;         "operatingsystems": [|if converters for multiple operating systems are configured, Ferocious will only show the ones that are relevant for the operating system Ferocious is currently running on|
|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;             "win"
|&nbsp;&nbsp;&nbsp;&nbsp;         ],
|&nbsp;&nbsp;&nbsp;&nbsp;         "priority": 0
|&nbsp;&nbsp;}
|]