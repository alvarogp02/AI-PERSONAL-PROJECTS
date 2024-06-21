
#ifndef CONFIG_PRACTICAS_H_
#define CONFIG_PRACTICAS_H_
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include	"debuglog.h"
#include	"utils_domoBoard.h"
#include	"domoBoard.h"
#include	"ModbusDomoboard.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
//Defininici�n de comando de configuraci�n
#define	P1_PULSADORES				0x11
#define	P1_INTERRUPTOR				0x12
#define	P1_CONMUTADOR				0x13
#define	P3_CONMUTADOR				0x32
#define	P4_PULSADORES				0x41
#define	P4_INTERRUPTOR				0x42
#define	P4_CONMUTADOR				0x43
#define PRACTICAS_MODBUS			0x50
#define	P5_INTERRUPTOR				0x54
#define	P6_INTERRUPTOR				0x62
#define P7_PIR						0x70
#define P9_ALL						0x90
#define P10_PER						0xA1
#define P10_GAR						0xA2

void SelectionConfiguration(uint8_t selConf);

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
void config_practica3_apt_2();

void config_practica4_apt_1();
void config_practica4_apt_2();
void config_practica4_apt_3();

void config_practica5_apt_4();

void SelectionConfiguration(uint8_t selConf);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern TCtrlTime 	ctrlPosPer;

#endif /* CONFIG_PRACTICAS_H_ */
