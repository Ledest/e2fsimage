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
 * $Id: mkdir.c,v 1.1 2004/01/17 22:13:59 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <errno.h>
#include <string.h>

int e2mkdir(ext2_filsys fs, ext2_ino_t e2dir, const char *path,
		 ext2_ino_t *newdir) {

	int ret;
	struct stat s;
	const char *dname;
	
	ret = lstat(path, &s);
	ERRNO_ERR(ret,"Could not 'stat': ", path);
	
	if (!S_ISDIR(s.st_mode)) {
		fprintf(stderr, "File '%s' is not a directory\n", path);
		return -1;
	}
		  
	dname = basename(path);
	
	ret = ext2fs_mkdir(fs, e2dir, 0, dname);
	E2_ERR(ret, "Could not 'mkdir': ", dname);

	if (verbose)
		printf ("Creating directory %s\n", dname);

	if (newdir) {
		ret = ext2fs_lookup(fs, e2dir, dname, strlen(dname), 0, newdir);
		E2_ERR(ret, "Could not 'lookup':", dname);
	}		
	return 0;
}
