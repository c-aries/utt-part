#ifndef __UTT_CACHE_H__
#define __UTT_CACHE_H__

#include <glib.h>
#include <glib/gstdio.h>

struct utt_cache {
  GList *list;
  FILE *savefile;
  void *data;
};
struct utt_cache_ops {
  void (*restore_from_disk) (struct utt_cache *cache);
};
struct utt_cache_item {
  gchar *name;
  struct utt_cache_ops *op;
  void *data;
};

struct utt_cache *utt_cache_new ();
void utt_cache_destroy (struct utt_cache *cache);
gboolean utt_cache_load_cachefile (struct utt_cache *cache, gchar *path);
gboolean utt_cache_set_cachefile (struct utt_cache *cache, gchar *path);
void utt_cache_register (struct utt_cache *cache, struct utt_cache_item *item);
void utt_cache_flush (struct utt_cache *cache);

#endif

