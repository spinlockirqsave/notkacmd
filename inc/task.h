/*
 * task.h
 *
 * Copyright(C) 2017, Piotr Gregor <piotr@dataandsignal.com>
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


#ifndef TASK_H
#define TASK_H


#include <functional>
#include <chrono>
#include <future>
#include <cstdio>

#include <QObject>


class Task {
public:
        explicit Task() : t() {}
        Task(Task const&) = delete;
        Task& operator=(Task const&) = delete;
        virtual ~Task() {}

        virtual void run() = 0;
        virtual void stop() = 0;

protected:
        std::thread t;
};

class PeriodicTask : public Task {
public:
        PeriodicTask(int interval_ms);
        virtual ~PeriodicTask();

        void run() override;
        void stop() override;

protected:
        virtual bool run_once() = 0;

        std::mutex              mutex;
        std::condition_variable should_stop;
        bool active;
        int interval_ms;
};

#endif // TASK_H
