# popcorn
A monolithic C/C++ kernel for i386 processors. 

`/bin/` -- The executable binaries. 
* `popcorn-alpha` is the bleeding-edge binary where I'm testing little details. It usually doesn't work when I'm doing heavy tweaks.
* `popcorn-beta` is the functioning prototype that should usually stay functional, while still having the new features I'm trying to implement. 
* `popcorn-stable` is the latest, most functional binary. It also functions as a backup of sorts.

`/obj/` -- Un-linked object code files

`/src/` -- the actual buildable source

`build.sh` -- my custom Makefile-like builder. Currently it is specific to this project, but it might be useful elsewhere. 
