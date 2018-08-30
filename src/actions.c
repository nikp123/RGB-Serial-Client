#include <gtk-3.0/gtk/gtk.h>
#include "serial.h"

#define CONNECTED_MESSAGE "Connected :D"
#define DISCONNECTED_MESSAGE "Disconnected!"
#define FIND_DEVICE_MESSAGE "Find the serial to connect to!"

GtkWidget *window, *mainBox;
GtkWidget *colorBox, *connectBox, *colorButtons[3], *serialBox;

GtkWidget *slider[2];

GtkWidget *connectText, *colorText; 
GtkWidget *serialSelection;
GtkWidget *reset, *aesthetic, *connectTrigger, *randomFade, *blink, *fadeBlink, *rainbow, *abortOperation;
GtkWidget *hsvWheel;

pthread_t messageThread;

#if defined(__linux__)||defined(__APPLE__)||defined(__unix__)
struct timeval newTime, oldTime;
#endif

#ifdef __WIN32__
SYSTEMTIME newTime, oldTime;
#endif

char connected = 0, doing = 0, additional = 50;
short duration = 1000;

unsigned char colors[3] = {0, 0, 0};

void destroy_window(GtkWidget *widget, gpointer data) {
	if(connected) destroySerialConnection();
	gtk_main_quit();
}

void connecttodevice(GtkWidget *widget, gpointer data) {
	if(connected) {
		/**watcherActive = 0;
		if(pthread_join(messageThread, NULL)) {
			fprintf(stderr, "Error joining thread!\n");
			exit(EXIT_FAILURE);
		}**/
		destroySerialConnection();	

		connected--;

		// set up the UI stuff
		gtk_button_set_label(GTK_BUTTON(connectTrigger), "Connect");	
		gtk_label_set_text(GTK_LABEL(colorText), DISCONNECTED_MESSAGE);	
		gtk_label_set_text(GTK_LABEL(connectText), FIND_DEVICE_MESSAGE);	
		gtk_widget_set_sensitive(colorBox, connected);
		gtk_widget_set_sensitive(serialSelection, !connected);
	} else {
		if(createSerialConnection(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(serialSelection)), 9600))
			return;
		/**watcherActive = 1;
		if(pthread_create(&messageThread, NULL, messageWatcher, NULL)) {
			fprintf(stderr, "Error creating thread!\n");
			destroySerialConnection();
			return;
		}**/
			
		connected++;
		doing = 0;
		#if defined(__linux__)||defined(__unix__)||defined(__APPLE__)
		gettimeofday(oldTime, NULL);
		#endif
		#ifdef __WIN32__
		GetLocalTime(&oldTime);
		#endif
		
		// set up the UI stuff	
		gtk_button_set_label(GTK_BUTTON(connectTrigger), "Disconnect");	
		gtk_label_set_text(GTK_LABEL(colorText), CONNECTED_MESSAGE);	
		gtk_label_set_text(GTK_LABEL(connectText), CONNECTED_MESSAGE);	
		gtk_widget_set_sensitive(colorBox, connected);
		gtk_widget_set_sensitive(serialSelection, !connected);
	}
}

void waitForCompletion() {
	#if defined(__linux__)||defined(__APPLE__)||defined(__unix__)
	gettimeofday(newTime, NULL);
	usleep(labs(newTime.tv_usec-oldTime.tv_usec) < 100000 ? 100000 - labs(newTime.tv_usec-oldTime.tv_usec) : 0);
	gettimeofday(oldTime, NULL);
	#endif
	#ifdef __WIN32__
	GetSystemTime(&newTime);
	Sleep((newTime.wSecond*1000+newTime.wMilliseconds-oldTime.wSecond*1000+oldTime.wMilliseconds) < 100 ? 100 - labs(newTime.wSecond*1000+newTime.wMilliseconds-oldTime.wSecond*1000+oldTime.wMilliseconds) : 0);
	GetSystemTime(&oldTime);
	#endif
}

void randomColor() {
	if(doing) return;
	doing++;
	if(sendMessage(0, 0, 0, 5, duration, 0)) {
		fprintf(stderr, "Error sending message!\n");
	}
	waitForCompletion();
	doing--;
}

void resetColor() {
	if(doing) return;
	doing++;
	if(sendMessage(0, 0, 0, 3, 0, 0)) {
		fprintf(stderr, "Error sending message!\n");
	}
	waitForCompletion();
	doing--;
	for(int i=0; i<3; i++) {
		colors[i] = 0;
	}
	gtk_hsv_set_color(GTK_HSV(hsvWheel), 0.0, 0.0, 0.0);
	gtk_range_set_value(GTK_RANGE(slider[0]), 1.0);
	gtk_range_set_value(GTK_RANGE(slider[1]), 50.0);
}

void blinkColor() {
	if(doing) return;
	doing++;
	if(sendMessage(colors[0], colors[1], colors[2], 4, duration, 0)) {
		fprintf(stderr, "Error sending message!\n");
	}
	waitForCompletion();
	doing--;
}

void aestheticColor() {
	if(doing) return;
	doing++;
	if(sendMessage(colors[0], colors[1], colors[2], 6, duration, additional)) {
		fprintf(stderr, "Error sending message!\n");
	}
	waitForCompletion();
	doing--;
}

void fadeBlinkColor() {
	if(doing) return;
	doing++;
	if(sendMessage(colors[0], colors[1], colors[2], 4, duration, 1)) {
		fprintf(stderr, "Error sending message!\n");
	}
	waitForCompletion();
	doing--;
}

void abortColor() {
	if(doing) return;
	doing++;
	if(sendMessage(0, 0, 0, 1, 0, 0)) {
		fprintf(stderr, "Error sending message!\n");
	}
	waitForCompletion();
	doing--;
}

void rainbowColor() {
	if(doing) return;
	doing++;
	if(sendMessage(0, 0, 0, 7, duration, 0)) {
		fprintf(stderr, "Error sending message!\n");
	}
	waitForCompletion();
	doing--;
}

void colorChanged(GtkHSV *hsv, gpointer user_data) {
	if(doing) return;
	doing++;
	double h,s,v, r, g, b;
	gtk_hsv_get_color(GTK_HSV(hsv), &h, &s, &v);
	gtk_hsv_to_rgb(h, s, v, &r, &g, &b);
	colors[0] = r*255;
	colors[1] = g*255;
	colors[2] = b*255;
	if(sendMessage(colors[0], colors[1], colors[2], 0, 0, 0)) {
		fprintf(stderr, "Error sending message!\n");
	}
	waitForCompletion();
	doing--;
}

void speedChanged(GtkRange *range, gpointer data) {
	duration = gtk_range_get_value(range)*1000;
}

void additionalChanged(GtkRange *range, gpointer data) {
	additional = gtk_range_get_value(range);
}

