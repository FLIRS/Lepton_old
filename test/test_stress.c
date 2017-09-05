//gcc test_stress.c -std=gnu11 -fdiagnostics-color -Wall -Wno-missing-braces -o test_stress && ./test_stress

#include "../Lepton.h"
#include "../Lepton_Packets.h"
#include "../Lepton_SPI.h"
#include "../Lepton_Transfer.h"

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


struct App_Statistic
{
   uint64_t Count;
   uint64_t Result [5];
};

int main (int argc, char * argv [])
{
   int Device;
   Device = Lepton_SPI_Open ("/dev/spidev0.0");
   
   struct Lepton_Frame Frame [1];
   
   struct App_Statistic Statistic [1] = {0};
   
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
      Result = Lepton_Transfer_Frame (Frame, Device);
      Statistic->Count = Statistic->Count + 1;
      Statistic->Result [Result] = Statistic->Result [Result] + 1;
      
      //read (Timer, &Timer_Count, sizeof (uint64_t));
      
      if (Statistic->Count % 100 == 0)
      {
         printf ("Count                                     %lli \n", Statistic->Count);
         printf ("Lepton_Transfer_Status_Ok                 %lli \n", Statistic->Result [Lepton_Transfer_Status_Ok]);
         printf ("Lepton_Transfer_Status_Discard            %lli \n", Statistic->Result [Lepton_Transfer_Status_Discard]);
         printf ("Lepton_Transfer_Status_Unsynced           %lli \n", Statistic->Result [Lepton_Transfer_Status_Unsynced]);
         printf ("Lepton_Transfer_Status_CRC_Mismatch_1     %lli \n", Statistic->Result [Lepton_Transfer_Status_CRC_Mismatch_1]);
         printf ("Lepton_Transfer_Status_CRC_Mismatch_2     %lli \n", Statistic->Result [Lepton_Transfer_Status_CRC_Mismatch_2]);
         printf ("\n");
      }
   }
}

