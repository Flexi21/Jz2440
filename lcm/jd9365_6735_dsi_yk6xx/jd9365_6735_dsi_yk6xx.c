/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK //xen 20160603
    #include <platform/mt_gpio.h>
    #include "cust_gpio_usage.h"
#else
    //#include <linux/gpio.h>
    #include <mt-plat/mtk_gpio.h>
#endif

#define LCM_ID_JD9365 (0x9365)

#define LCM_DBG(fmt, arg...)
	//LCM_PRINT ("[LCM-ILI9881-DSI-VDO] %s (line:%d) :" fmt "\r\n", __func__, __LINE__, ## arg)

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define LCM_DSI_CMD_MODE									0

#if defined(LCM_720_1440)
#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1440)
#elif defined(LCM_640_1280)
#define FRAME_WIDTH  										(640)
#define FRAME_HEIGHT 										(1280)
#else
#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)
#endif

#define LCM_PHYSICAL_WIDTH									(0)
#define LCM_PHYSICAL_HEIGHT									(0)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0x100   // END OF REGISTERS MARKER


#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif


#if defined(YK739_CUSTOMER_CHANGSHENGTAI_H209_HDPLUS)
 #define LCD_INX_JD9365_IPS_55
#else
 #define LCD_INX_JD9365_IPS_55
#endif 


// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update) \
	lcm_util.dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

struct LCM_setting_table {
    unsigned int cmd; //zxs 20151127
    unsigned char count;
    unsigned char para_list[5];//64];  //xen for less space 20170511
};

#if defined(LCD_INX_JD9365_IPS_55)
static unsigned char BufLcmInfo[] = "LCD_INX_55_JD9365_IPS_55_20171124";
static struct LCM_setting_table lcm_initialization_setting[] = {
{0xE0,1,{0x00}},

{0xE1,1,{0x93}},
{0xE2,1,{0x65}},
{0xE3,1,{0xF8}},
{0x80,1,{0x03}},

{0xE0,1,{0x01}},

{0x00,1,{0x00}},
{0x01,1,{0x32}},
{0x03,1,{0x00}},
{0x04,1,{0x3B}},

{0x17,1,{0x00}},
{0x18,1,{0xBF}},	//VGMP=4.5V
{0x19,1,{0x01}},
{0x1A,1,{0x00}},
{0x1B,1,{0xBF}},  //VGMN=-4.5V
{0x1C,1,{0x01}},

{0x1F,1,{0x7C}},
{0x20,1,{0x26}},
{0x21,1,{0x26}},
{0x22,1,{0x4E}},
{0x24,1,{0xFE}},  //C31P/N open

{0x37,1,{0x09}},	//SS=1,BGR=1

{0x38,1,{0x04}},	//JDT=101 zigzag inversion
{0x39,1,{0x08}},
{0x3A,1,{0x1F}},
{0x3B,1,{0x1F}},
{0x3C,1,{0x78}},
{0x3D,1,{0xFF}},
{0x3E,1,{0xFF}},
{0x3F,1,{0x00}},

{0x40,1,{0x04}},	//RSO=720 RGB
{0x41,1,{0xA0}},	//LN=640->1280 line

{0x43,1,{0x0F}},
{0x44,1,{0x0A}},
{0x45,1,{0x24}},

{0x55,1,{0x01}},
{0x56,1,{0x01}},
{0x57,1,{0x89}},
{0x58,1,{0x0A}},
{0x59,1,{0x4A}},
{0x5A,1,{0x2C}},
{0x5B,1,{0x1A}},
{0x5C,1,{0x19}},

//--- 1,{Gamma2.5  ----// 
{0x5D,1,{0x7C}}, 
{0x5E,1,{0x61}}, 
{0x5F,1,{0x50}}, 
{0x60,1,{0x42}}, 
{0x61,1,{0x3D}}, 
{0x62,1,{0x2E}}, 
{0x63,1,{0x32}}, 
{0x64,1,{0x1C}}, 
{0x65,1,{0x34}}, 
{0x66,1,{0x33}}, 
{0x67,1,{0x34}}, 
{0x68,1,{0x54}}, 
{0x69,1,{0x45}}, 
{0x6A,1,{0x53}}, 
{0x6B,1,{0x4B}}, 
{0x6C,1,{0x54}}, 
{0x6D,1,{0x52}}, 
{0x6E,1,{0x51}}, 
{0x6F,1,{0x08}}, 
{0x70,1,{0x7C}}, 
{0x71,1,{0x61}}, 
{0x72,1,{0x50}}, 
{0x73,1,{0x42}}, 
{0x74,1,{0x3D}}, 
{0x75,1,{0x2E}}, 
{0x76,1,{0x32}}, 
{0x77,1,{0x1C}}, 
{0x78,1,{0x34}}, 
{0x79,1,{0x33}}, 
{0x7A,1,{0x34}}, 
{0x7B,1,{0x54}}, 
{0x7C,1,{0x45}}, 
{0x7D,1,{0x53}}, 
{0x7E,1,{0x4B}}, 
{0x7F,1,{0x54}}, 
{0x80,1,{0x52}}, 
{0x81,1,{0x52}}, 
{0x82,1,{0x08}},

{0xE0,1,{0x02}},
 
{0x00,1,{0x57}},
{0x01,1,{0x77}},
{0x02,1,{0x44}},
{0x03,1,{0x46}},
{0x04,1,{0x48}},
{0x05,1,{0x4A}},
{0x06,1,{0x4C}},
{0x07,1,{0x4E}},
{0x08,1,{0x50}},
{0x09,1,{0x55}},
{0x0A,1,{0x52}},
{0x0B,1,{0x55}},
{0x0C,1,{0x55}},
{0x0D,1,{0x55}},
{0x0E,1,{0x55}},
{0x0F,1,{0x55}},
{0x10,1,{0x55}},
{0x11,1,{0x55}},
{0x12,1,{0x55}},
{0x13,1,{0x40}},
{0x14,1,{0x55}},
{0x15,1,{0x55}},

{0x16,1,{0x57}},
{0x17,1,{0x77}},
{0x18,1,{0x45}},
{0x19,1,{0x47}},
{0x1A,1,{0x49}},
{0x1B,1,{0x4B}},
{0x1C,1,{0x4D}},
{0x1D,1,{0x4F}},
{0x1E,1,{0x51}},
{0x1F,1,{0x55}},
{0x20,1,{0x53}},
{0x21,1,{0x55}},
{0x22,1,{0x55}},
{0x23,1,{0x55}},
{0x24,1,{0x55}},
{0x25,1,{0x55}},
{0x26,1,{0x55}},
{0x27,1,{0x55}},
{0x28,1,{0x55}},
{0x29,1,{0x41}},
{0x2A,1,{0x55}},
{0x2B,1,{0x55}},
 
{0x2C,1,{0x57}},
{0x2D,1,{0x77}},
{0x2E,1,{0x4F}},
{0x2F,1,{0x4D}},
{0x30,1,{0x4B}},
{0x31,1,{0x49}},
{0x32,1,{0x47}},
{0x33,1,{0x45}},
{0x34,1,{0x41}},
{0x35,1,{0x55}},
{0x36,1,{0x53}},
{0x37,1,{0x55}},
{0x38,1,{0x55}},
{0x39,1,{0x55}},
{0x3A,1,{0x55}},
{0x3B,1,{0x55}},
{0x3C,1,{0x55}},
{0x3D,1,{0x55}},
{0x3E,1,{0x55}},
{0x3F,1,{0x51}},
{0x40,1,{0x55}},
{0x41,1,{0x55}},

{0x42,1,{0x57}},
{0x43,1,{0x77}},
{0x44,1,{0x4E}},
{0x45,1,{0x4C}},
{0x46,1,{0x4A}},
{0x47,1,{0x48}},
{0x48,1,{0x46}},
{0x49,1,{0x44}},
{0x4A,1,{0x40}},
{0x4B,1,{0x55}},
{0x4C,1,{0x52}},
{0x4D,1,{0x55}},
{0x4E,1,{0x55}},
{0x4F,1,{0x55}},
{0x50,1,{0x55}},
{0x51,1,{0x55}},
{0x52,1,{0x55}},
{0x53,1,{0x55}},
{0x54,1,{0x55}},
{0x55,1,{0x50}},
{0x56,1,{0x55}},
{0x57,1,{0x55}},

{0x58,1,{0x40}},
{0x59,1,{0x00}},
{0x5A,1,{0x00}},
{0x5B,1,{0x10}},
{0x5C,1,{0x06}},
{0x5D,1,{0x30}},
{0x5E,1,{0x01}},
{0x5F,1,{0x02}},
{0x60,1,{0x30}},
{0x61,1,{0x03}},
{0x62,1,{0x04}},
{0x63,1,{0x06}},
{0x64,1,{0x6A}},
{0x65,1,{0x75}},
{0x66,1,{0x0C}},
{0x67,1,{0xB3}},
{0x68,1,{0x08}},
{0x69,1,{0x06}},
{0x6A,1,{0x6A}},
{0x6B,1,{0x10}},
{0x6C,1,{0x00}},
{0x6D,1,{0x04}},
{0x6E,1,{0x04}},
{0x6F,1,{0x88}},
{0x70,1,{0x00}},
{0x71,1,{0x00}},
{0x72,1,{0x06}},
{0x73,1,{0x7B}},
{0x74,1,{0x00}},
{0x75,1,{0xBC}},
{0x76,1,{0x00}},
{0x77,1,{0x05}},
{0x78,1,{0x2E}},
{0x79,1,{0x00}},
{0x7A,1,{0x00}},
{0x7B,1,{0x00}},
{0x7C,1,{0x00}},
{0x7D,1,{0x03}},
{0x7E,1,{0x7B}},

{0xE0,1,{0x04}},
{0x09,1,{0x10}},
{0x2B,1,{0x2B}},
{0x2E,1,{0x44}},

{0xE0,1,{0x00}},
{0xE6,1,{0x02}},
{0xE7,1,{0x02}},
{0x35,1,{0x00}},


{0x11, 0,   {0x00}},
{REGFLAG_DELAY, 120, {}},
{0x29, 0,   {0x00}},
{REGFLAG_DELAY, 20,  {}},	

{REGFLAG_END_OF_TABLE,0x00,{}}
};
#else
static unsigned char BufLcmInfo[] = "LCD_DEZHIXIN_ILI9881D_IVO_55_20171124";
static struct LCM_setting_table lcm_initialization_setting[] = {
{0xE0,1,{0x00}},

{0xE1,1,{0x93}},
{0xE2,1,{0x65}},
{0xE3,1,{0xF8}},
{0x80,1,{0x03}},

{0xE0,1,{0x01}},

{0x00,1,{0x00}},
{0x01,1,{0x32}},
{0x03,1,{0x00}},
{0x04,1,{0x3B}},

{0x17,1,{0x00}},
{0x18,1,{0xBF}},	//VGMP=4.5V
{0x19,1,{0x01}},
{0x1A,1,{0x00}},
{0x1B,1,{0xBF}},  //VGMN=-4.5V
{0x1C,1,{0x01}},

{0x1F,1,{0x7C}},
{0x20,1,{0x26}},
{0x21,1,{0x26}},
{0x22,1,{0x4E}},
{0x24,1,{0xFE}},  //C31P/N open

{0x37,1,{0x09}},	//SS=1,BGR=1

{0x38,1,{0x04}},	//JDT=101 zigzag inversion
{0x39,1,{0x08}},
{0x3A,1,{0x1F}},
{0x3B,1,{0x1F}},
{0x3C,1,{0x78}},
{0x3D,1,{0xFF}},
{0x3E,1,{0xFF}},
{0x3F,1,{0x00}},

{0x40,1,{0x04}},	//RSO=720 RGB
{0x41,1,{0xA0}},	//LN=640->1280 line

{0x43,1,{0x0F}},
{0x44,1,{0x0A}},
{0x45,1,{0x24}},

{0x55,1,{0x01}},
{0x56,1,{0x01}},
{0x57,1,{0x89}},
{0x58,1,{0x0A}},
{0x59,1,{0x4A}},
{0x5A,1,{0x2C}},
{0x5B,1,{0x1A}},
{0x5C,1,{0x19}},

//--- 1,{Gamma2.5  ----// 
{0x5D,1,{0x7C}}, 
{0x5E,1,{0x61}}, 
{0x5F,1,{0x50}}, 
{0x60,1,{0x42}}, 
{0x61,1,{0x3D}}, 
{0x62,1,{0x2E}}, 
{0x63,1,{0x32}}, 
{0x64,1,{0x1C}}, 
{0x65,1,{0x34}}, 
{0x66,1,{0x33}}, 
{0x67,1,{0x34}}, 
{0x68,1,{0x54}}, 
{0x69,1,{0x45}}, 
{0x6A,1,{0x53}}, 
{0x6B,1,{0x4B}}, 
{0x6C,1,{0x54}}, 
{0x6D,1,{0x52}}, 
{0x6E,1,{0x51}}, 
{0x6F,1,{0x08}}, 
{0x70,1,{0x7C}}, 
{0x71,1,{0x61}}, 
{0x72,1,{0x50}}, 
{0x73,1,{0x42}}, 
{0x74,1,{0x3D}}, 
{0x75,1,{0x2E}}, 
{0x76,1,{0x32}}, 
{0x77,1,{0x1C}}, 
{0x78,1,{0x34}}, 
{0x79,1,{0x33}}, 
{0x7A,1,{0x34}}, 
{0x7B,1,{0x54}}, 
{0x7C,1,{0x45}}, 
{0x7D,1,{0x53}}, 
{0x7E,1,{0x4B}}, 
{0x7F,1,{0x54}}, 
{0x80,1,{0x52}}, 
{0x81,1,{0x52}}, 
{0x82,1,{0x08}},

{0xE0,1,{0x02}},
 
{0x00,1,{0x57}},
{0x01,1,{0x77}},
{0x02,1,{0x44}},
{0x03,1,{0x46}},
{0x04,1,{0x48}},
{0x05,1,{0x4A}},
{0x06,1,{0x4C}},
{0x07,1,{0x4E}},
{0x08,1,{0x50}},
{0x09,1,{0x55}},
{0x0A,1,{0x52}},
{0x0B,1,{0x55}},
{0x0C,1,{0x55}},
{0x0D,1,{0x55}},
{0x0E,1,{0x55}},
{0x0F,1,{0x55}},
{0x10,1,{0x55}},
{0x11,1,{0x55}},
{0x12,1,{0x55}},
{0x13,1,{0x40}},
{0x14,1,{0x55}},
{0x15,1,{0x55}},

{0x16,1,{0x57}},
{0x17,1,{0x77}},
{0x18,1,{0x45}},
{0x19,1,{0x47}},
{0x1A,1,{0x49}},
{0x1B,1,{0x4B}},
{0x1C,1,{0x4D}},
{0x1D,1,{0x4F}},
{0x1E,1,{0x51}},
{0x1F,1,{0x55}},
{0x20,1,{0x53}},
{0x21,1,{0x55}},
{0x22,1,{0x55}},
{0x23,1,{0x55}},
{0x24,1,{0x55}},
{0x25,1,{0x55}},
{0x26,1,{0x55}},
{0x27,1,{0x55}},
{0x28,1,{0x55}},
{0x29,1,{0x41}},
{0x2A,1,{0x55}},
{0x2B,1,{0x55}},
 
{0x2C,1,{0x57}},
{0x2D,1,{0x77}},
{0x2E,1,{0x4F}},
{0x2F,1,{0x4D}},
{0x30,1,{0x4B}},
{0x31,1,{0x49}},
{0x32,1,{0x47}},
{0x33,1,{0x45}},
{0x34,1,{0x41}},
{0x35,1,{0x55}},
{0x36,1,{0x53}},
{0x37,1,{0x55}},
{0x38,1,{0x55}},
{0x39,1,{0x55}},
{0x3A,1,{0x55}},
{0x3B,1,{0x55}},
{0x3C,1,{0x55}},
{0x3D,1,{0x55}},
{0x3E,1,{0x55}},
{0x3F,1,{0x51}},
{0x40,1,{0x55}},
{0x41,1,{0x55}},

{0x42,1,{0x57}},
{0x43,1,{0x77}},
{0x44,1,{0x4E}},
{0x45,1,{0x4C}},
{0x46,1,{0x4A}},
{0x47,1,{0x48}},
{0x48,1,{0x46}},
{0x49,1,{0x44}},
{0x4A,1,{0x40}},
{0x4B,1,{0x55}},
{0x4C,1,{0x52}},
{0x4D,1,{0x55}},
{0x4E,1,{0x55}},
{0x4F,1,{0x55}},
{0x50,1,{0x55}},
{0x51,1,{0x55}},
{0x52,1,{0x55}},
{0x53,1,{0x55}},
{0x54,1,{0x55}},
{0x55,1,{0x50}},
{0x56,1,{0x55}},
{0x57,1,{0x55}},

{0x58,1,{0x40}},
{0x59,1,{0x00}},
{0x5A,1,{0x00}},
{0x5B,1,{0x10}},
{0x5C,1,{0x06}},
{0x5D,1,{0x30}},
{0x5E,1,{0x01}},
{0x5F,1,{0x02}},
{0x60,1,{0x30}},
{0x61,1,{0x03}},
{0x62,1,{0x04}},
{0x63,1,{0x06}},
{0x64,1,{0x6A}},
{0x65,1,{0x75}},
{0x66,1,{0x0C}},
{0x67,1,{0xB3}},
{0x68,1,{0x08}},
{0x69,1,{0x06}},
{0x6A,1,{0x6A}},
{0x6B,1,{0x10}},
{0x6C,1,{0x00}},
{0x6D,1,{0x04}},
{0x6E,1,{0x04}},
{0x6F,1,{0x88}},
{0x70,1,{0x00}},
{0x71,1,{0x00}},
{0x72,1,{0x06}},
{0x73,1,{0x7B}},
{0x74,1,{0x00}},
{0x75,1,{0xBC}},
{0x76,1,{0x00}},
{0x77,1,{0x05}},
{0x78,1,{0x2E}},
{0x79,1,{0x00}},
{0x7A,1,{0x00}},
{0x7B,1,{0x00}},
{0x7C,1,{0x00}},
{0x7D,1,{0x03}},
{0x7E,1,{0x7B}},

{0xE0,1,{0x04}},
{0x09,1,{0x10}},
{0x2B,1,{0x2B}},
{0x2E,1,{0x44}},

{0xE0,1,{0x00}},
{0xE6,1,{0x02}},
{0xE7,1,{0x02}},
{0x35,1,{0x00}},

{0x11, 0,   {0x00}},
{REGFLAG_DELAY, 120, {}},
{0x29, 0,   {0x00}},
{REGFLAG_DELAY, 20,  {}},	

{REGFLAG_END_OF_TABLE,0x00,{}}
};
#endif


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {

	// Display off sequence
	{0x22, 0, {0x00}},
	{REGFLAG_DELAY, 100, {}},

	{0x28, 0, {0x00}},
	{REGFLAG_DELAY, 10, {}},

    // Sleep Mode On
	{0x10, 0, {0x00}},
	{REGFLAG_DELAY, 100, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}

};

#ifdef BUILD_LK //added by xen for softlink lk&kernel lcm driver 20171020
static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
#else
static void push_table(void *cmdq, struct LCM_setting_table *table,
	unsigned int count, unsigned char force_update)
#endif
{
	unsigned int i;
    for(i = 0; i < count; i++) {	
        unsigned int cmd; //zxs 20151127
        cmd = table[i].cmd;
        switch (cmd) {
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
            case REGFLAG_END_OF_TABLE :
                break;
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

#if defined(DOUBLE_LCM_BY_IDPIN) //xen 20180117
static void lcm_get_params(LCM_PARAMS *params)
{

		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		//params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		//params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

        #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
        #else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE; //BURST_VDO_MODE;
        #endif

        #ifndef BUILD_LK //added by xen for softlink lk&kernel lcm driver 20171020
        	params->physical_width = LCM_PHYSICAL_WIDTH/1000;
        	params->physical_height = LCM_PHYSICAL_HEIGHT/1000;
        	params->physical_width_um = LCM_PHYSICAL_WIDTH;
        	params->physical_height_um = LCM_PHYSICAL_HEIGHT;	

        	params->dsi.switch_mode = CMD_MODE;
        	params->dsi.switch_mode_enable = 0;
        #endif

		if (get_lcd_id_state()==0)
		  params->dsi.g_StrLcmInfo = BufLcmInfo_0;
		else
		  params->dsi.g_StrLcmInfo = BufLcmInfo_1;

		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change,should be 0 when video mode in MT658X; or memory leakage

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=720*3;	

		params->dsi.vertical_sync_active				= 8;	//2;
		params->dsi.vertical_backporch					= 18;	//14;
		params->dsi.vertical_frontporch					= 20;	//16;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 
		
		params->dsi.horizontal_sync_active				= 40;	//2;
		params->dsi.horizontal_backporch				= 120;	//60;	//42;
		params->dsi.horizontal_frontporch				= 100;	//60;	//44;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		params->dsi.PLL_CLOCK = 210;//208;	

        #if defined(MTK_ESD_TEST) //zxs 20160318 //zxs 20151107
		params->dsi.esd_check_enable =1;
		params->dsi.customization_esd_check_enable =1;
		params->dsi.lcm_esd_check_table[0].cmd   =0x0A;
		params->dsi.lcm_esd_check_table[0].count = 1;//1;
		params->dsi.lcm_esd_check_table[0].para_list[0] =0x9C;

		params->dsi.noncont_clock=0;
		params->dsi.clk_lp_per_line_enable=1;
        #endif		
}
#else
static void lcm_get_params(LCM_PARAMS *params)
{

		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		//params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		//params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

        #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
        #else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE; //BURST_VDO_MODE;
        #endif

        #ifndef BUILD_LK //added by xen for softlink lk&kernel lcm driver 20171020
        	params->physical_width = LCM_PHYSICAL_WIDTH/1000;
        	params->physical_height = LCM_PHYSICAL_HEIGHT/1000;
        	params->physical_width_um = LCM_PHYSICAL_WIDTH;
        	params->physical_height_um = LCM_PHYSICAL_HEIGHT;	

        	params->dsi.switch_mode = CMD_MODE;
        	params->dsi.switch_mode_enable = 0;
        #endif

		params->dsi.g_StrLcmInfo = BufLcmInfo;

		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=720*3;	

#if defined(LCD_INX_55_JD9365_IPS)  
		params->dsi.vertical_sync_active				= 8;	//2;
		params->dsi.vertical_backporch					= 18;	//14;
		params->dsi.vertical_frontporch					= 20;	//16;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 
		
		params->dsi.horizontal_sync_active				= 40;	//2;
		params->dsi.horizontal_backporch				= 120;	//60;	//42;
		params->dsi.horizontal_frontporch				= 100;	//60;	//44;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		params->dsi.PLL_CLOCK = 210;//208;	
#else
		params->dsi.vertical_sync_active				= 8;	//2;
		params->dsi.vertical_backporch					= 18;	//14;
		params->dsi.vertical_frontporch					= 20;	//16;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 
		
		params->dsi.horizontal_sync_active				= 40;	//2;
		params->dsi.horizontal_backporch				= 120;	//60;	//42;
		params->dsi.horizontal_frontporch				= 100;	//60;	//44;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		params->dsi.PLL_CLOCK = 210;//208;	
#endif

#if defined(MTK_ESD_TEST) //zxs 20160318 //zxs 20151107
        params->dsi.esd_check_enable =1;
        params->dsi.customization_esd_check_enable =1;
        params->dsi.lcm_esd_check_table[0].cmd   =0x0A;
        params->dsi.lcm_esd_check_table[0].count = 1;//1;
        params->dsi.lcm_esd_check_table[0].para_list[0] =0x9C;

        params->dsi.noncont_clock=0;
        params->dsi.clk_lp_per_line_enable=1;
#endif		

}
#endif

static unsigned int lcm_compare_id(void)
{
	    int array[4];
    char buffer[5];
    char id_high=0;
    char id_low=0;
    int id=0;
    
    SET_RESET_PIN(1);
    MDELAY(10);
	SET_RESET_PIN(0);
    MDELAY(20);
	SET_RESET_PIN(1);
    MDELAY(50);

    array[0] = 0x00023700;// read id return two byte,version and id
    dsi_set_cmdq(array, 1, 1);
    read_reg_v2(0x04,buffer, 2);

    id_high = buffer[0]; 
    id_low = buffer[1];
    id = (id_high<<8)|id_low; 

    #ifdef BUILD_LK
    printf("luke: JD9365 %s %d, id = 0x%08x\n", __func__,__LINE__, id);
    #else

    printk("luke: JD9365 %s %d, id = 0x%08x\n", __func__,__LINE__, id);
    #endif

    return (id == LCM_ID_JD9365) ?1:0;
     
}                                     

static void lcm_init(void)
{
	SET_RESET_PIN(1);	 
	MDELAY(10); //20
	SET_RESET_PIN(0);
	MDELAY(20);  // 50 
	SET_RESET_PIN(1);
	MDELAY(50);  // 150

#if defined(DOUBLE_LCM_BY_IDPIN) //xjl 20161105
    if (get_lcd_id_state()==1)
    {
 #ifdef BUILD_LK //added by xen for softlink lk&kernel lcm driver 20171020
        push_table(lcm_initialization_setting_1, sizeof(lcm_initialization_setting_1) / sizeof(struct LCM_setting_table), 1);
 #else
	push_table(NULL, lcm_initialization_setting_1, sizeof(lcm_initialization_setting_1) / sizeof(struct LCM_setting_table), 1);
 #endif
    }
    else
    {
 #ifdef BUILD_LK //added by xen for softlink lk&kernel lcm driver 20171020
        push_table(lcm_initialization_setting_0, sizeof(lcm_initialization_setting_0) / sizeof(struct LCM_setting_table), 1);
 #else
	push_table(NULL, lcm_initialization_setting_0, sizeof(lcm_initialization_setting_0) / sizeof(struct LCM_setting_table), 1);
 #endif
    }
#else
 #ifdef BUILD_LK //added by xen for softlink lk&kernel lcm driver 20171020
        push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
 #else
	push_table(NULL, lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
 #endif
#endif
}


static void lcm_suspend(void)
{
	LCM_DBG("lcm_suspend");
#ifdef BUILD_LK //added by xen for softlink lk&kernel lcm driver 20171020
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
#else
	push_table(NULL, lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
#endif
	//SET_RESET_PIN(0);
    //MDELAY(100);
   	//SET_RESET_PIN(1); 
    
}


static void lcm_resume(void)
{
	LCM_DBG("lcm_resume");
	lcm_init();
	//push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(data_array, 7, 0);

}
#endif

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER jd9365_6735_dsi_lcm_drv_yk6xx = 
{
    .name		    = "jd9365_6735_dsi_video",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
	.update         = lcm_update,
#endif
};
