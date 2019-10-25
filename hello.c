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
#define NTHREADS_MIN                1  /**< Minimum Number of Working Threads */
#define NTHREADS_MAX  (THREAD_MAX - 1) /**< Maximum Number of Working Threads */
#define MESSAGE_SIZE  1024
#define MASTER_NODENUM 0 //! IODDR0
#define SLAVE_NODENUM  8 //! Cluster0
/**@}*/

/*============================================================================*
 * Benchmark                                                                  *
 *============================================================================*/


void do_master(void)
{
	int local;
	int remote;
	int portal_in;
	int portal_out;
	char message_in[MESSAGE_SIZE];
	char message_out[MESSAGE_SIZE];

	local  = MASTER_NODENUM;
	remote = SLAVE_NODENUM;

	kprintf("Master: Init!");

	kmemset(message_in, -1, MESSAGE_SIZE);
	kmemset(message_out, MASTER_NODENUM, MESSAGE_SIZE);

	KASSERT((portal_in = kportal_create(local)) >= 0);
	KASSERT((portal_out = kportal_open(local, remote)) >= 0);

	kprintf("Master: Input and Output portals are ready.");

	barrier();

		kprintf("Master: Reading message!");

		/* Read operations. */
		KASSERT(kportal_allow(portal_in, remote) == 0);
		KASSERT(kportal_read(portal_in, message_in, MESSAGE_SIZE) == MESSAGE_SIZE);

		for (int k = 0; k < MESSAGE_SIZE; k++)
			KASSERT(message_in[k] == SLAVE_NODENUM);

		kprintf("Master: Message OK!");

		kprintf("Master: Sending message!");

		/* Write operations. */
		KASSERT(kportal_write(portal_out, message_out, MESSAGE_SIZE) >= MESSAGE_SIZE);

	KASSERT(kportal_close(portal_out) == 0);
	KASSERT(kportal_unlink(portal_in) == 0);

	kprintf("Master: Exit!");
}

void do_slave(void)
{
	int local;
	int remote;
	int portal_in;
	int portal_out;
	char message_in[MESSAGE_SIZE];
	char message_out[MESSAGE_SIZE];

	local  = SLAVE_NODENUM;
	remote = MASTER_NODENUM;

	kprintf("Slave: Init!");

	kmemset(message_in, -1, MESSAGE_SIZE);
	kmemset(message_out, SLAVE_NODENUM, MESSAGE_SIZE);

	KASSERT((portal_in = kportal_create(local)) >= 0);
	KASSERT((portal_out = kportal_open(local, remote)) >= 0);

	kprintf("Slave: Input and Output portals are ready.");

	barrier();

		kprintf("Slave: Sending message!");

		/* Write operations. */
		KASSERT(kportal_write(portal_out, message_out, MESSAGE_SIZE) >= MESSAGE_SIZE);

		kprintf("Slave: Reading message!");

		/* Read operations. */
		KASSERT(kportal_allow(portal_in, remote) == 0);
		KASSERT(kportal_read(portal_in, message_in, MESSAGE_SIZE) == MESSAGE_SIZE);

		for (int j = 0; j < MESSAGE_SIZE; j++)
			KASSERT(message_in[j] == MASTER_NODENUM);

		kprintf("Slave: Message OK!");

	KASSERT(kportal_close(portal_out) == 0);
	KASSERT(kportal_unlink(portal_in) == 0);

	kprintf("Slave: Exit!");
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
	int nodenum;

	((void) argc);
	((void) argv);

	nodenum = knode_get_num();

	if (nodenum == MASTER_NODENUM || nodenum == SLAVE_NODENUM)
	{
		kprintf(HLINE);

			barrier_setup(1, 1);

				/* Slaves waiting 6 seconds because the boot of the IO is slow. */
				if (nodenum != MASTER_NODENUM)
				{
					timer(5 * CLUSTER_FREQ);
					timer(CLUSTER_FREQ);
				}

				/* Everyone waits everyone. */
				barrier();

					if (nodenum == MASTER_NODENUM)
						do_master();
					else
						do_slave();

				/* Everyone waits everyone. */
				barrier();

			barrier_cleanup();

		kprintf(HLINE);
	}

	return (0);
}
