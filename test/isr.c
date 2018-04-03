//gcc test_grab.c -std=gnu11 -fdiagnostics-color -Wall -Wno-missing-braces -lwiringPi -o grab

#include "util.h"

#define Lepton_Assert Assert_C

#include "../Lepton_SPI.h"
#include "../Lepton_I2C.h"
#include "../Lepton_Strings.h"
#include "../Lepton_Stream.h"
#include "../Lepton_Endian.h"

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

//
#include <signal.h>
#include <poll.h>


int Lep_Execute 
(
	int Device, 
	int Command,
	void * Data,
	size_t Size,
	int Micro_Sleep, 
	size_t Trial_Count
)
{
	{
		uint16_t Status;
		char Buffer [17] = {'\0'};
		Status = Lepton_I2C_Status (Device);
		Lepton_Strings_Base_Converter (be16toh (Status), Buffer, 16, 2);
		Log ("Lepton device (%d): status = %16s", Device, Buffer);
	}
	
	for (size_t I = 0; I < Trial_Count; I = I + 1)
	{
		int Stage = 0;
		uint16_t Status;
		Log ("Lepton device (%d): execute command, trial %i", Device, (int) I);
		Lepton_I2C_Execute 
		(
			Device, 
			Command, 
			Data,
			Size,
			&(Status),
			&(Stage)
		);
		{
			char Buffer [17] = {'\0'};
			Lepton_Strings_Base_Converter (be16toh (Status), Buffer, 16, 2);
			Log ("Lepton device (%d): status = %16s", Device, Buffer);
		}
		if (Stage == 0) {return 1;}
		usleep (Micro_Sleep);
	}
	return 0;
}


void Reboot (int Device)
{
	int const Micro_Sleep = 10;
	size_t const Trial_Count = 10;
	
	{
		int Status;
		Status = Lepton_I2C_Status (Device);
		char Buffer [17] = {'\0'};
		Lepton_Strings_Base_Converter (be16toh (Status), Buffer, 16, 2);
		Log ("Lepton device (%d): status = %16s", Device, Buffer);
	}
	
	
	Log ("Lepton device (%d): Rebooting", Device);
	Lepton_I2C_Write_Command (Device, Lepton_I2C_Command_Reboot);
	sleep (3);
	
	
	Log ("Lepton device (%d): Enabling vsync", Device);
	{
		struct Lepton_I2C_GPIO_Mode GPIO_Mode;
		GPIO_Mode.Value = htobe16 (Lepton_I2C_GPIO_Mode_Vsync);
		Lep_Execute 
		(
			Device, 
			Lepton_I2C_Command_GPIO_Mode_Set,
			(void *) &GPIO_Mode,
			sizeof (GPIO_Mode),
			Micro_Sleep,
			Trial_Count
		);
	}
	
	Log ("Lepton device (%d): Disabling FFC", Device);
	{
		struct Lepton_I2C_Shutter_Mode Mode;
		int Result;
		
		Result = Lep_Execute 
		(
			Device, 
			Lepton_I2C_Command_FFC_Mode_Get,
			(void *) &Mode,
			sizeof (Mode),
			Micro_Sleep,
			Trial_Count
		);
		if (!Result) {return;};
		
		//If successful then convert the received data from big endian to host endian.
		Lepton_Endian_be16tohv (sizeof (struct Lepton_I2C_Shutter_Mode) / sizeof (uint16_t), (uint16_t *) &Mode);
		
		Log ("%-40s : %010zu", "Shutter_Mode", (uint32_t) Mode.Shutter_Mode);
		Log ("%-40s : %010zu", "Temp_Lockout_State", (uint32_t) Mode.Temp_Lockout_State);
		Log ("%-40s : %010zu", "Video_Freeze_During_FFC", (uint32_t) Mode.Video_Freeze_During_FFC);
		Log ("%-40s : %010zu", "FFC_Desired", (uint32_t) Mode.FFC_Desired);
		Log ("%-40s : %010zu", "Elapsed_Time_Since_Last_FFC", (uint32_t) Mode.Elapsed_Time_Since_Last_FFC);
		Log ("%-40s : %010zu", "Desired_FFC_Period", (uint32_t) Mode.Desired_FFC_Period);
		Log ("%-40s : %010zu", "Explicit_Command_To_Open", (uint32_t) Mode.Explicit_Command_To_Open);
		Log ("%-40s : %010i", "Desired_FFC_Temp_Delta", (uint16_t) Mode.Desired_FFC_Temp_Delta);
		Log ("%-40s : %010i", "Imminent_Delay", (uint16_t) Mode.Imminent_Delay);
		
		//Disable shutter
		Mode.Shutter_Mode = 0;
		
		//Convert the data from host endian to big endian
		Lepton_Endian_htobe16v (sizeof (struct Lepton_I2C_Shutter_Mode) / sizeof (uint16_t), (uint16_t *) &Mode);
		
		Result = Lep_Execute 
		(
			Device, 
			Lepton_I2C_Command_FFC_Mode_Set,
			(void *) &Mode,
			sizeof (Mode),
			Micro_Sleep,
			Trial_Count
		);
		if (!Result) {return;};
		
		
		Result = Lep_Execute 
		(
			Device, 
			Lepton_I2C_Command_FFC_Mode_Get,
			(void *) &Mode,
			sizeof (Mode),
			Micro_Sleep,
			Trial_Count 
		);
		if (!Result) {return;};
		
		//If successful then convert the received data from big endian to host endian.
		Lepton_Endian_be16tohv (sizeof (struct Lepton_I2C_Shutter_Mode) / sizeof (uint16_t), (uint16_t *) &Mode);
		
		Log ("%-40s : %010zu", "Shutter_Mode", (uint32_t) Mode.Shutter_Mode);
		Log ("%-40s : %010zu", "Temp_Lockout_State", (uint32_t) Mode.Temp_Lockout_State);
		Log ("%-40s : %010zu", "Video_Freeze_During_FFC", (uint32_t) Mode.Video_Freeze_During_FFC);
		Log ("%-40s : %010zu", "FFC_Desired", (uint32_t) Mode.FFC_Desired);
		Log ("%-40s : %010zu", "Elapsed_Time_Since_Last_FFC", (uint32_t) Mode.Elapsed_Time_Since_Last_FFC);
		Log ("%-40s : %010zu", "Desired_FFC_Period", (uint32_t) Mode.Desired_FFC_Period);
		Log ("%-40s : %010zu", "Explicit_Command_To_Open", (uint32_t) Mode.Explicit_Command_To_Open);
		Log ("%-40s : %010i", "Desired_FFC_Temp_Delta", (uint16_t) Mode.Desired_FFC_Temp_Delta);
		Log ("%-40s : %010i", "Imminent_Delay", (uint16_t) Mode.Imminent_Delay);
	}
}


void Set_Edge_GPIO (int GPIO, char * Edge)
{
	size_t const Buffer_Count = 100;
	char Buffer [Buffer_Count];
	int L = snprintf (Buffer, Buffer_Count, "/sys/class/gpio/gpio%d/edge", GPIO);
	Assert (L > 0, "snprintf%s", "");
	int F = open (Buffer, O_WRONLY);
	write (F, Edge, strlen (Edge) + 1);
	close (F);
}

/*
int Open_GPIO (int GPIO, char * Direction)
{
	int L;
	size_t const Buffer_Count = 100;
	char Buffer [Buffer_Count];
	int F;
	int R;
	
	F = open ("/sys/class/gpio/unexport", O_WRONLY);
	Assert (F != -1, "open %s", "/sys/class/gpio/export");
	L = snprintf (Buffer, Buffer_Count, "%d", GPIO);
	R = write (F, Buffer, L);
	Assert (R == L, "write %s", Buffer);
	close (F);
	
	F = open ("/sys/class/gpio/export", O_WRONLY);
	Assert (F != -1, "open %s", "/sys/class/gpio/export");
	L = snprintf (Buffer, Buffer_Count, "%d", GPIO);
	R = write (F, Buffer, L);
	Assert (R == L, "write %s", Buffer);
	close (F);
	
	L = snprintf (Buffer, Buffer_Count, "/sys/class/gpio/gpio%d/direction", GPIO);
	F = open (Buffer, O_WRONLY);
	Assert (F != -1, "open %s", Buffer);
	R = write (F, Direction, strlen (Direction));
	Assert (R == (int) strlen (Direction), "write %s", Buffer);
	close (F);
	
	L = snprintf (Buffer, Buffer_Count, "/sys/class/gpio/gpio%d/value", GPIO);
	F = open (Buffer, O_RDONLY);
	Assert (F != -1, "open %s", Buffer);
	return F;
}
*/

int Open_GPIO (int GPIO)
{
	size_t const Buffer_Count = 100;
	char Buffer [Buffer_Count];
	int F;
	snprintf (Buffer, Buffer_Count, "/sys/class/gpio/gpio%d/value", GPIO);
	F = open (Buffer, O_RDONLY);
	Assert (F != -1, "open %s", Buffer);
	return F;
}

int I2C_Device;


int main (int argc, char * argv [])
{ 
	//No argument is used currently.
	Assert (argc == 1, "argc = %i", argc);
	Assert (argv != NULL, "argv = %p", argv);


	//Ignore SIGPIPE interrupt.
	//SIGPIPE interrupt can happen when the reading end of the pipe closes while pipe is being written to.
	signal (SIGPIPE, SIG_IGN);
	//signal (SIGPIPE, Signal_Handler);

	//I2C comm is used for setting/getting the camera registers.
	Log ("Open %s", "/dev/i2c-1");
	I2C_Device = Lepton_I2C_Open ("/dev/i2c-1");
	Reboot (I2C_Device);
	
	
	int F = Open_GPIO (17);

	
	struct pollfd fdset [1] = {0};
	

	while (1)
	{
		
		fdset [0].fd = F;
		fdset [0].events = POLLPRI;
		fdset [0].revents = 0;
		int R;
		R = poll (fdset, 1, 5000);
		Assert (R >= 0, "poll%s", "");
		
		if (fdset [0].revents & POLLPRI)
		{
			lseek (F, 0, SEEK_SET);
			fsync (F);
			fprintf (stderr, ".");
			//size_t const Buf_Count = 10;
			//char Buf [Buf_Count];
			//R = read (F, Buf, Buf_Count);
			//fwrite (Buf, sizeof (char), R, stderr);
			//fprintf (stderr, "\n");
		}
	}
	
	close (F);
	return 0;
}



