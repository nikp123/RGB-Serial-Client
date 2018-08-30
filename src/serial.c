#include "serial.h"

int temp, fd;
char watcherActive;
#ifdef __WIN32__
HANDLE hComm;
#endif

char **listSerialDevices(char **devices, int *deviceCount) {
	// this prevents crashes
	*deviceCount = 0;
	#if defined(__linux__)||defined(__APPLE__)
		DIR *d;
		struct dirent *dir;
		#ifdef __linux__
			d = opendir("/sys/class/tty/");
		#elif defined(__APPLE__)
			d = opendir("/dev/");
		#endif
		if (d) {
			devices = (char **)malloc(sizeof(char*));
			while((dir = readdir(d)) != NULL) {
				if(dir->d_name[0]=='.'||sscanf(dir->d_name, "tty%d", &temp)) continue; // because you know fucking why
				#ifdef __APPLE__
					if(strncmp(dir->d_name, "tty.", 4)) continue;
				#endif
				(*deviceCount)++;
				char **newDeviceList = (char**)realloc(devices, (*deviceCount)*sizeof(char*)+1);
				devices = newDeviceList;
				devices[(*deviceCount)-1] = (char*)malloc(strlen(dir->d_name)*sizeof(char)+1);
				strcpy(devices[(*deviceCount)-1], dir->d_name);
			}
			closedir(d);
		}
	#endif	
	#ifdef __WIN32__
	char device[11];
	// since windows has no "OFFICIAL" method of finding COM ports we have to do it like this
	devices = (char **)malloc(sizeof(char*));
	for(int i=0; i<128; i++) { // it's fucking disgusting, I know
		sprintf(device, "\\\\.\\COM%d\0", i);
		hComm = CreateFile(device, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hComm == INVALID_HANDLE_VALUE) continue;

		// successful probe here YAAAAY
		CloseHandle(hComm);
		(*deviceCount)++;
		char **newDeviceList = (char**)realloc(devices, (*deviceCount)*sizeof(char*)+1);
		devices = newDeviceList;
		devices[(*deviceCount)-1] = (char*)malloc(8*sizeof(char)+1);
		sprintf(devices[(*deviceCount)-1], "COM%d\0", i);
	}
	#endif	
	return devices;
}

#if defined(__linux__)||defined(__APPLE__)||defined(__unix__)
int set_interface_attribs (int fd, int speed, int parity) {
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0) {
		fprintf(stderr, "Error %d from tcgetattr\n", errno);
		return -1;
	}
	
	cfsetospeed (&tty, speed);
	cfsetispeed (&tty, speed);
	
	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // disable break processing
	tty.c_lflag = 0;                // no signaling chars, no echo,
	                                // no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN]  = 0;            // read doesn't block
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
	
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
	
	tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
	                                // enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;
	
	if (tcsetattr (fd, TCSANOW, &tty) != 0) {
		fprintf(stderr, "Error %d from tcsetattr\n", errno);
		return -1;
	}
	return 0;
}


int set_blocking (int fd, int should_block) {
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0) {
		fprintf(stderr, "Error %d from tggetattr\n", errno);
		return 1;
	}

	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
	
	if (tcsetattr (fd, TCSANOW, &tty) != 0) {
		fprintf(stderr, "Error %d setting term attributes\n", errno);
		return 1;
	}
	return 0;
}
#endif

int createSerialConnection(char *name, int baudrate) {	
	#if defined(__linux__)||defined(__APPLE__)||defined(__unix__)
	char *portname = (char*)malloc(sizeof(char)*(strlen(name)+6));
	strcpy(portname, "/dev/");
	strcat(portname, name);
	fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd<0) {
		fprintf(stderr, "Unable to connect to %s! (status=%d)\n", portname, errno);
		return 1;
	}
	if(set_interface_attribs(fd, baudrate, 0)||set_blocking(fd, 0)) return 1;
	free(portname);
	#endif
	#ifdef __WIN32__
	char *portname = (char*)malloc(sizeof(char)*(strlen(name)+8));
	strcpy(portname, "\\\\.\\");
	strcat(portname, name);
	hComm = CreateFile(portname, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hComm == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Error opening %s\n", portname);
		return 1;
	}
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	GetCommState(hComm, &dcbSerialParams);
	dcbSerialParams.BaudRate = baudrate;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.BaudRate = NOPARITY;
	dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;
	SetCommState(hComm, &dcbSerialParams);
	#endif
	return 0;
}

void destroySerialConnection(void) {
	#if defined(__linux__)||defined(__APPLE__)||defined(__unix__)
	close(fd);
	#endif
	#ifdef __WIN32__
	CloseHandle(hComm);
	#endif
}

int sendMessage(int red, int green, int blue, int instruction, int duration, int argument) {
	#if defined(__linux__)||defined(__APPLE__)||defined(__unix__)
	char messageString[100];
	sprintf(messageString, "%d,%d,%d,%d,%d,%d\n", red, green, blue, instruction, duration, argument);
	if(write(fd, messageString, strlen(messageString)) < 0) {
		fprintf(stderr, "Error sending message to %d (error=%d)\n", fd, errno);
		return 1;
	}
	#endif
	#ifdef __WIN32__
	char messageString[100];
	sprintf(messageString, "%d,%d,%d,%d,%d,%d\n", red, green, blue, instruction, duration, argument);
	if(!WriteFile(hComm, messageString, sizeof(char)*strlen(messageString), &temp, 0)) {
		fprintf(stderr, "Error sending message!\n");
		return 1;
	}
	#endif
	return 0;
}

void messageWatcher(void){
	#if defined(__linux__)||defined(__APPLE__)||defined(__unix__)
	while(watcherActive) {
		char buf[100];
		int n = read(fd, buf, sizeof buf); 
		puts(buf);
	}
	#endif
	/**
	 *	Port this on your own, I'm lazy so I won't.
	 **/
	return;
}
