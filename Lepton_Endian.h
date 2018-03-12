#pragma once

#include <stdint.h>
#include <endian.h>


void Lepton_Endian_be16tohv (size_t Count, uint16_t Data [Count])
{
	for (size_t I = 0; I < Count; I = I + 1)
	{
		Data [I] = be16toh (Data [I]);
	}
}



void Lepton_Endian_htobe16v (size_t Count, uint16_t Data [Count])
{
	for (size_t I = 0; I < Count; I = I + 1)
	{
		Data [I] = htobe16 (Data [I]);
	}
}
