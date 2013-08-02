# Test_YouTubeClientIPC

Quick 'n dirty test application that writes frames from webcam to 
disc, then encodes them, adds audio (all through ipc, using Runtime-VideoEncoder and
Runtime-YouTube). The frames are saved in a separate thread; the threaded writer should
implement the task-paradigm which would make it way cleanier to shutdown the 
thread. 
