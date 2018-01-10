#include <errno.h>
#include <string.h>
#include <stdio.h>



void Lepton_Print_Error (FILE * File, int Result, int E)
{
   fprintf (File, "R : %d, errno : %d (%s)\n", Result, errno, strerror (errno));
}

