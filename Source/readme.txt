Readme.txt
WoTMud v01.01b

<01.01>
	Added new Webserver class
	Added linking and several fixes to the magic system
	Changed chargen to support magic system changes
	Updated version number
	Updated credits



Beta Note: <01.00>

	The goal of this project was to develop a mud that closely resembled
Robert Jordan's Wheel of Time fantasy series.  I believe we've accomplished
that, if not perfectly, well enough to allow the players to easily immerse
themselves in what I believe to be the vision of Robert Jordan.
	
	Disclaimer: When I started this project, I knew very little (if any,
really) C. A large part of the reason for tackling this project was to learn
C and developing in linux.  This code is NOT bug free, (nor anywhere near
it), nor is it ideally developed. There are many, many design decisions that
I would love to change and redo, if only I had the time...


Notes to developers:

Setting up your MSVC++ project

Using Visual C++ 6 or greater...

	The Visual C project was created with some defaults in mind.  The
default project expects you to have installed the wotmud project into a
folder called "projects\wotmud" on your C drive. If you install to a
different location, you'll have to affect a few changes;

In "Project Settings", under both the Debug, and Release settings, change;

  1: Under debug, set the "Executable" and "Working directory" settings to
     point to wherever you installed the project.

  2: Under "Post-Build" step change the destination directory to reflect
  your installation path.

Using Linux 

  Unzip the archive wherever you like, and run the ./configure script.
	 
	 
Compile at will.


Logging in for the first time

  You'll have to create a character. This character will end up as level 1.
To change his level, quit out when your done creating him, find his pfile in
the data\players directories, load the pfile up with a text editor and
change the "Levl" field to 60 (max level). Save, then when you log back in,
you'll have administrator priveledges and you can test to your hearts
desire.

	
Known issues

	A Crypt() function needs to be written for the Visual C port.
	Ideally, one that is compatible with the Linux version. Currently,
	the windows port stores passwords in unencrypted ASCII format.

	Treesinging is broken.

	Maneuvers is unfinished.

	Sort helps is not sorting properly.

	This code was originally (and still) written for Linux. Linux and
	Microsoft see things a bit differently when it comes to formatting. 
	While I've attempted to clean up the code formatting as much as
	possible for this port, I'm sure your still going to find just plain
	ugly code. This is why. It looks just fine in linux. ;)


Send any code changes or questions to the Wheel of Time mailing list at:
wotmud@egroups.com (To subscribe to this mailing list, goto the url:
http://www.egroups.com/subscribe/WoTMud)

You can find the latest versions of the code at:
http://www.egroups.com/files/WoTMud/


