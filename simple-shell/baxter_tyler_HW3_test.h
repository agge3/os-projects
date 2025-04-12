/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge
* Project:: Assignment 3 – Simple Shell with Pipes
*
* File:: baxter_tyler_H3_test.h
*
* Description:: Test suite interface for Simple Shell.
*
**************************************************************/

#ifndef TEST_H
#define TEST_H

int TEST_Allocators(void);
int TEST_ARGV(void);
int TEST_ProcessOp(void);
int TEST_SHBuf(void);
int TEST_StartTest(const char *msg);
int TEST_SetPrompt(void);
void TEST_TwoCommands(void);
void TEST_SHBufBasic(void);

#endif // TEST_H

// vim: filetype=c:noexpandtab:shiftwidth=8:tabstop=8:softtabstop=8:textwidth=80
