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
 * $Id: dirent.c,v 1.20 2005/05/25 18:06:51 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fnmatch.h>

static char cpath[256] = "";

int filter(const struct dirent *d)
{
	const char *n = d->d_name;

	/* skip . and .. */
	if (n[0] == '.' &&
	    (n[1] == '\0' ||
	     (n[1] == '.' && n[2] == '\0')))
		return 0;
	/* skip excluded */
	else {
		int i;
		int ret = 1;
		size_t l = strlen(cpath);

		if (cpath[0]) {
			cpath[l] = '/';
			strcpy(cpath + l + 1, n);
		} else
			strcpy(cpath, n);
		for (i = 0; i < excluded_num; i++)
			if (!fnmatch(excluded[i], cpath, 0)) {
				if (verbose)
					printf("Excluded %s\n", cpath);
				ret = 0;
			}
		cpath[l] = '\0';
		return ret;
	}
}

/*
 * Scan a directory and copy all the files to the e2c->curr_e2dir
 * using scandir()
 */
int e2cpdir(e2i_ctx_t *e2c_old, ext2_ino_t newdir)
{
	struct dirent **namelist;
	int i,ret, len, count;
	char path[256], *ppath;
	uiddb_t uiddb;
	e2i_ctx_t e2c;
	
	memcpy(&e2c, e2c_old, sizeof(e2i_ctx_t));
	e2c.curr_e2dir = newdir;

	strcpy(cpath, e2c.curr_path + strlen(e2c.root_path) + !!strcmp(e2c.curr_path, e2c.root_path));

	ret = scandir(e2c.curr_path, &namelist, filter, NULL);
	if (ret < 0) {
		perror("scandir");
		return -1;
	}

	/* prepare the new pathname in path[] */
	len = strlen(e2c.curr_path);
	strncpy(path, e2c.curr_path, 256);
	if (path[len-1] != '/') {
		path[len++] = '/';
	}
	e2c.curr_path = path;
	ppath = path + len;
	count = ret;
	ret = 0;

	/* setup the uid database */
	uiddb_init(&uiddb);
	if (read_uids(&e2c, &uiddb) < 0) {
		uiddb_free(&uiddb);	
		return -1;
	}
	e2c.uid_db = &uiddb;

	/* check for . entry in .UIDGID */
	if (uiddb_search(&uiddb, ".", &e2c.default_uid, &e2c.default_gid)){
		e2c.preserve_uidgid = 0;
	}
	modinode(&e2c, ".", newdir);
	
	
	/* iterate over all directory items */
	for (i = 0; i<count; i++) {
		strncpy(ppath, namelist[i]->d_name, 256 - len);
		free(namelist[i]);
		/* skip . and .. */
		if (!strncmp(e2c.uid_file, ppath, strlen(e2c.uid_file))) continue;
		/* is there a special file (.DEVICES) */
		if (!strncmp(e2c.dev_file, ppath, strlen(e2c.dev_file))) {
			ret = read_special_file(&e2c);
			if (ret) break;
			continue;
		}
		/* select the action depending on the filetype */
		ret = e2filetype_select(&e2c);
		if (ret) break;
	}
	
	/* 
	 * in case of an error iterate over the remaining 
	 * entries and free them
	 */
	for (i++; i<count; i++) {
		free(namelist[i]);
	}

	/* free the user database */
	uiddb_free(e2c.uid_db);
	
	free(namelist);
    return ret;
}

static int e2check_hardlink(e2i_ctx_t *e2c, ino_t ino)
{
	struct ext2_inode inode;
	ext2_ino_t e2ino;
	int ret, e2mod;
	
	e2ino = inodb_search(e2c->ino_db, ino);
	if (e2ino == 0) return 1;

	/* read the inode, increase the link counter and write it back */
	ret = ext2fs_read_inode(e2c->fs, e2ino, &inode);
	E2_ERR(ret, "Ext2 read Inode Error", "");
		
	inode.i_links_count++;
	
	ret = ext2fs_write_inode(e2c->fs, e2ino, &inode);
	E2_ERR(ret, "Ext2 write Inode Error", "");

	/* be verbose and do statistics */
	if (verbose)
		printf("Creating hard link %s\n", e2c->curr_path);
	
	e2c->cnt->hardln++;
		
	/* resolve the filetype from i_mode */
	e2mod = mode2filetype(inode.i_mode);
	
	/* It is time to link the inode into the directory */
	return e2link(e2c, basename(e2c->curr_path), e2ino, e2mod);
}
	   
int e2filetype_select(e2i_ctx_t *e2c)
{
	struct stat s;  
	ext2_ino_t newe2dir;
	int ret;

	lstat(e2c->curr_path, &s);

	if ((S_ISREG(s.st_mode) || S_ISDIR(s.st_mode)) &&
	    access(e2c->curr_path, S_ISDIR(s.st_mode) ? R_OK|X_OK : R_OK)) {
		fprintf(stderr, "access: %s: '%s'\n", strerror(errno), e2c->curr_path);
		return e2c->unaccessible;
	}

	ret = e2check_hardlink(e2c, s.st_ino);
	if (ret <= 0) return ret;
	ret = -1;
	
	/* OK, its not a hard link */
	switch (s.st_mode & S_IFMT) {
		case S_IFDIR :
			/* create the new directory inode */
			ret = e2mkdir(e2c, &newe2dir);
			if (ret) break;
			/* This is the recursive call for every sub directory */
			ret = e2cpdir(e2c, newe2dir);
			break;
		case S_IFREG : ret = e2cp(e2c); break;
		case S_IFLNK : ret = e2symlink(e2c); break;
		case S_IFCHR :
		case S_IFIFO :
		case S_IFSOCK:
		case S_IFBLK : ret = e2mknod(e2c); break;
		
		default : fprintf(stderr, "Filetype of \"%s\" not supported: %x\n", e2c->curr_path,s.st_mode & S_IFMT);
	}
	return ret;
}	

