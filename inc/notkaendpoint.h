/*
 * notkaendpoint.h
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


#ifndef NOTKAENDPOINT_H
#define NOTKAENDPOINT_H


/**
 * @brief Interface implemented by communication end points.
 */
class NotkaEndPoint {
public:
        virtual int open() = 0;
        virtual void close() = 0;
        virtual ~NotkaEndPoint() {}
};

#endif // NOTKAENDPOINT_H
