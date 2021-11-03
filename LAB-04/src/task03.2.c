//--------------------------------
// Lab 4 - Analog Conversion and Digital Signal Processing - task03.2.c
//--------------------------------
//	Simple Assembly Math (Set 2): Use assembly, evaluate float math expressions
//
// + Needed to use IEEE 754 Floating Point Converter for Assembly values
// + Use "m" instead of "r" or "t" for VLDR

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <stdio.h>
#include "init.h"

//------------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------------
void reset_terminal();
float asm_subtask_1();
float asm_subtask_2(float m1, float m2);
float asm_subtask_3(float x);
float asm_subtask_4(float x);

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
	//Initialize the system
	Sys_Init();
	reset_terminal();

	// Task Set 1:
	printf("********** Task Set 1 **********\r\n\n");

	/* 1. 	Use Basic Inline Assembly:
	 * 			Load into registers
	 * 			Add two float numbers (hard-coded)
	 * 		Use Extended Assembly:
	 * 			Pass the resultant value to a C variable
	 * 		Print Result
	 */
	printf("***      Subtask 1: Addition      ***\r\n");
	printf("* 47.83 + 40.734\r\n");
	printf("* Expected Output: %f\r\n", (float) 41.83f + (float) 40.734f);
	printf("* Actual Output:   %f\r\n", asm_subtask_1());
	printf("*************************************\r\n\n");

	/*
	 * 2. 	Use Extended Assembly:
	 * 			Multiply two single precision float variables
	 * 		Print Result
	 */
	printf("***   Subtask 2: Multiplication   ***\r\n");
	float m1 = 7.501f, m2 = -67.13f;
	printf("* %f * %f\r\n", m1, m2);
	printf("* Expected Output: %f\r\n", (float)m1 * m2);
	printf("* Actual Output:   %f\r\n", asm_subtask_2(m1, m2));
	printf("*************************************\r\n\n");

	/*
	 * 3. 	Use Extended Assembly:
	 * 			Evaluate 2 * x / 3 + 5 using floating point mathematics
	 * 		Print Result
	 */
	printf("***  Subtask 3: ADD, MUL, and DIV ***\r\n");
	m1 = 4.0f;
	printf("2 * [%f] / 3 + 5\r\n", m1);
	printf("* Expected Output: %f\r\n", (float)2 * m1 / 3 + 5);
	printf("* Actual Output:   %f\r\n", asm_subtask_3(m1));

	printf("\r\n");

	m1 = 384.0f;
	printf("2 * [%f] / 3 + 5\r\n", m1);
	printf("* Expected Output: %f\r\n", (float)2 * m1 / 3 + 5);
	printf("* Actual Output:   %f\r\n", asm_subtask_3(m1));
	printf("*************************************\r\n\n");

	/*
	 * 4. 	Use Extended Assembly:
	 * 			Evaluate 2 * x / 3 + 5 using floating point math using MAC
	 * 		Print Result
	 */
	printf("***   Subtask 4: Math Using MAC   ***\r\n");
	m1 = 7.0f;
	printf("2 * [%f] / 3 + 5\r\n", m1);
	printf("* Expected Output: %f\r\n", 2 * m1 / 3 + 5);
	printf("* Actual Output:   %f\r\n", asm_subtask_4(m1));

	printf("\r\n");

	m1 = 49.125f;
	printf("2 * [%f] / 3 + 5\r\n", m1);
	printf("* Expected Output: %f\r\n", 2 * m1 / 3 + 5);
	printf("* Actual Output:   %f\r\n", asm_subtask_4(m1));

	printf("*************************************\r\n\n");

}

void reset_terminal() {
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

float asm_subtask_1() {
	float res = 0;
	// Load 41.83 into r3 & Load 40.734 into r4, then add r3 = r3 + r4
	asm volatile ("VLDR.F32 s0,=0x422751EC \r\n VLDR.F32 s1,=0x4222EF9E \r\n VADD.F32 s0, s0, s1");
	asm("VSTR s0, %0" : "=m" (res)); 	// Store s3 into res
	return (float) res;
}

float asm_subtask_2( float m1, float m2 ) {
	float res = 0;
	asm("VMUL.F32 %[out], %[in1], %[in2]" :[out] "=t" (res) :[in1] "t" (m1), [in2] "t" (m2));
	return res;
}

float asm_subtask_3( float x ) {
	float res = 0;
	asm volatile ("VLDR.F32 s3, =0x40000000 \r\n VMUL.F32 s2, %[in], s3" : :[in] "t" (x));
	asm volatile ("VLDR.F32 s3, =0x40400000 \r\n VDIV.F32 s1, s2, s3");
	asm volatile ("VLDR.F32 s3, =0x40A00000 \r\n VADD.F32 %[out], s1, s3" :[out] "=t" (res));
	return res;
}

float asm_subtask_4( float x ) {
	float res = 0;
	asm volatile ("VLDR.F32 s1, %[in]" : : [in] "m" (x));	// Add in report "t"->"m"
	asm volatile ("VLDR.F32 s3, =0x3F2AAAAB \r\n VLDR.F32 s4, =0x40A00000");
	asm volatile ("VMLA.F32 s4, s1, s3");
	asm volatile ("VSTR s4, %0" : "=m" (res));
	return res;
}
