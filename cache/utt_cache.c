#include <string.h>
#include "utt_cache.h"

typedef gboolean (*UttCheckFunc) (gpointer data, gpointer user_data);

/* deep first search */
static void
recursive_iterate_children (struct cache_node *node, UttCheckFunc cfunc, GFunc func, gpointer user_data)
{
  struct cache_node *sibling;

  if (!func) {
    return;
  }
  while (node) {
    recursive_iterate_children (node->children, cfunc, func, user_data);
    sibling = node->sibling;
    if (cfunc (node, user_data)) {
      func (node, user_data);
    }
    /* may node is already free */
    node = sibling;
  }
}

/*
 * cache_node_path:
 * @node: the cache_node object
 *
 * Get node's full path, node name separate by '/'.
 *
 * Return value: node path, value should be free with g_free ().
 */
static gchar *
cache_node_path (struct cache_node *node)
{
  g_return_val_if_fail (node, NULL);
  GList *list = NULL;
  gchar *name, *buf;
  gint len = 0;

  do {
    len += strlen (node->name) + 1;
    list = g_list_prepend (list, node->name);
    node = node->parent;
  } while (node);
  buf = g_malloc0 (len);
  do {
    if (buf[0] != '\0') {
      strncat (buf, "/", len);
    }
    name = list->data;
    strncat (buf, name, len);
    list = g_list_next (list);
  } while (list);
  return buf;
}

static void
append_to_cachefile (struct cache_node *node, FILE *fp)
{
  gchar *fullname;

  fullname = cache_node_path (node);
  fwrite (fullname, strlen (fullname) + 1, 1, fp);
  fwrite (&node->data_size, sizeof (node->data_size), 1, fp);
  fwrite (node->data, node->data_size, 1, fp);
  g_free (fullname);
}

static gboolean
check_if_node_have_data (struct cache_node *node, gpointer user_data)
{
  g_return_val_if_fail (node, FALSE);
  return !!node->data;
}

void
utt_cache_flush (struct utt_cache *cache)
{
  FILE *fp;

  if (!(cache && cache->root && cache->root->children)) {
    return;
  }

  fp = g_fopen (cache->cachefile, "wb");
  if (!fp) {
    g_warning ("open cachefile '%s' fail", cache->cachefile);
    return;
  }
  recursive_iterate_children (cache->root->children, (UttCheckFunc)check_if_node_have_data,
			      (GFunc)append_to_cachefile, fp);
  fclose (fp);
}

struct utt_cache *
utt_cache_new ()
{
  struct utt_cache *cache;

  cache = g_new0 (struct utt_cache, 1);
  return cache;
}

static void
load_cachefile (struct utt_cache *cache, FILE *fp)
{
  #define FULLNAME_MAXSIZE 256
  gchar fullname[FULLNAME_MAXSIZE];
  gchar ch;
  gint ret, i, data_size;
  gchar *data;

  i = 0;
  for (;;) {
    ret = fread (&ch, sizeof (ch), 1, fp);
    if (ret != 1) {
      break;
    }
    fullname[i++] = ch;
    if (ch == '\0') {
      ret = fread (&data_size, sizeof (data_size), 1, fp);
      if (ret != 1) {
	g_warning ("read data_size fail");
	break;
      }
      data = g_malloc (data_size);
      ret = fread (data, data_size, 1, fp);
      if (ret != 1) {
	g_warning ("read data fail");
	g_free (data);
	break;
      }
      utt_cache_add (cache, fullname, data, data_size);
      g_free (data);
      i = 0;
      continue;
    }
    if (i >= FULLNAME_MAXSIZE) {
      g_warning ("fullname size is bigger than %d", FULLNAME_MAXSIZE);
      break;
    }
  }
  #undef FULLNAME_MAXSIZE
}

gboolean
utt_cache_set_cachefile (struct utt_cache *cache, gchar *path)
{
  FILE *fp;

  if (!cache || !path) {
    return FALSE;
  }

  if (cache->cachefile) {
    g_free (cache->cachefile);
  }
  cache->cachefile = g_strdup (path);

  fp = g_fopen (path, "rb");
  if (!fp) {
    return FALSE;
  }
  load_cachefile (cache, fp);
  fclose (fp);
  return TRUE;
}

static void
free_tree_node (struct cache_node *node, void *user_data)
{
  g_free (node->name);
  node->name = NULL;
  g_free (node->data);
  node->data = NULL;
  g_free (node);
}

static gboolean
always_return_true (gpointer data, gpointer user_data)
{
  return TRUE;
}

static void
free_cache_tree (struct cache_root *root)
{
  if (!root) {
    return;
  }
  recursive_iterate_children (root->children, always_return_true,
			      (GFunc)free_tree_node, NULL);
  g_free (root);
}

void
utt_cache_destroy (struct utt_cache *cache)
{
  if (!cache) {
    return;
  }
  if (cache->cachefile) {
    g_free (cache->cachefile);
  }
  free_cache_tree (cache->root);
  g_free (cache);
}

static gboolean
validate_cache_char (gchar ch)
{
  if (g_ascii_isalnum (ch) || ch == '-' || ch == '_') {
    return TRUE;
  }
  return FALSE;
}

static gboolean
validate_cache_name (gchar *name)
{
  gint i, end;

  g_return_val_if_fail (name, FALSE);
  end = strlen (name) - 1;
  if (!(end >= 0 && validate_cache_char (name[0]) && validate_cache_char (name[end]))) {
    g_warning ("invalidate name '%s'", name);
    return FALSE;
  }
  for (i = 1; i < end; i++) {
    if (validate_cache_char (name[i])) {
      continue;
    }
    if (name[i] != '/' ||
	(name[i] == '/' && name[i + 1] == '/')) {
      g_warning ("invalidate name '%s'\n", name);
      return FALSE;
    }
  }
  return TRUE;
}

static struct cache_node *
cache_children_find_name (struct cache_node *children, gchar *name)
{
  struct cache_node *node = children;

  while (node && g_strcmp0 (node->name, name) != 0) {
    node = node->sibling;
  }
  return node;
}

/*
 * utt_cache_add_internal:
 * @cache: the utt_cache object
 * @name: the register cache path
 * @data: save data
 * @data_size: @data size
 * @replace: if found data already exist, replace it or not.
 *
 * Add custom data to utt cahce tree.
 * @data bind with @name, its size is @data_size.
 *
 * Return value: TRUE, if add success, FALSE on fail.
 */
static gboolean
utt_cache_add_internal (struct utt_cache *cache, gchar *name, void *data, gint data_size, gboolean replace)
{
  struct cache_node **children, *new_node, *node, *parent;
  gchar **vector;
  gboolean ret = FALSE;
  gint i;

  g_return_val_if_fail (cache && data && (data_size > 0), FALSE);
  g_return_val_if_fail (validate_cache_name (name), FALSE);

  if (G_UNLIKELY (!cache->root)) {
    cache->root = g_new0 (struct cache_root, 1);
  }
  children = &cache->root->children;
  parent = NULL;
  vector = g_strsplit (name, "/", 0);
  for (i = 0; vector[i]; i++) {
    node = cache_children_find_name (*children, vector[i]);
    if (!node) {
      new_node = g_new0 (struct cache_node, 1);
      new_node->name = g_strdup (vector[i]);
      if (!vector[i + 1]) {
	new_node->data_size = data_size;
	new_node->data = g_malloc (data_size);
	memcpy (new_node->data, data, data_size);
	new_node->parent = parent;
      }
      if (*children) {
	new_node->sibling = *children;
      }
      *children = new_node;
      children = &new_node->children;
      parent = new_node;
    }
    else {
      if (!vector[i + 1]) {
	if (replace) {
	  g_free (node->data);
	  node->data_size = data_size;
	  node->data = g_malloc (data_size);
	  memcpy (node->data, data, data_size);
	  ret = TRUE;
	}
	else {
	  ret = FALSE;
	}
	break;
      }
      children = &node->children;
      parent = node;
    }
  }
  g_strfreev (vector);
  return ret;
}

gboolean
utt_cache_add (struct utt_cache *cache, gchar *name, void *data, gint data_size)
{
  return utt_cache_add_internal (cache, name, data, data_size, FALSE);
}

gboolean
utt_cache_update (struct utt_cache *cache, gchar *name, void *data, gint data_size)
{
  return utt_cache_add_internal (cache, name, data, data_size, TRUE);
}

struct utt_cache_item *
utt_cache_query (struct utt_cache *cache, gchar *name)
{
  struct cache_node *children, *node;
  struct utt_cache_item *item = NULL;
  gchar **vector;
  gint i;

  if (!(cache && cache->root && validate_cache_name (name))) {
    return NULL;
  }

  children = cache->root->children;
  vector = g_strsplit (name, "/", 0);
  for (i = 0; vector[i]; i++) {
    node = cache_children_find_name (children, vector[i]);
    if (!node) {
      break;
    }
    children = node->children;
  }
  g_strfreev (vector);
  if (node) {
    item = g_new0 (struct utt_cache_item, 1);
    item->data = node->data;
    item->data_size = node->data_size;
  }
  return item;
}
