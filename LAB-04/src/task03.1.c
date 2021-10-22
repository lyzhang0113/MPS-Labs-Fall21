//--------------------------------
// Lab 4 - Analog COnv. and Digital Signal Processing - task01.c
//--------------------------------
//
//

#include <stdio.h>
#include "init.h"

void reset_terminal();
int32_t asm_subtask_1();
int32_t asm_subtask_2(int32_t m1, int32_t m2);
int32_t asm_subtask_3(int32_t x);
int32_t asm_subtask_4(int32_t x);

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
	printf("* Actual Output:   %ld\r\n", asm_subtask_1());
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
	printf("* Actual Output:   %ld\r\n", asm_subtask_2(m1, m2));
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
	printf("* Actual Output:   %ld\r\n", asm_subtask_3(m1));

	m1 = 33804;
	printf("2 * [%ld] / 3 + 5\r\n", m1);
	printf("* Expected Output: %ld\r\n", 2 * m1 / 3 + 5);
	printf("* Actual Output:   %ld\r\n", asm_subtask_3(m1));
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
	printf("* Actual Output:   %ld\r\n", asm_subtask_4(m1));

	m1 = 49125;
	printf("2 * [%ld] / 3 + 5\r\n", m1);
	printf("* Expected Output: %ld\r\n", 2 * m1 / 3 + 5);
	printf("* Actual Output:   %ld\r\n", asm_subtask_4(m1));

	printf("*************************************\r\n\n");

}

void reset_terminal() {
    printf("\033[0m\033[2J\033[;H\033[r"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
}

int32_t asm_subtask_1() {
	int32_t res = 0;
	// Load 479188563 into r3 & Load 407729234 into r4, then add r3 = r3 + r4
	asm volatile ("LDR r3, =0x1C8FD653 \r\n LDR r4, =0x184D7452 \r\n ADD r3, r3, r4");
	asm("STR r3, %0" : "=m" (res)); 	// Store r3 into res
	return res;
}

int32_t asm_subtask_2(int32_t m1, int32_t m2) {
	int32_t res = 0;
	asm("MUL %[out], %[in1], %[in2]" :[out] "=r" (res) :[in1] "r" (m1), [in2] "r" (m2));
	return res;
}

int32_t asm_subtask_3(int32_t x) {
	int32_t res = 0;
	asm volatile ("LDR r3, =0x02 \r\n MUL r2, %[in], r3" : :[in] "r" (x));
	asm volatile ("LDR r3, =0x03 \r\n SDIV r1, r2, r3");
	asm volatile ("LDR r3, =0x05 \r\n ADD %[out], r1, r3" :[out] "=r" (res));
	return res;
}

int32_t asm_subtask_4(int32_t x) {
	int32_t res = 0;
	asm volatile ("LDR r3, =0x03 \r\n SDIV r2, %[in], r3" : :[in] "r" (x));
	asm volatile ("LDR r3, =0x02 \r\n LDR r4, =0x05" : : : "r4");
	asm("MLA %[out], r2, r3, r4" :[out] "=r" (res));
	return res;
}
