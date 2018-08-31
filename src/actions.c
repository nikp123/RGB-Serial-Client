#include <gtk-3.0/gtk/gtk.h>
#include "serial.h"

#define CONNECTED_MESSAGE "Connected :D"
#define DISCONNECTED_MESSAGE "Disconnected!"
#define FIND_DEVICE_MESSAGE "Find the serial to connect to!"

GtkWidget *window, *mainBox;
GtkWidget *colorBox, *connectBox, *colorButtons[3], *customDelayBox, *serialBox, *delayBox, *optionBox;

GtkWidget *slider[2];
GtkWidget *customDelay;
GtkWidget *customDelayEntry;

GtkWidget *connectText, *colorText, *delayText, *optionText;
GtkWidget *serialSelection;
GtkWidget *reset, *aesthetic, *connectTrigger, *refresh, *randomFade, *blink, *fadeBlink, *rainbow, *abortOperation;
GtkWidget *hsvWheel;

pthread_t messageThread;

#if defined(__linux__)||defined(__APPLE__)||defined(__unix__)
struct timeval newTime, oldTime;
#endif

#ifdef __WIN32__
SYSTEMTIME newTime, oldTime;
#endif

char connected = 0;
unsigned int additional = 50, duration = 1000;

// reffering to serial devices
int deviceNum = 0;
char **devices;

unsigned char colors[3] = {0, 0, 0};

void refresh_serial_devices() {
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
	}
	
	// get serial devices
	if(deviceNum) {
		for(int i=0; i<deviceNum; i++) free(devices[i]);
		free(devices);
	}	
	devices = listSerialDevices(devices, &deviceNum);
	if(deviceNum < 0) fprintf(stderr, "No serial devices found!\n");
	
	gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(serialSelection));

	// create list of available serial devices
	for(int i = 0; i < deviceNum; i++) {
		gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(serialSelection), devices[i], devices[i]);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(serialSelection), 0);
}

void destroy_window(GtkWidget *widget, gpointer data) {
	if(connected) destroySerialConnection();
	gtk_main_quit();
}

void toggle_delay(void) {
	gboolean active = gtk_toggle_button_get_active(customDelay);
	gtk_widget_set_sensitive(customDelayEntry, active);
	gtk_widget_set_sensitive(slider[0], !active);
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

		// set up the UI stuff	
		gtk_button_set_label(GTK_BUTTON(connectTrigger), "Disconnect");	
		gtk_label_set_text(GTK_LABEL(colorText), CONNECTED_MESSAGE);	
		gtk_label_set_text(GTK_LABEL(connectText), CONNECTED_MESSAGE);	
		gtk_widget_set_sensitive(colorBox, connected);
		gtk_widget_set_sensitive(serialSelection, !connected);
	}
}

void colorChanged(GtkHSV *hsv, gpointer user_data) {
	double h,s,v, r, g, b;
	gtk_hsv_get_color(GTK_HSV(hsv), &h, &s, &v);
	gtk_hsv_to_rgb(h, s, v, &r, &g, &b);
	colors[0] = r*255;
	colors[1] = g*255;
	colors[2] = b*255;
	if(sendMessage(colors[0], colors[1], colors[2], 0, 0, 0)) {
		fprintf(stderr, "Error sending message!\n");
	}
}

void speedChanged_entry(GtkEntry *entry, gpointer data) {
	double thing;
	if(!sscanf(gtk_entry_get_text(entry), "%lf", &thing)) {
		//do error status or something
		GtkWidget *dialog = gtk_message_dialog_new(window,
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_CLOSE,
                                 "Invalid input %s (must be in \'0.00\' format)",
                                 gtk_entry_get_text(entry));
		gtk_dialog_run(GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
		return;
	}
	duration = thing*1000;
}

void speedChanged(GtkRange *range, gpointer data) {
	duration = gtk_range_get_value(range)*1000;
}


void additionalChanged(GtkRange *range, gpointer data) {
	additional = gtk_range_get_value(range);
}

void randomColor() {
	if(sendMessage(0, 0, 0, 5, duration, 0)) {
		fprintf(stderr, "Error sending message!\n");
	}
}

void resetColor() {
	if(sendMessage(0, 0, 0, 3, 0, 0)) {
		fprintf(stderr, "Error sending message!\n");
	}
	for(int i=0; i<3; i++) {
		colors[i] = 0;
	}
	gtk_hsv_set_color(GTK_HSV(hsvWheel), 0.0, 0.0, 0.0);
	gtk_range_set_value(GTK_RANGE(slider[0]), 1.0);
	gtk_range_set_value(GTK_RANGE(slider[1]), 50.0);
}

void blinkColor() {
	if(sendMessage(colors[0], colors[1], colors[2], 4, duration, 0)) {
		fprintf(stderr, "Error sending message!\n");
	}
}

void aestheticColor() {
	if(sendMessage(colors[0], colors[1], colors[2], 6, duration, additional)) {
		fprintf(stderr, "Error sending message!\n");
	}
}

void fadeBlinkColor() {
	if(sendMessage(colors[0], colors[1], colors[2], 4, duration, 1)) {
		fprintf(stderr, "Error sending message!\n");
	}
}

void abortColor() {
	if(sendMessage(0, 0, 0, 1, 0, 0)) {
		fprintf(stderr, "Error sending message!\n");
	}
}

void rainbowColor() {
	if(sendMessage(0, 0, 0, 7, duration, 0)) {
		fprintf(stderr, "Error sending message!\n");
	}
}

