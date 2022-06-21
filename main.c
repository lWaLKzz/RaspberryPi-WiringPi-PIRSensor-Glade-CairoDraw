#include <stdio.h>
#include <wiringPi.h> 
#include <gtk/gtk.h> 
#include <math.h> 
#include <cairo.h> 

#define BuzzerPin    1 
#define PIRPin       0
int durum = 0;
GtkBuilder      *builder; 
GtkWidget       *window;
GtkLabel		*label;
GtkWidget       *area;

gboolean time_handler(GtkLabel *label);

gboolean on_drawingArea_draw(GtkDrawingArea *widget,cairo_t *cr){
	GtkAllocation* alloc = g_new(GtkAllocation, 1);
    gtk_widget_get_allocation(area, alloc);
    int height = alloc->height;
	int width = alloc->width;
    g_free(alloc);
    
	cairo_set_line_width(cr, 9);
	if(durum==1){cairo_set_source_rgb(cr, 0, 1, 0); }
	else{cairo_set_source_rgb(cr, 1, 0, 0);}
	

	cairo_translate(cr, width/2, height/2); 
	cairo_arc(cr, 0, 0,25, 0, 2 * M_PI);
	
	cairo_fill(cr); 
	return FALSE;
}

PI_THREAD (pir) 
{
	while(1){
		if(!(digitalRead(PIRPin))){
		digitalWrite(BuzzerPin, HIGH);
		durum = 0;
		printf("Hareket Algılanmadı.\n");
		}
		else{
		digitalWrite(BuzzerPin, LOW);
		durum = 1;
		printf("Hareket Algılandı!\n");
		
		}
		delay(1000);
	}
}

int main(int argc, char *argv[])
{	
	if(wiringPiSetup() == -1){
	printf("wiringPi Bağlantı Hatası !");
	exit(1);
	}

	pinMode(BuzzerPin, OUTPUT);
	pinMode(PIRPin,INPUT);
    piThreadCreate(pir);
	
    gtk_init(&argc, &argv);
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "win.glade", NULL); 
    window = GTK_WIDGET(gtk_builder_get_object(builder, "gtkWin")); 
    label = GTK_LABEL(gtk_builder_get_object(builder, "durum")); 
    area = GTK_WIDGET(gtk_builder_get_object(builder, "drawingArea")); 
   
  
    
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    gtk_widget_show(window);
    g_timeout_add(500, (GSourceFunc)time_handler,label);
         
    gtk_main();
    return 0;
}

gboolean time_handler(GtkLabel *label){
	
	char pirTrue[50] = "Haraket algılandı.";
	char pirFalse[50] = "Haraket algılanmadı.";
	if(durum==1){
		gtk_label_set_text(label,pirTrue);
	}
	else{
		gtk_label_set_text(label,pirFalse);
	}
	 
	gtk_widget_queue_draw(area); 
	
    return TRUE;
}

