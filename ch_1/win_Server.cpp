#include <winsock2.h>
#include <windows.h>

// Needed for Winsock2 library
#pragma comment(lib, "ws2_32.lib")

// Define message IDs
#define IDC_EDIT_IN			101
#define IDC_EDIT_OUT		102
#define IDC_SEND_MSG_BUTTON	103
#define WM_SOCKET			104

int portNumber = 5555;

HWND chatHistoryEditBox		= NULL;
HWND serverMessageWindow	= NULL;

//chatHistory is a fixed size array for demonstration purposes
char chatHistory[10000]; 
sockaddr sockAddrClient;

const int MAX_CLIENTS	= 3;
int clientNumber		= 0;
SOCKET ServerSocket		= NULL;

SOCKET Socket[MAX_CLIENTS - 1];

// Function Prototype for Windows API message handler
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Windows version of main() function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int showCmd) {
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));

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
	wClass.style			= CS_HREDRAW|CS_VREDRAW;

	// Notify OS of new application window
	if(!RegisterClassEx(&wClass)) {
		int errCode	= GetLastError();
		MessageBox(NULL,
			"Window class creation failed\r\nError code:",
			"Window Class Failed",
			MB_ICONERROR);
	}

	HWND hWnd = CreateWindowEx(NULL,
		"Window Class",			// Classname
		"Windows Chat Server",	// Title of window
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
		int errCode = GetLastError();

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
		// Respond to user inputs
		case WM_COMMAND:
			// Filter input messages
			switch(LOWORD(wParam)) {
				case IDC_SEND_MSG_BUTTON:
				{
					// Prepare buffer
					char textBuffer[1024];
					ZeroMemory(textBuffer, sizeof(textBuffer));

					SendMessage(serverMessageWindow,		// Message recipient's ID
						WM_GETTEXT,							// Type of message
						sizeof(textBuffer),					// Additional message data
						reinterpret_cast<LPARAM>(textBuffer));// Default message text

					// Broadcast message to all connected clients
					for(int i = 0; i <= clientNumber; i++) {
						send(Socket[i], textBuffer, strlen(textBuffer), 0);
					}
 
					SendMessage(serverMessageWindow,	// Message recipient's ID
						WM_SETTEXT,						// Type of message		
						NULL,							// Additional message data
						(LPARAM)"");					// Default message text
				}
				break;
			}
		break;

		// Respond to GUI creation message
		case WM_CREATE: 
		{
			ZeroMemory(chatHistory,sizeof(chatHistory));

			// Create incoming message box
			chatHistoryEditBox = CreateWindowEx(WS_EX_CLIENTEDGE,// Extended Style of window
				"EDIT",									// Classname
				"",										// Window name
				WS_CHILD|WS_VISIBLE|ES_MULTILINE|		// Pipe delimited styles
				ES_AUTOVSCROLL|ES_AUTOHSCROLL|
				ES_READONLY,
				50,										// X position
				120,									// Y position
				400,									// width
				200,									// height
				hWnd,									// parent window handle
				(HMENU)IDC_EDIT_IN,						// Type of window based on ID
				GetModuleHandle(NULL),					// Handle to the current instance
				NULL);									// Pointer passed back to this window

			if(!chatHistoryEditBox) {
				MessageBox(hWnd,							// Parent window's handler
					"Could not create incoming edit box.",	// Display Text
					"Error",								// Display Caption
					MB_OK|MB_ICONERROR);					// Error Icon Type
			}

			HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);

			SendMessage(chatHistoryEditBox,	// Message recipient's ID
					WM_SETFONT,				// Type of message
					(WPARAM)hfDefault,		// Additional message data
					MAKELPARAM(FALSE,0));	// Default message text

			// Set placeholder text
			SendMessage(chatHistoryEditBox,						// Message recipient's ID
					WM_SETTEXT,									// Type of message
					NULL,										// Additional message data
					(LPARAM)"Waiting for client to connect...");// Default message text

			// Create outgoing message box
			serverMessageWindow = CreateWindowEx(WS_EX_CLIENTEDGE,	// Extended Style of window
				"EDIT",												// Classname
				"",													// Window name
				WS_CHILD|WS_VISIBLE|ES_MULTILINE|					// Pipe delimited styles
				ES_AUTOVSCROLL|ES_AUTOHSCROLL,
				50,													// X position
				50,													// Y position
				400,												// width
				60,													// height
				hWnd,												// parent window handle
				(HMENU)IDC_EDIT_IN,									// Type of window based on ID			
				GetModuleHandle(NULL),								// Handle to the current instance
				NULL);												// Pointer passed back to this window

			if(!serverMessageWindow) {
				MessageBox(hWnd,							// Parent window's handler
					"Could not create outgoing edit box.",	// Display Text
					"Error",								// Display Caption
					MB_OK|MB_ICONERROR);					// Icon Type
			}

			SendMessage(serverMessageWindow,// Message recipient's ID
					WM_SETFONT,				// Type of message
					(WPARAM)hfDefault,		// Display Caption
					MAKELPARAM(FALSE,0));	// Error Icon Type

			// Set placeholder text
			SendMessage(serverMessageWindow,		// Message recipient's ID
					WM_SETTEXT,						// Type of message
					NULL,							// Display Caption
					(LPARAM)"Type message here...");// Default message text

			// Create a push button
			HWND hWndButton = CreateWindow( 
				"BUTTON",					// The control's classname
				"Send Message",				// Display Text
				WS_TABSTOP|WS_VISIBLE|		// Windows pre-defined styles, delimited by "|"
				WS_CHILD|BS_DEFPUSHBUTTON,
				50,							// X co-ordinate of rectangle of upper left point
				330,						// Y co-ordinate of rectangle of upper left point
				85,							// Width of rectangle
				23,							// Height of rectangle
				hWnd,						// The parent window's handle object
				(HMENU)IDC_SEND_MSG_BUTTON, // The Label's ID
				GetModuleHandle(NULL),		// The HINSTANCE of your program
				NULL);						// Pointer passed back to this window
			
			SendMessage(hWndButton,		// Message recipient's ID
				WM_SETFONT,				// Type of message
				(WPARAM)hfDefault,		// Additional message data
				MAKELPARAM(FALSE,0));	// Default message text	

			// Set up Winsock
			WSADATA WsaData;

			int errCode = WSAStartup(MAKEWORD(2,2), &WsaData);
			if(errCode != 0) {
				MessageBox(hWnd,
					"Winsock initialization failed",
					"Critical Error",
					MB_ICONERROR);

				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			// Assign TCP socket
			ServerSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
			if(ServerSocket == INVALID_SOCKET) {
				MessageBox(hWnd,
					"Socket creation failed",
					"Critical Error",
					MB_ICONERROR);

				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			SOCKADDR_IN SockAddr;
			SockAddr.sin_port			= htons(portNumber);
			SockAddr.sin_family			= AF_INET;
			SockAddr.sin_addr.s_addr	= htonl(INADDR_ANY);

			// Bind the socket to the portnum
			if(bind(ServerSocket, (LPSOCKADDR)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR)
			{
				MessageBox(hWnd, "Unable to bind socket", "Error", MB_OK);

				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			errCode = WSAAsyncSelect(ServerSocket,	// Socket being selected
					hWnd,							// Handle for the window where the socket event happened
					WM_SOCKET,						// Message Recipient ID
					(FD_CLOSE|FD_ACCEPT|FD_READ));	// List of File Descriptor events to be triggered

			if(errCode)
			{
				MessageBox(hWnd,				// Parent window's handler	
					"WSAAsyncSelect failed",	// Display Text
					"Critical Error",			// Display Caption
					MB_ICONERROR);				// Error Icon Type

				// Kill current window
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			// Listen for new clients
			if(listen(ServerSocket,SOMAXCONN) == SOCKET_ERROR)
			{
				MessageBox(hWnd,
					"Unable to listen!",
					"Error",
					MB_OK);

				// Kill current window
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}
		}
		break;

		// Handle the message sent when the window is closed
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			shutdown(ServerSocket, SD_BOTH);
			closesocket(ServerSocket);
			WSACleanup();
			return 0;
		}
		break;

		// Handle all socket messages
		case WM_SOCKET:
		{
			// Filter socket messages based on File descriptor action
			switch(WSAGETSELECTEVENT(lParam))
			{
				case FD_READ:
				{
					// Determine which client sent the message amd then broadcast the message
					for(int i = 0; i <= MAX_CLIENTS; i++) {
						char incomingString[1024];
						ZeroMemory(incomingString, sizeof(incomingString));

						// Pull the message from the socket
						int inDataLength = recv(Socket[i],
							(char*)incomingString,
							sizeof(incomingString) / sizeof(incomingString[0]),
							0);

						// Append the message to the chat history
						if(inDataLength != -1) {
							strncat_s(chatHistory, incomingString, inDataLength);
							strcat_s(chatHistory,"\r\n");
	
							SendMessage(chatHistoryEditBox,
								WM_SETTEXT,
								sizeof(incomingString) - 1,
								reinterpret_cast<LPARAM>(&chatHistory));

							for(int i = 0; i <= clientNumber; i++) {
								send(Socket[i], incomingString, strlen(incomingString), 0);
							}
						}
					}
				}
				break;

				// When a client file descriptor closes
				case FD_CLOSE:
				{
					char* clientCloseMsg = "Client has left the chat session!";

					// Append chatHistory with client departure notification
					strcat_s(chatHistory, clientCloseMsg);
					strcat_s(chatHistory, "\r\n");

					SendMessage(chatHistoryEditBox,
						WM_SETTEXT,
						sizeof(clientCloseMsg) - 1,
						reinterpret_cast<LPARAM>(&chatHistory));
				}
				break;

				// Handle message when a new client connects to the Chat server
				case FD_ACCEPT:
				{
					if(clientNumber < MAX_CLIENTS) {
						int size			= sizeof(sockaddr);
						char* newConnMsg	= "Client connected!";
						
						// Accept a client request
						Socket[clientNumber] = accept(wParam, &sockAddrClient, &size);

						if (Socket[clientNumber] == INVALID_SOCKET) {
							int nret = WSAGetLastError();
							WSACleanup();
						}

						// Append chatHistory with client acceptance notification
						strcat_s(chatHistory, newConnMsg);
						strcat_s(chatHistory, "\r\n");

						SendMessage(chatHistoryEditBox,
							WM_SETTEXT,
							sizeof(newConnMsg) - 1,
							reinterpret_cast<LPARAM>(&chatHistory));
					}

					clientNumber++;
				}
				break;
    		}   
		}
	}
    
    return DefWindowProc(hWnd, msg, wParam, lParam);
}