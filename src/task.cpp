/*
 * task.cpp
 *
 * Copyright(C) 2017, Piotr Gregor <piotrgregor@rsyncme.org>
 *
 * Notka Online Clipboard
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include "../inc/globals.h"
#include "../inc/task.h"


PeriodicTask::PeriodicTask(int interval_ms)
        : Task(),
          mutex(),
          should_stop(),
          active(false),
          interval_ms(interval_ms)
{
        run();
}

PeriodicTask::~PeriodicTask()
{
}

void PeriodicTask::run()
{
        std::unique_lock<std::mutex> lock(mutex);

        if (active)
                /* Task is already running. */
                return;

        t = std::thread([this] () {

                std::chrono::milliseconds wait_ms(interval_ms) ;

                std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
                std::chrono::time_point<std::chrono::steady_clock> end = now + std::chrono::milliseconds(interval_ms);

                std::unique_lock<std::mutex> lock(mutex);

                while (active) {
                        while (now < end) {
                                //wait_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - now).;
                                should_stop.wait_for(lock, end - now);

                                if (!active) {
                                        /* Task signalled to exit. */
                                        lock.unlock();
                                        return;
                                }

                                now = std::chrono::steady_clock::now();
                        }

                        /* Reset the timeout. */
                        end = now + std::chrono::milliseconds(interval_ms);

                        /* Run the task. */
                        lock.unlock();
                        volatile bool should_stop_hint = run_once();

                        lock.lock();

                        if (should_stop_hint)
                                break;
                }

                active = false;
        });

        active = true;
}

void PeriodicTask::stop()
{
        std::unique_lock<std::mutex> lock(mutex);

        active = false;
        lock.unlock();

        /* Signal the task to exit. */
        should_stop.notify_one();

        /* Synchronize exit with the task. */
        t.join();
}


