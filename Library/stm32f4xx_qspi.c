/**
  ******************************************************************************
  * @file    stm32f4xx_qspi.c
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Serial peripheral interface (QSPI):
  *           + Initialization and Configuration
  *           + Indirect Data Read/Write functions
  *           + Memory Mapped Mode Data Read functions  
  *           + Automatic Polling functions
  *           + DMA transfers management
  *           + Interrupts and flags management 
  *
  *  @verbatim
  *
 ===============================================================================
                       ##### How to use this driver #####
 ===============================================================================
    [..]
        (#) Enable peripheral clock using   RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_QSPI,ENABLE);
            function.

        (#) Enable CLK, BK1_IO0, BK1_IO1, BK1_IO2, BK1_IO3, BK1_NCS, BK2_IO0, 
            BK2_IO1, BK2_IO2, BK2_IO3 and BK2_NCS GPIO clocks using 
            RCC_AHB1PeriphClockCmd() function. 
  
        (#) Peripherals alternate function: 
           (++) Connect the pin to the desired peripherals' Alternate 
                 Function (AF) using GPIO_PinAFConfig() function.
           (++) Configure the desired pin in alternate function by:
                 GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF.
           (++) Select the type, pull-up/pull-down and output speed via 
                 GPIO_PuPd, GPIO_OType and GPIO_Speed members.
           (++) Call GPIO_Init() function.
  
        (#) Program the Flash Size, CS High Time, Sample Shift, Prescaler, Clock Mode 
            values using the QSPI_Init() function.
            
        (#) Enable QSPI using  QSPI_Cmd() function.
        
        (#) Set QSPI Data Length using QSPI_SetDataLength() function.
  
        (#) Configure the FIFO threshold using QSPI_SetFIFOThreshold() to select 
            at which threshold the FTF event is generated.

        (#) Enable the NVIC and the corresponding interrupt using the function 
            QSPI_ITConfig() if you need to use interrupt mode. 
  
        (#) When using the DMA mode 
           (++) Configure the DMA using DMA_Init() function.
           (++) Active the needed channel Request using SPI_I2S_DMACmd() function.
   
        (#) Enable the SPI using the QSPI_DMACmd() function.
   
        (#) Enable the DMA using the DMA_Cmd() function when using DMA mode.  
  
    @endverbatim 
   *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_qspi.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup QSPI
  * @brief QSPI driver modules
  * @{
  */
#if defined(STM32F412xG) || defined(STM32F413_423xx) || defined(STM32F446xx) || defined(STM32F469_479xx)
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define QSPI_CR_CLEAR_MASK                           0x00FFFFCF
#define QSPI_DCR_CLEAR_MASK                          0xFFE0F7FE
#define QSPI_CCR_CLEAR_MASK                          0x90800000
#define QSPI_PIR_CLEAR_MASK                          0xFFFF0000
#define QSPI_LPTR_CLEAR_MASK                         0xFFFF0000
#define QSPI_CCR_CLEAR_INSTRUCTION_MASK              0xFFFFFF00
#define QSPI_CCR_CLEAR_DCY_MASK                      0xFFC3FFFF
#define QSPI_CR_CLEAR_FIFOTHRESHOLD_MASK             0xFFFFF0FF
#define QSPI_CR_INTERRUPT_MASK                       0x001F0000
#define QSPI_SR_INTERRUPT_MASK                       0x0000001F
#define QSPI_FSR_INTERRUPT_MASK                      0x0000001B
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/* Initialization and Configuration functions *********************************/

/** @defgroup <PPP>_Private_Functions 
  * @{
  */

/** @defgroup <PPP>_Group1 Function Group1 Name
 *  @brief   Function group1 name description (copied from the header file) 
 *
@verbatim  
 ===============================================================================
     ##### < Function group1 name (copied from the header file)
 Note: do not use "Peripheral" or "PPP" word in the function group name >  #####
 ===============================================================================

       [..] < OPTIONAL:
              Add here the most important information to know about the IP features
              covered by this group of function.
        
              For system IPs, this section contains how to use this group API.
            >

@endverbatim
  * @{
  */   

/**
  * @brief  Deinitializes the QSPI peripheral registers to their default
  *         reset values.
  * @param  None
  * @retval None
  */
void QSPI_DeInit(void)
{
  /* Enable QSPI reset state */
  RCC_AHB3PeriphResetCmd(RCC_AHB3Periph_QSPI, ENABLE);
  /* Release QSPI from reset state */
  RCC_AHB3PeriphResetCmd(RCC_AHB3Periph_QSPI, DISABLE);
}

/**
  * @brief  Fills each QSPI_InitStruct member with its default value.
  * @param  QSPI_InitStruct: pointer to a QSPI_InitTypeDef structure which will be initialized.
  * @retval None
  */
void QSPI_StructInit(QSPI_InitTypeDef* QSPI_InitStruct)
{
/*--------- Reset QSPI init structure parameters default values ------------*/
  /* Initialize the QSPI_SShift member */
  QSPI_InitStruct->QSPI_SShift = QSPI_SShift_NoShift ;
  /* Initialize the QSPI_Prescaler member */  
  QSPI_InitStruct->QSPI_Prescaler = 0 ;
  /* Initialize the QSPI_CKMode member */
  QSPI_InitStruct->QSPI_CKMode = QSPI_CKMode_Mode0 ;
  /* Initialize the QSPI_CSHTime member */
  QSPI_InitStruct->QSPI_CSHTime = QSPI_CSHTime_1Cycle ;
  /* Initialize the QSPI_FSize member */
  QSPI_InitStruct->QSPI_FSize = 0 ;
  /* Initialize the QSPI_FSelect member */
  QSPI_InitStruct->QSPI_FSelect = QSPI_FSelect_1 ;
  /* Initialize the QSPI_DFlash member */
  QSPI_InitStruct->QSPI_DFlash = QSPI_DFlash_Disable ;
}

/**
  * @brief  Fills each QSPI_ComConfig_InitStruct member with its default value.
  * @param  QSPI_ComConfig_InitStruct: pointer to a QSPI_ComConfig_InitTypeDef structure which will be initialized.
  * @retval None
  */
void QSPI_ComConfig_StructInit(QSPI_ComConfig_InitTypeDef* QSPI_ComConfig_InitStruct)
{
/*--------- Reset QSPI ComConfig init structure parameters default values ------------*/
    
/* Set QSPI Communication configuration structure parameters default values */
  /* Initialize the QSPI_ComConfig_DDRMode member */
  QSPI_ComConfig_InitStruct->QSPI_ComConfig_DDRMode = QSPI_ComConfig_DDRMode_Disable ;
  /* Initialize the QSPI_ComConfig_DHHC member */
  QSPI_ComConfig_InitStruct->QSPI_ComConfig_DHHC = QSPI_ComConfig_DHHC_Disable ;
  /* Initialize the QSPI_ComConfig_SIOOMode member */
  QSPI_ComConfig_InitStruct->QSPI_ComConfig_SIOOMode = QSPI_ComConfig_SIOOMode_Disable ;
  /* Initialize the QSPI_ComConfig_FMode member */
  QSPI_ComConfig_InitStruct->QSPI_ComConfig_FMode = QSPI_ComConfig_FMode_Indirect_Write ;
  /* Initialize the QSPI_ComConfig_DMode member */
  QSPI_ComConfig_InitStruct->QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_NoData ;
  /* Initialize the QSPI_ComConfig_DummyCycles member */
  QSPI_ComConfig_InitStruct->QSPI_ComConfig_DummyCycles = 0 ;
  /* Initialize the QSPI_ComConfig_ABSize member */
  QSPI_ComConfig_InitStruct->QSPI_ComConfig_ABSize = QSPI_ComConfig_ABSize_8bit ;
  /* Initialize the QSPI_ComConfig_ABMode member */
  QSPI_ComConfig_InitStruct->QSPI_ComConfig_ABMode = QSPI_ComConfig_ABMode_NoAlternateByte ;
  /* Initialize the QSPI_ComConfig_ADSize member */
  QSPI_ComConfig_InitStruct->QSPI_ComConfig_ADSize = QSPI_ComConfig_ADSize_8bit ;
  /* Initialize the QSPI_ComConfig_ADMode member */
  QSPI_ComConfig_InitStruct->QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_NoAddress ;
  /* Initialize the QSPI_ComConfig_IMode member */
  QSPI_ComConfig_InitStruct->QSPI_ComConfig_IMode = QSPI_ComConfig_IMode_NoInstruction ;
  /* Initialize the QSPI_ComConfig_Ins member */
  QSPI_ComConfig_InitStruct->QSPI_ComConfig_Ins = 0 ;
}

/**
  * @brief  Initializes the QSPI peripheral according to the specified 
  *         parameters in the QSPI_InitStruct.
  * @param  QSPI_InitStruct: pointer to a QSPI_InitTypeDef structure that
  *         contains the configuration information for the specified QSPI peripheral.
  * @retval None
  */
void QSPI_Init(QSPI_InitTypeDef* QSPI_InitStruct)
{
  uint32_t tmpreg = 0;
  
  /* Check the QSPI parameters */
  assert_param(IS_QSPI_SSHIFT(QSPI_InitStruct->QSPI_SShift));
  assert_param(IS_QSPI_PRESCALER(QSPI_InitStruct->QSPI_Prescaler));
  assert_param(IS_QSPI_CKMODE(QSPI_InitStruct->QSPI_CKMode));
  assert_param(IS_QSPI_CSHTIME(QSPI_InitStruct->QSPI_CSHTime));
  assert_param(IS_QSPI_FSIZE(QSPI_InitStruct->QSPI_FSize));
	assert_param(IS_QSPI_FSEL(QSPI_InitStruct->QSPI_FSelect));
	assert_param(IS_QSPI_DFM(QSPI_InitStruct->QSPI_DFlash));
  
  /*------------------------ QSPI CR Configuration ------------------------*/
  /* Get the QUADSPI CR1 value */
  tmpreg = QUADSPI->CR;
  /* Clear PRESCALER and SSHIFT bits */
  tmpreg &= QSPI_CR_CLEAR_MASK;
  /* Configure QUADSPI: Prescaler and Sample Shift */
  tmpreg |= (uint32_t)(((QSPI_InitStruct->QSPI_Prescaler)<<24)
                        |(QSPI_InitStruct->QSPI_SShift)
	                      |(QSPI_InitStruct->QSPI_FSelect)
	                      |(QSPI_InitStruct->QSPI_DFlash));  
  /* Write to QUADSPI CR */
  QUADSPI->CR = tmpreg;
  
  /*------------------------ QUADSPI DCR Configuration ------------------------*/
  /* Get the QUADSPI DCR value */
  tmpreg = QUADSPI->DCR;
  /* Clear FSIZE, CSHT and CKMODE bits */
  tmpreg &= QSPI_DCR_CLEAR_MASK;
  /* Configure QSPI: Flash Size, Chip Select High Time and Clock Mode */
  tmpreg |= (uint32_t)(((QSPI_InitStruct->QSPI_FSize)<<16)
                        |(QSPI_InitStruct->QSPI_CSHTime)
                        |(QSPI_InitStruct->QSPI_CKMode));  
  /* Write to QSPI DCR */
  QUADSPI->DCR = tmpreg;  
}

/**
  * @brief  Initializes the QSPI CCR according to the specified 
  *         parameters in the QSPI_ComConfig_InitStruct.
  * @param  QSPI_ComConfig_InitStruct: pointer to a QSPI_ComConfig_InitTypeDef structure that
  *         contains the communication configuration informations about QSPI peripheral.
  * @retval None
  */
void QSPI_ComConfig_Init(QSPI_ComConfig_InitTypeDef* QSPI_ComConfig_InitStruct)
{
  uint32_t tmpreg = 0;

  /* Check the QSPI Communication Control parameters */
  assert_param(IS_QSPI_FMODE       (QSPI_ComConfig_InitStruct->QSPI_ComConfig_FMode));
  assert_param(IS_QSPI_SIOOMODE    (QSPI_ComConfig_InitStruct->QSPI_ComConfig_SIOOMode));
  assert_param(IS_QSPI_DMODE       (QSPI_ComConfig_InitStruct->QSPI_ComConfig_DMode));
  assert_param(IS_QSPI_DCY         (QSPI_ComConfig_InitStruct->QSPI_ComConfig_DummyCycles));
  assert_param(IS_QSPI_ABSIZE      (QSPI_ComConfig_InitStruct->QSPI_ComConfig_ABSize));
  assert_param(IS_QSPI_ABMODE      (QSPI_ComConfig_InitStruct->QSPI_ComConfig_ABMode));
  assert_param(IS_QSPI_ADSIZE      (QSPI_ComConfig_InitStruct->QSPI_ComConfig_ADSize));
  assert_param(IS_QSPI_ADMODE      (QSPI_ComConfig_InitStruct->QSPI_ComConfig_ADMode));
  assert_param(IS_QSPI_IMODE       (QSPI_ComConfig_InitStruct->QSPI_ComConfig_IMode));
  assert_param(IS_QSPI_INSTRUCTION (QSPI_ComConfig_InitStruct->QSPI_ComConfig_Ins));
	assert_param(IS_QSPI_DDRMODE     (QSPI_ComConfig_InitStruct->QSPI_ComConfig_DDRMode));
	assert_param(IS_QSPI_DHHC        (QSPI_ComConfig_InitStruct->QSPI_ComConfig_DHHC));
  
  /*------------------------ QUADSPI CCR Configuration ------------------------*/
  /* Get the QUADSPI CCR value */
  tmpreg = QUADSPI->CCR;
  /* Clear FMODE Mode bits */
  tmpreg &= QSPI_CCR_CLEAR_MASK;
  /* Configure QUADSPI: CCR Configuration */
  tmpreg |=  (uint32_t)( (QSPI_ComConfig_InitStruct->QSPI_ComConfig_FMode)
                       | (QSPI_ComConfig_InitStruct->QSPI_ComConfig_DDRMode)
											 | (QSPI_ComConfig_InitStruct->QSPI_ComConfig_DHHC)
                       | (QSPI_ComConfig_InitStruct->QSPI_ComConfig_SIOOMode)
                       | (QSPI_ComConfig_InitStruct->QSPI_ComConfig_DMode)
                       | (QSPI_ComConfig_InitStruct->QSPI_ComConfig_ABSize)
                       | (QSPI_ComConfig_InitStruct->QSPI_ComConfig_ABMode)                                                                       
                       | (QSPI_ComConfig_InitStruct->QSPI_ComConfig_ADSize)
                       | (QSPI_ComConfig_InitStruct->QSPI_ComConfig_ADMode)
                       | (QSPI_ComConfig_InitStruct->QSPI_ComConfig_IMode)
                       | (QSPI_ComConfig_InitStruct->QSPI_ComConfig_Ins)
                       |((QSPI_ComConfig_InitStruct->QSPI_ComConfig_DummyCycles)<<18));    
  /* Write to QUADSPI DCR */
  QUADSPI->CCR = tmpreg;      
}

/**
  * @brief  Enables or disables QSPI peripheral.
  * @param  NewState: new state of the QSPI peripheral. 
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void QSPI_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable QSPI peripheral */
    QUADSPI->CR |= QUADSPI_CR_EN;
  }
  else
  {
    /* Disable QSPI peripheral */
    QUADSPI->CR &= ~ QUADSPI_CR_EN;
  }
}

/**
  * @brief  Configure the QSPI Automatic Polling Mode.
  * @param  QSPI_Match: Value to be compared with the masked status register to get a match. 
  *          This parameter can be any value between 0x00000000 and 0xFFFFFFFF.
  * @param  QSPI_Mask: Mask to be applied to the status bytes received in polling mode.. 
  *          This parameter can be any value between 0x00000000 and 0xFFFFFFFF.
  * @param  QSPI_Match_Mode: indicates which method should be used for determining a 搈atch
  **/
#endif
  