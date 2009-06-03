#include <gtk/gtk.h>
#include <tag_c.h>
#include "tbw.h"

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
  gtk_list_store_clear ( store );
}

void
scroll_to_song (GtkTreeIter iter)
{
  GtkTreePath *path = gtk_tree_model_get_path ( GTK_TREE_MODEL ( store ), &iter );
  gtk_tree_view_scroll_to_cell ( GTK_TREE_VIEW ( view ),
                               path,
                               NULL,
                               FALSE,
                               0,
                               0);
}

gchar *
get_formatted_song ( gchar *filename )
{
  TagLib_File *file;
  TagLib_Tag *tag;

	file = taglib_file_new(filename);

  if(file == NULL)
  {
  	return ( NULL );
  }

	tag = taglib_file_tag(file);

	gchar *row = g_strconcat(taglib_tag_title(tag)," - <span size='smaller'><i>",
													 taglib_tag_artist(tag),"</i></span>", NULL);

  taglib_tag_free_strings();
  taglib_file_free(file);

  return ( row );
}

void
add_item_to_playlist(GtkListStore *store, gchar* filename, gchar *uri)
{
	GtkTreeIter iter;
	gchar *row = get_formatted_song ( filename );
  
  if ( row == NULL )
    return;

	gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, "", 1, row, 2, uri, -1);

  g_free ( row );
}

int
main (int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *controls;
  GtkCellRenderer     *renderer;
  /* GtkListStore        *store; Esto es global ahora, yeah! sujetense los pantalones. 
  GtkWidget           *view; */
  GtkWidget           *scrollview;

  gtk_init(&argc, &argv);
  
  window = tbw_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect( window, "destroy", G_CALLBACK ( gtk_main_quit ), NULL );  
  
  view = gtk_tree_view_new ();
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (view), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (view), TRUE);
	gtk_tree_selection_set_mode (GTK_TREE_SELECTION (gtk_tree_view_get_selection (GTK_TREE_VIEW (view))), GTK_SELECTION_SINGLE);
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view), -1,"Columna", renderer, "markup", 0, NULL);
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view), -1,"Columna", renderer, "markup", 1, NULL);
  store = gtk_list_store_new (3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL(store));  

  scrollview = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollview), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (scrollview), GTK_WIDGET (view));

  controls = tbw_controls_new ( store );    

  tbw_window_pack ( scrollview, TRUE, TRUE, 0 );
  tbw_window_pack ( controls, FALSE, FALSE, 0 );  

  tabu_player_main();
  gtk_main();

  return 0;
}
