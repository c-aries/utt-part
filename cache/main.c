#include "utt_cache.h"

void
query_content (struct utt_cache *cache)
{
  struct utt_cache_item *item = NULL;
  gint page;

  item = utt_cache_query (cache, "utt-wubi/page");
  if (item && item->data_size == sizeof (page)) {
    page = *(gint *)item->data;
    g_print ("page = %d\n", page);
  }
  g_free (item);
}

void
add_content (struct utt_cache *cache)
{
  gint page = 2;

  utt_cache_add (cache, "utt-wubi/page", &page, sizeof (page));
  utt_cache_add (cache, "utt/main", &page, sizeof (page));
  utt_cache_add (cache, "utt-wubi/info", &page, sizeof (page));
  page = 3;
  utt_cache_update (cache, "utt-wubi/page", &page, sizeof (page));
  query_content (cache);
}

int
main (int argc, char *argv[])
{
  struct utt_cache *cache;

  cache = utt_cache_new ();
  utt_cache_set_cachefile (cache, "cache-file");
  /* add_content (cache); */
  query_content (cache);
  /* utt_cache_flush (cache); */
  utt_cache_destroy (cache);
  return 0;
}
