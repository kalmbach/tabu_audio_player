/* tabu-playlist.h */

#ifndef _TABU_PLAYLIST_H
#define _TABU_PLAYLIST_H

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TABU_PLAYLIST(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  TABU_TYPE_PLAYLIST, TabuPlaylist))

#define TABU_PLAYLIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  TABU_TYPE_PLAYLIST, TabuPlaylistClass))

#define TABU_IS_PLAYLIST(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  TABU_TYPE_PLAYLIST))

typedef struct _TabuPlaylist TabuPlaylist;
typedef struct _TabuPlaylistClass TabuPlaylistClass;

struct _TabuPlaylist
{
  GtkScrolledWindow parent;

  GtkWidget *tree_view;
  GtkCellRenderer *cell_renderer;
  GtkTreeViewColumn *tree_view_column;
  GtkListStore *list_store;
};


struct _TabuPlaylistClass
{
  GtkScrolledWindowClass parent_class;
};

GType tabu_playlist_get_type (void);

GtkWidget *tabu_playlist_new (void);

G_END_DECLS

#endif /* _TABU_PLAYLIST_H */
