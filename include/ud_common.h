/*
   -----------------------------------------------------------------------------
   
   Copyright (C) 2015  Kristjan Runarsson, Terry Garyet, Red oak Canyon LLC.

   This file is part of the Red Oak Canyon Power Management Daemon (rocpmd).
      
   rocpmd is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
                  
   rocpmd is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
                              
   You should have received a copy of the GNU General Public License
   along with rocpmd.  If not, see <http:  www.gnu.org/licenses/>.
   
   ----------------------------------------------------------------------------

   File name: ud_comman.h
   Date:      2015-08-25 06:27
   Author:    Kristjan Runarsson

   ----------------------------------------------------------------------------
*/

#ifndef __UD_COMMON_H__
#define __UD_COMMON_H__

#define ROCPMD_COMM_ERROR               -2
#define ROCPMD_SEND_OFF                  1
#define ROCPMD_SEND_POWER_LEVEL_PERCENT  2
#define ROCPMD_SEND_POWER_LEVEL_RAW      3

#define ROCPMD_POWER_OFF_UDS_NAME    "rocpmd_off"
#define ROCPMD_POWER_LEVEL_UDS_NAME  "rocpmd_power_level"


#endif
