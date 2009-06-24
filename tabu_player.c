#include <gst/gst.h>
#include <string.h>
#include "tbw.h"

static gboolean playing = FALSE;
static GstElement *pipeline = NULL;
static gchar *current_song = NULL;
static GMainLoop *loop = NULL;
static GtkTreeIter iter;
static gchar *pointer = "<span size='smaller'>></span>";

gboolean
tabu_player_is_playing ()
{
  return ( playing );
}

void 
tabu_player_next ()
{
  gtk_list_store_set ( get_tabu_playlist(), &iter, 0, "", -1 );

  if ( gtk_tree_model_iter_next ( GTK_TREE_MODEL ( get_tabu_playlist () ), &iter ) )
  {
    g_free ( current_song );
    gtk_tree_model_get ( GTK_TREE_MODEL ( get_tabu_playlist () ), &iter, 2, &current_song, -1 );    
    g_object_set( G_OBJECT ( pipeline ), "uri", current_song, NULL );
  }
  else if ( gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( get_tabu_playlist () ), &iter ) )
  {
    g_free ( current_song );
    gtk_tree_model_get ( GTK_TREE_MODEL( get_tabu_playlist () ), &iter, 2, &current_song, -1);
    g_object_set(G_OBJECT(pipeline), "uri", current_song, NULL);
  }
  gtk_list_store_set ( get_tabu_playlist(), &iter, 0, pointer, -1 );
  scroll_to_song ( iter );

  gst_element_set_state ( GST_ELEMENT ( pipeline ), GST_STATE_NULL );
  gst_element_set_state ( GST_ELEMENT ( pipeline ), GST_STATE_PLAYING );
  playing = TRUE;
}

void 
tabu_player_previous ()
{
  GtkTreePath *path;

  gtk_list_store_set ( get_tabu_playlist(), &iter, 0, "", -1 );
  path = gtk_tree_model_get_path ( GTK_TREE_MODEL ( get_tabu_playlist () ), &iter );

  if ( gtk_tree_path_prev ( path ) )
  {
    g_free ( current_song );
    gtk_tree_model_get_iter ( GTK_TREE_MODEL ( get_tabu_playlist () ), &iter, path );
    gtk_tree_model_get ( GTK_TREE_MODEL ( get_tabu_playlist () ), &iter, 2, &current_song, -1 );    
    g_object_set( G_OBJECT ( pipeline ), "uri", current_song, NULL );
  }

  gtk_list_store_set ( get_tabu_playlist(), &iter, 0, pointer, -1 );
  scroll_to_song ( iter );

  gst_element_set_state ( GST_ELEMENT ( pipeline ), GST_STATE_NULL );
  gst_element_set_state ( GST_ELEMENT ( pipeline ), GST_STATE_PLAYING );
  playing = TRUE;                                            
}

void
tabu_player_clear_playlist ( )
{
  if ( current_song )
  {
    g_free ( current_song );
    current_song = NULL;    
  }
 
  /* stop playing the current song */
  gst_element_set_state ( GST_ELEMENT ( pipeline ), GST_STATE_NULL );
  playing = FALSE;  

  /* clear the playlist */
  gtk_list_store_clear ( get_tabu_playlist() );
}

void
tabu_player_play_selection ( GtkTreeSelection *selection)
{
  GtkTreeModel *store;
  gtk_list_store_set ( get_tabu_playlist(), &iter, 0, "", -1 );

  if ( gtk_tree_selection_get_selected ( selection, &store, &iter ) )
  {
    g_free ( current_song );

    gst_element_set_state ( GST_ELEMENT ( pipeline ), GST_STATE_NULL );
    gtk_tree_model_get ( GTK_TREE_MODEL( get_tabu_playlist () ), &iter, 2, &current_song, -1);

    gtk_list_store_set ( get_tabu_playlist(), &iter, 0, pointer, -1 );
    scroll_to_song ( iter );     

    g_object_set(G_OBJECT(pipeline), "uri", current_song, NULL);

    gst_element_set_state ( GST_ELEMENT ( pipeline ), GST_STATE_PLAYING );
    playing = TRUE;
  }
}

void
tabu_player_remove_selection ( GtkTreeSelection *selection )
{
  GtkTreeModel *store;

  if ( gtk_tree_selection_get_selected ( selection, &store, &iter ) )
  {
    gchar *song = NULL;
    gtk_tree_model_get ( GTK_TREE_MODEL ( get_tabu_playlist ( ) ), &iter, 2, &song, -1 );

    if ( song && current_song )
    {
      /* if the song to be removed is the currently playing song */
      if ( !strcmp ( song, current_song ) )
      {
        /* stop playback */
        gst_element_set_state ( GST_ELEMENT ( pipeline ), GST_STATE_NULL );
        g_free ( current_song );
        current_song = NULL;
        playing = FALSE;
      }      
    }

    g_free ( song );

    /* ok, now remove the song from the playlist */
    gtk_list_store_remove ( GTK_LIST_STORE ( store ), &iter );
  }
}

void 
tabu_player_play ()
{
  if ( current_song == NULL )
  {
    if ( gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( get_tabu_playlist () ), &iter ) )
    {      
      gtk_tree_model_get ( GTK_TREE_MODEL( get_tabu_playlist () ), &iter, 2, &current_song, -1);
      gtk_list_store_set ( get_tabu_playlist(), &iter, 0, pointer, -1 );
      scroll_to_song ( iter );     
      g_object_set(G_OBJECT(pipeline), "uri", current_song, NULL);
    }
  }

  if ( playing )
  {
    gst_element_set_state ( GST_ELEMENT ( pipeline ), GST_STATE_PAUSED );
    playing = FALSE;
  }
  else 
  {
    gst_element_set_state ( GST_ELEMENT ( pipeline ), GST_STATE_PLAYING );
    playing = TRUE;
  }
}

static gboolean
bus_call ( GstBus *bus, GstMessage *msg, gpointer data )
{
  switch ( GST_MESSAGE_TYPE ( msg ) )
  {
    case GST_MESSAGE_EOS:
    {
      g_print ( "End-of-stream\n" );
      tabu_player_next ( );
      break;
    }
    case GST_MESSAGE_ERROR:
    {
      gchar *debug;
      GError *error;
    
      gst_message_parse_error ( msg, &error, &debug );
      g_free ( debug );
      
      g_print ( "Error: %s\n", error->message );
      g_main_loop_quit ( loop );
      break;
    }
    default:
      break;
  }

  return TRUE;
}

void
tabu_player_quit ()
{
  /* g_main_loop_quit ( loop ); */

  if ( pipeline != NULL )
  {
    g_free ( current_song );
    gst_element_set_state ( GST_ELEMENT ( pipeline ), GST_STATE_NULL );
    gst_object_unref ( GST_OBJECT ( pipeline ) );
    pipeline = NULL;
  }         
}

void
tabu_player_main ()
{
  GstBus *bus;

  /* initialize GStreamer */
  gst_init ( NULL, NULL );
  /* loop = g_main_loop_new ( NULL, FALSE ); */

  pipeline = gst_element_factory_make ( "playbin", "player" );
  bus = gst_pipeline_get_bus (GST_PIPELINE ( pipeline ) );
  gst_bus_add_watch ( bus, bus_call, NULL );
  gst_object_unref ( bus );

  /* g_main_loop_run ( loop ); */
}
