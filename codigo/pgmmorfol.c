/*********************************************************************************
 Module:       pgmmorfol.c
 Description:  
 Date:         08-12-1998
 Author:       Alberto Perez (ligeramente modificado por J.C. Perez)
 ********************************************************************************/

/* includes */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pgm.h>

/* constants */
#define TRUE   1
#define FALSE  0

#define OBJECT        0
#define BACKGROUND  255

/* code */

void pgm_dilate(
		gray  **pgm, 
		int     cols, 
		int     rows, 
		int     num
		)
{
  int     n, i, j;
  gray  **tmp_pgm;

  /* allocate space */
  tmp_pgm = pgm_allocarray(cols, rows);
  if (tmp_pgm == NULL)
    pm_error("no more memory.");

  for (n=0; n < num; n++) {

    /* copy pgm -> tmp_pgm */
    for (i=0; i < rows; i++)
      for (j=0; j < cols; j++)
	tmp_pgm[i][j] = pgm[i][j];
    
    /* dilate 1 pixel */
    for (i=1; i < rows-1; i++)
      for (j=1; j < cols-1; j++)
	if (tmp_pgm[i][j] == BACKGROUND &&
	    (tmp_pgm[i-1][j-1] == OBJECT ||
	     tmp_pgm[i-1][j  ] == OBJECT ||
	     tmp_pgm[i-1][j+1] == OBJECT ||
	     tmp_pgm[i  ][j-1] == OBJECT ||
	     tmp_pgm[i  ][j+1] == OBJECT ||
	     tmp_pgm[i+1][j-1] == OBJECT ||
	     tmp_pgm[i+1][j  ] == OBJECT ||
	     tmp_pgm[i+1][j+1] == OBJECT))
	  pgm[i][j] = OBJECT;

  } /* end for */

  pgm_freearray(tmp_pgm, rows);
  
} /* end pgm_dilate() */

void pgm_erode(
		gray  **pgm, 
		int     cols, 
		int     rows, 
		int     num
)		
{
  int     n, i, j;
  gray  **tmp_pgm;

  /* allocate space */
  tmp_pgm = pgm_allocarray(cols, rows);
  if (tmp_pgm == NULL)
    pm_error("no more memory.");

  for (n=0; n < num; n++) {

    /* copy pgm -> tmp_pgm */
    for (i=0; i < rows; i++)
      for (j=0; j < cols; j++)
	tmp_pgm[i][j] = pgm[i][j];
    
    /* dilate 1 pixel */
    for (i=1; i < rows-1; i++)
      for (j=1; j < cols-1; j++)
	if (tmp_pgm[i][j] == OBJECT &&
	    (tmp_pgm[i-1][j-1] == BACKGROUND ||
	     tmp_pgm[i-1][j  ] == BACKGROUND ||
	     tmp_pgm[i-1][j+1] == BACKGROUND ||
	     tmp_pgm[i  ][j-1] == BACKGROUND ||
	     tmp_pgm[i  ][j+1] == BACKGROUND ||
	     tmp_pgm[i+1][j-1] == BACKGROUND ||
	     tmp_pgm[i+1][j  ] == BACKGROUND ||
	     tmp_pgm[i+1][j+1] == BACKGROUND))
	  pgm[i][j] = BACKGROUND;

  } /* end for */

  pgm_freearray(tmp_pgm, rows);
  
} /* end pgm_erode() */

void main(int argc, char *argv[])
{
  char   *prog_name;
  char    usage[]="[-d #dilataciones] [-e #erosiones] [<pgm_file>]";
  int     argn;
  FILE   *fd_in=NULL; 
  int     dilataciones=0, erosiones=0;

  gray  **pgm_in;
  int     cols, rows;
  gray    maxval;

  /* init */
  prog_name = argv[0];
  pgm_init(&argc, argv);

  /* reading parameters ... */
  argn = 1; 
  while ( argn < argc && argv[argn][0] == '-' && argv[argn][1] != '\0' ) {
    if ( pm_keymatch( argv[argn], "-d", 2 ) ) {
      ++argn;
      if ( argn == argc )
        pm_usage( usage );
      if (sscanf(argv[argn], "%d", &dilataciones) != 1)
        pm_usage( usage );
      if (dilataciones < 0) 
	pm_error("-d # must be at least 0.");
    }
    else if ( pm_keymatch( argv[argn], "-e", 2 ) ) {
      ++argn;
      if ( argn == argc )
        pm_usage( usage );
      if (sscanf(argv[argn], "%d", &erosiones) != 1)
        pm_usage( usage );
      if (erosiones < 0) 
	pm_error("-e # must be al least 0.");
    } 
    else 
      pm_usage( usage );
    argn++;
  } /* end while */

  if (argn < argc)
    fd_in = pm_openr(argv[argn]);
  else
    fd_in = stdin;
      
  /* reading input image */ 
  pgm_in = pgm_readpgm(fd_in, &cols, &rows, &maxval);
  if (pgm_in == NULL)
    pm_error("No more memory");
  
  /* dilate */
  pgm_dilate(pgm_in, cols, rows, dilataciones);
  /* erode */
  pgm_erode(pgm_in, cols, rows, erosiones);

  /* out imagen */
  pgm_writepgm(stdout, pgm_in, cols, rows, 255, 0);

  /* closing & freeing */
  pm_close(fd_in);
  pgm_freearray(pgm_in, rows);

  /* saing bye, bye */
  exit(0);

} /* end main() */









