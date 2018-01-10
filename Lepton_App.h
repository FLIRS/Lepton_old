#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "Lepton_SPI.h"
#include "Lepton_I2C.h"
#include "Lepton_Pixels.h"
#include "Lepton_Stream.h"


struct Lepton_App
{
   int I2C_Device;
   int I2C_Stage;
   int SPI_Device;
   uint16_t Status;
   
   struct Lepton_Pixel_Grayscale16 Pixmap [Lepton3_Width * Lepton3_Height];
   uint32_t Pulse_Count;
   uint32_t Segment_Per_Frame;
   uint32_t Frame_Count;
   int Timer;
   
   time_t Timestamp;
};


bool Lepton_App_Get_FPA_Temperature_Celcius 
(struct Lepton_App * App, float * Temperature)
{
   struct Lepton_I2C_Temperature_100K Temperature_100K;
   App->I2C_Stage = 0;
   Lepton_I2C_Execute 
   (
      App->I2C_Device, 
      Lepton_I2C_Command_FPA_Kelvin, 
      (void *) &Temperature_100K,
      sizeof (struct Lepton_I2C_Temperature_100K),
      &(App->Status),
      &(App->I2C_Stage)
   );
   if (App->I2C_Stage != 0) {return false;}
   *Temperature = (be16toh (Temperature_100K.Value) / 100.0f) - 273.15f;
   return true;
}


void Lepton_App_Enable_Vsync (struct Lepton_App * App)
{
   struct Lepton_I2C_GPIO_Mode Mode;
   Mode.Value = htobe16 (Lepton_I2C_GPIO_Mode_Vsync);
   App->I2C_Stage = 0;
   while (1)
   {
      Lepton_I2C_Execute 
      (
         App->I2C_Device, 
         Lepton_I2C_Command_GPIO_Mode_Set, 
         (void *) &Mode,
         sizeof (struct Lepton_I2C_GPIO_Mode),
         &(App->Status),
         &(App->I2C_Stage)
      );
      if (App->I2C_Stage == 0) {break;}
      usleep (10);
   }
}


void Lepton_App_Reboot (struct Lepton_App * App)
{
   Lepton_I2C_Write_Command (App->I2C_Device, Lepton_I2C_Command_Reboot);
   sleep (3);
   App->Status = Lepton_I2C_Status (App->I2C_Device);
   Lepton_App_Enable_Vsync (App);
}


//
void Lepton_App_Update (struct Lepton_App * App)
{
   App->Pulse_Count ++;
   App->Segment_Per_Frame ++;
   int Result;
   Result = Lepton_Stream_Accept (App->SPI_Device, App->Pixmap);
   switch (Result)
   {
      case Lepton_Stream_SPI_Error:
      case Lepton_Stream_Shifting:
      case Lepton_Stream_Invalid_Row:
      case Lepton_Stream_Discard:
      case Lepton_Stream_Mismatch:
      case Lepton_Stream_Invalid_Segment:
      break;
   };
   //printf ("S: %i\n", Result);
   if (Result == 4)
   {
      //Should be 4 segments but it is 12 due to US Export restriction.
      //printf ("Seg/Frame: %i\n", App->Segment_Per_Frame);
      App->Segment_Per_Frame = 0;
      App->Frame_Count ++;
      time (&(App->Timestamp));
   }
   
   //Check if the camera goes haywire.
   //There should only be 4 segment (*3 if US Export restriction) per frame in Lepton 3.
   if (App->Segment_Per_Frame > 300)
   {
      Lepton_App_Reboot (App);
      App->Segment_Per_Frame = 0;
   }
}


void Lepton_App_Init 
(struct Lepton_App * App, char * I2C_Name, char * SPI_Name)
{
   App->I2C_Device = Lepton_I2C_Open (I2C_Name);
   App->SPI_Device = Lepton_SPI_Open (SPI_Name);
   App->Status = 0;
   App->I2C_Stage = 0;
   Lepton_App_Reboot (App);
   
   /*
   {
      //Create a monotonic clock.
      App->Timer = timerfd_create (CLOCK_MONOTONIC, 0);
      assert (App->Timer > 0);
      //Make so reading the timer does not block.
      int fcntl_Result = fcntl (App->Timer, F_SETFL, O_NONBLOCK);
      assert (fcntl_Result == 0);
      struct itimerspec Period;
      Period.it_value.tv_sec = 3;
      Period.it_value.tv_nsec = 0;
      Period.it_interval.tv_sec = 3;
      Period.it_interval.tv_nsec = 0;
      int settime_Result = timerfd_settime (App->Timer, 0, &Period, NULL);
      assert (settime_Result == 0);
   }
   */
}
