/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 2001 Christian Hohnstaedt, 2018 Felix Krull
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
 * $Id: copy.c,v 1.12 2005/05/25 18:06:51 chris2511 Exp $
 *
 */

#include "e2fsimage.h"
#include <sys/xattr.h>

/* error handling rule: in case of an error, the process is going to exit
 * anyway. Bottom line: we don't care about cleaning up memory properly. */

#define XATTR_ERR(ret) \
	if (ret < 0) { \
	    if (errno == ENOTSUP) return 0; \
	    fprintf(stderr, "%s(%d): xattr Error: %s\n", __FILE__, __LINE__, strerror(errno)); \
		return ret; \
	}

static int copy_xattr(const char *path, const char *attr, struct ext2_xattr_handle *xattr_handle)
{
    errcode_t ret;

    if (verbose)
        printf("  setting xattr %s\n", attr);

    ssize_t value_ret = lgetxattr(path, attr, NULL, 0);
    if (value_ret == 0) return 0;
    XATTR_ERR(value_ret);

    char *value = malloc((size_t) value_ret);
    ret = lgetxattr(path, attr, value, (size_t) value_ret);
    XATTR_ERR(ret);

    ret = ext2fs_xattr_set(xattr_handle, attr, value, (size_t) value_ret);
    E2_ERR(ret, "Ext2 xattr Error", "");

    free(value);

    return 0;
}

int copy_xattrs(e2i_ctx_t *e2c, ext2_ino_t e2ino)
{
    errcode_t ret;

    /* get xattr keys */
    ssize_t attrs_ret = llistxattr(e2c->curr_path, NULL, 0);
    if (attrs_ret == 0) return 0;
    XATTR_ERR(attrs_ret);

    char *attrs = malloc((size_t) attrs_ret);
    ret = llistxattr(e2c->curr_path, attrs, (size_t) attrs_ret);
    XATTR_ERR(ret);

    /* open xattr handle */
    struct ext2_xattr_handle *xattr_handle;
    ret = ext2fs_xattrs_open(e2c->fs, e2ino, &xattr_handle);
    E2_ERR(ret, "Ext2 xattr Error", "");

    for (const char *attr = attrs; attr < attrs + attrs_ret; attr += strlen(attr) + 1) {
        ret = copy_xattr(e2c->curr_path, attr, xattr_handle);
        if (ret) return -1;
    }

    /* write and close xattrs */
    ret = ext2fs_xattrs_write(xattr_handle);
    E2_ERR(ret, "Ext2 xattr Error", "");

    ret = ext2fs_xattrs_close(&xattr_handle);
    E2_ERR(ret, "Ext2 xattr Error", "");

    free(attrs);

    return 0;
}
