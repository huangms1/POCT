#ifndef __APP_MSG_DEFINE_H__
#define __APP_MSG_DEFINE_H__

/****************锟街革拷锟斤拷********************/
#define COMMA_SEPARATOR															','

#define TOP_SEPARATOR															'^'

#define EQUAL_SEPARATOR															'='

#define TOP_SEPARATE_STRING													    "^"

#define EQUAL_SEPARAT_STRING												    "="

/****************锟街革拷锟斤拷********************/



/****************锟斤拷息帧帧锟斤拷********************/

#define APP_MSG_TYPE_I_NO														"0"

/****************锟斤拷息帧帧锟斤拷********************/


/****************帧锟斤拷锟斤拷********************/

#define APP_MSG_TYPE_C															"C"

#define APP_MSG_TYPE_R															"R"

#define APP_MSG_TYPE_D															"D"

#define APP_MSG_TYPE_I															"I"

/****************帧锟斤拷锟斤拷********************/


/****************锟斤拷元********************/

#define APP_MSG_UNIT_PC															"PC"

#define APP_MSG_UNIT_COMM														"COMM"

#define APP_MSG_UNIT_MAIN														"MAIN"

#define APP_MSG_UNIT_TEMP														"TEMP"

/****************锟斤拷元********************/


/****************锟斤拷锟斤拷锟斤拷********************/

#define APP_MSG_MAIN_CMD_EXE												"EXE"

#define APP_MSG_MAIN_CMD_SET												"SET"

#define APP_MSG_MAIN_CMD_RPT												"RPT"

#define APP_MSG_MAIN_CMD_QUY												"QUY"

/*锟斤拷锟斤拷锟斤拷锟斤拷*/
#define APP_MSG_MAIN_CMD_DEB                                                "DEB"
/*写锟斤拷锟斤拷*/
#define APP_MSG_MAIN_CMD_WR                                                 "WR"
/*锟斤拷锟斤拷锟斤拷*/
#define APP_MSG_MAIN_CMD_RD                                                 "RD"

/*锟斤拷锟斤拷锟斤拷*/
#define APP_MSG_MAIN_CMD_LED                                                "LED"

#define APP_MSG_MAIN_CMD_PARA                                               "PARA"

/****************锟斤拷锟斤拷锟斤拷********************/
typedef enum {
	CMD_UPG = 1U,
	CMD_INI,
	CMD_OPEN,
	CMD_CLOS,
	CMD_DOWN,
	CMD_LJXB,
	CMD_QXZZ,
	CMD_QXDB,
	CMD_XTHS,
	CMD_RYFP,
	CMD_ZD,
	CMD_NZL,
	CMD_YBX,
	CMD_PCR,
	CMD_MOV
}APP_MSG_CMD_EXE_ENUM;



/****************锟斤拷锟斤拷锟斤拷********************/

#define APP_MSG_SUB_CMD_RESET                                               "RESET"

#define APP_MSG_SUB_CMD_PCR													"PCR"

#define APP_MSG_SUB_CMD_POW													"POW"

#define APP_MSG_SUB_CMD_INI													"INI"

#define APP_MSG_SUB_CMD_HAND												"HAND"

#define APP_MSG_SUB_CMD_ADDR												"ADDR"

#define APP_MSG_SUB_CMD_IP                                                  "IP"

#define APP_MSG_SUB_CMD_MAC                                                 "MAC"

#define APP_MSG_SUB_CMD_OPEN                                                "OPEN"

#define APP_MSG_SUB_CMD_CLOS                                                "CLOS"

#define APP_MSG_SUB_CMD_DOWN                                                "DOWN"

#define APP_MSG_SUB_CMD_LJXB                                                "LJXB"

#define APP_MSG_SUB_CMD_QXZZ                                                "QXZZ"

#define APP_MSG_SUB_CMD_QXDB                                                "QXDB"

#define APP_MSG_SUB_CMD_XTHS                                                "XTHS"

#define APP_MSG_SUB_CMD_RYFP                                                "RYFP"

#define APP_MSG_SUB_CMD_NZL                                                 "NZL"

#define APP_MSG_SUB_CMD_YBX                                                 "YBX"

#define APP_MSG_SUB_CMD_PCR                                                 "PCR"

#define APP_MSG_SUB_CMD_MOV                                                 "MOV"

#define APP_MSG_SUB_CMD_VER                                                 "VER"

#define APP_MSG_SUB_CMD_UPG                                                 "UPG"

#define APP_MSG_SUB_CMD_ZD                                                  "ZD"

#define APP_MSG_SUB_CMD_ERR                                                 "ERR"

/****************锟斤拷锟斤拷锟斤拷********************/


/****************锟斤拷锟斤拷********************/

#define MAIN_PARA_ON															    "ON"

#define MAIN_PARA_OFF																"OFF"

#define MAIN_PARA_MCL                                                               "MCL"

#define MAIN_PARA_T_t                                                               "T_t"

#define MAIN_PARA_M01                                                               "M01"

#define MAIN_PARA_M02                                                               "M02"

#define MAIN_PARA_M03                                                               "M03"

#define MAIN_PARA_M04                                                               "M04"

#define MAIN_PARA_M05                                                               "M05"

#define MAIN_PARA_M06                                                               "M06"

#define MAIN_PARA_M07                                                               "M07"

#define MAIN_PARA_M08                                                               "M08"

#define MAIN_PARA_M09                                                               "M09"

#define MAIN_PARA_M10                                                               "M10"

#define MAIN_PARA_M11                                                               "M11"

#define MAIN_PARA_M12                                                               "M12"

#define MAIN_PARA_M13                                                               "M13"

#define MAIN_PARA_M14                                                               "M14"

#define MAIN_PARA_M15                                                               "M15"

#define MAIN_PARA_LED_FAM                                                            "FAM"

#define MAIN_PARA_LED_ROX                                                            "ROX"

#define MAIN_PARA_LED_CY5                                                            "CY5"

#define MAIN_PARA_LED_CY5DOT5                                                         "CY5.5"

#define MAIN_PARA_LED_HEX                                                            "HEX"

#define MAIN_PARA_LED_CY7                                                            "CY7"

#define MAIN_PARA_LED_ATTO                                                           "ATTO"

#define MAIN_PARA_LED_AMCA                                                           "AMCA"

/****************锟斤拷锟斤拷********************/


/****************锟接诧拷********************/

#define SUB_PARA_T																	"T"

#define SUB_PARA_F																	"F"

#define SUB_PARA_CH1																"1"

#define SUB_PARA_CH2																"2"

#define SUB_PARA_CH3																"3"

#define SUB_PARA_CH4																"4"

/****************锟接诧拷********************/


#endif //__APP_MSG_DEFINE_H__
