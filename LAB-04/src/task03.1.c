//--------------------------------
// Lab 4 - Analog COnv. and Digital Signal Processing - task01.c
//--------------------------------
//
//

#include <stdio.h>
#include "init.h"

void reset_terminal();

int32_t result_int32;

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	reset_terminal();

	// Task Set 1:
	printf("********** Task Set 1 **********\r\n\n");

	/* 1. 	Use Basic Inline Assembly:
	 * 			Load into registers
	 * 			Add two integer numbers (hard-coded)
	 * 		Use Extended Assembly:
	 * 			Pass the resultant value to a C variable
	 * 		Print Result
	 */
	printf("***      Subtask 1: Addition      ***\r\n");
	printf("* 479188563 + 407729234\r\n");
	printf("* Expected Output: %ld\r\n", (int32_t) 479188563 + (int32_t) 407729234);

	asm volatile ("LDR r3, =0x1C8FD653"); 				// Load 479188563 into r3
	asm volatile ("LDR r4, =0x184D7452"); 				// Load 407729234 into r4
	asm volatile ("ADD r3, r3, r4"); 						// r3 = r3 + r4
	asm("STR r3, %0" : "=m" (result_int32)); 	// Store r3 into result_int32

	printf("* Actual Output:   %ld\r\n", result_int32);
	printf("*************************************\r\n\n");

	/*
	 * 2. 	Use Extended Assembly:
	 * 			Multiply two int32_t variables (signed long)
	 * 		Print Result
	 */
	printf("***   Subtask 2: Multiplication   ***\r\n");
	int32_t m1 = 7501, m2 = -6713;
	printf("* %ld * %ld\r\n", m1, m2);
	printf("* Expected Output: %ld\r\n", m1 * m2);

	asm("MUL %[out], %[in1], %[in2]" :[out] "=r" (result_int32) :[in1] "r" (m1), [in2] "r" (m2));

	printf("* Actual Output:   %ld\r\n", result_int32);
	printf("*************************************\r\n\n");

	/*
	 * 3. 	Use Extended Assembly:
	 * 			Evaluate 2 * x / 3 + 5 using 32-bit integer math
	 * 		Print Result
	 */
	printf("***  Subtask 3: ADD, MUL, and DIV ***\r\n");
	m1 = 4;
	printf("2 * [%ld] / 3 + 5\r\n", m1);
	printf("* Expected Output: %ld\r\n", 2 * m1 / 3 + 5);

	asm volatile ("LDR r3, =0x02");
	asm("MUL %[out], %[in], r3" :[out] "=r" (m1) :[in] "r" (m1));
	asm volatile ("LDR r3, =0x03");
	asm("SDIV %[out], %[in], r3" :[out] "=r" (m1) :[in] "r" (m1));
	asm volatile ("LDR r3, =0x05");
	asm("ADD %[out], %[in], r3" :[out] "=r" (result_int32) :[in] "r" (m1));

	printf("* Actual Output:   %ld\r\n", result_int32);

	m1 = 33804;
	printf("2 * [%ld] / 3 + 5\r\n", m1);
	printf("* Expected Output: %ld\r\n", 2 * m1 / 3 + 5);

	asm volatile ("LDR r3, =0x02");
	asm("MUL %[out], %[in], r3" :[out] "=r" (m1) :[in] "r" (m1));
	asm volatile ("LDR r3, =0x03");
	asm("SDIV %[out], %[in], r3" :[out] "=r" (m1) :[in] "r" (m1));
	asm volatile ("LDR r3, =0x05");
	asm("ADD %[out], %[in], r3" :[out] "=r" (result_int32) :[in] "r" (m1));

	printf("* Actual Output:   %ld\r\n", result_int32);
	printf("*************************************\r\n\n");

	/*
	 * 4. 	Use Extended Assembly:
	 * 			Evaluate 2 * x / 3 + 5 using 32-bit integer math using MAC
	 * 		Print Result
	 */
	printf("***   Subtask 4: Math Using MAC   ***\r\n");
	m1 = 7;
	printf("2 * [%ld] / 3 + 5\r\n", m1);
	printf("* Expected Output: %ld\r\n", 2 * m1 / 3 + 5);
	asm volatile ("LDR r3, =0x03");
	asm("SDIV %[out], %[in], r3" :[out] "=r" (m1) :[in] "r" (m1));
	asm volatile ("LDR r2, =0x02");
	asm volatile ("LDR r3, =0x05");
	asm("MLA %[out], %[in], r2, r3" :[out] "=r" (result_int32) :[in] "r" (m1));
	printf("* Actual Output:   %ld\r\n", result_int32);

	m1 = 49125;
	printf("2 * [%ld] / 3 + 5\r\n", m1);
	printf("* Expected Output: %ld\r\n", 2 * m1 / 3 + 5);
	asm volatile ("LDR r3, =0x03");
	asm("SDIV %[out], %[in], r3" :[out] "=r" (m1) :[in] "r" (m1));
	asm volatile ("LDR r2, =0x02");
	asm volatile ("LDR r3, =0x05");
	asm("MLA %[out], %[in], r2, r3" :[out] "=r" (result_int32) :[in] "r" (m1));
	printf("* Actual Output:   %ld\r\n", result_int32);

	printf("*************************************\r\n\n");

}

void reset_terminal() {
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}
