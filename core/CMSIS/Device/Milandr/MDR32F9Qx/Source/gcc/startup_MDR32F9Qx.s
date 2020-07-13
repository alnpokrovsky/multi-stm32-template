/* File: startup_ARMCM3.S
 * Purpose: startup file for Cortex-M3 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 16 August 2013
 *
 * Copyright (c) 2011 - 2013 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
	.syntax	unified
	.cpu cortex-m3
  	.fpu softvfp
  	.thumb

.global g_pfnVectors
.global Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word _sidata
/* start address for the .data section. defined in linker script */
.word _sdata
/* end address for the .data section. defined in linker script */
.word _edata
/* start address for the .bss section. defined in linker script */
.word _sbss
/* end address for the .bss section. defined in linker script */
.word _ebss

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:

/* Copy the data segment initializers from flash to SRAM */
  movs r1, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r3, =_sidata
  ldr r3, [r3, r1]
  str r3, [r0, r1]
  adds r1, r1, #4

LoopCopyDataInit:
  ldr r0, =_sdata
  ldr r3, =_edata
  adds r2, r0, r1
  cmp r2, r3
  bcc CopyDataInit
  ldr r2, =_sbss
  b LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
  movs r3, #0
  str r3, [r2], #4

LoopFillZerobss:
  ldr r3, = _ebss
  cmp r2, r3
  bcc FillZerobss

/* Call the clock system intitialization function.*/
    bl  SystemInit
/* Call static constructors */
    bl __libc_init_array
/* Call the application's entry point.*/
  bl main
  bx lr
.size Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval : None
*/
    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex M3.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
  .size g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
  .word _estack           /* Top of Stack */
  .word Reset_Handler         /* Reset Handler */
  .word NMI_Handler           /* NMI Handler */
  .word HardFault_Handler     /* Hard Fault Handler */
  .word MemManage_Handler     /* MPU Fault Handler */
  .word BusFault_Handler      /* Bus Fault Handler */
  .word UsageFault_Handler    /* Usage Fault Handler */
  .word 0                     /* Reserved */
  .word 0                     /* Reserved */
  .word 0                     /* Reserved */
  .word 0                     /* Reserved */
  .word SVC_Handler           /* SVCall Handler */
  .word DebugMon_Handler      /* Debug Monitor Handler */
  .word 0                     /* Reserved */
  .word PendSV_Handler        /* PendSV Handler */
  .word SysTick_Handler       /* SysTick Handler */

	/* External interrupts */
  .word CAN1_IRQHandler       /* IRQ0 */
  .word CAN2_IRQHandler       /* IRQ1 */
  .word USB_IRQHandler        /* IRQ2 */
  .word 0                     /* IRQ3  reserved */
  .word 0                     /* IRQ4  reserved */
  .word DMA_IRQHandler        /* IRQ5 */
  .word UART1_IRQHandler      /* IRQ6 */
  .word UART2_IRQHandler      /* IRQ7 */
  .word SSP1_IRQHandler       /* IRQ8 */
  .word 0                     /* IRQ9  reserved */
  .word I2C_IRQHandler        /* IRQ10 */
  .word POWER_IRQHandler      /* IRQ11 */
  .word WWDG_IRQHandler       /* IRQ12 */
  .word 0                     /* IRQ13 reserved */
  .word Timer1_IRQHandler     /* IRQ14 */
  .word Timer2_IRQHandler     /* IRQ15 */
  .word Timer3_IRQHandler     /* IRQ16 */
  .word ADC_IRQHandler        /* IRQ17 */
  .word 0                     /* IRQ18 reserved */
  .word COMPARATOR_IRQHandler /* IRQ19 */
  .word SSP2_IRQHandler       /* IRQ20 */
  .word 0                     /* IRQ21 reserved */
  .word 0                     /* IRQ22 reserved */
  .word 0                     /* IRQ23 reserved */
  .word 0                     /* IRQ24 reserved */
  .word 0                     /* IRQ25 reserved */
  .word 0                     /* IRQ26 reserved */
  .word BACKUP_IRQHandler     /* IRQ27 */
  .word EXT_INT1_IRQHandler   /* IRQ28 */
  .word EXT_INT2_IRQHandler   /* IRQ29 */
  .word EXT_INT3_IRQHandler   /* IRQ30 */
  .word EXT_INT4_IRQHandler   /* IRQ31 */


/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

	def_irq_handler NMI_Handler
	def_irq_handler HardFault_Handler
	def_irq_handler MemManage_Handler
	def_irq_handler BusFault_Handler
	def_irq_handler UsageFault_Handler
	def_irq_handler SVC_Handler
	def_irq_handler DebugMon_Handler
	def_irq_handler PendSV_Handler
	def_irq_handler SysTick_Handler

	def_irq_handler CAN1_IRQHandler
	def_irq_handler CAN2_IRQHandler
	def_irq_handler USB_IRQHandler
	def_irq_handler DMA_IRQHandler
	def_irq_handler UART1_IRQHandler
	def_irq_handler UART2_IRQHandler
	def_irq_handler SSP1_IRQHandler
	def_irq_handler I2C_IRQHandler
	def_irq_handler POWER_IRQHandler
	def_irq_handler WWDG_IRQHandler
	def_irq_handler Timer1_IRQHandler
	def_irq_handler Timer2_IRQHandler
	def_irq_handler Timer3_IRQHandler
	def_irq_handler ADC_IRQHandler
	def_irq_handler COMPARATOR_IRQHandler
	def_irq_handler SSP2_IRQHandler
	def_irq_handler BACKUP_IRQHandler
	def_irq_handler EXT_INT1_IRQHandler
	def_irq_handler EXT_INT2_IRQHandler
	def_irq_handler EXT_INT3_IRQHandler
	def_irq_handler EXT_INT4_IRQHandler

	.end
