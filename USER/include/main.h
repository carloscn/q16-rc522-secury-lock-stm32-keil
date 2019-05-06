/***********************************************************************
????:main.h
?    ?:
????:2013.4.25
? ? ?:
?    ?:
***********************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_

#include "stm32f10x.h"
#include <stdio.h>
#include <Printf.h>
#include "delay.h"
#include "KEY.H"
#include "LED.H"
#include "rc522.h"
#include "sci.h"
#include "DMA.H"
#include "stm32f10x_dma.h"
void NVIC_Configuration(void);
void Parse_Data(void);
#endif
