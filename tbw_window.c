#include <gtk/gtk.h>
#include "tbw.h"

GtkWidget *main_vertical_box = NULL;
static gboolean over_maximize = FALSE;
static gboolean over_minimize = FALSE;
static gboolean over_close = FALSE;
static gboolean window_is_maximized = FALSE;

gboolean on_title_clicked ( GtkWidget *widget, GdkEventButton *event, gpointer data );
gboolean on_title_motion_notify_event ( GtkWidget *widget, GdkEventMotion *event, gpointer data );
gboolean on_title_expose_event ( GtkWidget *widget, GdkEventExpose *event, gpointer data );
static void on_alpha_screen_changed ( GtkWidget* pWidget, GdkScreen* pOldScreen, GtkWidget* pLabel );

gboolean 
on_title_clicked ( GtkWidget *widget, GdkEventButton *event, gpointer data )
{
    if ( event->type == GDK_BUTTON_PRESS )
    {
      if ( event->button == 1 )
      {        
        gtk_window_begin_move_drag ( GTK_WINDOW ( gtk_widget_get_toplevel ( widget ) ),
                                     event->button,
                                     event->x_root,
                                     event->y_root,
                                     event->time );   
        if ( event->y < 20 )
        {
          if ( ( event->x > ( widget->allocation.width - 50 ) ) && ( event->x < ( widget->allocation.width - 30 ) ) )
          {
            if ( !window_is_maximized )
            {
              gtk_window_maximize ( GTK_WINDOW ( widget ) );
              window_is_maximized = TRUE;
            }
            else
            {
              gtk_window_unmaximize ( GTK_WINDOW ( widget ) );
              window_is_maximized = FALSE;
            } 
          }
          else if ( ( event->x > ( widget->allocation.width - 30 ) ) && ( event->x < ( widget->allocation.width - 10 ) ) )
          {
            tabu_player_quit ( );                        
            gtk_main_quit ( );
          }          
          else if ( (event->x > ( widget->allocation.width - 70 ) ) && ( event->x < ( widget->allocation.width - 50 ) ) )
          { 
            gtk_window_iconify ( GTK_WINDOW ( widget ) );
          }
        }
      }
    }

    return FALSE;
}

gboolean
on_title_motion_notify_event ( GtkWidget *widget, GdkEventMotion *event, gpointer data )
{
  over_maximize = FALSE;
  over_minimize = FALSE;
  over_close    = FALSE;
  if ( event->y < 20 )
  {
    if ( ( event->x > ( widget->allocation.width - 50 ) ) && ( event->x < ( widget->allocation.width - 30 ) ) )
    {
      over_maximize = TRUE;
    }
    else if ( ( event->x > ( widget->allocation.width - 30 ) ) && (event->x < (widget->allocation.width - 10 ) ) )
    {
      over_close = TRUE;
    }
    else if ( ( event->x > ( widget->allocation.width - 70 ) ) && ( event->x < ( widget->allocation.width - 50 ) ) )
    {
      over_minimize = TRUE;
    }
  }
  gtk_widget_queue_draw( widget );

  return FALSE;
}

gboolean
on_title_expose_event ( GtkWidget *widget, GdkEventExpose *event, gpointer data )
{
  cairo_t *cr;
  cairo_pattern_t *pat;
  double x0;
  double y0;
  double y1;
  double x1;
  double radio;

  /* fondo transparente para el title */
  cr = gdk_cairo_create ( widget->window );
  cairo_rectangle ( cr, 0, 0, widget->allocation.width, widget->allocation.height );
  cairo_set_source_rgba ( cr, 1.0f, 1.0f, 1.0f, 0.0f );
	cairo_set_operator ( cr, CAIRO_OPERATOR_SOURCE );
	cairo_paint ( cr );
  cairo_destroy ( cr );

  /* path del title */
  cr = gdk_cairo_create ( widget->window );
  x0 = widget->allocation.x;
  y0 = widget->allocation.y;
  y1 = y0 + widget->allocation.height;
  x1 = x0 + widget->allocation.width;
  radio = 10;

  cairo_arc ( cr, x0 + radio, y0 + radio, radio, 180 * ( 3.14 / 180 ), 270 * ( 3.14 / 180 ) );
  cairo_line_to ( cr, x1 - radio, y0 );
  cairo_arc ( cr, x1 - radio, y0 + radio, radio, 270 * ( 3.14 / 180 ) , 0 );
  cairo_line_to ( cr, x1, y1 );
  cairo_line_to ( cr, x0, y1 );
  cairo_close_path ( cr );

  /* pintamos el path del title con un patron lineal horizontal */
	cairo_set_operator ( cr, CAIRO_OPERATOR_SOURCE );
  pat = cairo_pattern_create_linear ( x0, y1/2,  x1, y1/2 );
  cairo_pattern_add_color_stop_rgba ( pat, 1, 0.0f, 0.0f, 0.0f, 0.9f );
  cairo_pattern_add_color_stop_rgba ( pat, 0, 0.2f, 0.2f, 0.2f, 0.9f );
  cairo_set_source ( cr, pat );
	cairo_fill ( cr );
  cairo_pattern_destroy ( pat );

  /* escribimos el text en el title. */
  cairo_select_font_face ( cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL );
  cairo_set_font_size ( cr, 11.0 );
  cairo_set_source_rgb ( cr, 1.0, 1.0, 1.0 );
  cairo_move_to ( cr, x0+10, y1/2 + 5 );
  cairo_show_text ( cr, "Tabu - Basic Audio Player" );
  cairo_stroke ( cr );

  /* dibujamos el container para los buttons del title */
  cairo_move_to ( cr, x1 - 10, y0 );
  cairo_line_to ( cr, x1 - 10, y0 + 15 );
  cairo_arc ( cr, x1 - 15, y0 + 15, 5,  0, 90 * ( 3.14 / 180 ) );
  cairo_line_to ( cr, x1 - 65, y0 + 20 );
  cairo_arc ( cr, x1 - 65, y0 + 15, 5, 90 * ( 3.14 / 180 ), 180 * ( 3.14 / 180 ) );
  cairo_line_to ( cr, x1 - 70, y0 );
  cairo_close_path ( cr );  
  cairo_set_source_rgba ( cr, 0.1f, 0.1f, 0.1f, 0.9f );
  cairo_set_operator ( cr, CAIRO_OPERATOR_SOURCE );
  cairo_fill ( cr );
  cairo_stroke ( cr );

  /* dibujamos el Close Button */
  if ( over_close )
  {
    cairo_set_source_rgba ( cr, 1.0f, 0.0f, 0.0f, 0.9f );
  }else
  {
    cairo_set_source_rgba ( cr, 1.0f, 1.0f, 1.0f, 0.9f );
  }
  cairo_set_line_width ( cr, 2.0 );
  cairo_move_to ( cr, x1 - 24, y0 + 6 );
  cairo_line_to ( cr, x1 - 16, y0 + 14 );
  cairo_move_to ( cr, x1 - 16, y0 + 6 );
  cairo_line_to ( cr, x1 - 24, y0 + 14 );
  cairo_stroke ( cr );

  /* dibujamos el Maximize Button */
  if ( over_maximize )
  {
    cairo_set_source_rgba ( cr, 1.0f, 0.5f, 0.0f, 0.9f );    
  }else
  {
    cairo_set_source_rgba ( cr, 1.0f, 1.0f, 1.0f, 0.9f );
  }
  cairo_set_line_width ( cr, 2.0 );
  cairo_arc ( cr, x1-40, y0+10, 4, 0, 6.28 );
  cairo_stroke ( cr );

  /* dibujamos el Minimize Button */
  if ( over_minimize )
  {
    cairo_set_source_rgba ( cr, 1.0f, 1.0f, 0.2f, 0.9f );    
  }else
  {
    cairo_set_source_rgba ( cr, 1.0f, 1.0f, 1.0f, 0.9f );
  }
  cairo_set_line_width ( cr, 2.0 );
  cairo_move_to ( cr, x1 - 64, y0 + 14 );
  cairo_line_to ( cr, x1 - 56, y0 + 14 );
  cairo_stroke ( cr );


  cairo_destroy ( cr );
        
  return FALSE;
}

static void
on_alpha_screen_changed ( GtkWidget* pWidget, GdkScreen* pOldScreen, GtkWidget* pLabel )
{                       
	GdkScreen*   pScreen   = gtk_widget_get_screen ( pWidget );
	GdkColormap* pColormap = gdk_screen_get_rgba_colormap ( pScreen );
      
	if ( !pColormap )
		pColormap = gdk_screen_get_rgb_colormap ( pScreen );

	gtk_widget_set_colormap ( pWidget, pColormap );
}

void
tbw_window_pack ( GtkWidget *widget, gboolean expand, gboolean fill, guint padding )
{
  gtk_box_pack_start ( GTK_BOX ( main_vertical_box ), GTK_WIDGET ( widget ), expand, fill, padding );
  gtk_widget_show_all ( GTK_WIDGET ( main_vertical_box ) );
}

GtkWidget *
tbw_window_new ( GtkWindowType type ) 
{
  GtkWidget *window;
  GtkWidget *window_title;

  window = gtk_window_new ( type );
  gtk_window_set_default_size ( GTK_WINDOW ( window ), 300, 450 ); 
  gtk_window_set_position ( GTK_WINDOW ( window ), GTK_WIN_POS_CENTER );  
  gtk_widget_set_app_paintable ( window, TRUE );
  gtk_window_set_decorated ( GTK_WINDOW ( window ), FALSE );
	
  window_title = gtk_drawing_area_new ( );
  gtk_widget_set_size_request ( GTK_WIDGET ( window_title), -1, 40 );
  gtk_widget_add_events ( GTK_WIDGET ( window_title ), GDK_BUTTON_PRESS_MASK );
  gtk_widget_add_events ( GTK_WIDGET ( window_title ), GDK_POINTER_MOTION_MASK );
  g_signal_connect ( window_title, "expose-event", G_CALLBACK ( on_title_expose_event ), NULL );
  g_signal_connect ( window_title, "motion-notify-event", G_CALLBACK ( on_title_motion_notify_event ), NULL );
  g_signal_connect_swapped ( window_title, "button-press-event", G_CALLBACK ( on_title_clicked ), window );

  main_vertical_box = gtk_vbox_new ( FALSE, 0 );
  gtk_box_pack_start ( GTK_BOX ( main_vertical_box ), GTK_WIDGET ( window_title ), FALSE, FALSE, 0 );
  gtk_container_add ( GTK_CONTAINER ( window ), GTK_WIDGET ( main_vertical_box ) );

  on_alpha_screen_changed ( window, NULL, NULL );
  gtk_widget_realize ( GTK_WIDGET ( window ) );
	gdk_window_set_back_pixmap ( window->window, NULL, FALSE );
  gtk_widget_show_all ( window );

  return ( window );
}
