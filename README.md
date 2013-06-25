Copyright (c) 2013, Branden Clark
All rights reserved.

=================         IMPORTANT         =================

See COPYRIGHT for license details.


=================         Purpose         =================

To provide a GUI for Dynamorio and it's extensions.
Currently in progress:
--Dr. Memory
--Dr. Heapstat


=================         Building         =================

--download the source
--create a build directory (in or out of source)
--build the plugins you want (from the build dir)
  --located in src/tools
  --qmake PATH_TO_/SOME_TOOL.pro
  --make
  --the plugins will automatically be placed in BUILD_DIR/tools
--build the mainWindow
  --qmake PATH_TO_/DR-GUI.pro
  --make
--run the program
  --./DR-GUI


=================         USING         =================

To load a tool look in the 'tools' menu.
For information on a specific tool check their docs.


=================         FAQ         =================

How do I make my own plugin?
--Read the documentation (soon!)

