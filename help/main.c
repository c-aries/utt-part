#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

int
main (int argc, char *argv[])
{
  gtk_init (&argc, &argv);
  gtk_main ();
  exit (EXIT_SUCCESS);
}
