#include <gdk/gdkkeysyms.h>
#include <string.h>
#include <gtk/gtk.h>
#include <tag_c.h>
#include "tbw.h"

/*
TODO Filtrar solo audio files en el open file dialog
TODO Maximizar, restaurar en un entorno con varios desks virtuales
TODO ICONO de la applicacion cuando minimiza
TODO Mostrar tiempo de pista (restante/total) o (pasado/total)
TODO Barrita de progresso y seeker
TODO Recuperar la playlist que tenia al cerrar? optional?
*/


/* Wellcome, global evils */
GtkListStore *store;
GtkWidget *view;

GtkListStore *
get_tabu_playlist ()
{
  return ( store );
}

void
clear_tabu_playlist ()
{
  tabu_player_clear_playlist ( );
}

void
scroll_to_song (GtkTreeIter iter)
{
  GtkTreePath *path = gtk_tree_model_get_path ( GTK_TREE_MODEL ( store ), &iter );
  gtk_tree_view_scroll_to_cell ( GTK_TREE_VIEW ( view ), path, NULL, FALSE, 0, 0 );
}

gchar *
get_formatted_song ( gchar *filename )
{
  TagLib_File *file;
  TagLib_Tag *tag;
  gchar *title = NULL;
  gchar *artist = NULL; 
  gchar *row = NULL;

	file = taglib_file_new ( filename );

  if ( file == NULL )
  {
  	return ( NULL );
  }

	tag = taglib_file_tag ( file );  
  title = taglib_tag_title ( tag );
  artist = taglib_tag_artist ( tag );

  if ( strlen ( title )  == 0 )
  {
    gchar **tokens = NULL;
    int i = 0;

    tokens = g_strsplit ( filename, "/", 0 );
    if ( tokens != NULL )
    {
      while ( tokens[i] != NULL )
        i++;
  
      title = g_strdup ( tokens[i-1] );      
    }
    g_strfreev ( tokens );
  }

  if ( strlen ( artist ) == 0 )
    artist = "Unknown";
  
	row = g_strconcat ( 
    g_strdup ( g_markup_escape_text ( title, -1 ) ),
    " - <span size='smaller'><i>", 
    g_strdup ( g_markup_escape_text ( artist, -1 ) ), 
    "</i></span>", 
    NULL );

  /*g_free ( title );
  g_free ( artist );*/

  taglib_tag_free_strings ( );
  taglib_file_free ( file );

  return ( row );
}

void
add_item_to_playlist ( GtkListStore *store, gchar* filename, gchar *uri )
{
	GtkTreeIter iter;
	gchar *row = get_formatted_song ( filename );
  
  if ( row == NULL )
    return;

	gtk_list_store_append ( store, &iter );
  gtk_list_store_set ( store, &iter, 0, "", 1, row, 2, uri, -1 );

  g_free ( row );
}

void
playlist_row_activated_callback ( GtkTreeView *view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data )
{
  GtkTreeSelection *selection = gtk_tree_view_get_selection ( view );

  tabu_player_play_selection ( selection );
}

void
playlist_key_press_callback ( GtkWidget *tview, GdkEventKey *event, gpointer data )
{
  GtkTreeSelection *selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tview ) );
  if ( event->type == GDK_KEY_PRESS )
  {
    switch ( event->keyval )
    {
      /* if the delete key is pressed. remove the item from the playlist */
      case GDK_Delete:
        tabu_player_remove_selection ( selection );
        break;

      default: 
        return;
    }
  }
}


int
main ( int argc, char *argv[] )
{
  GtkWidget *window;
  GtkWidget *controls;
  GtkCellRenderer     *renderer;
  /* GtkListStore        *store; Esto es global ahora, yeah! sujetense los pantalones. 
  GtkWidget           *view; */
  GtkWidget           *scrollview;

  gtk_init( &argc, &argv );
  
  window = tbw_window_new ( GTK_WINDOW_TOPLEVEL );
  g_signal_connect ( window, "destroy", G_CALLBACK ( gtk_main_quit ), NULL );  
  
  view = gtk_tree_view_new ( );
	gtk_tree_view_set_headers_visible ( GTK_TREE_VIEW ( view ), FALSE );
	gtk_tree_view_set_rules_hint ( GTK_TREE_VIEW ( view ), TRUE );
	gtk_tree_selection_set_mode ( GTK_TREE_SELECTION ( gtk_tree_view_get_selection ( GTK_TREE_VIEW ( view ) ) ), GTK_SELECTION_SINGLE );
  renderer = gtk_cell_renderer_text_new ( );
  gtk_tree_view_insert_column_with_attributes ( GTK_TREE_VIEW ( view ), -1, "Columna", renderer, "markup", 0, NULL );
  gtk_tree_view_insert_column_with_attributes ( GTK_TREE_VIEW ( view ), -1, "Columna", renderer, "markup", 1, NULL );
  store = gtk_list_store_new ( 3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING );
  gtk_tree_view_set_model ( GTK_TREE_VIEW ( view ), GTK_TREE_MODEL ( store ) );  

  g_signal_connect ( G_OBJECT ( view ), "row-activated", G_CALLBACK ( playlist_row_activated_callback ), NULL );
  g_signal_connect ( G_OBJECT ( view ), "key-press-event", G_CALLBACK ( playlist_key_press_callback ), NULL );

  scrollview = gtk_scrolled_window_new ( NULL, NULL );
	gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrollview ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_container_add ( GTK_CONTAINER ( scrollview ), GTK_WIDGET ( view ) );

  controls = tbw_controls_new ( store );    

  tbw_window_pack ( scrollview, TRUE, TRUE, 0 );
  tbw_window_pack ( controls, FALSE, FALSE, 0 );  

  tabu_player_main ( );
  gtk_main ( );

  return 0;
}
