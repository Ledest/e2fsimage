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
 * $Id: main.c,v 1.6 2004/01/18 13:52:20 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <unistd.h>

int default_uid = 0;
int default_gid = 0;
int verbose = 0;
int preserve_uidgid = 0;

static void usage(char *name) {
	printf("%s [-o outfile] [-d rootdir] [-u uid] [-g gid] [-v] [-c]", name);
	exit(0);
}

int main(int argc, char *argv[] )
{
	int ret = 0, c;
	ext2_filsys fs;
	char *e2fsfile = NULL, *rootdir = NULL;
	
	init_ext2_err_tbl();
	
	do {
		c = getopt(argc, argv, "vcho:d:u:g:");
		switch (c) {
			case 'v': verbose = 1; break;
			case 'p': preserve_uidgid = 1; break;
			case 'u': default_uid = atoi(optarg); break;
			case 'g': default_gid = atoi(optarg); break;
			case 'o': e2fsfile = optarg; break;
			case 'd': rootdir = optarg; break;
			case 'h': usage(argv[0]); break;
		}
			 
	} while (c >= 0);
//	init_fs(&fs, "e2file", 1024);
//	fs->umask = 022;	
	if(! (rootdir && e2fsfile)) usage (argv[0]);
	ret = ext2fs_open (e2fsfile, EXT2_FLAG_RW, 1, 1024, unix_io_manager, &fs);
	E2_ERR(ret, "Error opening fs: ", e2fsfile);

	ext2fs_read_inode_bitmap(fs);
	ext2fs_read_block_bitmap(fs);

	if (argc <2) {
		fprintf(stderr, "Error give src dir\n");
		return -1;
	}
	e2cpdir(fs, EXT2_ROOT_INO, rootdir);
/*
	for(i=0; i<5; i++) {
		ret = e2mkdir(fs, thedir, "src", &thedir);
		if(ret) return ret;
	}
	
	copy_file(fs, thedir, "src/e2fsimage.h");
	e2symlink(fs, thedir, "src/lnk2e2fsimage.h");
*/
	ext2fs_flush(fs);

	ret = ext2fs_close(fs);
	E2_ERR(ret, "Error closing the filesystem file:", e2fsfile);

	return ret;
}

