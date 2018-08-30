#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#if defined(__unix__)||defined(__APPLE__)||defined(__linux__)
	#include <dirent.h>
	#include <errno.h>
	#include <fcntl.h> 
	#include <termios.h>
	#include <unistd.h>
#endif

#ifdef __WIN32__
	#include <windows.h>
#endif

char **listSerialDevices(char **devices, int *deviceCount);
int createSerialConnection(char *name, int baudrate);
void destroySerialConnection(void); 
int sendMessage(int red, int green, int blue, int instruction, int duration, int argument);
void messageWatcher(void);

extern char watcherActive;
