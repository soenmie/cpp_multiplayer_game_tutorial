README FILE: Chapter 2 Source Code
==================================

## Files Listing & Description

	- udp_echo_server.c

		A C source file which creates a UDP socket application that will resend all text message sent to it.

	- udp_echo_client.c

		C code which establishes a UDP socket and accepts user keyboard input. When a new message is entered
		it writes it to the UDP socket and displays the return message on screen.

## How To Build And Run

	### Windows
	
	1. Obtain a copy of either Visual Studio 2012 or the free Visual Studio 2012 Express for Desktop
	2. Install Visual Studio 2012
	3. Once installed create a new Project and call it "UdpEchoClient"
	4. From the project templates choose "Empty C++"
	5. Copy the contents of the file "udp_echo_server.c"
	6. Press Ctrl+A to select all the text
	7. Hold down the Ctrl key, press "k" then press "f" the result will format the source code with the proper indentation
	8. Repeat steps (3) -> (7) but call the project "UdpEchoServer" and copy the contents of the file "udp_echo_server.c"

	### Unix/Linux
	1. Open the terminal application
	2. Type "cd " this will redirect to the home directory of the current user
	3. Type "mkdir UdpEchoCode" to create a new folder
	4. Copy the files "udp_echo_server.c" and "udp_echo_client.c" to the directory created in step (3)
	5. Type "cd ~/UdpEchoCode" this will move you into the UdpEchoCode folder
	6. Type "gcc udp_echo_server.c -o udpserver"
	7. Type "gcc udp_echo_client.c -o udpclient"
	8. To run the server type "./udpserver"
	9. Open a new terminal tab
	10. Type "./udpclient" to run the client, repeat steps (9) -> (10) to test using multiple clients

	
## Files Listing & Description

	- boostChatServer.cpp

		Updated version of the chat server code from chapter one re-written using the Boost.Asio library.
	
	- boostChatClient.cpp
	
		Updated version of the chat client code from chapter one re-written using the Boost.Asio library.

## How To Build And Run

	1. Obtain a copy of the Boost C++ libraries http://www.boost.org/users/history/version_1_53_0.html
	2. Once the compressed file has been downloaded place it within a new folder or directory and unzip or untar it
	3. Read the instructions carefully at http://www.boost.org/doc/libs/1_53_0/more/getting_started/windows.html
		- Or http://www.boost.org/doc/libs/1_53_0/more/getting_started/unix-variants.html
		- If stuck be sure to browse Boost installation question on the popular website StackOverflow 
		- http://stackoverflow.com/search?q=%5Bboost%5D+install
	4. In a nutshell you do the following
		0) On Windows open command prompt or if using Unix/Linux open the terminal application
		1) Change directories until you get to the folder where you unzipped or untarred Boost
		2) There are two files, one called bootstrap.bat and the other called boostrap.sh
		3) Windows users will run bootstrap.bat which is a Windows Batch script
			- Unix/Linux users will type "sh bootstrap.sh" and run the shell script
		4) Wait until bootstrap has finished
		5) There will be a new file produced called "b2"
			- On Windows from the command prompt use "start b2.exe"
			- Unix/Linux users will just type "./b2"
		6) Once b2 finishes your system will have all the Boost static library files

		### Windows

		7) For Windows you'll need to setup Visual Studio 2012 
			- Create a new project
			- Name the project BoostChatServer
			- Create a new source C++ file, call it whatever you want but I suggest "main.cpp"
			- From the solution explorer right click the project name it will be in bold font
			- When the right click menu displays click "properties" this will take you to the Property Pages
			- Configuration Properties -> C/C++ -> Additional Include Directories
				- Click the black arrow and when the dialog menu displays navigate to the boost_1_53_0 folder
			- Configuration Properties -> Linker -> Additional Library Directories
				- Click the black arrow and when the dialog menu displays navigate to the boost_1_53_0\stage\lib folder
			- Click Apply
		8) Perform step (7) again but replace BoostChatServer with BoostChatClient
		9) Copy the boostChatServer.cpp code to main.cpp of the BoostChatServer project
		10) Build the project and Run it afterwards
		11) Copy the boostChatClient.cpp code to main.cpp of the BoostChatClient project
		12) Build the project and Run it afterwards

		### Unix/Linux
		
		13) On Unix/Linux
			- From the terminal run the following commands

			g++ boostChatClient.cpp -l boost_system -l boost_date_time -l boost_thread -std=c++0x -o client

			g++ boostChatServer.cpp -l boost_system -l boost_date_time -l boost_thread -std=c++0x -o server

			- If your Mac OS X machine gives you any problems try using Clang

			clang++ boostChatClient.cpp -l boost_system -l boost_date_time -l boost_thread -std=c++0x -o client

			clang++ boostChatServer.cpp -l boost_system -l boost_date_time -l boost_thread -std=c++0x -o server

			- In addition Mac OS X requires an installation of Xcode from the AppStore in order to use g++ or clang++ from the terminal