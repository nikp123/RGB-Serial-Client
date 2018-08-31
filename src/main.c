#include "actions.c"

int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(window, "destroy", G_CALLBACK(destroy_window), NULL);
	
	// create list of available serial devices
	serialSelection = gtk_combo_box_text_new();
	
	refresh_serial_devices();

	// create togglebuttons
	customDelay = gtk_toggle_button_new_with_label("Custom delay:");
	
	// create labels
	connectText = gtk_label_new(FIND_DEVICE_MESSAGE);
	colorText = gtk_label_new(DISCONNECTED_MESSAGE);
	delayText = gtk_label_new("Delay:");
	optionText = gtk_label_new("Option:");
	
	// create buttons
	blink = gtk_button_new_with_label("Blink");
	fadeBlink = gtk_button_new_with_label("Fade");
	reset = gtk_button_new_with_label("Reset");
	randomFade = gtk_button_new_with_label("Random");
	rainbow = gtk_button_new_with_label("Rainbow :D");
	connectTrigger = gtk_button_new_with_label("Connect");
	refresh = gtk_button_new_with_label("Refresh");
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
	g_signal_connect(refresh, "clicked", G_CALLBACK(refresh_serial_devices), NULL);

	// create wheel
	hsvWheel = gtk_hsv_new();
	g_signal_connect(hsvWheel, "changed", G_CALLBACK(colorChanged), NULL);
	gtk_hsv_set_metrics(GTK_HSV(hsvWheel), 200, 20);

	// create sliders	
	slider[0] = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 5.0, 0.1);
	slider[1] = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 255.0, 1.0);
	g_signal_connect(slider[0], "value-changed", G_CALLBACK(speedChanged), NULL);
	g_signal_connect(slider[1], "value-changed", G_CALLBACK(additionalChanged), NULL);
	gtk_range_set_value(GTK_RANGE(slider[0]), 1.0);
	gtk_range_set_value(GTK_RANGE(slider[1]), 50.0);

	// create entries
	customDelayEntry = gtk_entry_new();
	
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
	gtk_box_pack_start(GTK_BOX(serialBox), refresh, 0, 0, 0); 
	
	// Pack connect box
	connectBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_add(GTK_CONTAINER(connectBox), connectText);
	gtk_container_add(GTK_CONTAINER(connectBox), serialBox);
	
	// delay box
	delayBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_pack_end(GTK_BOX(delayBox), slider[0], 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(delayBox), delayText);		

	// customDelay box
	customDelayBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_pack_end(GTK_BOX(customDelayBox), customDelayEntry, 1, 1, 1);		
	gtk_container_add(GTK_CONTAINER(customDelayBox), customDelay);		
	gtk_widget_set_sensitive(customDelayEntry, 0); 					// Make color sliders not available until connected
  g_signal_connect(customDelay, "toggled", G_CALLBACK(toggle_delay), NULL);
  g_signal_connect(customDelayEntry, "preedit-changed", G_CALLBACK(speedChanged_entry), NULL);
  g_signal_connect(customDelayEntry, "activate", G_CALLBACK(speedChanged_entry), NULL);

	// option box
	optionBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_pack_end(GTK_BOX(optionBox), slider[1], 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(optionBox), optionText);		

	// Pack color box
	colorBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_box_pack_start(GTK_BOX(colorBox), colorText, 1, 1, 1); 
	gtk_container_add(GTK_CONTAINER(colorBox), colorButtons[0]);		
	gtk_container_add(GTK_CONTAINER(colorBox), colorButtons[1]);		
	gtk_container_add(GTK_CONTAINER(colorBox), colorButtons[2]);		
	gtk_container_add(GTK_CONTAINER(colorBox), hsvWheel);		
	gtk_container_add(GTK_CONTAINER(colorBox), delayBox);		
	gtk_container_add(GTK_CONTAINER(colorBox), customDelayBox);		
	gtk_container_add(GTK_CONTAINER(colorBox), optionBox);		
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
