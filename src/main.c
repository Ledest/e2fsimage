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
 * $Id: main.c,v 1.10 2004/01/27 15:34:12 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <unistd.h>

int default_uid = 0;
int default_gid = 0;
int verbose = 0;
int preserve_uidgid = 0;
inodb_t *ino_db = NULL;

static void usage(char *name) {
	printf("%s [-f imgfile] [-d rootdir] [-u uid] [-g gid] [-s size] [-v] [-n]\n\n"
			"-f  filesystem image file\n"
			"-d  root directory to be copied to the image\n"
			"-u  uid to use instead of the real one\n"
			"-g  gid to use instead of the real one\n"
			"-v  be verbose\n"
			"-s  size of the filesystem\n"
			"-n  do not create the filesystem, use an existing one\n", name);
	exit(0);
}

int main(int argc, char *argv[] )
{
	int ret = 0, c, create=1, ksize=4096;
	ext2_filsys fs;
	char *e2fsfile = NULL, *rootdir = NULL;
	
	init_ext2_err_tbl();
	
	do {
		c = getopt(argc, argv, "vnhf:d:u:g:s:");
		switch (c) {
			case 'v': verbose = 1; break;
			case 'p': preserve_uidgid = 1; break;
			case 'u': default_uid = atoi(optarg); break;
			case 'g': default_gid = atoi(optarg); break;
			case 'f': e2fsfile = optarg; break;
			case 'd': rootdir = optarg; break;
			case 'h': usage(argv[0]); break;
			case 'n': create = 0; break;
			case 's': ksize = atoi(optarg); break;
		}
			 
	} while (c >= 0);
/*	
 *	init_fs(&fs, "e2file", 1024);
 *	fs->umask = 022;	
 */
	if (create) {
		mke2fs(e2fsfile, ksize);
	}
	
	ino_db = inodb_init();
	if(! (rootdir && e2fsfile)) usage (argv[0]);
	ret = ext2fs_open (e2fsfile, EXT2_FLAG_RW, 1, 1024, unix_io_manager, &fs);
	E2_ERR(ret, "Error opening filesystem: ", e2fsfile);

	ext2fs_read_inode_bitmap(fs);
	ext2fs_read_block_bitmap(fs);
#if 0
	{
		struct ext2_inode in;
		int i;
		ext2fs_read_inode(fs, 13, &in);
		for (i=0; i<sizeof(struct ext2_inode); i++) {
			if (i%4 == 0) printf("\n");
			printf(" %03d", ((char *)&in)[i]);
		}
		printf("\n\n");
		ext2fs_read_inode(fs, 15, &in);
		for (i=0; i<sizeof(struct ext2_inode); i++) {
			if (i%4 == 0) printf("\n");
			printf(" %03d", ((char *)&in)[i]);
		}
	}
#endif
	ret = e2cpdir(fs, EXT2_ROOT_INO, rootdir);
	inodb_free(ino_db);
	if (ret) return ret;
	
	ext2fs_flush(fs);
	
	ret = ext2fs_close(fs);
	E2_ERR(ret, "Error closing the filesystem file:", e2fsfile);

	return ret;
}

