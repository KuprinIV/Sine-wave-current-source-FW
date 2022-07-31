# Sine-wave-current-source-FW
This is USB-controlled sine wave current source firmware. Host controller is STM32L052C8T6.
Directory contents 
  - /Core/Inc/stm32l0xx_hal_conf.h                                                      HAL configuration file
  - /Core/Inc/stm32l0xx_it.h                                                            Interrupt handlers header file
  - /Core/Inc/main.h                                                                    Main program header file  
  - /Core/Inc/sine_array.h                                                              Contains half period of sine wave samples
  - /Core/Inc/sine_cs.h                                                                 Sine current source driver header file
  
  - /Core/Src/stm32l0xx_it.c                                                            Interrupt handlers
  - /Core/Src/main.c                                                                    Main program, hardware initialization
  - /Core/Src/stm32l0xx_hal_msp.c                                                       HAL MSP module
  - /Core/Src/system_stm32l0xx.c                                                        STM32L0xx system clock configuration file
  - /Core/Src/sine_cs.c                                                                 Sine current source driver source file
  
  - /Drivers                                                                            Contains CMSIS and HAL periphery drivers

  - /Middlewares/ST/STM32_USB_Device_Library/Class/CS_Control/Inc/usbd_cs_control.h     USB interface for control current source header file
  - /Middlewares/ST/STM32_USB_Device_Library/Class/CS_Control/Src/usbd_cs_control.c     USB interface commands and configuration descriptors file
  - /Middlewares/ST/STM32_USB_Device_Library/Class/Core                                 HAL USB Core driver functions

  - /USB_DEVICE                                                                         USB device descriptors and configuration
  
  
  
  
  
