README FILE: Chapter 1 Source Code
==================================

## Files Listing & Description
  
  - mac_linux_Client.c

  		Is a C source code file which creates the client side binary of the chat application.
  		Used in conjunction with mac_linux_Server.c.
  
  - mac_linux_Server.c

  		Is a C source code file which creates the server side binary fo the chat appliation 
  		Used in conjunction with mac_linux_Client.c.

  - win_Client.cpp

  		C++ source code file which creates the client side Windows executable of the chat application.
  		Used in conjunction with win_Server.cpp.

  - win_Server.cpp

  		C++ source code file which creates the server side Windows executable of the chat application.
  		Used in conjunction with win_Client.cpp.

 ## How To Build And Run

 	- Linux Operating systems

 		1. Copy the source files mac_linux_Client.c and mac_linux_Server.c to a directory.
 		2. Open a program called "Terminal".
 		3. Use the change directory command "cd" until you reach the directory where the files were placed in step (1).
 		4. Enter the command "gcc mac_linux_Client.c -o Client"
 		5. Enter the command "gcc mac_linux_Server.c -o Server"
 		6. Open a new tab within the terminal application.
 		7. In the new terminal tab run the command "./Server" this will start the server.
 		8. In the next terminal tab run the command "./Client" this will start the client.
 		9. Open a third tab and repeat (8).

 	- Mac OS X

 		0. Obtain the "Command Line Tools for Xcode" this will vary depending on your version of Mac OS X.
 		1. Copy the source files mac_linux_Client.c and mac_linux_Server.c to a directory.
 		2. Open a program called "Terminal".
 		3. Use the change directory command "cd" until you reach the directory where the files were placed in step (1).
 		4. Enter the command "clang mac_linux_Client.c -o Client"
 		5. Enter the command "clang mac_linux_Server.c -o Server"
 		6. Open a new tab within the terminal application.
 		7. In the new terminal tab run the command "./Server" this will start the server.
 		8. In the next terminal tab run the command "./Client" this will start the client.
 		9. Open a third tab and repeat (8).

 	- Windows Versions > Windows 2000

 		0. Obtain a copy of either Visual Studio 2010 or Visual Studio 2012 express for desktop.
 		1. Install Visual Studio once you have downloaded the installation files from Microsoft's Visual studio website.
 		2. Create an empty C++ project.
 		3. Create a new source file and name it main.cpp
 		4. Copy paste the code from win_Server.cpp
 		5. Press F7 to build the project.
 		6. Repeat steps (2 -> 5) but this time for win_Client.cpp
 		7. At this point you may navigate to the projects folder from the file Explorer and 
 			run the .exe files or open each project and press Ctrl+F5 which will start the app.
