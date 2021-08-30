################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c \
/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc.c \
/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c \
/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c \
/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c \
/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c \
/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c \
/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c \
/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c 

OBJS += \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.o \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc.o \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.o \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.o \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.o \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.o \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.o \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.o \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.o 

C_DEPS += \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.d \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc.d \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.d \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.d \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.d \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.d \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.d \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.d \
./stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.d 


# Each subdirectory must supply rules for building sources it contributes
stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.o: /home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c stm32lib/STM32F7xx_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/LAB-01-Template/inc" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc.o: /home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc.c stm32lib/STM32F7xx_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/LAB-01-Template/inc" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.o: /home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c stm32lib/STM32F7xx_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/LAB-01-Template/inc" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.o: /home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c stm32lib/STM32F7xx_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/LAB-01-Template/inc" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.o: /home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c stm32lib/STM32F7xx_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/LAB-01-Template/inc" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.o: /home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c stm32lib/STM32F7xx_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/LAB-01-Template/inc" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.o: /home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c stm32lib/STM32F7xx_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/LAB-01-Template/inc" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.o: /home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c stm32lib/STM32F7xx_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/LAB-01-Template/inc" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.o: /home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c stm32lib/STM32F7xx_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/LAB-01-Template/inc" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/zhangl33/STM32CubeIDE/MPS-Labs-Fall21/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

