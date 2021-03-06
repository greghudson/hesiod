/* Copyright 1988, 1996 by the Massachusetts Institute of Technology.
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 * M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

/* This file is a simple driver for the Hesiod library. */

static const char rcsid[] = "$Id: hesinfo.c,v 1.5 2000-01-05 22:00:46 ghudson Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "hesiod.h"

extern int optind;

int main(int argc, char **argv)
{
  char **list, **p, *bindname, *name, *type;
  int lflag = 0, errflg = 0, bflag = 0, c;
  void *context;

  while ((c = getopt(argc, argv, "lb")) != EOF)
    {
      switch (c)
	{
	case 'l':
	  lflag = 1;
	  break;
	case 'b':
	  bflag = 1;
	  break;
	default:
	  errflg++;
	  break;
	}
    }
  if (argc - optind != 2 || errflg)
    {
      fprintf(stderr,"Usage: %s [-bl] name type\n",argv[0]);
      fprintf(stderr,"\t-l selects long format\n");
      fprintf(stderr,"\t-b also does hes_to_bind conversion\n");
      return 2;
    }

  name = argv[optind];
  type = argv[optind + 1];

  if (hesiod_init(&context) < 0)
    {
      if (errno == ENOEXEC)
	fprintf(stderr, "hesiod_init: Invalid Hesiod configuration file.\n");
      else
	perror("hesiod_init");
    }

  /* Display bind name if requested. */
  if (bflag)
    {
      if (lflag)
	printf("hes_to_bind(%s, %s) expands to\n", name, type);
      bindname = hesiod_to_bind(context, name, type);
      if (!bindname)
	{
	  if (lflag)
	    printf("nothing\n");
	  if (errno == ENOENT)
	    fprintf(stderr, "hesiod_to_bind: Unknown rhs-extension.\n");
	  else
	    perror("hesiod_to_bind");
	  exit(1);
	}
      printf("%s\n", bindname);
      hesiod_free_string(context, bindname);
      if (lflag)
	printf("which ");
    }

  if (lflag) 
    printf("resolves to\n");

  /* Do the hesiod resolve and check for errors. */
  list = hesiod_resolve(context, name, type);
  if (!list)
    { 
      if (lflag)
	printf("nothing\n");
      if (errno == ENOENT)
	fprintf(stderr, "hesiod_resolve: Hesiod name not found.\n");
      else
	perror("hesiod_resolve");
      return 1;
    }

  /* Display the results. */
  for (p = list; *p; p++)
    printf("%s\n", *p);

  hesiod_free_list(context, list);
  hesiod_end(context);
  return 0;
}
