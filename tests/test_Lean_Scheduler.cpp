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
    
    /* Test 1: Call run once after initialization, expect one call */
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

/**
 * @brief   Test that a task registered for two systick
 *          actually runs for every two systicks
 * 
 */
TEST(Lean_Scheduler_TestGroup, run_TwicePerSysTick)
{
    /* Build sample task table */
    Scheduler::Task taskTable_runTwicePerSysTick[TEST_NUM_TASKS_1] = {
        {task1, 2},     /*!< 2: twice per systick */
    };
    
    /* Reinitialize object for this specific test */
    myScheduler.init(taskTable_runTwicePerSysTick, 
                    TEST_NUM_TASKS_1, 
                    SYSTICK_INTERVAL_10mS
                    );
    
    /* Test 1: Run once after initialization, expect one call */
    mock().expectOneCall("task1");
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 2: Run again without ticking */
    mock().expectNoCall("task1");
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 3: Run again after 1 tick only */
    mock().expectNoCall("task1");
    (void)myScheduler.tick();
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 4: Run again after additional 1 tick 
               Task should be called now
    */
    mock().expectOneCall("task1");
    (void)myScheduler.tick();
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 5: Run again.  
               Task should not be called
    */
    mock().expectNoCall("task1");
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 5: Run again after two ticks.
               Task should be called
    */
    mock().expectOneCall("task1");
    (void)myScheduler.tick();
    (void)myScheduler.tick();
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 6: Run after elapsing > 2 ticks.
     *         Task should only be called once
     *         even when run() is called repeatedly.
    */
    const int arbitrary_value = 7;

    mock().expectOneCall("task1");
    for( int i=0; i < arbitrary_value; ++i )
    {
        /* simulate 7 ticks */
        (void)myScheduler.tick();
    }
    myScheduler.run();
    myScheduler.run();
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

}

/**
 * @brief   Test with two tasks on the scheduler tray
 *          Continuous task + Once per systick task
 * 
 */
TEST(Lean_Scheduler_TestGroup, run_TwoTasks_ContinuousAndSingle)
{
    /* Build sample task table */
    Scheduler::Task taskTable_runTwicePerSysTick[TEST_NUM_TASKS_2] = {
        {task1, 0},     /*!< 0: continuous */
        {task2, 1}      /*!< 1: once per systick */
    };
    
    /* Reinitialize object for this specific test */
    myScheduler.init(taskTable_runTwicePerSysTick, 
                    TEST_NUM_TASKS_2, 
                    SYSTICK_INTERVAL_10mS
                    );
    
    /* Test 1: Run once after initialization, expect one call for each task */
    mock().expectOneCall("task1");
    mock().expectOneCall("task2");
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 2: No tick */
    mock().expectOneCall("task1");
    mock().expectNoCall("task2");
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 3: After one tick */
    mock().expectOneCall("task1");
    mock().expectOneCall("task2");
    (void)myScheduler.tick();
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 4: No tick again */
    mock().expectOneCall("task1");
    mock().expectNoCall("task2");
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 5: Performance after Arbitrary ticks */
    const int arbitrary_value = 7;

    mock().expectNCalls(3, "task1");
    mock().expectOneCall("task2");
    for( int i=0; i < arbitrary_value; ++i )
    {
        /* simulate 7 ticks */
        (void)myScheduler.tick();
    }
    myScheduler.run();
    myScheduler.run();
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

}

/**
 * @brief   Test with three tasks on the scheduler tray
 *          one (1) Continuous task +
 *          one (1) Once per systick task + 
 *          one (1) once per two systicks
 * 
 */
TEST(Lean_Scheduler_TestGroup, run_ThreeTasks_012)
{
    /* Build sample task table */
    Scheduler::Task taskTable_runTwicePerSysTick[TEST_NUM_TASKS_3] = {
        {task1, 0},     /*!< 0: continuous */
        {task2, 1},     /*!< 1: once per systick */
        {task3, 2}      /*!< 1: once per two systicks */
    };
    
    /* Reinitialize object for this specific test */
    myScheduler.init(taskTable_runTwicePerSysTick, 
                    TEST_NUM_TASKS_3, 
                    SYSTICK_INTERVAL_10mS
                    );
    
    /* Test 1: Run once after initialization, expect one call for each task */
    mock().expectOneCall("task1");
    mock().expectOneCall("task2");
    mock().expectOneCall("task3");
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 2: No tick */
    mock().expectOneCall("task1");
    mock().expectNoCall("task2");
    mock().expectNoCall("task3");
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 3: tick=1 since start */
    mock().expectOneCall("task1");
    mock().expectOneCall("task2");
    mock().expectNoCall("task3");
    (void)myScheduler.tick();
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 4: tick=2 since start */
    mock().expectOneCall("task1");
    mock().expectOneCall("task2");
    mock().expectOneCall("task3");
    (void)myScheduler.tick();
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 5: tick=2 again */
    mock().expectOneCall("task1");
    mock().expectNoCall("task2");
    mock().expectNoCall("task3");
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

    /* Test 6: tick=3 since start */
    mock().expectOneCall("task1");
    mock().expectOneCall("task2");
    mock().expectNoCall("task3");
    (void)myScheduler.tick();
    myScheduler.run();
    mock().checkExpectations();
    mock().clear();

}

/**
 * @brief Test three tasks with different intervals
 * 
 */
TEST(Lean_Scheduler_TestGroup, run_ThreeTasks_DifferentIntervals)
{
    /* Build sample task table */
    Scheduler::Task taskTable_runTwicePerSysTick[TEST_NUM_TASKS_3] = {
        {task1, 1},     /*!< 1: once per systick */
        {task2, 5},     /*!< 1: once per five systicks */
        {task3, 7}      /*!< 1: once per seven systicks */
    };
    
    /* Reinitialize object for this specific test */
    myScheduler.init(taskTable_runTwicePerSysTick, 
                    TEST_NUM_TASKS_3, 
                    SYSTICK_INTERVAL_10mS
                    );
    
    /* create an artificial main loop that runs with the systick */
    for( uint32_t ctr=0; ctr < 100; ++ctr ){

        mock().expectOneCall("task1");  /* Expect one call every loop */
        
        /* Expect a call on task2 for every multiples of 5 */
        if( 0 == ctr % 5 )
        {
            mock().expectOneCall("task2");
        }
        else
        {
            mock().expectNoCall("task2");
        }

        /* Expect a call on task3 for every multiples of 7 */
        if( 0 == ctr % 7 )
        {
            mock().expectOneCall("task3");
        }
        else
        {
            mock().expectNoCall("task3");
        }
        
        myScheduler.run();
        mock().checkExpectations();
        mock().clear();
    
        myScheduler.tick();
    }

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