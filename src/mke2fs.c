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
 * $Id: mke2fs.c,v 1.6 2006/01/11 22:08:58 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <string.h>

int mke2fs(const char *fname, const char *fstype, const char *label, int bsize, int size)
{
	int pid, status, fd;
	char buf[4096];
	/* open the target filesystem image */
	FILE *fp = fopen(fname, "wb+");

	if (!fp) {
		perror("Error opening file");
		return 1;
	}

	memset(buf, 0, sizeof(buf));

	fseek(fp, ((size + 3) / 4 - 1) * 4096, SEEK_SET);
	fwrite(buf, sizeof(buf), 1, fp);
	fclose(fp);

	/* redirect stdout of mke2fs to dev/null */
	fd = open("/dev/null", O_WRONLY);

	pid = fork();
	if (!pid) {
		const char newpath[] = ":/sbin:/usr/sbin:/usr/local/sbin";
		char const *argv[11] = {"mke2fs", "-q", "-F"};
		int i = 3;
		/* add /sbin, /usr/sbin and /usr/local/sbin to the PATH */
		char *bp = getenv("PATH");

		strncpy(buf, bp, sizeof(buf) - sizeof(newpath));
		strcat(buf, newpath);

		if (label) {
			argv[i++] = "-L";
			argv[i++] = label;
		}
		if (fstype) {
			argv[i++] = "-t";
			argv[i++] = fstype;
		}
		if (bsize) {
			char b[16];
			if (sprintf(b, "%d", bsize) > 0) {
				argv[i++] = "-b";
				argv[i++] = b;
			}
		}
		argv[i] = fname;
		argv[i + 1] = NULL;

		if (fd) dup2(fd, 1);
		setenv("PATH", buf, 1);

		execvp("mke2fs", (char* const*)argv);
		fputs("Could not execute 'mke2fs'\n", stderr);
	}
	waitpid(pid, &status, 0);
	if (fd) close(fd);

	if (WEXITSTATUS(status) !=0 ) {
		fprintf(stderr, "mke2fs failed with return code %d\n", WEXITSTATUS(status));
		return -1;
	}
	return 0;
}
		
