/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 2001 Christian Hohnstaedt.
 *
 *  All rights reserved.
 *
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  - Neither the name of the author nor the names of its contributors may be 
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 *
 * http://www.hohnstaedt.de/e2fsimage
 * email: christian@hohnstaedt.de
 *
 * $Id: dirent.c,v 1.9 2004/01/28 20:17:28 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Scan a directory and copy all the files to the e2c->curr_e2dir
 * Arguments:
 *  fs - the filesystem
 *  e2c->curr_e2dir - the directory inode in the e2file
 *  e2c->curr_path - the name of the directory containing all subdirs upto here
 */

int e2cpdir(e2i_ctx_t *e2c)
{
	struct dirent **namelist;
	int i,ret, len, count;
	char path[256], *ppath;

	ret = scandir(e2c->curr_path, &namelist, 0, 0);
	if (ret < 0) {
		perror("scandir");
		return -1;
	}
	else {
		len = strlen(e2c->curr_path);
		strncpy(path, e2c->curr_path, 256);
		if (path[len-1] != '/') {
			path[len++] = '/';
		}
		ppath = path + len;
		count = ret;
		for (i = 0; i<count; i++) {
			strncpy(ppath, namelist[i]->d_name, 256 - len);
			free(namelist[i]);
			if (!strncmp(".", ppath, 2)) continue ;
			if (!strncmp("..", ppath, 3)) continue ;
			e2c->curr_path = path;
			
			if (!strncmp(e2c->dev_file, ppath, strlen(e2c->dev_file))) {
				ret = read_special_file(e2c);
				if (ret) return ret;
				continue;
			}
			ret = e2filetype_select(e2c);
			if (ret) return ret;
        }
        free(namelist);
    }
	return 0;
}

static int e2check_hardlink(e2i_ctx_t *e2c, ino_t ino)
{
	const char *fname;
	struct ext2_inode inode;
	ext2_ino_t e2ino;
	int ret;
	
	e2ino = inodb_search(e2c->ino_db, ino);
	if (e2ino == 0) return 1;

	/* hard link */
	ret = ext2fs_read_inode(e2c->fs, e2ino, &inode);
	E2_ERR(ret, "Ext2 read Inode Error", "");
		
	inode.i_links_count++;
	
	ret = ext2fs_write_inode(e2c->fs, e2ino, &inode);
	E2_ERR(ret, "Ext2 write Inode Error", "");
				 
	if (e2c->verbose)
		printf("Creating hard link %s\n", e2c->curr_path);

	fname = basename(e2c->curr_path);
	
	e2c->cnt.hardln++;
		
	/* It is time to link the inode into the directory */
	ret = ext2fs_link(e2c->fs, e2c->curr_e2dir, fname, e2ino, EXT2_FT_REG_FILE);
	if (ret == EXT2_ET_DIR_NO_SPACE) {
		/* resize the directory */
		if (ext2fs_expand_dir(e2c->fs, e2c->curr_e2dir) == 0)
			ret = ext2fs_link(e2c->fs, e2c->curr_e2dir, fname, e2ino, EXT2_FT_REG_FILE);
	}			  
	E2_ERR(ret, "Ext2 Link Error", fname);
		
	return 0;
		
}
	   
int e2filetype_select(e2i_ctx_t *e2c)
{
	struct stat s;  
	ext2_ino_t newe2dir, olde2dir;
	int ret;
	
	lstat(e2c->curr_path, &s);
	
	ret = e2check_hardlink(e2c, s.st_ino);
	if (ret <= 0) return ret;
	
	if (S_ISDIR(s.st_mode)) {
		ret = e2mkdir(e2c, &newe2dir);
		if (ret) return ret;
		olde2dir = e2c->curr_e2dir;
		e2c->curr_e2dir = newe2dir;
		ret = e2cpdir(e2c);
		e2c->curr_e2dir = olde2dir;
		if (ret) return ret;
	}
	if (S_ISREG(s.st_mode)) {
		ret = e2cp(e2c);
		if (ret) return ret;
	}
	if (S_ISLNK(s.st_mode)) {
		ret = e2symlink(e2c);
		if (ret) return ret;
	}
	if (S_ISSF(s.st_mode)) {
		ret = e2mknod(e2c);
		if (ret) return ret;
	}
	return 0;
}	

