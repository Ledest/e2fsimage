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
 * $Id: sfile.c,v 1.1 2004/01/25 23:03:37 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#define BUF_SIZE 4096

int special_inode(ext2_filsys fs, ext2_ino_t e2dir, const char *pathdev,
	int rdev, mode_t mode) 
{
	ext2_ino_t e2ino;
	struct ext2_inode inode;
	int ret;
	const char *fname;
	struct stat s;
	
	/* create a new inode for this special device file */
	ret = ext2fs_new_inode(fs, e2dir, mode, 0, &e2ino);
	E2_ERR(ret, "Could not create new inode for:", pathdev);
	
	/* populate the new inode */
	ext2fs_inode_alloc_stats(fs, e2ino, 1);
	memset(&s, 0, sizeof(struct stat));
	s.st_mode = mode;
	
	init_inode(&inode, &s);
	//inode.i_dev = rdev;

	ret = ext2fs_write_inode(fs, e2ino, &inode);
	E2_ERR(ret, "Ext2 Inode Error", "");
	
	fname = basename(pathdev);

	/* It is time to link the inode into the directory */
	ret = ext2fs_link(fs, e2dir, fname, e2ino, EXT2_FT_REG_FILE);
	if (ret == EXT2_ET_DIR_NO_SPACE) {
		/* resize the directory */
		if (ext2fs_expand_dir(fs, e2dir) == 0)
			ret = ext2fs_link(fs, e2dir, fname, e2ino, EXT2_FT_REG_FILE);
	}			  
	E2_ERR(ret, "Ext2 Link Error", fname);
	return 0;	
}

int read_special_file(ext2_filsys fs, ext2_ino_t e2dir, const char *pathdev)
{
	FILE *fp;
	char dir[256], fname[80], *pdir, type;
	int len, ret, n, major, minor, mode;
	dev_t rdev;
	

	/*copy the full name and remove the basename */
	strncpy(dir, pathdev, 256);
	pdir = strrchr(dir, '/');
	pdir[0] = '\0';
	
	fp = fopen(pathdev, "r");
	ERRNO_ERR(!fp, "Failed to open: ", pathdev);

	while ((n=fscanf(fp, "%79s %c %d %d %o", fname, &type, &major, &minor, &mode))) {
		rdev = (major << 8) + minor;
		if (type == 'c')
			mode |= S_IFCHR;
		if (type == 'b')
			mode |= S_IFBLK;

		special_inode(fs, e2dir, dir, rdev, mode);
	}
	return 0;
}				
