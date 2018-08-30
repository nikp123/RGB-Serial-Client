#include "actions.c"

int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(window, "destroy", G_CALLBACK(destroy_window), NULL);

	// get serial devices
	char **devices;
	int deviceNum;
	devices = listSerialDevices(devices, &deviceNum);
	if(deviceNum < 0) fprintf(stderr, "No serial devices found!\n");
	
	// create list of available serial devices
	serialSelection = gtk_combo_box_text_new();
	for(int i = 0; i < deviceNum; i++) {
		gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(serialSelection), devices[i], devices[i]);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(serialSelection), 0);
	
	// create labels
	connectText = gtk_label_new(FIND_DEVICE_MESSAGE);
	colorText = gtk_label_new(DISCONNECTED_MESSAGE);
	// create buttons
	blink = gtk_button_new_with_label("Blink");
	fadeBlink = gtk_button_new_with_label("Fade");
	reset = gtk_button_new_with_label("Reset");
	randomFade = gtk_button_new_with_label("Random");
	rainbow = gtk_button_new_with_label("Rainbow :D");
	connectTrigger = gtk_button_new_with_label("Connect");
	aesthetic = gtk_button_new_with_label("A E S T H E T I C");
	abortOperation = gtk_button_new_with_label("Stop!");
	g_signal_connect(reset, "clicked", G_CALLBACK(resetColor), NULL);
	g_signal_connect(randomFade, "clicked", G_CALLBACK(randomColor), NULL);
	g_signal_connect(connectTrigger, "clicked", G_CALLBACK(connecttodevice), NULL);
	g_signal_connect(aesthetic, "clicked", G_CALLBACK(aestheticColor), NULL);
	g_signal_connect(blink, "clicked", G_CALLBACK(blinkColor), NULL);
	g_signal_connect(fadeBlink, "clicked", G_CALLBACK(fadeBlinkColor), NULL);
	g_signal_connect(abortOperation, "clicked", G_CALLBACK(abortColor), NULL);
	g_signal_connect(rainbow, "clicked", G_CALLBACK(rainbowColor), NULL);
	// create wheel
	hsvWheel = gtk_hsv_new();
	g_signal_connect(hsvWheel, "changed", G_CALLBACK(colorChanged), NULL);
	gtk_hsv_set_metrics(GTK_HSV(hsvWheel), 200, 20);
	// create sliders	
	slider[0] = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 65.535, 0.01);
	slider[1] = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 255.0, 1.0);
	g_signal_connect(slider[0], "value-changed", G_CALLBACK(speedChanged), NULL);
	g_signal_connect(slider[1], "value-changed", G_CALLBACK(additionalChanged), NULL);
	gtk_range_set_value(GTK_RANGE(slider[0]), 1.0);
	gtk_range_set_value(GTK_RANGE(slider[1]), 50.0);
	
	// Pack the color buttons together
	colorButtons[0] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	colorButtons[1] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	colorButtons[2] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_pack_start(GTK_BOX(colorButtons[0]), reset, 1, 1, 0);
	gtk_container_add(GTK_CONTAINER(colorButtons[0]), randomFade);
	gtk_container_add(GTK_CONTAINER(colorButtons[0]), aesthetic);
	gtk_container_add(GTK_CONTAINER(colorButtons[1]), blink);
	gtk_container_add(GTK_CONTAINER(colorButtons[1]), fadeBlink);
	gtk_container_add(GTK_CONTAINER(colorButtons[1]), abortOperation);
	gtk_container_add(GTK_CONTAINER(colorButtons[2]), rainbow);
	gtk_box_set_homogeneous(GTK_BOX(colorButtons[0]), 1);
	gtk_box_set_homogeneous(GTK_BOX(colorButtons[1]), 1);
	gtk_box_set_homogeneous(GTK_BOX(colorButtons[2]), 1);
	
	// Pack serial box
	serialBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_pack_start(GTK_BOX(serialBox), serialSelection, 1, 1, 0);
	gtk_container_add(GTK_CONTAINER(serialBox), connectTrigger);
	
	// Pack connect box
	connectBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_box_pack_start(GTK_BOX(connectBox), connectText, 0, 0, 0); 
	gtk_container_add(GTK_CONTAINER(connectBox), serialBox);
		
	// Pack color box
	colorBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_box_pack_start(GTK_BOX(colorBox), colorText, 1, 1, 1); 
	gtk_container_add(GTK_CONTAINER(colorBox), colorButtons[0]);		
	gtk_container_add(GTK_CONTAINER(colorBox), colorButtons[1]);		
	gtk_container_add(GTK_CONTAINER(colorBox), colorButtons[2]);		
	gtk_container_add(GTK_CONTAINER(colorBox), hsvWheel);		
	gtk_container_add(GTK_CONTAINER(colorBox), slider[0]);		
	gtk_container_add(GTK_CONTAINER(colorBox), slider[1]);		
	gtk_widget_set_sensitive(colorBox, connected); 					// Make color sliders not available until connected

	// Attach it to the mainBox
	mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
	gtk_box_pack_start(GTK_BOX(mainBox), connectBox, 0, 0, 0);
	gtk_container_add(GTK_CONTAINER(mainBox), colorBox);		
	gtk_container_add(GTK_CONTAINER(window), mainBox);

	// set up window and run
	gtk_widget_show_all(window);
	gtk_main();
}
