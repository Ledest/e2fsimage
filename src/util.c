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
 * $Id: util.c,v 1.1 2004/01/17 22:13:59 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <string.h>

const char *basename(const char *path)
{
	const char *bn;
	/* extract the filename from the path */
	bn = strrchr(path,'/');
	if (!bn){
		bn = path;
	}
	else {
		bn++;
	}
	/* now ptr points to the basename of 'pathlink' */
	return bn;
}

void init_inode(struct ext2_inode *i, struct stat *s)
{
	/* do the root squash */
	s->st_uid=default_uid;
	s->st_gid=default_gid;

	memset(i, 0, sizeof(struct ext2_inode));

	i->i_links_count = 1;
	i->i_mode = s->st_mode;
	i->i_size = s->st_size;
	i->i_uid = s->st_uid;
	i->i_gid = s->st_gid;
	i->i_atime = s->st_atime;
	i->i_ctime = s->st_ctime;
	i->i_mtime = s->st_mtime;
									  
}
			

