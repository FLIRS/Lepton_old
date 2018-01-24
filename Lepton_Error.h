#pragma once


#ifndef Lepton_Assert
#define Lepton_Assert(A, Importance, Message, ...)
#endif

#ifndef Lepton_Log
#define Lepton_Log(Importance, Message, ...)
#endif


/*
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>

#define KWHT "\x1B[1;37;40m"
#define KMAG "\x1B[1;35;40m"
#define KYEL "\x1B[1;33;40m"
#define KNRM "\x1B[0m"

static void Lepton_Logger 
(
   int Uniqid, 
   int const Importance,
   char const * Assertion,
   char const * Function_Name, 
   char const * File_Name, 
   int const Line,
   char * const Format,
   ...
)
{
   va_list Ap;
   va_start (Ap, Format);
   switch (Importance)
   {
      case 0:
         fprintf (stderr, ""KYEL"%d. "KNRM "" KWHT"%s:%d"KNRM": "KMAG"runtime log"KNRM". ", Uniqid, File_Name, Line);
         vfprintf (stderr, Format, Ap);
         fprintf (stderr, "\n");
      break;
      default:
         fprintf (stderr, "Uniq Id           : %i\n", Uniqid);
         fprintf (stderr, "Importance        : %i\n", Importance);
         fprintf (stderr, "Assertion         : %s\n", Assertion);
         fprintf (stderr, "Function_Name     : %s\n", Function_Name);
         fprintf (stderr, "File_Name         : %s\n", File_Name);
         fprintf (stderr, "Line              : %i\n", Line);
         fprintf (stderr, "errno             : %i\n", errno);
         fprintf (stderr, "strerror          : %s\n", strerror (errno));
         vfprintf (stderr, Format, Ap);
         fprintf (stderr, "\n\n");
         abort ();
      break;
   };
   va_end (Ap);
}

#ifndef Lepton_Assert
#define Lepton_Assert(A, Importance, Message, ...) if (!(A)) {Lepton_Logger(__COUNTER__, Importance, #A, __func__, __FILE__, __LINE__, Message, __VA_ARGS__); }
#endif

#ifndef Lepton_Log
#define Lepton_Log(      Importance, Message, ...) Lepton_Logger(__COUNTER__, Importance, "", __func__, __FILE__, __LINE__, Message, __VA_ARGS__)
#endif
*/
