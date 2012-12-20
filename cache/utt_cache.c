#include "utt_cache.h"

void
utt_cache_register (struct utt_cache *cache, struct utt_cache_item *item)
{
  cache->list = g_list_prepend (cache->list, item);
}

static void
list_func (struct utt_cache_item *item, struct utt_cache *cache)
{
  g_print ("%s\n", item->name);
}

void
utt_cache_flush (struct utt_cache *cache)
{
  g_list_foreach (cache->list, (GFunc)list_func, cache);
}

struct utt_cache *
utt_cache_new ()
{
  struct utt_cache *cache;

  cache = g_new0 (struct utt_cache, 1);
  return cache;
}

gboolean
utt_cache_set_cachefile (struct utt_cache *cache, gchar *path)
{
  FILE *fp;

  if (!cache) {
    return FALSE;
  }
  fp = g_fopen (path, "wb+");
  if (!fp) {
    g_warning ("open %s fail", path);
    return FALSE;
  }
  
  if (cache->savefile) {
    fclose (cache->savefile);
  }
  cache->savefile = fp;
  /* parse and restore cache list data */
  return TRUE;
}

gboolean
utt_cache_load_cachefile (struct utt_cache *cache, gchar *path)
{
  FILE *fp;

  if (!cache) {
    return FALSE;
  }
  fp = g_fopen (path, "wb+");
  if (!fp) {
    g_warning ("open %s fail", path);
    return FALSE;
  }
  
  if (cache->savefile) {
    fclose (cache->savefile);
  }
  cache->savefile = fp;
  /* parse and restore cache list data */
  return TRUE;
}

void
utt_cache_destroy (struct utt_cache *cache)
{
  if (!cache) {
    return;
  }
  if (cache->list) {
    g_list_free (cache->list);
  }
  if (cache->savefile) {
    fclose (cache->savefile);
  }
  g_free (cache);
}
