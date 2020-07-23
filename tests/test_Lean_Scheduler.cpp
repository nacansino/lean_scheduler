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

#define TEST_NUM_TASKS_0 (0)
#define TEST_NUM_TASKS_1 (1)
#define TEST_NUM_TASKS_2 (2)
#define TEST_NUM_TASKS_3 (3)
#define TEST_NUM_TASKS_4 (4)
#define SYSTICK_INTERVAL_10mS (10000U) /* duration of a systick, in us */

/**
 * @brief Test group for Lean_Scheduler
 * 
 */
TEST_GROUP(Lean_Scheduler_TestGroup)
{
    uint32_t ctr_ret;

    /* Build sample task table */
    Scheduler::Task taskTable[TEST_NUM_TASKS_4] = {
        {task1, 1},     /*!< 1: Run synchronous with systick */
        {task2, 0},     /*!< 0: continuous task */
        {task3, 5},     /*!< 5: Run every 5 sys ticks */
        {task4, 100}    /*!< 100: Run every 100 ticks */
    };

    /* Instance Declaration */
    Scheduler myScheduler;
    
    void setup()
    {
        /* Initialize scheduler. Assume initialization would pass */
        (void)myScheduler.init(taskTable, TEST_NUM_TASKS_4, SYSTICK_INTERVAL_10mS);
    }

    void teardown()
    {
        mock().clear();
    }
};

/**
 * @brief Tests the systick counter increment
 * 
 */
TEST(Lean_Scheduler_TestGroup, tick_Increment)
{

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

/**
 * @brief Edge condition tests on init method
 * 
 */
TEST(Lean_Scheduler_TestGroup, init_EdgeConditions)
{

    bool retval;
    constexpr uint32_t systick_interval = 10000U;

    /* Local instance */
    Scheduler sch1;

    /* TEST 1: Check the case of passing uninitialized table */
    retval = sch1.init(NULL, TEST_NUM_TASKS_3, systick_interval);
    CHECK_FALSE(retval);

    /* TEST 2: Check the case of uninitialized table */
    Scheduler::Task taskTable_uninit[TEST_NUM_TASKS_3] = {
        {task1, 1},
        {NULL, 0},
        {task3, 5}
    };  /* Build sample task table with NULL function member */
    retval = sch1.init(taskTable_uninit, TEST_NUM_TASKS_3, systick_interval);
    CHECK_FALSE(retval);
    
    /* TEST 3: Check the case where num_tasks exceed the actual tasks */
    Scheduler::Task taskTable_ok[TEST_NUM_TASKS_3] = {
        {task1, 1},
        {task2, 0},
        {task3, 5}
    };
    retval = sch1.init(taskTable_uninit, TEST_NUM_TASKS_4, systick_interval);
    CHECK_FALSE(retval);

    /* TEST 4: Test OK. use the global table */
    retval = sch1.init(taskTable, TEST_NUM_TASKS_4, systick_interval);
    CHECK_TRUE(retval);

}

/**
 * @brief Test Edge conditions on calls on run
 * 
 */
TEST(Lean_Scheduler_TestGroup, run_EdgeConditions)
{
    Scheduler sch1;

    /* Run should not return segfault when called uninitialized */
    sch1.run();

}

/**
 * @brief Test if all attached tasks are called in one pass
 * 
 */
TEST(Lean_Scheduler_TestGroup, run_All)
{

    /* Expect one call for each of the tasks */
    mock().expectOneCall("task1");
    mock().expectOneCall("task2");
    mock().expectOneCall("task3");
    mock().expectOneCall("task4");
    
    myScheduler.run();

    mock().checkExpectations();

}

/**
 * @brief   Test that a continuous tasks continue running
 *          even if sys_tick is not incremented
 * 
 */
TEST(Lean_Scheduler_TestGroup, run_Continuous)
{
    const uint8_t expect_calls = 50;

    /* Build sample task table */
    Scheduler::Task taskTable_runContinuous[TEST_NUM_TASKS_1] = {
        {task2, 0},     /*!< 0: continuous task */
    };

    /* Reinitialize object for this specific test */
    myScheduler.init(taskTable_runContinuous, 
                    TEST_NUM_TASKS_1, 
                    SYSTICK_INTERVAL_10mS
                    );

    /*  Expect [expect_calls] number of calls on task2, 
        the continuos task, when run() is called that many times
     */
    mock().expectNCalls(expect_calls, "task2");

    for( size_t i = 0; i < expect_calls; ++i){
        myScheduler.run();  /* Run the scheduler 50 times */
    }

    mock().checkExpectations();

}

/**
 * @brief   Test that a task registered for one systick
 *          actually runs for every one systick only
 * 
 */
TEST(Lean_Scheduler_TestGroup, run_OncePerSysTick)
{

    /* Build sample task table */
    Scheduler::Task taskTable_runOncePerSysTick[TEST_NUM_TASKS_1] = {
        {task1, 1},     /*!< 1: once per systick */
    };
    
    /* Reinitialize object for this specific test */
    myScheduler.init(taskTable_runOncePerSysTick, 
                    TEST_NUM_TASKS_1, 
                    SYSTICK_INTERVAL_10mS
                    );
    
    /* Test 1: Call run once, expect one call */
    mock().expectOneCall("task1");
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 2: Call run again
     * expect no call because there has not been
     * a systick since last call
     */
    mock().expectNoCall("task1");
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 3: Call run again after one tick */
    mock().expectOneCall("task1");
    (void)myScheduler.tick();
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 4: Call run again without a tick;
     * function must not be called
     */
    mock().expectNoCall("task1");
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 5: SysTick overruns the scheduler
     *  Even if systick() is incremented multiple times,
     *  task should only be called once
     */
    mock().expectOneCall("task1");
    (void)myScheduler.tick();
    (void)myScheduler.tick();
    (void)myScheduler.tick();
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();
    
}
/* 
 * Mock Task definitions for Testing
 */
void task1(){
    mock().actualCall("task1");
}

void task2(){
    mock().actualCall("task2");
}

void task3(){
    mock().actualCall("task3");
}

void task4(){
    mock().actualCall("task4");
}