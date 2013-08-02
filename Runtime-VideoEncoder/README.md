# VideoEncoder

This lib/app can be used to use a service based interface to [avconv](http://www.libav.org) to 
create and manipulate video. The video encoding is done in a separate process to which you 
communicate through a named pipe or unix domain socket. On windows we install a service.

