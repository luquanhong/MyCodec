/* ------------------------------------------------------------------
 * Copyright (C) 1998-2010 PacketVideo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */

#ifndef OSCLCONFIG_CHECK_H_INCLUDED
#define OSCLCONFIG_CHECK_H_INCLUDED

/*! \addtogroup osclconfig OSCL config
 *
 * @{
 */

/**
\def Make sure the basic types are defined,
either in osclconfig_limits_typedefs.h or elsewhere.
*/
typedef int8 __int8__check__;
typedef uint8 __uint8__check__;
typedef int16 __int16__check__;
typedef uint16 __uint16__check__;
typedef int32 __int32__check__;
typedef uint32 __uint32__check__;
#define TOsclFileOffset	int32


/**
When OSCL_HAS_BASIC_LOCK is 1,
type TOsclBasicLockObject should be defined as the type used as
a mutex object or handle on the target platform.  It can
be either typedef'd as a C-compilable type or can be #defined.
Examples:
typedef pthread_mutex_t TOsclBasicLockObject;
#define TOsclBasicLockObject RMutex
*/


/*! @} */

#endif // OSCLCONFIG_CHECK_H_INCLUDED


