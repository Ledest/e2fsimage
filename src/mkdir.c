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
 * $Id: mkdir.c,v 1.4 2004/01/28 20:17:28 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <errno.h>
#include <string.h>

int e2mkdir(e2i_ctx_t *e2c, ext2_ino_t *newdir) {

	int ret;
	struct stat s;
	const char *dname;
	
	ret = lstat(e2c->curr_path, &s);
	ERRNO_ERR(ret,"Could not 'stat': ", e2c->curr_path);
	
	if (!S_ISDIR(s.st_mode)) {
		fprintf(stderr, "File '%s' is not a directory\n", e2c->curr_path);
		return -1;
	}
		  
	dname = basename(e2c->curr_path);
	
	ret = ext2fs_mkdir(e2c->fs, e2c->curr_e2dir, 0, dname);
	E2_ERR(ret, "Could not create dir: ", dname);

	if (e2c->verbose)
		printf ("Creating directory %s\n", dname);

	e2c->cnt.dir++;
	
	if (newdir) {
		ret = ext2fs_lookup(e2c->fs, e2c->curr_e2dir, dname, strlen(dname), 0, newdir);
		E2_ERR(ret, "Could not Ext2-lookup: ", dname);
	}		
	return 0;
}
