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
 * $Id: e2fsimage.h,v 1.6 2004/01/18 13:52:20 chris2511 Exp $ 
 *
 */                           

#include <e2p/e2p.h>
#include <ext2fs/ext2fs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef E2FSIMAGE_H
#define E2FSIMAGE_H

#define E2_ERR(ret,x,y) if (ret) { fprintf(stderr,"%s(%d): %s%s - ext2 error: %s\n", \
					__FILE__, __LINE__, x, y, error_message(ret)); return ret; }
#define ERRNO_ERR(ret,x,y)  if (ret) { fprintf(stderr,"%s(%d): %s%s - Error: %s\n", \
	                    __FILE__, __LINE__, x, y, strerror(errno)); return ret; }

extern int default_uid;
extern int default_gid;
extern int verbose;
extern int preserve_uidgid;

int init_fs(ext2_filsys *fs, char *fsname, int size);
int e2cp(ext2_filsys fs, ext2_ino_t e2ino, const char *pathfile);
int e2symlink(ext2_filsys fs, ext2_ino_t e2dir, const char *pathlink);
int e2mkdir(ext2_filsys fs, ext2_ino_t e2dir, const char *path,
	   	ext2_ino_t *newdir);
int e2cpdir(ext2_filsys fs, ext2_ino_t parent, const char *dirpath);
int e2filetype_select(ext2_filsys fs, ext2_ino_t e2dir, const char *path);

/* functions from util.c */
const char *basename(const char *path);
void init_inode(struct ext2_inode *i, struct stat *s);

#endif
