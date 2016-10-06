


# Overview

This software is a variant of the malloc subsystem of GNU's Standard C
library (GLibc).  It separates the metadata from the client memory for
increased security.  The software currently targets the x86_64 linux
architecture and has been tested mainly on Ubuntu 14.04.


#  Building

Typing `make` at the toplevel should build an entire installation
of Glibc with our modifications in place. 

This will:

* download the glibc source in `./build/glibc` and checkout XXX
* configure the build in `./build/glibc-build`
* compile and install in `./build/glibc-install`


# Testing 

A very basic test can be done by doing `make check` at the top level.

### Using the testrun.sh script to test applications.

* The simple case. 
```
./build/glibc-build/testrun.sh /bin/echo "Boo!"
```
should run the `/bin/echo` program with argument `"Boo!"` using the
built GLibc, in particular using SRI's malloc. This works as
long as your binary does not require an other library than GLibc.

* The not so simple case.

If your executable relies on other dynamic libraries, then they will
need to be linked/copied into the `./build/glibc-build` area. 

Example:
```
cd ./build/glibc-build

ln -s /lib/x86_64-linux-gnu/libselinux.so.1 .
ln -s /lib/x86_64-linux-gnu/libacl.so.1 .
ln -s /lib/x86_64-linux-gnu/libpcre.so.3 .
ln -s /lib/x86_64-linux-gnu/libattr.so.1 .

cd ../../

./build/glibc-build/testrun.sh /bin/ls
```



### Using the mhooks and replay programs to debug scenarios.

### Using gdb ...


# Design Details

We have attempted to make as few changes to the underlying
glibc/ptmalloc/dlmalloc algorithms in order to achieve our
goal.

The metadata for a client pointer is contained in a per arena hash table.
Access to this table is protected by the same lock that protects access
to other aspects of the arena (such as the bins etc). So no additional
synchronization overhead is incurred in accessing a pointer's metadata,
once the pointer's arena has been established.
The per arena hash table is an implementation of Dynamic hashing
by Per-Ake Larson (CACM April 1988 pp 446-457), supported underneath
by a custom pool allocator that relies on mmapped regions.

Determining which arena a client pointer belongs to is done by
a *lock-free* algorithm that keeps track of the underlying
regions that are under our control.



Pointers to the files in question?

Maintaining the important glibc invariant

Mmapped memory also has metadata, which we store in the main arena.

Chunks no longer overlap.

Minimum size increased so as to avoid messing with the fenceposts.

Psmalloc?

# Testing Regime


# Performance Measurements


# Possible Improvements


# Acknowledgements

This document summarizes the research performed under Darpa Contract
Number N66001-15-C-4061 by SRI International, and presents the
project's results. The project started in August 2015 and was
completed in August 2016. The Principal Investigator for this project
was Drew Dean, until his departure in July 2016. Ian A. Mason took
over as PI after Drew Dean left. The co-investigators were Bruno
Dutertre (SRI) and Dan Wallach (Rice University).
