#include <errno.h>
#include <string.h>
#include <stdio.h>

enum Lepton_Debug
{
   Lepton_Debug_None = 0,
   Lepton_Debug_stderr = 1,
   Lepton_Debug_assert = 2,
   Lepton_Debug_Extra = 4
};

void Lepton_Print_Error (FILE * File, int Result, int E)
{
   fprintf (File, "R : %d, errno : %d (%s)\n", Result, errno, strerror (errno));
}

