#include "utt_cache.h"

int
main (int argc, char *argv[])
{
  struct utt_cache *cache;

  cache = utt_cache_new ();
  utt_cache_load_cachefile (cache, "cache-file");
  utt_cache_destroy (cache);
  return 0;
}
