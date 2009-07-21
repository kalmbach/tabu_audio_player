/* tabu-playlist.c */

#include "tabu-playlist.h"

static GtkType tabu_playlist_type = 0;
static void tabu_playlist_class_init (TabuPlaylistClass *klass);
static void tabu_playlist_init (TabuPlaylist *self);

GType
tabu_playlist_get_type (void)
{
	/* if the type is not registered, then register it */
	if (!tabu_playlist_type)
	{
		static const GTypeInfo tabu_playlist_type_info =
		{
			sizeof (TabuPlaylistClass),
			NULL,
			NULL,
			(GClassInitFunc) tabu_playlist_class_init,
			NULL,
			NULL,
			sizeof (TabuPlaylist),
			0,
			(GInstanceInitFunc) tabu_playlist_init,
		};
		tabu_playlist_type = g_type_register_static (GTK_TYPE_SCROLLED_WINDOW, "TabuPlaylist", &tabu_playlist_type_info, 0);
	}

	return tabu_playlist_type;
}

static void
tabu_playlist_class_init (TabuPlaylistClass *klass)
{
}

static void
tabu_playlist_init (TabuPlaylist *self)
{
  self->tree_view = gtk_tree_view_new ( );

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (self->tree_view), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (self->tree_view), TRUE);

	gtk_tree_selection_set_mode ( 
    GTK_TREE_SELECTION ( 
      gtk_tree_view_get_selection (GTK_TREE_VIEW (self->tree_view))), 
      GTK_SELECTION_SINGLE);

  self->cell_renderer = gtk_cell_renderer_text_new ();

  self->tree_view_column = gtk_tree_view_column_new_with_attributes(
    "",
    self->cell_renderer,
    "markup", 0,
    NULL );

  gtk_tree_view_insert_column (
    GTK_TREE_VIEW (self->tree_view), 
    self->tree_view_column, 
    -1);

  self->list_store = gtk_list_store_new ( 
    2, 
    G_TYPE_STRING, 
    G_TYPE_STRING );

  gtk_tree_view_set_model (
    GTK_TREE_VIEW (self->tree_view), 
    GTK_TREE_MODEL (self->list_store));  
/*
  g_signal_connect (
    G_OBJECT (self->tree_view), 
    "row-activated", 
    G_CALLBACK (playlist_row_activated_callback), 
    NULL);

  g_signal_connect ( 
    G_OBJECT (self->tree_view), 
    "key-press-event", 
    G_CALLBACK (playlist_key_press_callback), 
    NULL);
*/

	gtk_scrolled_window_set_policy (
    GTK_SCROLLED_WINDOW (self), 
    GTK_POLICY_AUTOMATIC, 
    GTK_POLICY_AUTOMATIC);

	gtk_container_add ( 
    GTK_CONTAINER ( GTK_SCROLLED_WINDOW (self)), 
    GTK_WIDGET (self->tree_view));

}

GtkWidget *
tabu_playlist_new (void)
{
  return (GTK_WIDGET (g_object_new (tabu_playlist_get_type(), NULL)));
}
