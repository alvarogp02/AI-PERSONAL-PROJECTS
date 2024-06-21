

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include	<Arduino.h>
#include	"Gest_Modbus.h"
#include	"config_practicas.h"
#include	"EEPROM.h"

/****************************************************************************/
/***        Variables Locales                                             ***/
/****************************************************************************/
/* First step MBS: create an instance */
ModbusSlave mbs;

/****************************************************************************/
/***                 Definici�n de funciones                              ***/
/****************************************************************************/
//void writecoil(unsigned int addrReg);

/****************************************************************************/
/***                 Functions                                            ***/
/****************************************************************************/
void Init_RTU_Modbus()
{
	/* configure modbus communication
	 * 115200 bps, 8E1, two-device network */
	/* Second step MBS: configure */
	/* the Modbus slave configuration parameters */
	const unsigned char 	SLAVE 	= ADDR_SLAVE;		//Address SLAVE
	const long 				BAUD 	= SERIAL_BPS;
	const char 				PARITY 	= SERIAL_PARITY;
	//const char 				TXENPIN = 0; //EN_485;
	const char 				TXENPIN = TX_EN_PIN;

	//Para la conexi�n 485/ModBus usamos
	Serial485 = &Serial;

	mbs.set_BankCoils(Cregs, MB_O_COILS);
	mbs.set_BankDiscreteInputs(Dregs, MB_I_CONTATCS);
	mbs.set_BankOutputHoldingRegister(Aregs, MB_A_REGS);
	mbs.set_BankInputRegister(Iregs, MB_I_REGS);

	mbs.configure(SLAVE,BAUD,PARITY,TXENPIN);
}


/*
 *
 */

//void writecoil(unsigned int addrReg){
void writecoil(){
	for (int addrReg = 0; addrReg < MB_O_COILS; addrReg++) {

		switch (addrReg) {
		case MB_RELE:
			domoboard.setActuator(&domoboard.RELE, Cregs[MB_RELE] != 0x00);
			break;

		case MB_TRIAC:
			domoboard.setActuator(&domoboard.TRIAC, Cregs[MB_TRIAC] != 0x00);
			break;

		case   MB_ACTPIR:
			mbDomoboard.PIR_MOV.Sensor->Activo = Cregs[addrReg]!=0x00;

			//Save PIR Activation
			EEPROM.update(ADDR_ACTPIR, Cregs[addrReg] != 0x00);

			break;

		case MB_ACTSRC:
			mbDomoboard.PHOTORES.Sensor->Activo = Cregs[addrReg] != 0x00;

			//Save SRC Activation
			EEPROM.update(ADDR_ACTSRC, Cregs[addrReg] != 0x00);
			break;

		case MB_ACTTOR:
			mbDomoboard.PHOTOTTOR.Sensor->Activo = Cregs[addrReg] != 0x00;

			//Save foto_ttor Activation
			EEPROM.update(ADDR_ACTTOR, Cregs[addrReg] != 0x00);
			break;

		case MB_PERUP:
		case MB_PERDOWN:
			if((uint8_t) (Aregs[MB_SELPRACT] & 0xFF) == P10_PER)
				UpDown_Persiana();
			break;
		}
	}
}

/*
 *
 */

void writeholdingregister(){
	//char msg[50];

	for (int addrReg = 0; addrReg < MB_A_REGS; addrReg++) {
		switch (addrReg) {

		case MB_SELPRACT:
			//Save pratice selected
			EEPROM.update(ADDR_SELPRACT, Aregs[ADDR_SELPRACT]&0xFF);

			//Go To Selecction Configuration
			SelectionConfiguration((uint8_t) (Aregs[MB_SELPRACT] & 0xFF));
			break;

		case MB_TMP_PIR:
			//Almacenar Tiempo de activaci�n Sensor PIR (Big Endian)
			EEPROM.update(ADDR_TIEMPO_PIR_1, (Aregs[MB_TMP_PIR]>>8)&0xFF);
			EEPROM.update(ADDR_TIEMPO_PIR_2, (Aregs[MB_TMP_PIR]&0xFF));
			break;

		case MB_SRC_HL:
			//Almacenar nivel superior de activaci�n SRC (Big Endian)
			EEPROM.update(ADDR_SRC_HL_1, (Aregs[MB_SRC_HL] >> 8) & 0xFF);
			EEPROM.update(ADDR_SRC_HL_2, (Aregs[MB_SRC_HL] & 0xFF));
			break;

		case MB_SRC_LL:
			//Almacenar nivel inferior de activaci�n SRC (Big Endian)
			EEPROM.update(ADDR_SRC_LL_1, (Aregs[MB_SRC_LL] >> 8) & 0xFF);
			EEPROM.update(ADDR_SRC_LL_2, (Aregs[MB_SRC_LL] & 0xFF));
			break;

		case MB_TTOR_LL:
			//Almacenar nivel inferior de activaci�n SRC (Big Endian)
			EEPROM.update(ADDR_TTOR_LL_1, (Aregs[MB_TTOR_LL] >> 8) & 0xFF);
			EEPROM.update(ADDR_TTOR_LL_2, (Aregs[MB_TTOR_LL] & 0xFF));
			break;

		case MB_STAPER:

			update_PersianaState();

			break;
		}
	}
}


void RTU_ModBus()
{

	if(mbs.update()){
		//DEBUGLOGLN("%s: UPDATE", __FUNCTION__);
		writecoil();
		writeholdingregister();
	}
}

/*
 * void load_config().- load initial configuration for arduino aplication
 */
void load_Config(){
	//Leemos configuraci�n Actual "Selecci�n Pr�ctica"
	Aregs[MB_SELPRACT] = EEPROM.read(ADDR_SELPRACT);	//Read EEPROM

	Aregs[MB_SELPRACT] = Aregs[MB_SELPRACT] != 0x00 ? Aregs[MB_SELPRACT] : 0x11;	//Seleccionamos la pr�ctica 1 apartado 1 por defecto

//	DEBUGLOGLN("Pr�ctica Seleccionada: 0x%X", Aregs[MB_SELPRACT]);

	/*
	 * Config PIR
	 */
	//Leer activaci�n del PIR
	Cregs[MB_ACTPIR] = EEPROM.read(ADDR_ACTPIR);

//	DEBUGLOGLN("PIR activado: %d", Cregs[MB_ACTPIR]);

	//Leer Tiempo activaci�n PIR
	Aregs[MB_TMP_PIR] = ((EEPROM.read(ADDR_TIEMPO_PIR_1)&0xFF) << 8) + (EEPROM.read(ADDR_TIEMPO_PIR_2)&0xFF) ;

//	DEBUGLOGLN("Tiempo Activaci�n del PIR: %d Segundos", Aregs[MB_TMP_PIR]);

	/*
	 * Configuration SRC
	 */
	Cregs[MB_ACTSRC] = EEPROM.read(ADDR_ACTSRC);

//	DEBUGLOGLN("PhotoResistor activado: %d", Cregs[MB_ACTSRC]);


	Aregs[MB_SRC_HL] = (EEPROM.read(ADDR_SRC_HL_1) << 8)
			+ (EEPROM.read(ADDR_SRC_HL_2) & 0xFF);
	Aregs[MB_SRC_LL] = (EEPROM.read(ADDR_SRC_LL_1) << 8)
			+ (EEPROM.read(ADDR_SRC_LL_2) & 0xFF);

//	DEBUGLOGLN("Nivel Activaci�n SRC: Bajo --> %d | Alto --> %d", Aregs[MB_SRC_LL], Aregs[MB_SRC_HL]);

	/*
	 * Configuration Phototransistor
	 */

	Cregs[MB_ACTTOR] = EEPROM.read(ADDR_ACTTOR);

//	DEBUGLOGLN("PhotoTransistor activado: %d", Cregs[MB_ACTTOR]);


	Aregs[MB_TTOR_LL] = (EEPROM.read(ADDR_TTOR_LL_1) << 8)
			+ (EEPROM.read(ADDR_TTOR_LL_2) & 0xFF);

//	DEBUGLOGLN("Nivel Activaci�n PhotoTransistor: %d", Aregs[MB_TTOR_LL]);

	writecoil();
	writeholdingregister();
}


