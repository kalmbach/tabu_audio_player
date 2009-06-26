#include <gtk/gtk.h>
#include "tbw.h"

/*static gchar *GTK_CLEAR_IMAGE = "/usr/share/icons/gnome/24x24/actions/gtk-clear.png";*/

#define GTK_CLEAR_IMAGE "gtk-clear"
static gchar *gtk_clear_image_path = NULL;
static gboolean over_addfile = FALSE;
static gboolean over_resize = FALSE;
static gchar *file_chooser_last_folder = NULL;

static gboolean on_barea_expose_event ( GtkWidget *widget, GdkEventExpose *event, gpointer data );
gboolean on_barea_motion_notify_event ( GtkWidget *widget, GdkEventMotion *event, gpointer data );
gboolean on_barea_button_press_event ( GtkWidget *widget, GdkEventButton *event, gpointer data );

/* a better way to get the path of a stock image */
gchar *
tbw_get_icon_path (const char *icon)
{
  gchar	*ret = NULL;
  GtkIconTheme	*theme = NULL;

  theme = gtk_icon_theme_get_default ();
  if ( theme )
  {
    GtkIconInfo *info = gtk_icon_theme_lookup_icon ( theme,	icon,	24,	GTK_ICON_LOOKUP_FORCE_SIZE );
    if ( info )    {
      ret = g_strdup ( gtk_icon_info_get_filename ( info ) );
      gtk_icon_info_free ( info );
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
  cairo_pattern_add_color_stop_rgba ( pat, 1, 0.0f, 0.0f, 0.0f, 0.9f );
  cairo_pattern_add_color_stop_rgba ( pat, 0, 0.2f, 0.2f, 0.2f, 0.9f );
  cairo_set_source ( cr, pat );
	cairo_fill ( cr );
  cairo_pattern_destroy ( pat );

  /* dibujamos el Play Button */
  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER );  
  cairo_set_source_rgba ( cr, 0.0f, 0.0f, 0.0f, 1.0f );
  cairo_set_line_width ( cr, 2.0 );
  cairo_arc ( cr, x1 - 30, y0 + 30, 21, 0, 6.28 );
  cairo_stroke ( cr );
  cairo_arc ( cr, x1 - 30, y0 + 30, 20, 0, 6.28 );
  cairo_set_source_rgba ( cr, 1.0f, 0.5f, 0.0f, 1.0f );
	cairo_fill ( cr );
  cairo_stroke ( cr );
  cairo_set_operator ( cr, CAIRO_OPERATOR_ADD );
  cairo_arc ( cr, x1 - 30, y0 + 30, 20, 3.14, 6.28 );
  cairo_curve_to ( cr, x1 - 30, y0 + 20, x1 - 30, y0 + 40, x1 - 50, y0 + 30 );
  cairo_set_source_rgba ( cr, 1.0f, 1.0f, 1.0f, 0.1f );
  cairo_fill ( cr );
  cairo_stroke ( cr );

  
  if ( tabu_player_is_playing() )
  {
    cairo_set_operator ( cr, CAIRO_OPERATOR_OVER ); 
    cairo_set_line_width ( cr, 9.0 );
    cairo_set_source_rgba ( cr, 0.1f, 0.1f, 0.1f, 0.9f );
    cairo_move_to ( cr, x1 - 35, y0 + 18 );
    cairo_line_to ( cr, x1 - 35, y0 + 42 );
    cairo_move_to ( cr, x1 - 25, y0 + 18 );
    cairo_line_to ( cr, x1 - 25, y0 + 42 );
    cairo_stroke ( cr );    
  }
  else
  {
    cairo_set_operator ( cr, CAIRO_OPERATOR_OVER ); 
    cairo_set_line_width ( cr, 9.0 );
    cairo_set_source_rgba ( cr, 0.1f, 0.1f, 0.1f, 0.9f );
    cairo_move_to ( cr, x1 - 38, y0 + 18 );
    cairo_line_to ( cr, x1 - 22, y0 + 30 );
    cairo_line_to ( cr, x1 - 38, y0 + 42 );
    cairo_stroke ( cr );   
  }

  /* dibujamos el Next Button */
  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER );  
  cairo_set_source_rgba ( cr, 0.0f, 0.0f, 0.0f, 1.0f );
  cairo_set_line_width ( cr, 2.0 );
  cairo_arc ( cr, x1 - 70, y0 + 30, 16, 0, 6.28 );
  cairo_stroke ( cr );
  cairo_arc ( cr, x1 - 70, y0 + 30, 15, 0, 6.28 );
  cairo_set_source_rgba ( cr, 1.0f, 0.5f, 0.0f, 0.9f );
  cairo_fill ( cr );
  cairo_stroke ( cr );
  cairo_set_operator ( cr, CAIRO_OPERATOR_ADD );
  cairo_arc ( cr, x1 - 70, y0 + 30, 15, 3.14, 6.28 );
  cairo_curve_to ( cr, x1 - 70, y0 + 20, x1 - 70, y0 + 40, x1 - 85, y0 + 30 );
  cairo_set_source_rgba ( cr, 1.0f, 1.0f, 1.0f, 0.1f );
  cairo_fill ( cr );
  cairo_stroke ( cr );
  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER ); 
  cairo_set_line_width ( cr, 4.0 );
  cairo_set_source_rgba ( cr, 0.1f, 0.1f, 0.1f, 0.9f );
  cairo_move_to ( cr, x1 - 72, y0 + 22 );
  cairo_line_to ( cr, x1 - 62, y0 + 30 );
  cairo_line_to ( cr, x1 - 72, y0 + 38 );
  cairo_move_to ( cr, x1 - 62, y0 + 30 );
  cairo_line_to ( cr, x1 - 78, y0 + 30 );
  cairo_stroke ( cr );

  /* dibujamos el Prev Button */
  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER );  
  cairo_set_source_rgba ( cr, 0.0f, 0.0f, 0.0f, 1.0f );
  cairo_set_line_width ( cr, 2.0 );
  cairo_arc ( cr, x1 - 105, y0 + 30, 16, 0, 6.28 );
  cairo_stroke ( cr );
  cairo_arc ( cr, x1 - 105, y0 + 30, 15, 0, 6.28 );
  cairo_set_source_rgba ( cr, 1.0f, 0.5f, 0.0f, 0.9f );
  cairo_fill ( cr );
  cairo_stroke ( cr );
  cairo_set_operator ( cr, CAIRO_OPERATOR_ADD );
  cairo_arc ( cr, x1 - 105, y0 + 30, 15, 3.14, 6.28 );
  cairo_curve_to ( cr, x1 - 105, y0 + 20, x1 - 105, y0 + 40, x1 - 120, y0 + 30 );
  cairo_set_source_rgba ( cr, 1.0f, 1.0f, 1.0f, 0.1f );
  cairo_fill ( cr );
  cairo_stroke ( cr );
  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER ); 
  cairo_set_line_width ( cr, 4.0 );
  cairo_set_source_rgba ( cr, 0.1f, 0.1f, 0.1f, 0.9f );
  cairo_move_to ( cr, x1 - 103, y0 + 22 );
  cairo_line_to ( cr, x1 - 113, y0 + 30 );
  cairo_line_to ( cr, x1 - 103, y0 + 38 );
  cairo_move_to ( cr, x1 - 113, y0 + 30 );
  cairo_line_to ( cr, x1 - 97, y0 + 30 );
  cairo_stroke ( cr );

  /* dibujamos el Add Button */
  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER ); 
  if ( over_addfile )
  {
    cairo_set_line_width ( cr, 9.0 );
    cairo_set_source_rgba ( cr, 0.0f, 0.0f, 0.0f, 0.9f );
    cairo_move_to ( cr, x0 + 18, y0 + 30 );
    cairo_line_to ( cr, x0 + 42, y0 + 30 );
    cairo_move_to ( cr, x0 + 30, y0 + 18 );
    cairo_line_to ( cr, x0 + 30, y0 + 42 );
    cairo_stroke ( cr );    
  }  
  cairo_set_line_width ( cr, 6.0 );
  cairo_set_source_rgba ( cr, 1.0f, 0.5f, 0.0f, 0.9f );
  cairo_move_to ( cr, x0 + 20, y0 + 30 );
  cairo_line_to ( cr, x0 + 40, y0 + 30 );
  cairo_move_to ( cr, x0 + 30, y0 + 20) ;
  cairo_line_to ( cr, x0 + 30, y0 + 40 );
  cairo_stroke ( cr );

  /* dibujamos el Clear Button */
  cairo_set_operator ( cr, CAIRO_OPERATOR_OVER );
  image = cairo_image_surface_create_from_png ( gtk_clear_image_path );

  cairo_set_source_surface ( cr, image, x0 + 50, y0 + 20 );
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

  cairo_destroy (cr);

  return FALSE;
}

GtkWidget *
tbw_controls_new ( GtkListStore *playlist )
{
  GtkWidget *barea;
  GtkWidget *box;

  gtk_clear_image_path = tbw_get_icon_path ( GTK_CLEAR_IMAGE );

  barea = gtk_drawing_area_new ( );
  gtk_widget_add_events ( GTK_WIDGET ( barea ), GDK_BUTTON_PRESS_MASK );
  gtk_widget_add_events ( GTK_WIDGET ( barea ), GDK_POINTER_MOTION_MASK );
  g_signal_connect ( barea, "button-press-event", G_CALLBACK ( on_barea_button_press_event ), playlist );
  g_signal_connect ( barea, "motion-notify-event", G_CALLBACK ( on_barea_motion_notify_event ), NULL );
  g_signal_connect ( barea, "expose-event", G_CALLBACK ( on_barea_expose_event ), NULL );

  box = gtk_hbox_new ( FALSE, 0 );
  gtk_widget_set_size_request ( GTK_WIDGET ( box ), -1, 60 );
  gtk_box_pack_start ( GTK_BOX ( box ), GTK_WIDGET ( barea ), TRUE, TRUE, 0 );
  gtk_widget_show_all ( GTK_WIDGET ( box ) );

  return ( box );
}
