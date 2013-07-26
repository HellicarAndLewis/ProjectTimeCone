ProjectTimeCone
===============

An interactive installation that use multiple cameras to create a bullet-time like effect using frame interpolation.

### Setup development environment Windows (7, 8) 

- Download [openFrameworks 0.7.4](http://www.openframeworks.cc/download) for 
  [win vs2010](http://www.openframeworks.cc/versions/v0.7.4/of_v0.7.4_vs2010_release.zip)
  or [mac](http://www.openframeworks.cc/versions/v0.7.4/of_v0.7.4_osx_release.zip), 
  and extract it to: `c:\projects\ProjectTimeCone`

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

### Setup Runtime-VideoEncoer

- [Download the nightly of libav (from 2013.07.24).](http://win32.libav.org/win32/libav-win32-20130724.7z)
- After extracting the package copy **all the dll files** and the **avconv.exe** to `c:\projects\ProjectTimeCone\apps\ProjectTimeCone\Runtime-VideoEncoder\bin\avconv\win\`
- To compile, open a VS2010 terminal, go to `Runtime-VideoEncoder/build/cmake/` and execute `build_release.bat` or `build_debug.bat`


### Setup Runtime-YouTube

- Create a Google Application:
  - Go to [Google API Console](https://code.google.com/apis/console) and create a new project. [See this page](https://developers.google.com/youtube/registering_an_application) for more information on how to do that (just 2 steps)
  - Make sure that you enable the **YouTube Data API v3** in the servics pane.
  - Click on the **API Access** menu item, then on that big blue button **Create an oAuth client ID...**, create an `installed application`
- Go to `c:\projects\ProjectTimeCone\apps\ProjectTimeCone\Runtime-YouTube\bin\data` and copy `youtube_example.cfg` to `youtube.cfg`
- After you've created a YouTube application, open the `youtube.cfg` file and copy the `client ID` and `client SECRET` into this file. Next step is to authorize this application by an specifc google account. 
- Open `apps\ProjectTimeCone\addons\roxlu\addons\YouTube\html\index.html` in Chrome, paste the `client ID` into the field and press `GO`. Accept the permission popup, and copy the `auth-code` you get into `youtube.cfg`.
- To compile, open a VS2010 terminal, go to `Runtime-YouTube/build/cmake/` and execute `build_release.bat` or `build_debug.bat`


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

### Configuring a project with some shared addons

_Add these include directories_

````
.\..\addons\roxlu\lib\include\
.\..\Runtime-VideoEncoder\src\videoencoder\include\
.\..\addons\roxlu\extern\include\
.\..\addons\roxlu\addons\UV\include\
.\..\addons\roxlu\addons\YouTube\include\
.\..\addons\roxlu\addons\SQLite\extern\sqlite\
.\..\addons\roxlu\extern\lib\win\vs2010\MD\32\curl\
.\..\addons\roxlu\addons\SQLite\include\
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
ws2_32.lib
psapi.lib
iphlpapi.lib
.\..\addons\roxlu\extern\lib\win\vs2010\MD\32d\msgpack.lib
 
````




