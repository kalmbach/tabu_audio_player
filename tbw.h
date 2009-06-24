#ifndef __TBW_H__
#define __TBW_H__

#include <gtk/gtk.h>

/* Functions of tabu.c */
void add_item_to_playlist ( GtkListStore *store, gchar* filename, gchar *uri );
GtkListStore *get_tabu_playlist ( );
void scroll_to_song ( GtkTreeIter iter );
void clear_tabu_playlist ( );

/* Functions of tbw_window.c */
void tbw_window_pack ( GtkWidget *widget, gboolean expand, gboolean fill, guint padding );
GtkWidget *tbw_window_new ( GtkWindowType type );

/* Functions of tbw_controls.c */
GtkWidget *tbw_controls_new ( GtkListStore *widget );

/* Functions of tabu_player.c */
void tabu_player_quit ( );
void tabu_player_main ( );
void tabu_player_next ( );
void tabu_player_previous ( );
void tabu_player_play ( );
void tabu_player_clear_playlist ( );
void tabu_player_play_selection ( GtkTreeSelection *selection );
void tabu_player_remove_selection ( GtkTreeSelection *selection );
gboolean tabu_player_is_playing ( );

#endif

