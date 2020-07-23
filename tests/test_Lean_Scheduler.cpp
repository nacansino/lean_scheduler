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
#include "Scheduler.hpp"

/**
 * Prototypes of Mock tasks
 */
void task1();
void task2();
void task3();
void task4();

/**
 * @brief Test group for Lean_Scheduler
 * 
 */
TEST_GROUP(Lean_Scheduler_TestGroup)
{
    /* Instance Declaration */
    Scheduler myScheduler;

    void setup()
    {

    }
};

/**
 * @brief Test to drive I/F development
 * 
 */
TEST(Lean_Scheduler_TestGroup, Init){

    /* Build sample task table */
    Scheduler::Task taskTable[] = {
        {task1, 1},     /*!< 1: Run synchronous with systick */
        {task2, 0},     /*!< 0: continuous task */
        {task3, 5},     /*!< 5: Run every 5 sys ticks */
        {task4, 100}    /*!< 100: Run every 100 ticks */
    };

}

/* Demo tasks for initialization */

void task1(){}
void task2(){}
void task3(){}
void task4(){}