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
 * $Id: initfs.c,v 1.1 2004/01/13 23:02:53 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

static int create_root_dir(ext2_filsys fs)
{
	errcode_t retval;
	struct ext2_inode inode;

	retval = ext2fs_mkdir(fs, EXT2_ROOT_INO, EXT2_ROOT_INO, 0);
	if (retval) {
		fprintf(stderr, "Error while creating root dir");
		return 1;
	}
	retval = ext2fs_read_inode(fs, EXT2_ROOT_INO, &inode);
	if (retval) {
		fprintf(stderr, "Error while reading root inode");
		return 1;
	}
	
	inode.i_uid = default_uid;
	inode.i_gid = default_gid;
	
	retval = ext2fs_write_inode(fs, EXT2_ROOT_INO, &inode);
	if (retval) {
		fprintf(stderr, "Error while setting root inode ownership");
		return 1;
	}
	return 0;
}


int init_fs(ext2_filsys *fs, char *fsname, int size)
{
	struct ext2_super_block super;
	int ret, i;
	struct stat s;
	char *buf;
	FILE *fp;
	
	memset(&super, 0, sizeof(struct ext2_super_block) );

	super.s_rev_level = 1; 
	super.s_feature_incompat |= EXT2_FEATURE_INCOMPAT_FILETYPE;
	super.s_feature_ro_compat |= EXT2_FEATURE_RO_COMPAT_SPARSE_SUPER;
	super.s_blocks_count = size/1024;
	
	ret = stat(fsname, &s);
	
	if (!ret) {  /* is the existing file a regular one ? */
		if (!S_ISREG(s.st_mode)) {
			fprintf(stderr, "Operation on non regular file '%s' cowardly refused\n",
				   	fsname);
			return 1;
		}
		/* use the original size if size is smaller */
		if (size < s.st_size) size = s.st_size;
	}
	fp = fopen(fsname, "wb+");
	if (!fp) {
		perror("Error opening file");
		return 1;
	}
	
	buf = malloc(1024);
	if (!buf) {
		fclose(fp);
		return -1;
	}
	for (i=0; i<size/1024; i++) {
		fwrite(buf, 1024, 1, fp);
	}
	fwrite(buf, 1, (size % 1024), fp);
	free(buf);
	fclose(fp);

	ret = ext2fs_initialize(fsname, 0, &super, unix_io_manager, fs);

	if (ret) {
		fprintf(stderr, "Error while setting up superblock (%d) on file '%s'\n",
			ret, fsname);
		return 2;
	}
	
	(*fs)->super->s_max_mnt_count = 0;
	(*fs)->super->s_checkinterval = 0;
	
	ret = ext2fs_allocate_tables(*fs);
	if (ret) {
		fprintf(stderr, "Error allocating the inode tables on file '%s'\n", fsname);
		return 2;
	}
	
	return create_root_dir(*fs);
}
