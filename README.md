# EFI-Script to disable a defective memory area before booting macOS
Tested on MacBook Pro Late 2013 on MacOS Big Sur.

ToDos (Contributions welcome!):
 - Testing with newer devices and newer macOs Versions
 - Add a few seconds delay to actually see the script executing (exits too fast to see anything)
 - Provide better documentation (how to compile)
 - Cleaner Code

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/G2G2GIZ3R)  

If you expierience Hangs of MacOS which are related to faulty memory or get the following errors, this script might help you to mark a certain area of your memory as defective so that MacOS never tries to access it during runtime:  
PPMOO2, PPMOO3, PPMOO4 --> "There may be an issue with the onboard memory."

This script works similar to the gnu grub **"badram"** command.  
You can use tools like **memtest86+** to find the exact location and size of the area you have to disable.  
You can use tools like **Refind** to execute the compiled script before booting MacOS.
