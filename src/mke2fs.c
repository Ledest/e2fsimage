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
 * $Id: mke2fs.c,v 1.2 2004/02/21 16:57:09 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <string.h>

int mke2fs(const char *fname, int size)
{
	int pid, i, status, fd;
	FILE *fp;
	char *buf, *bp ;
	
	/* open the target filesystem image */
	fp = fopen(fname, "wb+");
	if (!fp) {
		perror("Error opening file");
		return 1;
	}

	/* fill the image with zeros */
	buf = malloc(1024);
	if (!buf) {
		fclose(fp);
		return -1;
	}
	memset(buf, 0, 1024 );
	for (i=0; i<size; i++) {
		fwrite(buf, 1024, 1, fp);
	}
	fclose(fp);

	/* redirect stdout of mke2fs to dev/null */
	fd = open("/dev/null", O_WRONLY);
	
	/* add /sbin /usr/sbin and /usr/local/sbin to the PATH */
	bp = getenv("PATH");
	strncpy(buf, bp, 1000);
	strcat(buf, ":/sbin:/usr/sbin:/usr/local/sbin" ); 
	
	pid = fork();
	if (!pid) {
		if (fd) dup2(fd, 1);
		setenv("PATH", buf, 1);	
		
		execlp("mkfs.ext2", "mkfs.ext2", "-F", fname, NULL);
		execlp("mke2fs", "mke2fs", "-F", fname, NULL);
		fprintf(stderr,"Hoppla !!\n");
	}
	waitpid(pid, &status, 0);
	if (fd) close(fd);
	free(buf);
	
	if (WEXITSTATUS(status) !=0 ) {
		fprintf(stderr, "mke2fs failed with return code %d\n", WEXITSTATUS(status));
		return -1;
	}
	return 0;
}
		
