/***********************************************************/
/* This source is part of SNEmulDS                         */
/* ------------------------------------------------------- */
/* (c) 1997-1999, 2006-2007 archeide, All rights reserved. */
/***********************************************************/
/*
This program is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
GNU General Public License for more details.
*/

#ifndef __main_h__
#define __main_h__

#include "fatfslayerTGDS.h"
#include "utilsTGDS.h"

#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int argc;
extern sint8 **argv;
extern int main(int argc, char argv[argvItems][MAX_TGDSFILENAME_LENGTH]);

#ifdef __cplusplus
}
#endif