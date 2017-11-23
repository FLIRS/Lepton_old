//gcc test_vsync.c -std=gnu11 -fdiagnostics-color -Wall -Wno-missing-braces -lwiringPi && ./a.out

#include "../Lepton_SPI.h"
#include "../Lepton_Packets.h"
#include "../Lepton_Stream.h"

//printf
#include <stdio.h>

//timerfd_create
#include <sys/timerfd.h>

//read
#include <unistd.h>

//strerror
#include <string.h>

//errrno
#include <errno.h>

//INT_EDGE_RISING
//wiringPiISR
//wiringPiSetup
#include <wiringPi.h>


void Print_Packet (struct Lepton_Packet * P)
{
   uint8_t * D = (uint8_t *) P;
   //for (size_t I = 0; I < sizeof (struct Lepton_Packet); I = I + 1)
   for (size_t I = 0; I < 10; I = I + 1)
   {
      printf ("%02x ", D [I]);
   }
   printf ("\n");
}


int Device;
int Timer;
size_t const Packet_Count = 60;
struct Lepton_Packet Packet [60];


volatile int Segment_Count = 0;
volatile int Segment_Error = 0;
volatile int Segment_Delta = 0;

void Interrupt_Handle ()
{
   int Success;
   Success = Lepton_SPI_Transfer_Stream (Packet, Lepton_Height, Device);
   if (!Success) {return;}
   Success = Lepton_Stream_Validity (Packet, Lepton_Height);
   if (!Success) 
   {
      Segment_Error = Segment_Error + 1;
      Lepton_SPI_Transfer_Stream (Packet, 1, Device);
      return;
   }
   
   Segment_Count = Segment_Count + 1;
   
   return;
   
   for (size_t I = 0; I < Lepton_Height; I = I + 1)
   {
      printf ("%04i ", I);
      printf ("%04i ", (Packet [I].Number));
      printf ("%04i ", (Packet [I].Reserved & 0xF0) >> 4);
      printf ("\n");
   }
   printf ("\n");
   
}


int main (int argc, char * argv [])
{ 
   wiringPiSetup ();
   int Pin = 2;
   //int Edge = INT_EDGE_FALLING;
   int Edge = INT_EDGE_RISING;
   wiringPiISR (Pin, Edge, &Interrupt_Handle);
   

   Device = Lepton_SPI_Open ("/dev/spidev0.0");
   Timer = timerfd_create (CLOCK_MONOTONIC, 0);
   assert (Timer > 0);
   {
      struct itimerspec Config;
      int Result;
      Config.it_interval.tv_sec = 0;
      Config.it_interval.tv_nsec = Lepton3_Nano_Seconds_Per_Frame / 2;
      Config.it_value.tv_sec = 0;
      Config.it_value.tv_nsec = Config.it_interval.tv_nsec;
      Result = timerfd_settime (Timer, 0, &Config, NULL);
      assert (Result == 0);
   }
   
   uint8_t N = 0;
   while (1)
   {
      Segment_Delta = Segment_Count - Segment_Delta;
      if ((N % 8) == 0)
      {
         printf ("| %20.20s | %20.20s | %20.20s |\n", "Segment_Error", "Segment_Count", "Segment_Delta");
      }
      printf ("| %20i | %20i | %20i |\n", Segment_Error, Segment_Count, Segment_Delta);
      Segment_Delta = Segment_Count;
      N = N + 1;
      sleep (1);
   }
   
}



