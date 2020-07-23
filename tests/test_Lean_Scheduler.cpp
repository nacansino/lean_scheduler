/**
 * @file test_Lean_Scheduler.cpp
 * @author Niel Cansino (nielcansino@gmail.com)
 * @brief Test stub for Lean Scheduler library
 * @version 0.1
 * @date 2020-07-23
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Scheduler.hpp"

/**
 * Prototypes of Mock tasks
 */
void task1();
void task2();
void task3();
void task4();

#define TEST_NUM_TASKS (4)

/**
 * @brief Test group for Lean_Scheduler
 * 
 */
TEST_GROUP(Lean_Scheduler_TestGroup)
{
    const uint32_t systick_interval_us = 10000; /* duration of a systick, in us */
    uint32_t ctr_ret;

    /* Build sample task table */
    Scheduler::Task taskTable[TEST_NUM_TASKS] = {
        {task1, 1},     /*!< 1: Run synchronous with systick */
        {task2, 0},     /*!< 0: continuous task */
        {task3, 5},     /*!< 5: Run every 5 sys ticks */
        {task4, 100}    /*!< 100: Run every 100 ticks */
    };

    /* Instance Declaration */
    Scheduler myScheduler;
    
    void setup()
    {
        /* Initialize scheduler */
        myScheduler.init(taskTable, TEST_NUM_TASKS, systick_interval_us);
    }
};

/**
 * @brief Test to drive I/F development
 * 
 */
TEST(Lean_Scheduler_TestGroup, Init){

    /* Test call on tick() */
    ctr_ret = myScheduler.tick();

    /* Call scheduler.run() */
    myScheduler.run();

}

/**
 * @brief Tests the systick counter increment
 * 
 */
TEST(Lean_Scheduler_TestGroup, tick_Increment){

    /* Must be zero after initialization */
    CHECK_EQUAL(0, myScheduler.getTickCount());

    /* First Call - tick and read */
    CHECK_EQUAL(1, myScheduler.tick());

    /* Second Call - tick and read */
    CHECK_EQUAL(2, myScheduler.tick());

    /* Test if it can exceed counting 2^16 */
    for(uint32_t i = 0; i < 65536; ++i){
        (void)myScheduler.tick();
    }
    /* Expect 2(previous value) + 65536(loop) + 1 (this call) */
    CHECK_EQUAL(2 + 65536 + 1, myScheduler.tick());

}

/* 
 * Mock Task definitions for Testing
 */
void task1(){

}

void task2(){

}

void task3(){

}

void task4(){

}