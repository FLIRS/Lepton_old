//gcc test_stress2.c -std=gnu11 -fdiagnostics-color -Wall -Wno-missing-braces -o test_stress2 && ./test_stress2

#include "../Lepton_Major.h"

//printf
#include <stdio.h>

//timerfd_create
#include <sys/timerfd.h>

//read
#include <unistd.h>

//errno
#include <errno.h>

//uint64_t
#include <stdint.h>

//strerror
#include <string.h>

//malloc
#include <stdlib.h>


int main (int argc, char * argv [])
{
   struct Lepton_Major Major [1];
   
   Lepton_Major_Construct (Major);
   Lepton_Major_Open (Major, "/dev/spidev0.0", NULL);
   Lepton_Major_Printf (Major);
   
   int Timer;
   struct itimerspec Timer_Specification;
   uint64_t Timer_Count;
   Timer = timerfd_create (CLOCK_MONOTONIC, 0);
   Timer_Specification.it_value.tv_sec = 0;
   Timer_Specification.it_value.tv_nsec = Lepton_Nano_Seconds_Per_Frame / 2;
   Timer_Specification.it_interval.tv_sec = 0;
   Timer_Specification.it_interval.tv_nsec = Lepton_Nano_Seconds_Per_Frame / 2;
   timerfd_settime (Timer, 0, &Timer_Specification, NULL);
   
   while (1)
   {
      int Result;
      Result = Lepton_Major_Transfer (Major);
      if (Result) {};
      read (Timer, &Timer_Count, sizeof (uint64_t));
      
      if (Major->Transaction_Count % 100 == 0)
      {
         Lepton_Major_Printf (Major);
      }
   }
}


