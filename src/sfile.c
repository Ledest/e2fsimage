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
 * $Id: sfile.c,v 1.5 2004/01/28 09:16:57 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#define BUF_SIZE 4096

static int special_inode(ext2_filsys fs, ext2_ino_t e2dir, const char *pathdev,
	int rdev, mode_t mode, ino_t ino) 
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
	
	if (ino) { 
		ret = inodb_add(ino_db, ino, e2ino);
		if (ret) return -1;
	}
	
	init_inode(&inode, &s);
	inode.i_block[0] = rdev;

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

int e2mknod(ext2_filsys fs, ext2_ino_t e2dir, const char *pathfile)
{
	int ret;
	struct stat s;
	
	ret = lstat(pathfile, &s);
    ERRNO_ERR(ret, "Could not stat: ", pathfile);
	
	if (!(S_ISCHR(s.st_mode) || S_ISBLK(s.st_mode)) ) {
		fprintf(stderr, "File '%s' is not a block or charspecial device\n", pathfile);
		return -1;
	}
	if (verbose)
		printf("Creating special file: %s\n", pathfile);
	
	return special_inode(fs, e2dir, basename(pathfile), s.st_rdev, s.st_mode, s.st_ino);
}
			

int read_special_file(ext2_filsys fs, ext2_ino_t e2dir, const char *pathdev)
{
	FILE *fp;
	char fname[80], *line_buf, type;
	int n, major, minor, mode, ln=0;
	dev_t rdev;
	
	fp = fopen(pathdev, "r");
	ERRNO_ERR(!fp, "Failed to open: ", pathdev);
	fname[0] = '\0';
	
	line_buf = (char *)malloc(256);
	
	while (fgets(line_buf, 256, fp) != 0) {
		ln++;
		if (strlen(line_buf)>254) {
			char c = line_buf[254];
			fprintf(stderr, "Line too long %d\n",ln);
			while (c != '\n' && c >0) c = fgetc(fp);
			continue;
		}
		n = sscanf(line_buf, "%79s %c %d %d %o",
			fname, &type, &major, &minor, &mode);
		
		if (line_buf[0] == '\n' || fname[0] == '#' ) continue;
		if (n != 5) {
			fprintf(stderr, "Bad entry in %s, line %d (%s)\n",
				pathdev, ln, fname);
			free(line_buf);
			return -1;
		}	
		rdev = (major << 8) + minor;
		switch (type) {
			case 'c' : mode |= S_IFCHR; break;
			case 'b' : mode |= S_IFBLK; break;
			default:
				fprintf(stderr, "Bad mode (%c) in %s, line %d\n",
					type, pathdev, ln);
				free(line_buf);
				return -1;
		}
		if (verbose)
			printf("Creating special file: %s (%c, Major %d, Minor: %d)\n",
					fname, type, major, minor);
		special_inode(fs, e2dir, fname, rdev, mode, 0);
		fname[0] = '\0';
	}
	free(line_buf);
	return 0;
}				
