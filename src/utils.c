/*
    Copyright (C) 2016-2017, Jaguar Land Rover. All Rights Reserved.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

/*!----------------------------------------------------------------------------

    @file      utils.c

    This file contains the utility functions that are used by various parts of
    the main shared memory test code.

    There are dump functions defined here that can be called by the code
    during debugging to gain insight into the state and structure of the
    various shared memory segment data structures.

-----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>
#include <limits.h>

#include "sharedMemory.h"
#include "utils.h"


/*! @{ */

//
//  WARNING!!!: All references (and pointers) to data in the VSI signal buffer
//  are performed by using the offset from the shared memory segment base
//  address.  All of these references need to be relocatable references so
//  this will work in multiple processes that have their shared memory
//  segments mapped to different base addresses.
//


/*!-----------------------------------------------------------------------

    d u m p S e m a p h o r e

    @brief Dump the fields of a semaphore.

    This function will dump all of the fields of the specified semaphore that
    make sense.  The mutex and condition variable are not dumped as we have no
    explicit information about what is inside them.

    @param[in] semaphore - The address of the semaphore to be dumped.

------------------------------------------------------------------------*/
void dumpSemaphore ( semaphore_p semaphore )
{
#ifdef SEM_DUMP
    LOG ( "semaphore: %3p\n", (void*)((long)&semaphore->mutex % 0x1000 ) );

    //
    //  Display our count values.
    //
    LOG ( "   message count.........: %'d\n", semaphore->messageCount );
    LOG ( "   waiter count..........: %'d\n", semaphore->waiterCount );

    //
    //  Display the semaphore data...
    //
    LOG ( "   semaphore.mutex.lock..: %d\n", semaphore->mutex.__data.__lock );
    LOG ( "   semaphore.mutex.count.: %d\n", semaphore->mutex.__data.__count );
    LOG ( "   semaphore.mutex.owner.: %d\n", semaphore->mutex.__data.__owner );
    LOG ( "   semaphore.mutex.nusers: %d\n", semaphore->mutex.__data.__nusers );
    LOG ( "   semaphore.mutex.kind..: %d\n", semaphore->mutex.__data.__kind );

#endif
    return;
}


//
//  TODO: The following function is for debugging purposes.
//
//  This function will return the time in microseconds since the shared memory
//  segment was first accessed after being initialized.  The time is in
//  microseconds instead of the raw nanosecond form because the nanosecond
//  portion of the time is not very interesting and just clutters up the
//  output.
//
//  The first time this function is called it will set the global base time
//  contained in the shared memory control block.  All subsequent times are
//  then computed relative to this base time.
//
#define NS_PER_SEC ( 1000000000 )
#define NS_PER_US  ( 1000 )

unsigned long getIntervalTime ( void )
{
#ifdef LOG
    unsigned long currentTime;
    struct timespec timeSpec;

    if ( smControl != 0 )
    {
        clock_gettime ( CLOCK_REALTIME, &timeSpec );
        currentTime = timeSpec.tv_sec * NS_PER_SEC + timeSpec.tv_nsec;

        if ( smControl->globalTime == 0 )
        {
            smControl->globalTime = currentTime;
        }
        return ( currentTime - smControl->globalTime ) / NS_PER_US;
    }
    else
    {
        return 0;
    }
#endif
}


/*! @} */

// vim:filetype=c:syntax=c
