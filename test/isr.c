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
#include <sys/epoll.h>


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
	
	Lepton_I2C_Write_Command (Device, Lepton_I2C_Command_Reboot);
	sleep (3);
	
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
		
		//Disable shutter
		Mode.Shutter_Mode = htobe16 (0);
		
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

	
	int Eventpoll = epoll_create1 (0);
	Assert (Eventpoll != -1, "epoll_create error%s", "");
	
	{
		struct epoll_event Event;
		Event.data.fd = F;
		Event.events = EPOLLIN | EPOLLET;
		int R = epoll_ctl (Eventpoll, EPOLL_CTL_ADD, F, &Event);
		Assert (R != -1, "epoll_ctl (%d) error", Eventpoll);
	}

	while (1)
	{
		
		size_t const Event_List_Size = 100;
		struct epoll_event Event_List [Event_List_Size];
		int N = epoll_wait (Eventpoll, Event_List, Event_List_Size, -1);
		Assert (N != -1, "epoll_wait (%d) error", Eventpoll);
		for (int I = 0; I < N; I = I + 1)
		{
			if (Event_List [I].data.fd == F)
			{
				fprintf (stderr, ".");
				size_t const Buf_Count = 10;
				char Buf [Buf_Count];
				lseek (F, 0, SEEK_SET);
				read (F, Buf, Buf_Count);
			}
			else
			{
			}
		}
	}
	
	close (F);
	return 0;
}



