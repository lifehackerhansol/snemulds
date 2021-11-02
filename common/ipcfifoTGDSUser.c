/*
			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

*/

#include "ipcfifoTGDS.h"
#include "ipcfifoTGDSUser.h"
#include "apu_shared.h"
#include "InterruptsARMCores_h.h"
#include "powerTGDS.h"

#ifdef ARM7
#include <string.h>
#include "pocketspc.h"
#include "apu.h"
#include "dsp.h"
#include "main.h"
#include "mixrate.h"
#include "spifwTGDS.h"
#include "apu_shared.h"

#endif

#ifdef ARM9
#include <stdbool.h>
#include "memmap.h"
#include "common.h"
#include "cfg.h"
#include "main.h"
#include "core.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#endif


#ifdef ARM9
__attribute__((section(".itcm")))
#endif
struct sIPCSharedTGDSSpecific* getsIPCSharedTGDSSpecific(){
	
	#ifdef ARM7
	struct sIPCSharedTGDSSpecific* sIPCSharedTGDSSpecificInst = (__attribute__((packed)) struct sIPCSharedTGDSSpecific*)(NDS_CACHED_SCRATCHPAD + (80*4));
	#endif
	
	#ifdef ARM9
	struct sIPCSharedTGDSSpecific* sIPCSharedTGDSSpecificInst = (__attribute__((packed)) struct sIPCSharedTGDSSpecific*)(NDS_UNCACHED_SCRATCHPAD + (80*4));
	#endif
	
	return sIPCSharedTGDSSpecificInst;
}

//inherits what is defined in: ipcfifoTGDS.c
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void HandleFifoNotEmptyWeakRef(uint32 cmd1,uint32 cmd2){
	
	switch (cmd1) {
		//ARM7 command handler
		#ifdef ARM7
		case (SNEMULDS_SETUP_ARM7):{
			playBuffer = (uint16*)0x6000000;
			int i   = 0;
			for (i = 0; i < MIXBUFSIZE * 4; i++) {
				playBuffer[i] = 0;
			}
			update_spc_ports(); //APU Ports from SnemulDS properly binded with Assembly APU Core
			ApuReset();
			DspReset();
			SetupSound();
			
			struct sIPCSharedTGDS * TGDSIPC = getsIPCSharedTGDS();
			uint32 * fifomsg = (uint32 *)&TGDSIPC->fifoMesaggingQueue[0];
			setValueSafe(&fifomsg[10], (uint32)0);
		}
		break;
		
		case SNEMULDS_APUCMD_RESET: //case 0x00000001:
		{
			// Reset
			StopSound();

			memset(playBuffer, 0, MIXBUFSIZE * 8);

			getsIPCSharedTGDSSpecific()->APU_ADDR_CNT = 0; 
			ApuReset();
			DspReset();

			SetupSound();
			paused = false;
			SPC_disable = false;
			SPC_freedom = false;
		}
		break;
		case SNEMULDS_APUCMD_PAUSE:{ //case 0x00000002:{
			// Pause/unpause
			if (!paused) {
				StopSound();
			} else {
				SetupSound();
			}
			if (SPC_disable)
				SPC_disable = false;        
			paused = !paused;
		}
		break;
		case SNEMULDS_APUCMD_PLAYSPC:{ //case 0x00000003:{ // PLAY SPC
			//Reset APU
			StopSound();
			memset(playBuffer, 0, MIXBUFSIZE * 8);
			getsIPCSharedTGDSSpecific()->APU_ADDR_CNT = 0; 
			ApuReset();
			DspReset();
			SetupSound();
			
			//Load APU payload
			LoadSpc((const u8*)cmd2);
			struct sIPCSharedTGDS * TGDSIPC = getsIPCSharedTGDS();
			uint32 * fifomsg = (uint32 *)&TGDSIPC->fifoMesaggingQueue[0];
			fifomsg[40] = (uint32)0;	//release ARM9 APU_playSpc()
			
			paused = false;
			SPC_disable = false;
			SPC_freedom = false;
		}
		break;
			
		case SNEMULDS_APUCMD_SPCDISABLE:{ //case 0x00000004:{ // DISABLE 
			SPC_disable = true;
			getsIPCSharedTGDSSpecific()->APU_ADDR_CNT = 0;
		}
		break;        
		
		case SNEMULDS_APUCMD_CLRMIXERBUF:{ //case 0x00000005:{ // CLEAR MIXER BUFFER 
			memset(playBuffer, 0, MIXBUFSIZE * 8);
		}
		break;

		case SNEMULDS_APUCMD_SAVESPC:{ //case 0x00000006:{ // //Save APU Memory Snapshot -> u8 * inSPCBuffer @ ARM9 EWRAM
			SaveSpc((const u8*)cmd2);
			struct sIPCSharedTGDS * TGDSIPC = getsIPCSharedTGDS();
			uint32 * fifomsg = (uint32 *)&TGDSIPC->fifoMesaggingQueue[0];
			fifomsg[40] = (uint32)0;	//release ARM9 APU_playSpc()
		}
		break;  
			
		case SNEMULDS_APUCMD_LOADSPC:{ //case 0x00000007:{ // LOAD state 
			LoadSpc((const u8*)cmd2);
			struct sIPCSharedTGDS * TGDSIPC = getsIPCSharedTGDS();
			uint32 * fifomsg = (uint32 *)&TGDSIPC->fifoMesaggingQueue[0];
			fifomsg[40] = (uint32)0;	//release ARM9 APU_loadSpc()
			getsIPCSharedTGDSSpecific()->APU_ADDR_CNT = 0; 
		}
		break;
		#endif
	}
	
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoEmptyWeakRef(uint32 cmd1,uint32 cmd2){
}

//project specific stuff
#ifdef ARM9
//small hack to update SNES_ADDRESS at opcodes2.s
void update_ram_snes(){
    //snes_ram_address = (uint32)&snes_ram_bsram[0x6000];
}
#endif

//APU Ports from SnemulDS properly binded with Assembly APU Core
void update_spc_ports(){
    struct s_apu2 *APU2 = (struct s_apu2 *)(&getsIPCSharedTGDSSpecific()->APU2);
	//must reflect to ipcfifoTGDSUser.h defs
	ADDRPORT_SPC_TO_SNES	=	(uint32)(uint8*)&getsIPCSharedTGDSSpecific()->PORT_SPC_TO_SNES[0];
	ADDRPORT_SNES_TO_SPC	=	(uint32)(uint8*)&getsIPCSharedTGDSSpecific()->PORT_SNES_TO_SPC[0]; 
	ADDR_APU_PROGRAM_COUNTER=	(uint32)(volatile uint32*)&getsIPCSharedTGDSSpecific()->APU_PROGRAM_COUNTER;	//0x27E0000	@APU PC
	
	ADDR_SNEMUL_CMD	=	(uint32)(volatile uint32*)&getsIPCSharedTGDSSpecific()->APU_ADDR_CMD;	//0x027FFFE8	// SNEMUL_CMD
	ADDR_SNEMUL_ANS	=	(uint32)(volatile uint32*)&getsIPCSharedTGDSSpecific()->APU_ADDR_ANS;	//0x027fffec	// SNEMUL_ANS
	ADDR_SNEMUL_BLK	=	(uint32)(volatile uint32*)&getsIPCSharedTGDSSpecific()->APU_ADDR_BLK;	//0x027fffe8	// SNEMUL_BLK
	getsIPCSharedTGDSSpecific()->APU_ADDR_BLKP = (uint8 *)ADDR_SNEMUL_BLK;
	
	//todo: APU_ADDR_CNT: is unused by APU Core?
}


#ifdef ARM9
//Callback update sample implementation

__attribute__((section(".itcm")))
void updateSoundContextStreamPlaybackUser(u32 srcFrmt){
}
#endif

void setupSoundUser(u32 srcFrmtInst){
	#ifdef ARM7
	#endif
	
	#ifdef ARM9
	#endif
}

void stopSoundUser(u32 srcFrmt){
	#ifdef ARM7
	#endif
	
	#ifdef ARM9
	#endif
}

