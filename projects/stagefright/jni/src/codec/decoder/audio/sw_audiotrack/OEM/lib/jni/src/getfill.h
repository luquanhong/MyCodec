/* ------------------------------------------------------------------
 * Copyright (C) 2008 PacketVideo
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
/*
------------------------------------------------------------------------------


 Pathname: ./AAC_baseline/pv_aac_dec/c/include/getfill.h

     Date: 07/05/2001

------------------------------------------------------------------------------
 REVISION HISTORY

 Description: Changed "definition" to "declaration" on line 28 per
              review comments.

 Description:

------------------------------------------------------------------------------
 INCLUDE DESCRIPTION

 This file contains prototype declaration for getfill function.

------------------------------------------------------------------------------
*/

/*----------------------------------------------------------------------------
; CONTINUE ONLY IF NOT ALREADY DEFINED
----------------------------------------------------------------------------*/
#ifndef GETFILL_H
#define GETFILL_H

/*----------------------------------------------------------------------------
; INCLUDES
----------------------------------------------------------------------------*/
#include "s_bits.h"

/*----------------------------------------------------------------------------
; MACROS
; Define module specific macros here
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
; DEFINES
; Include all pre-processor statements here.
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
; EXTERNAL VARIABLES REFERENCES
; Declare variables used in this module but defined elsewhere
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
; SIMPLE TYPEDEF'S
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
; ENUMERATED TYPEDEF'S
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
; STRUCTURES TYPEDEF'S
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
; GLOBAL FUNCTION DEFINITIONS
; Function Prototype declaration
----------------------------------------------------------------------------*/
void getfill(BITS    *pInputStream);

/*----------------------------------------------------------------------------
; END
----------------------------------------------------------------------------*/
#endif


