#ifndef __UTT_CACHE_H__
#define __UTT_CACHE_H__

#include <glib.h>
#include <glib/gstdio.h>

struct cache_node {
  gchar *name;
  gchar *data;
  gint data_size;
  struct cache_node *children;
  struct cache_node *sibling;
  struct cache_node *parent;
};
struct cache_root {
  struct cache_node *children;
};
struct utt_cache {
  struct cache_root *root;
  gchar *cachefile;
};
struct utt_cache_item {
  void *data;
  gint data_size;
};

struct utt_cache *utt_cache_new ();
void utt_cache_destroy (struct utt_cache *cache);
gboolean utt_cache_set_cachefile (struct utt_cache *cache, gchar *path);
void utt_cache_flush (struct utt_cache *cache);
gboolean utt_cache_add (struct utt_cache *cache, gchar *name, void *data, gint data_size);
gboolean utt_cache_update (struct utt_cache *cache, gchar *name, void *data, gint data_size);
struct utt_cache_item *utt_cache_query (struct utt_cache *cache, gchar *name);

#endif
