Concurrent Remote Multicursor Computing Environment
===========
A solution to provide a Linux computing environment with concurrent, remote virtual multicursor support. Each virtual cursor is represented by its own window. A virtual cursor can claim ownership to any window by clicking on it and will retain ownership until its connection is terminated. Only the owner of a window can interact with the owned window; all other mouse events will be ignored. Outside of ownership, everything functions as a normal mouse would.

**This build is a prototype and is NOT ready for serious use!**

This build is functional and has been tested on Ubuntu 12.04 LTS using Unity/LightDM.

**To do:**

1. Allow virtual cursors to relinquish ownership of a window by entering a specific key/mouse combination.
2. Change the drag initiation combination to be Shift + RightClick instead of just RightClick.
3. Implement Motif support as per the spec.
4. Implement proper logging instead of just printing to the console.
5. Audit the entire code base for memory leaks and unchecked errors.

