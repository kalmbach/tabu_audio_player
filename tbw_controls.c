#include <gtk/gtk.h>
#include <gst/gst.h>
#include "tbw.h"

#define TABU_CONTROLS_HEIGHT 57

static gchar *tabu_play_icon_path = DATA_DIR "/tabu/play.png";
static gchar *tabu_pause_icon_path = DATA_DIR "/tabu/pause.png";
static gchar *tabu_next_icon_path = DATA_DIR "/tabu/next.png";
static gchar *tabu_prev_icon_path = DATA_DIR "/tabu/prev.png";

static gchar *tabu_clear_image_path = DATA_DIR "/tabu/clear.png";
static gchar *tabu_add_image_path = DATA_DIR "/tabu/open.png";
static gboolean over_addfile = FALSE;
static gboolean over_resize = FALSE;
static gchar *file_chooser_last_folder = NULL;

static gdouble progressvalue = 0.0;

static gboolean on_barea_expose_event ( 
  GtkWidget *widget, 
  GdkEventExpose *event, 
  gpointer data );

gboolean on_barea_motion_notify_event ( 
  GtkWidget *widget, 
  GdkEventMotion *event, 
  gpointer data );

gboolean on_barea_button_press_event ( 
  GtkWidget *widget, 
  GdkEventButton *event, 
  gpointer data );

/* a better way to get the path of a stock image */
gchar *
tbw_get_icon_path (const char *icon)
{
  gchar	*ret = NULL;
  GtkIconTheme	*theme = NULL;

  theme = gtk_icon_theme_get_default ();
  if (theme)
  {
    GtkIconInfo *info = gtk_icon_theme_lookup_icon ( 
                          theme,	
                          icon,	
                          24,	
                          GTK_ICON_LOOKUP_FORCE_SIZE);

    if (info)    
    {
      ret = g_strdup (gtk_icon_info_get_filename (info));
      gtk_icon_info_free (info);
    }
  }
  
  return ret;
}


gboolean
on_barea_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  over_addfile = FALSE;
  over_resize = FALSE;

  if ((event->y > 20) && (event->y < 40))
  {
    if ((event->x > 20) && (event->x < 40))
    {
	    over_addfile = TRUE;
    }
  }
  else if ( ( event->x > ( widget->allocation.width - 10 ) ) && ( event->y > ( widget->allocation.height - 10 ) ))
  {
    over_resize = TRUE;
  }

  gtk_widget_queue_draw (widget);

  return FALSE;
}

gboolean
update_expose ( widget )
{
  gtk_widget_queue_draw ( widget );
  return TRUE;
}

gboolean 
on_barea_button_press_event ( GtkWidget *widget, GdkEventButton *event, gpointer data )
{    
  if ( event->type == GDK_BUTTON_PRESS )
  {
    if ( event->button == 1 )
    {        
      /* El Boton de Agregar archivos ha sido presionado */
      if ( ( event->y > 20 ) && ( event->y < 40 ) )
      {
        if ( ( event->x > 20 ) && ( event->x < 40 ) )
        {
          GtkWidget *selection;
          gint response;
          GSList *files;

          selection = gtk_file_chooser_dialog_new ( "Add Files", NULL,
                                                  GTK_FILE_CHOOSER_ACTION_OPEN,
                                                  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                  GTK_STOCK_OK, GTK_RESPONSE_OK, NULL );

          if ( file_chooser_last_folder == NULL )
          {
            gtk_file_chooser_set_current_folder ( GTK_FILE_CHOOSER ( selection ), g_get_home_dir ( ) );
          } 
          else
          {
            gtk_file_chooser_set_current_folder ( GTK_FILE_CHOOSER ( selection ), file_chooser_last_folder );            
          }

          gtk_file_chooser_set_select_multiple ( GTK_FILE_CHOOSER ( selection ), TRUE );
          gtk_widget_show_all ( selection );

          response = gtk_dialog_run ( GTK_DIALOG ( selection ) );

          if ( response == GTK_RESPONSE_OK )
          {
            g_free ( file_chooser_last_folder );
            file_chooser_last_folder = gtk_file_chooser_get_current_folder ( GTK_FILE_CHOOSER ( selection ) );
            files = gtk_file_chooser_get_filenames ( GTK_FILE_CHOOSER ( selection ) );
            while ( files != NULL )
            {
        			add_item_to_playlist ( GTK_LIST_STORE ( data ), 
                                    g_uri_unescape_string ( files->data, NULL ),
                                    g_strjoin ( NULL, "file://", g_uri_unescape_string ( files->data, NULL ), NULL ) );
        			files = g_slist_next ( files );
            }
            g_slist_free ( files );
          }
          gtk_widget_destroy ( selection );
        }
      }         

      /* Play button baby */
      if ( ( event->y > 10 ) && ( event->y < 50 ) )
      {
        if ( ( event->x > ( widget->allocation.width - 50 ) ) && ( event->x < ( widget->allocation.width - 10 ) ) )
        {
          tabu_player_play ();
        }
      }

      /* Es hora del siguiente */
      if ( ( event->y > 15 ) && ( event->y < 45 ) )
      {
        if ( ( event->x > ( widget->allocation.width - 85 ) ) && ( event->x < ( widget->allocation.width - 55 ) ) )
        {
          tabu_player_next ();
        }
      }    

      /* Volveeeer, con la frente marchita...  */
      if ( ( event->y > 15 ) && ( event->y < 40 ) )
      {
        if ( ( event->x > ( widget->allocation.width - 120 ) ) && ( event->x < ( widget->allocation.width - 90 ) ) )
        {
          tabu_player_previous ();
        }
      }    

      /* A limpiar, a limpiar, cada cosa en su lugar */
      if ( ( event->y > 20 ) && ( event->y < 45 ) )
      {
        if ( ( event->x > 50 ) && ( event->x < 70 ) ) 
        {
          clear_tabu_playlist ();
        }
      }    

      /* El tamaño si importa */
      if ( ( event->y > ( widget->allocation.height - 20 ) ) && ( event->x > ( widget->allocation.width - 20 ) ) )
      {
        gtk_window_begin_resize_drag ( GTK_WINDOW ( gtk_widget_get_toplevel ( widget ) ),
                                       GDK_WINDOW_EDGE_SOUTH_EAST,
                                       event->button,
                                       event->x_root,
                                       event->y_root,
                                       event->time);
      }
    }
  }

  return FALSE;  
}

static gboolean
on_progressbar_expose_event ( GtkWidget *widget, GdkEventExpose *event, gpointer data )
{
  cairo_t *cr;
  cairo_pattern_t *pat;
  double x0 = 0;
  double y0 = 0;
  double y1 = 0;
  double x1 = 0;
 

 /* fondo transparente para el title */
  cr = gdk_cairo_create ( widget->window );
  cairo_rectangle ( cr, widget->allocation.x, widget->allocation.y, widget->allocation.width, widget->allocation.height );
  cairo_set_source_rgba ( cr, 1.0f, 1.0f, 1.0f, 0.0f );
	cairo_set_operator ( cr, CAIRO_OPERATOR_SOURCE );
	cairo_paint ( cr );
  cairo_destroy ( cr );

  /* path del title */
  cr = gdk_cairo_create ( widget->window );
  y1 += widget->allocation.height;
  x1 += widget->allocation.width;

  cairo_move_to ( cr, x1, y1 );
  cairo_line_to ( cr, x0, y1 );
  cairo_line_to ( cr, x0, y0 );
  cairo_line_to ( cr, x1, y0 );
  cairo_close_path ( cr );

  /* pintamos el path del title con un patron lineal horizontal */
	cairo_set_operator ( cr, CAIRO_OPERATOR_SOURCE );
  pat = cairo_pattern_create_linear ( x0, y1/2,  x1, y1/2 );
  cairo_pattern_add_color_stop_rgba ( pat, 1, 0.0f, 0.0f, 0.0f, 0.8f );
  cairo_pattern_add_color_stop_rgba ( pat, 0, 0.1f, 0.1f, 0.1f, 0.9f );
  cairo_set_source ( cr, pat );
	cairo_fill ( cr );
  cairo_pattern_destroy ( pat );

  /* dibujar progreso */
  gint64 pos = tabu_player_get_current_position();
  gint64 len = tabu_player_get_current_length();

  gint64 prog = (x1 * pos) / len;

  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER );
  cairo_set_source_rgba ( cr, 1.0f, 0.5f, 0.0f, 0.6f );
  cairo_set_line_width ( cr, 8 );
  int i = 0;
  for (i=0; i<prog; i++)
  {
    if ( ( i % 5 ) == 0)
      cairo_move_to ( cr, x0 + i + 1, y0 + 5 );  
    else
      cairo_move_to ( cr, x0 + i, y0 + 5 );
    cairo_line_to ( cr, x0 + i + 1, y0 + 5 );  
  }

  cairo_stroke ( cr );
  
  cairo_destroy ( cr );
        
  return FALSE;
}

static gboolean
on_barea_expose_event ( GtkWidget *widget, GdkEventExpose *event, gpointer data )
{
  cairo_t *cr;
  cairo_pattern_t *pat;
  cairo_surface_t *image;
  double x0 = 0;
  double y0 = 0;
  double y1 = 0;
  double x1 = 0;
  double radio = 10;

  /* fondo transparente para el title */
  cr = gdk_cairo_create ( widget->window );
  cairo_rectangle ( cr, widget->allocation.x, widget->allocation.y, widget->allocation.width, widget->allocation.height );
  cairo_set_source_rgba ( cr, 1.0f, 1.0f, 1.0f, 0.0f );
	cairo_set_operator ( cr, CAIRO_OPERATOR_SOURCE );
	cairo_paint ( cr );
  cairo_destroy ( cr );

  /* path del title */
  cr = gdk_cairo_create ( widget->window );
  y1 += widget->allocation.height;
  x1 += widget->allocation.width;

  cairo_arc ( cr, x1 - radio, y1 - radio, radio, 0, 1.57 );
  cairo_line_to ( cr, x0 - radio, y1 );
  cairo_arc ( cr, x0 + radio, y1 - radio, radio, 1.5 , 3.14 );
  cairo_line_to ( cr, x0, y0 );
  cairo_line_to ( cr, x1, y0 );
  cairo_close_path ( cr );

  /* pintamos el path del title con un patron lineal horizontal */
	cairo_set_operator ( cr, CAIRO_OPERATOR_SOURCE );
  pat = cairo_pattern_create_linear ( x0, y1/2,  x1, y1/2 );
  cairo_pattern_add_color_stop_rgba ( pat, 1, 0.0f, 0.0f, 0.0f, 0.8f );
  cairo_pattern_add_color_stop_rgba ( pat, 0, 0.1f, 0.1f, 0.1f, 0.9f );
  cairo_set_source ( cr, pat );
	cairo_fill ( cr );
  cairo_pattern_destroy ( pat );

  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER );
  if ( tabu_player_is_playing() )
  {
    image = cairo_image_surface_create_from_png ( tabu_pause_icon_path );
  }
  else
  {
    image = cairo_image_surface_create_from_png ( tabu_play_icon_path );
  } 
  cairo_set_source_surface ( cr, image, x1 - 48, y1 - 47 );
  cairo_paint ( cr );
  cairo_stroke ( cr );
  cairo_surface_destroy ( image ); 
  

  /* dibujamos el Next Button */
  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER ); 
  image = cairo_image_surface_create_from_png ( tabu_next_icon_path ); 
  cairo_set_source_surface ( cr, image, x1 - 84, y1 - 42 );
  cairo_paint ( cr );
  cairo_stroke ( cr );
  cairo_surface_destroy ( image ); 
 
  /* dibujamos el Prev Button */
  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER );  
  image = cairo_image_surface_create_from_png ( tabu_prev_icon_path ); 
  cairo_set_source_surface ( cr, image, x1 - 119, y1 - 42 );
  cairo_paint ( cr );
  cairo_stroke ( cr );
  cairo_surface_destroy ( image ); 

  /* dibujamos el Add Button */
  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER );
  image = cairo_image_surface_create_from_png ( tabu_add_image_path );

  cairo_set_source_surface ( cr, image, x0 + 10, y1 - 42 );
  cairo_paint ( cr );
  cairo_stroke ( cr );
  cairo_surface_destroy ( image );


  /* dibujamos el Clear Button */
  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER );
  image = cairo_image_surface_create_from_png ( tabu_clear_image_path );

  cairo_set_source_surface ( cr, image, x0 + 45, y1 - 42 );
  cairo_paint ( cr );
  cairo_stroke ( cr );
  cairo_surface_destroy ( image );

  /* dibujamos el resize control */
  if ( over_resize )
  {
    cairo_set_operator ( cr, CAIRO_OPERATOR_OVER );    
    cairo_set_source_rgba ( cr, 1.0f, 0.5f, 0.0f, 0.6f );
    cairo_arc ( cr, x1 - 10, y1 - 10, 10, 0, 1.57 );
    cairo_line_to ( cr, x1 - 20, y1 );
    cairo_line_to ( cr, x1, y1 - 20 );  
    cairo_line_to ( cr, x1, y1 - 10 );
    cairo_fill ( cr );  
    cairo_stroke ( cr );    
  }  

  gchar time_buffer[25];
  
  g_snprintf(time_buffer, 24, 
    "%u:%02u.%02u", 
    GST_TIME_ARGS ( tabu_player_get_current_position()));
  /*
  gchar length_buffer[25];
  gchar diff_buffer[25];
  g_snprintf(length_buffer, 24, 
    "%u:%02u.%02u", 
    GST_TIME_ARGS ( tabu_player_get_current_length ()));
  g_snprintf(diff_buffer, 24, 
    "%u:%02u.%02u", 
    GST_TIME_ARGS ( tabu_player_get_current_length () - tabu_player_get_current_position ()));
  */

  cairo_select_font_face ( cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL );
  cairo_set_font_size ( cr, 11.0 );
  cairo_set_source_rgba ( cr, 1.0, 0.5, 0.0, 0.8f );
  cairo_move_to ( cr, x1 - 172, y1 - 20 );
  cairo_show_text ( cr, time_buffer);
  cairo_stroke ( cr );

  /*
  cairo_set_source_rgba ( cr, 1.0, 1.0, 1.0, 0.1f );
  cairo_move_to ( cr, x0 + 100, y1 - 33 );
  cairo_show_text ( cr, length_buffer);
  cairo_stroke ( cr );

  cairo_set_source_rgba ( cr, 1.0, 0.2, 0.0, 0.1f );
  cairo_move_to ( cr, x0 + 100, y1 - 8 );
  cairo_show_text ( cr, diff_buffer);
  cairo_stroke ( cr );
  */

  cairo_destroy ( cr );
        
  return FALSE;
}

GtkWidget *
tbw_controls_new ( GtkListStore *playlist )
{
  GtkWidget *barea;
  GtkWidget *progressbar_area;
  GtkWidget *box;

  barea = gtk_drawing_area_new ( );
  gtk_widget_add_events ( GTK_WIDGET ( barea ), GDK_BUTTON_PRESS_MASK );
  gtk_widget_add_events ( GTK_WIDGET ( barea ), GDK_POINTER_MOTION_MASK );
  g_signal_connect ( barea, "button-press-event", G_CALLBACK ( on_barea_button_press_event ), playlist );
  g_signal_connect ( barea, "motion-notify-event", G_CALLBACK ( on_barea_motion_notify_event ), NULL );
  g_signal_connect ( barea, "expose-event", G_CALLBACK ( on_barea_expose_event ), NULL );

  progressbar_area = gtk_drawing_area_new ( );
  g_signal_connect ( progressbar_area, "expose-event", G_CALLBACK ( on_progressbar_expose_event ), NULL );

  box = gtk_vbox_new ( FALSE, 0 );  
  gtk_widget_set_size_request ( GTK_WIDGET ( barea ), -1, TABU_CONTROLS_HEIGHT - 10 );
  gtk_widget_set_size_request ( GTK_WIDGET ( progressbar_area ), -1, 10 );
  gtk_box_pack_start ( GTK_BOX ( box ), GTK_WIDGET ( progressbar_area), TRUE, TRUE, 0 );
  gtk_box_pack_start ( GTK_BOX ( box ), GTK_WIDGET ( barea ), TRUE, TRUE, 0 );

  gtk_widget_show_all ( GTK_WIDGET ( box ) );

  g_timeout_add (500, (GSourceFunc) update_expose, progressbar_area);
  g_timeout_add (1000, (GSourceFunc) update_expose, barea);

  return ( box );
}
