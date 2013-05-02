/*
   Copyright (C) 2000-2001, 2004 MySQL AB
    All rights reserved. Use is subject to license terms.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

/*
  Rename a table
*/

#include "fulltext.h"

int mi_rename(const char *old_name, const char *new_name)
{
  char from[FN_REFLEN],to[FN_REFLEN];
#ifdef USE_RAID
  uint raid_type=0,raid_chunks=0;
#endif
  DBUG_ENTER("mi_rename");

#ifdef EXTRA_DEBUG
  check_table_is_closed(old_name,"rename old_table");
  check_table_is_closed(new_name,"rename new table2");
#endif
#ifdef USE_RAID
  {
    MI_INFO *info;
    if (!(info=mi_open(old_name, O_RDONLY, 0)))
      DBUG_RETURN(my_errno);
    raid_type =      info->s->base.raid_type;
    raid_chunks =    info->s->base.raid_chunks;
    mi_close(info);
  }
#ifdef EXTRA_DEBUG
  check_table_is_closed(old_name,"rename raidcheck");
#endif
#endif /* USE_RAID */

  fn_format(from,old_name,"",MI_NAME_IEXT,MY_UNPACK_FILENAME|MY_APPEND_EXT);
  fn_format(to,new_name,"",MI_NAME_IEXT,MY_UNPACK_FILENAME|MY_APPEND_EXT);
  if (my_rename_with_symlink(from, to, MYF(MY_WME)))
    DBUG_RETURN(my_errno);
  fn_format(from,old_name,"",MI_NAME_DEXT,MY_UNPACK_FILENAME|MY_APPEND_EXT);
  fn_format(to,new_name,"",MI_NAME_DEXT,MY_UNPACK_FILENAME|MY_APPEND_EXT);
#ifdef USE_RAID
  if (raid_type)
    DBUG_RETURN(my_raid_rename(from, to, raid_chunks, MYF(MY_WME)) ? my_errno :
		0);
#endif
  DBUG_RETURN(my_rename_with_symlink(from, to,MYF(MY_WME)) ? my_errno : 0);
}