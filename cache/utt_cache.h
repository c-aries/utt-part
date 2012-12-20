#ifndef __UTT_CACHE_H__
#define __UTT_CACHE_H__

#include <glib.h>
#include <glib/gstdio.h>

struct utt_cache {
  GList *list;
  FILE *savefile;
  void *data;
};

struct utt_cache *utt_cache_new ();
void utt_cache_destroy (struct utt_cache *cache);
gboolean utt_cache_load_cachefile (struct utt_cache *cache, gchar *path);

#endif

