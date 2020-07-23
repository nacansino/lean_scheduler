/**
 * @file Scheduler.cpp
 * @author Niel Cansino (nielcansino@gmail.com)
 * @brief A lean, lightweight, and portable scheduler designed for embedded C++ applications.
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

#include "Scheduler.hpp"

/**
 * @brief Class constructor
 * 
 */
Scheduler::Scheduler(/* args */)
{
}

/**
 * @brief Destroy the Scheduler:: Scheduler object
 * 
 */
Scheduler::~Scheduler()
{
}

/**
 * Function implementations
 */

/**
 * @brief   Initializes the scheduler object.
 *          This function binds the array of tasks [taskTable] 
 *          to be executed by the scheduler.
 *          This also gives the object information on how long a systick is.
 * 
 * @param taskTable 
 * @param num_tasks 
 */
void Scheduler::init(Task* const taskTable, const uint16_t num_tasks, const uint32_t systick_interval)
{
    /* Initialize system tick counter to zero */
    sys_tick_ctr_ = 0;
}

/**
 * @brief Increments the system tick 
 * 
 * @return uint32_t 
 */
uint32_t Scheduler::tick(void)
{
    return ++sys_tick_ctr_;
}

/**
 * @brief Get the system tick counter value
 * 
 * @return uint32_t System Tick Counter Value
 */
uint32_t Scheduler::getTickCount(void)
{
    return sys_tick_ctr_;
}

/**
 * @brief Runs the tasks registered via init().
 * 
 */
void Scheduler::run(void)
{

}
