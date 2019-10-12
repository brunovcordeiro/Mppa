/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <kbench.h>
#include <nanvix/sys/thread.h>

/**
 * @name Benchmark Parameters
 */
/**@{*/
#define NTHREADS_MIN                1  /**< Minimum Number of Working Threads      */
#define NTHREADS_MAX  (c - 1) /**< Maximum Number of Working Threads      */
/**@}*/

/*============================================================================*
 * Benchmark                                                                  *
 *============================================================================*/

/**
 * @brief Dummy hello world.
 *
 * @param arg Unused argument.
 */
static void *hello_world(void *arg)
{
	((void) arg);

    kprintf("Hello world from Core %d", core_get_id());

	return (NULL);
}

/**
 * @brief Hello World Kernel
 *
 * @param nthreads Number of working threads.
 */
void kernel_hello_world(int nthreads)
{
	kthread_t tid[25];

	for (int i = 0; i < NITERATIONS; i++)
    {
        /* Spawn threads. */
        for (int k = 0; k < nthreads; k++)
            kthread_create(&tid[k], hello_world, NULL);

        /* Wait for threads. */
        for (int k = 0; k < nthreads; k++)
            kthread_join(tid[k], NULL);
	}
}

/*============================================================================*
 * Benchmark Driver                                                           *
 *============================================================================*/

/**
 * @brief Fork-Join Benchmark
 *
 * @param argc Argument counter.
 * @param argv Argument variables.
 */
int main(int argc, const char *argv[])
{
	((void) argc);
	((void) argv);

	kprintf(HLINE);

		kernel_hello_world(25);

	kprintf(HLINE);

	return (0);
}
