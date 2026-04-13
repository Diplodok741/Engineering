/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

/*
 *@Note
 串口打印调试例程：
 USART1_Tx(PA9)。
 本例程演示使用 USART1(PA9) 作打印调试口输出。

*/
//extern "C"{
//#include "debug.h"
//}
//#include<iostream>
///* Global typedef */
//
///* Global define */
//
///* Global Variable */
//
//
///*********************************************************************
// * @fn      main
// *
// * @brief   Main program.
// *
// * @return  none
// */
//int main(void)
//{
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	Delay_Init();
//	USART_Printf_Init(115200);
//	printf("SystemClk:%d\r\n",SystemCoreClock);
//
//	printf("This is printf example\r\n");
//
//	while(1)
//    {
//
//	}
//}
#include <limits>
#include<iostream>
using namespace std;

void MyFunc(int c)
{
    if (c > numeric_limits< char> ::max())
    {
        throw invalid_argument("MyFunc argument too large.");
    }
    //...
}

// extern "C" void SystemInit()
// {
//
// }

int main()
{
    try
    {
        MyFunc(256); //cause an exception to throw
    }

    catch (invalid_argument& e)
    {
        //cerr << e.what() << endl;
        return -1;
    }
    //...
    return 0;
}
