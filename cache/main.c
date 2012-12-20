#include "utt_cache.h"

struct utt_cache_item item0 = {
  .name = "item0",
};

int
main (int argc, char *argv[])
{
  struct utt_cache *cache;

  cache = utt_cache_new ();
  utt_cache_set_cachefile (cache, "cache-file");
  utt_cache_register (cache, &item0);
  utt_cache_flush (cache);
  utt_cache_destroy (cache);
  return 0;
}
