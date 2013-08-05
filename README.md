ProjectTimeCone
===============

An interactive installation that use multiple cameras to create a bullet-time like effect using frame interpolation.

### Get the right development tools
 - Download CMake 2.8+
 - Download Git 

#### Windows 7
- For Windows 7 download [openFrameworks 0.7.4](http://www.openframeworks.cc/download) for  [win vs2010](http://www.openframeworks.cc/versions/v0.7.4/of_v0.7.4_vs2010_release.zip)

#### Windows 8
 - Download and install the _MicroSoft VisualStudio C++ Redistributable Package (x86)_ [link](http://www.microsoft.com/en-us/download/details.aspx?id=5555) and extract it to: `c:\projects\ProjectTimeCone`
 - For Windows 8, clone the current OF git repository and make sure you have this revision: 5125fdf5c1e52659690497b0bfaceafb17f8143a (git checkout 5125fdf5c1e52659690497b0bfaceafb17f8143a )
 - Install Visual Studio 2012 Professional Trial (you can use it for 90 days)

#### Mac
 - Download the [mac](http://www.openframeworks.cc/versions/v0.7.4/of_v0.7.4_osx_release.zip) version. 

- Clone the repository:

````sh
$ cd c:\projects\ProjectTimeCone\apps
$ git clone git@github.com:HellicarAndLewis/ProjectTimeCone.git

$ cd ProjectTimeCone
$ git submodule init
$ git submodule update

$ cd addons/roxlu
$ git checkout master

````

### Setup Runtime-VideoEncoder

- [Download the nightly of libav (from 2013.07.24).](http://win32.libav.org/win32/libav-win32-20130724.7z)
- Extract the file 
- Copy all the dll files and the **avconv.exe** to `Runtime-VideoEncoder/bin/avconv/win/`
- **To compile**
  - open a VS2010/VS2012 terminal
  - go to `Runtime-VideoEncoder/build/cmake/` 
     - for release: `build_release_[vs2010,vs2012].bat` 
     - for debug:  `build_debug_[vs2010,vs2012].bat`

### Install Runtime-VideoEncoder

- The debug version can be run from the command line 
- The release version should be installed as service
- To install the release version do:    
  - Open a command prompt
  - Remove the service:
  - Install service: 
      - Open a Command Prompt as Administrator:
      - from start screen, search for `cmd.exe`
      - then right click and select Run as Administrator from bottom menu
      - Type: `sc create "VideoEncoder" binpath= "C:\projects\ProjectTimeCone\apps\ProjectTimeCone\Runtime-VideoEncoder\bin\Runtime-VideoEncoder.exe"  

_Start/stop the VideoEncoder Service_
- `net start VideoEncoder`
- `net stop VideoEncoder`

### Setup Runtime-YouTube

- Create a Google Application:
  - Get a Google account _and make sure_ to login into YouTube before continueing
  - Go to [Google API Console](https://code.google.com/apis/console) and create a new project. [See this page for more info](https://developers.google.com/youtube/registering_an_application) 
  - Make sure that you enable the _YouTube Data API v3_ in the servics pane.
  - Click on the _API Access_, then _Create an oAuth client ID..._
     - Leave all things default
     - Create an _installed application_
     - Select _other_ as type
- Go to `ProjectTimeCone\Runtime-YouTube\bin\data` and copy `youtube_example.cfg` to `youtube.cfg`
- Open the `youtube.cfg` file and copy the `Client ID` and `Client SECRET` into this file. 
- Open `ProjectTimeCone\addons\roxlu\addons\YouTube\html\index.html` in Chrome
  - paste the `client ID` into the field and press `GO`. 
  - Accept the permission popup, and copy the `auth-code` you get into `youtube.cfg`.
- **To compile**:
  - open a VS2010/vs2012 terminal
  - go to `Runtime-YouTube/build/cmake/` 
     - for release: `build_release_[vs2010,vs2012].bat` 
     - for debug:  `build_debug_[vs2010,vs2012].bat`

### Install Runtime-YouTube
- Open a Command Prompt as Administrator:
  - from start screen, search for `cmd.exe`
  - then right click and select Run as Administrator from bottom menu
- Type: `sc create "YouTubeUploader" binpath= "C:\projects\ProjectTimeCone\apps\ProjectTimeCone\Runtime-YouTube\bin\Runtime-YouTube.exe"


_Start/stop the YouTubeUploader Service_
- `net start YouTubeUploader`
- `net stop YouTubeUploader`

### Setup Test-YouTubeClientIPC

- Make sure the destination path for the frames exists: `Test-YouTubeClientIPC\bin\data\frames\` if not, create it.
- Compile the app + run it (open project file in VS2010 or XCode)

### Updating changes made in a submodule

Before you commit your changes, check if you are working in a branch or not. By default,
after doing `git submodule update` you're not working in a brnach. 

_Check if you're working in a branch_

````sh
$ cd submodule
$ git branch -a
````

_Not working in a branch_

````sh
$ cd submodule
$ git checkout master
$ git commit -am "Updating changes of submodule"
$ git push origin master
````

_Working in a branch_

````sh
$ cd submodule
$ git commit -am "Updating changes of submodule"
$ git push origin master
````

_tell the root project to use the current submodule commit_
```sh
$ cd ../to/project/root
$ git commit -am "Updated submodule"
$ git push origin master
````
  
### Getting the latest version of a submodule

````sh
$ cd project
$ git remote update
$ git clone origin/master   # get the latest submodule info
$ git submodule update      # let git update the submodule info which is stored in the root project
````

### Configuring a project for the YouTube and VideoEncoder code

When you're using the `Runtime-VideoEncoder/src/videoencoder` and `addons/roxlu/addons/Youtube`
code you need to add some files and configs to your Visual Studio project so everything compiles
nice and smooth. Follow the steps below.

_General config settings_
- Add `WIN32_LEAN_AND_MEAN` to your preprocessor flags
- Change Configuration Properties > General > Character Set to **not set**  
- After adding the sqlite sources, you need to tell MSVC2012 that it's a `C` file and not a `C++`: 
  Right click the sqlite3.c file, go to 'C/C++ > Advanced > Compile As' and select "Compile as C Code"

_We need to add the following source files:_

 - roxlu/addons/log/src/*.cpp
 - roxlu/addons/sqlite/src/*.cpp
 - roxlu/addons/sqlite/extern/sqlite/*.cpp
 - roxlu/addons/uv/src/*.cpp
 - roxlu/addons/youtube/src/*.cpp
 - Runtime-VideoEncoder/src/videoencoder/src/*.cpp
 - roxlu/lib/src/*.cpp

_Add these include directories:_

````
.\..\addons\roxlu\lib\include\
.\..\Runtime-VideoEncoder\src\videoencoder\include\
.\..\addons\roxlu\extern\include\
.\..\addons\roxlu\addons\UV\include\
.\..\addons\roxlu\addons\YouTube\include\
.\..\addons\roxlu\addons\SQLite\extern\sqlite\
.\..\addons\roxlu\addons\SQLite\include\
.\..\addons\roxlu\addons\PCRE\include\
.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\curl\
.\..\addons\roxlu\addons\Log\include\
````

_As a one liner for VS_
````
.\..\addons\roxlu\lib\include\;.\..\Runtime-VideoEncoder\src\videoencoder\include\;.\..\addons\roxlu\extern\include\;.\..\addons\roxlu\addons\UV\include\;.\..\addons\roxlu\addons\YouTube\include\;.\..\addons\roxlu\addons\SQLite\extern\sqlite\;.\..\addons\roxlu\addons\SQLite\include\;.\..\addons\roxlu\addons\PCRE\include\;.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\curl\;.\..\addons\roxlu\addons\Log\include\
````

_Add these preprocessor flags_

````
WIN32_LEAN_AND_MEAN
````

_Add these linker input libraries_

````
.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\libuv.lib
.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\libcurl.lib
.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\jansson.lib
.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\pcre.lib
.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\pcrecpp.lib
ws2_32.lib
psapi.lib
iphlpapi.lib
````

_As a one liner for VS_
````
.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\libuv.lib;.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\libcurl.lib;.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\jansson.lib;.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\pcre.lib;.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\pcrecpp.lib;ws2_32.lib;psapi.lib;iphlpapi.lib;.\..\addons\roxlu\extern\lib\win\vs2010\MD\32d\msgpack.lib; 
````

_Copy these DLLs files to your bin dir_
````
addons/roxlu/extern/bin/win/vs2010/MD/32/janson.dll        
addons/roxlu/extern/bin/win/vs2010/MD/32/libcurl.dll
addons/roxlu/extern/bin/win/vs2010/MD/32/libcurl.dll
addons/roxlu/extern/bin/win/vs2010/MD/32/libeay32.dll
addons/roxlu/extern/bin/win/vs2010/MD/32/pcre.dll
addons/roxlu/extern/bin/win/vs2010/MD/32/pcrecpp.dll
````      




