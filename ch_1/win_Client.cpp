#include <winsock2.h>
#include <windows.h>

// Needed for Winsock2 library
#pragma comment(lib, "ws2_32.lib")

// Define message IDs
#define IDC_EDIT_IN			101
#define IDC_EDIT_OUT		102
#define IDC_SEND_MSG_BUTTON	103
#define WM_SOCKET			104
#define IDC_STATIC_NICKNAME 105
#define IDC_EDIT_NICKNAME	106

// Define the max length of text inputs
#define BUFFERSIZE 1024

char *hostname		= "localhost";
int portNumber		= 5555;

// Handles to UI widgets
HWND nickNameLbl			= NULL;
HWND nickNameEdit			= NULL;
HWND chatHistoryEditBox		= NULL;
HWND messageEditBox			= NULL;

SOCKET sckt	= NULL;

char chatHistory[10000];

// Function Prototype for Windows API message handler
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Windows version of main() function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int showCmd) {
	WNDCLASSEX wClass;
	ZeroMemory(&wClass,sizeof(WNDCLASSEX));

	// Main application window settings
	wClass.cbClsExtra		= NULL;
	wClass.cbSize			= sizeof(WNDCLASSEX);
	wClass.cbWndExtra		= NULL;
	wClass.hbrBackground	= (HBRUSH)COLOR_WINDOW;
	wClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wClass.hIcon			= NULL;
	wClass.hIconSm			= NULL;
	wClass.hInstance		= hInstance;
	wClass.lpfnWndProc		= (WNDPROC)WinProc;
	wClass.lpszClassName	= "Window Class";
	wClass.lpszMenuName		= NULL;
	wClass.style			= CS_HREDRAW | CS_VREDRAW;

	// Notify OS of new application window
	if(!RegisterClassEx(&wClass)) {
		int errCode	= GetLastError();
		MessageBox(NULL,									// Parent window's handler
			"Window class creation failed\r\nError code:",	// Display Text
			"Window Class Failed",							// Display Caption
			MB_ICONERROR);									// Error Icon Type
	}

	// Set options for our chat client window
	HWND hWnd = CreateWindowEx(NULL,
		"Window Class",			// Classname
		"Chatroom Client",		// Title of window
		WS_OVERLAPPEDWINDOW,	// Styling
		200,					// Initial horizontal position
		200,					// Initial vertical position
		640,					// Width of window
		480,					// Height of window
		NULL,					// Handle to parent of window
		NULL,					// Handle to child of window
		hInstance,				// Handle to the current instance
		NULL);					// Pointer passed back to this window

	if(!hWnd) {
		int errCode	= GetLastError();

		MessageBox(NULL,								// Parent window's handler
			"Window creation failed\r\nError code:",	// Display Text
			"Window Creation Failed",					// Display Caption
			MB_ICONERROR);								// Error Icon Type
	}

    ShowWindow(hWnd, showCmd);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// Begin listening for messages
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

// Event handlers for messages triggered by user inputs and application responses
LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		// GUI creation message
		case WM_CREATE:
		{
			ZeroMemory(chatHistory, sizeof(chatHistory));

			// Create a label for the incoming message box
			nickNameLbl = CreateWindow(
				"STATIC",							// The static control's classname
				"Nickname:",						// Label's Text
				WS_CHILD | WS_VISIBLE | SS_LEFT,	// Windows pre-defined styles, delimited by "|"
				50,                                 // X co-ordinate of rectangle of upper left point
				20,									// Y co-ordinate of rectangle of upper left point
				70,									// Width of rectangle
				25,									// Height of rectangle
				hWnd,								// The parent window's handle object
				(HMENU) IDC_STATIC_NICKNAME,		// The Label's ID
				GetModuleHandle(NULL),				// The HINSTANCE of your program
				NULL);

			if(!nickNameLbl) {
				MessageBox(hWnd,							// Parent window's handler
					"Could not create label for edit box.", // Display Text
					"Error",								// Display Caption
					MB_OK|MB_ICONERROR);					// Error Icon Type
			}

			// Create nickname edit box
			nickNameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, // Extended Style of window
				"EDIT",										// Classname
				"",											// Window name
				WS_CHILD|WS_VISIBLE|ES_MULTILINE|			// Pipe delimited styles
				ES_AUTOVSCROLL|ES_AUTOHSCROLL,		
				125,										// X position
				17,											// y position
				200,										// width
				25,											// height
				hWnd,										// parent window handle
				(HMENU)IDC_EDIT_IN,							// Type of window based on ID
				GetModuleHandle(NULL),						// Handle to the current instance
				NULL);										// Pointer passed back to this window

			if(!nickNameLbl) {
				MessageBox(hWnd,								// Parent window's handler
					"Could not create nickname edit text box.",	// Display Text
					"Error",									// Display Caption
					MB_OK|MB_ICONERROR);						// Error Icon Type
			}	

			SendMessage(nickNameEdit,			// Message recipient's ID
				WM_SETTEXT,						// Type of message
				NULL,							// Additional message data
				(LPARAM)"Enter new nickname");	// Default message text

			// Create incoming message box
			chatHistoryEditBox = CreateWindowEx(WS_EX_CLIENTEDGE,	// Extended Style of window
				"EDIT",										// Classname
				"",											// Window name
				WS_CHILD|WS_VISIBLE|ES_MULTILINE|			// Pipe delimited styles
				ES_AUTOVSCROLL|ES_AUTOHSCROLL|
				ES_READONLY,
				50,											// X position
				120,										// y position
				400,										// width
				200,										// height
				hWnd,										// parent window handle
				(HMENU)IDC_EDIT_IN,							// Type of window based on ID
				GetModuleHandle(NULL),						// Handle to the current instance
				NULL);										// Pointer passed back to this window

			if(!chatHistoryEditBox) {
				MessageBox(hWnd,							// Parent window's handler
					"Could not create incoming edit box.",	// Display Text
					"Error",								// Display Caption
					MB_OK|MB_ICONERROR);					// Error Icon Type
			}

			HGDIOBJ defaultGdi = GetStockObject(DEFAULT_GUI_FONT);

			SendMessage(chatHistoryEditBox,		// Message recipient's ID
				WM_SETFONT,						// Type of message
				(WPARAM)defaultGdi,				// Additional message data
				MAKELPARAM(FALSE,0));			// Default message text

			SendMessage(chatHistoryEditBox,						// Message recipient's ID
				WM_SETTEXT,										// Type of message
				NULL,											// Additional message data
				(LPARAM)"Attempting to connect to server...");	// Default message text

			// Create outgoing message box
			messageEditBox = CreateWindowEx(WS_EX_CLIENTEDGE,	// Extended Style of window
				"EDIT",										// Classname
				"",											// Window name
				WS_CHILD|WS_VISIBLE|ES_MULTILINE|			// Pipe delimited styles
				ES_AUTOVSCROLL|ES_AUTOHSCROLL,
				50,											// X position
				50,											// Y position
				400,										// width
				60,											// height
				hWnd,										// parent window handle
				(HMENU)IDC_EDIT_OUT,						// Type of window based on ID
				GetModuleHandle(NULL),						// Handle to the current instance
				NULL);										// Pointer passed back to this window

			if(!messageEditBox) {
				MessageBox(hWnd,							// Parent window's handler
					"Could not create outgoing edit box.",	// Display Text
					"Error",								// Display Caption
					MB_OK|MB_ICONERROR);					// Error Icon Type
			}

			SendMessage(messageEditBox,	// Message recipient's ID
				WM_SETFONT,				// Type of message
				(WPARAM)defaultGdi,		// Additional message data
				MAKELPARAM(FALSE,0));	// Default message text	

			SendMessage(messageEditBox,			// Message recipient's ID
				WM_SETTEXT,						// Type of message
				NULL,							// Additional message data
				(LPARAM)"Type message here...");// Default message text

			// Create a send message button
			HWND hWndButton = CreateWindow( 
				"BUTTON",					// The control's classname
				"Send Message",				// Text
				WS_TABSTOP|WS_VISIBLE|		// Windows pre-defined styles, delimited by "|"
				WS_CHILD|BS_DEFPUSHBUTTON,	
				50,							// X co-ordinate of rectangle of upper left point
				330,						// Y co-ordinate of rectangle of upper left point
				85,							// Width of rectangle
				23,							// Height of rectangle
				hWnd,						// The parent window's handle object
				(HMENU)IDC_SEND_MSG_BUTTON,	// The Label's ID
				GetModuleHandle(NULL),		// The HINSTANCE of your program
				NULL);						// Pointer passed back to this window
			
			SendMessage(hWndButton,		// Message recipient's ID
				WM_SETFONT,				// Type of message
				(WPARAM)defaultGdi,		// Additional message data
				MAKELPARAM(FALSE,0));	// Default message text	

			// Set up Winsock
			WSADATA WsaData;

			// Initialize Windows socket API
			int errCode = WSAStartup(MAKEWORD(2,2), &WsaData);
			if(errCode != 0) {
				MessageBox(hWnd,						// Parent window's handler
					"Winsock initialization failed",	// Display Text
					"Critical Error",					// Display Caption
					MB_ICONERROR);						// Error Icon Type

				// Kill current window
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			// Create TCP socket
			sckt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(sckt == INVALID_SOCKET) {
				MessageBox(hWnd,				// Parent window's handler
					"Socket creation failed",	// Display Text
					"Critical Error",			// Display Caption
					MB_ICONERROR);				// Error Icon Type

				// Kill current window
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			// Check socket for activity
			errCode = WSAAsyncSelect(sckt, hWnd, WM_SOCKET, (FD_CLOSE|FD_READ));
			if(errCode) {
				MessageBox(hWnd,				// Parent window's handler
					"WSAAsyncSelect failed",	// Display Text
					"Critical Error",			// Display Caption
					MB_ICONERROR);				// Error Icon Type

				// Kill current window
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			// Resolve IP address for hostname
			struct hostent *host;
			if((host = gethostbyname(hostname)) == NULL) {
				MessageBox(hWnd,					// Parent window's handler
					"Unable to resolve host name",	// Display Text
					"Critical Error",				// Display Caption
					MB_ICONERROR);					// Error Icon Type

				// Kill current window
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			// Set up our Socket address structure
			SOCKADDR_IN SockAddr;
			SockAddr.sin_port = htons(portNumber);
			SockAddr.sin_family = AF_INET;
			SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

			connect(sckt, (LPSOCKADDR)(&SockAddr), sizeof(SockAddr));
		}
		break;

		// Respond to GUI user input
		case WM_COMMAND:
			// Filter input message
			switch(LOWORD(wParam)) {
				// Message when the user clicks "Send"
				case IDC_SEND_MSG_BUTTON:
				{
					char editOutBuffer[BUFFERSIZE];
					char nickNameBuffer[BUFFERSIZE];

					ZeroMemory(editOutBuffer, sizeof(editOutBuffer));
					ZeroMemory(nickNameBuffer, sizeof(nickNameBuffer));

					// Get Nickname 
					SendMessage(nickNameEdit,						// Message recipient's ID
						WM_GETTEXT,									// Type of message
						sizeof(nickNameBuffer),						// Additional message data
						reinterpret_cast<LPARAM>(nickNameBuffer));	// Default message text	
					
					// Append ':' so users can tell who sent the message
					strncat_s(nickNameBuffer, ": ", 2);		

					// Capture the message content
					SendMessage(messageEditBox,						// Message recipient's ID
						WM_GETTEXT,									// Type of message
						sizeof(editOutBuffer),						// Additional message data
						reinterpret_cast<LPARAM>(editOutBuffer));	// Default message text	
					
					// Create the message string from nickname and the content
					strncat_s(nickNameBuffer, editOutBuffer, strlen(editOutBuffer));

					send(sckt, nickNameBuffer, strlen(nickNameBuffer),0);

					// Clear out the message box
					SendMessage(messageEditBox,	// Message recipient's ID
						WM_SETTEXT,			// Type of message
						NULL,				// Additional message data
						(LPARAM)"");		// Default message text	
				}
				break;
			}
			break;

		// Respond to application closing message
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			shutdown(sckt, SD_BOTH);
			closesocket(sckt);
			WSACleanup();
			return 0;
		}
		break;

		// Respond to messages about the TCP socket
		case WM_SOCKET:
		{
			// Check for socket errors before doing anything else
			if(WSAGETSELECTERROR(lParam)) {
				// Display error
				MessageBox(hWnd,					// Parent window's handler
					"Connection to server failed",	// Display Text
					"Error",						// Display Caption
					MB_OK|MB_ICONERROR);			// Error Icon Type

				// Shutdown
				SendMessage(hWnd,	// Message recipient's ID
					WM_DESTROY,		// Type of message	
					NULL,			// Additional message data
					NULL);			// Default message text

				break;
			}

			// Filter File Descriptor messages
			switch(WSAGETSELECTEVENT(lParam)) {
				case FD_READ:
				{
					// Prepare array for new input
					char incomingMsg[1024];
					ZeroMemory(incomingMsg, sizeof(incomingMsg));

					// Read from socket and dump its contents into the array
					int msgLength = recv(sckt,
						(char*)incomingMsg,
						sizeof(incomingMsg) / sizeof(incomingMsg[0]),
						0);

					// Append to the chat history
					strncat_s(chatHistory, incomingMsg, msgLength);
					strcat_s(chatHistory, "\r\n");

					// Display the message on the GUI
					SendMessage(chatHistoryEditBox,					// Message recipient's ID
						WM_SETTEXT,									// Type of message
						sizeof(incomingMsg) - 1,					// Additional message data
						reinterpret_cast<LPARAM>(&chatHistory));	// Default message text
				}
				break;

				// If the server disconnects
				case FD_CLOSE:
				{
					MessageBox(hWnd,				// Message recipient's ID
						"Server closed connection", // Type of message
						"Connection closed!",		// Additional message data
						MB_ICONINFORMATION|MB_OK);	// Default message text

					closesocket(sckt);

					SendMessage(hWnd,	// Message recipient's ID
						WM_DESTROY,		// Type of message
						NULL,			// Additional message data
						NULL);			// Default message text
				}
				break;
			}
		} 
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}