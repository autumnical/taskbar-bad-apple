<img width="643" height="128" alt="image" src="https://github.com/user-attachments/assets/8f192764-d477-45db-b5a3-d424af12a99a" />

# 

# stats
cpu: 0.1%

ram: 1.6 mb

audio: plays via .wav

rendering: 32x32

behavior: closes automatically when the video/song finishes

# how it works:

instead of using a heavy library, it's just reading raw bytes from the exe resources. the taskbar icon updates in real-time by creating a new icon handle every few frames. since it's just black and white silhouettes, 32x32 is actually enough to see what's going on, and your pattern recognition helps too.

# warnings:

windows smartscreen will probably scream at you because the code uses low-level stuff without a digital signature. it's not malware, it's just old-school code that modern windows doesn't recognize.

# build command:

```windres resources.rc -O coff -o resources.res```

```gcc main.cpp resources.res -o BadApple.exe -lwinmm -mwindows```

# 


![01-01-2026](https://github.com/user-attachments/assets/658ae57b-b219-441a-8194-3169a6465ca7)
