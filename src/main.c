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
 * $Id: main.c,v 1.14 2004/01/28 12:54:00 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <unistd.h>


static void usage(char *name)
{
	printf("%s [-f imgfile] [-d rootdir] [-u uid] [-g gid] [-s size] "
			"[-v] [-n] [-D devicefile]\n\n"
			"-f  filesystem image file\n"
			"-d  root directory to be copied to the image\n"
			"-u  uid to use instead of the real one\n"
			"-g  gid to use instead of the real one\n"
			"-v  be verbose\n"
			"-s  size of the filesystem\n"
			"-D  device filename\n"
			"-p  preserve uid and gid\n"
			"-n  do not create the filesystem, use an existing one\n\n",
			name);
	exit(0);
}

int main(int argc, char *argv[] )
{
	int ret = 0, c, create=1, ksize=4096;
	char *e2fsfile = NULL;
	e2i_ctx_t e2c;
	
	init_ext2_err_tbl();
	memset(&e2c, 0, sizeof(e2c));
	
	e2c.dev_file = ".DEVICES";
	e2c.curr_e2dir = EXT2_ROOT_INO;
	e2c.curr_path = ".";
	
	printf("%s - Version: %s\n",  argv[0], VER);
	
	do {
		c = getopt(argc, argv, "vnhpf:d:u:g:s:D:");
		switch (c) {
			case 'v': e2c.verbose = 1; break;
			case 'p': e2c.preserve_uidgid = 1; break;
			case 'u': e2c.default_uid = atoi(optarg); break;
			case 'g': e2c.default_gid = atoi(optarg); break;
			case 'f': e2fsfile = optarg; break;
			case 'd': e2c.curr_path = optarg; break;
			case 'h': usage(argv[0]); break;
			case 'n': create = 0; break;
			case 's': ksize = atoi(optarg); break;
			case 'D': e2c.dev_file = optarg; break;
		}
			 
	} while (c >= 0);
	
	if (create) {
		mke2fs(e2fsfile, ksize);
	}
	
	e2c.ino_db = inodb_init();
	
	if(! (e2c.curr_path && e2fsfile)) usage (argv[0]);
	ret = ext2fs_open (e2fsfile, EXT2_FLAG_RW, 1, 1024, unix_io_manager, &e2c.fs);
	E2_ERR(ret, "Error opening filesystem: ", e2fsfile);

	ext2fs_read_inode_bitmap(e2c.fs);
	ext2fs_read_block_bitmap(e2c.fs);
	
	ret = e2cpdir(&e2c);
	inodb_free(e2c.ino_db);
	if (ret) return ret;
	
	ext2fs_flush(e2c.fs);
	
	ret = ext2fs_close(e2c.fs);
	E2_ERR(ret, "Error closing the filesystem file:", e2fsfile);

	return ret;
}

