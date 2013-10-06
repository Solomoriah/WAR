/*
    Solomoriah's WAR!

    reader.c -- news/mail reader

    Copyright 1993, 1994, 2001, 2013 Chris Gonnerman
    All rights reserved.

    3 Clause BSD License

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    Redistributions of source code must retain the above copyright
    notice, self list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright
    notice, self list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    Neither the name of the author nor the names of any contributors
    may be used to endorse or promote products derived from self software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "genio.h"
#include "reader.h"
#include "dispsupp.h"

static long r_index[MAXINDEX];
static char killed[MAXINDEX];
static int index_cnt = 0;

static long pages[MAXINDEX];
static int page_cnt = 0;

/*
    reader() displays a list of headings from the given file.  the
    user may page or "arrow" up or down to view long listings.  when
    a heading is selected with the (v)iew command, viewer() is called.

    mode is 0 for news, 1 for mail.  mode 1 enables deletion of headings.
*/

void reader(char *fname, int mode)
{
    int top, pos, ch, killcnt;
    FILE *fp;

    fp = fopen(fname, "r");

    if(fp == NULL) {
        if(mode)
            saystat("No Mail in your box.");
        else
            saystat("No News to Read.");
        return;
    }

    readerscr(mode);

    indexer(fp);

    top = pos = index_cnt - 1;

    show_screen(top, fp);

    do {
        gmove(pos - top + 8, 1);
        gputch('>');

        ch = ggetch();

        gmove(pos - top + 8, 1);
        gputch(' ');

        switch(ch) {

        case 'j' :
        case GKEY_DOWN:
            pos++;
            break;

        case 'k' :
        case GKEY_UP:
            pos--;
            break;

        case 'n' :
        case GKEY_PAGEDOWN:
            pos += 11;
            break;

        case 'p' :
        case GKEY_PAGEUP:
            pos -= 10;
            break;

        case 'd' :
            if(!mode)
                break;

            killed[pos]++;
            killed[pos] %= 2;

            show_screen(top, fp);

            break;

        case 'v' :
            viewer(fp, pos, mode);
            show_screen(top, fp);
            break;

        case ' ' :
        case 'q' :
            ch = '\033';
            break;

        }

        if(pos < 0)      pos = 0;
        if(pos >= index_cnt) pos = index_cnt - 1;

        if(pos > top + 8 || pos < top - 7) {
            top = pos;
            show_screen(top, fp);
        }

    } while(ch != '\033');

    fclose(fp);

    gerase(0);
    mainscreen();

    /* handle deletion. */

    if(mode) {
        for(killcnt = 0, pos = 0; pos < index_cnt; pos++)
            killcnt += killed[pos];

        if(killcnt != 0) { /* deleted some... */

            if(killcnt == index_cnt) /* deleted ALL */
                unlink(fname);
            else
                delete_msgs(fname);
        }
    }
}


/*
    viewer() displays text from the given file, from the given
    index offset to the end of file or until a line beginning with
    a HEADERMARK is encountered.
*/

void viewer(FILE *fp, int pos, int mode)
{
    int ch, i, len, done, pg, opg;
    char dummy[128];

    len = strlen(HEADERMARK);

    fseek(fp, r_index[pos], 0);

    viewerscr(mode);
    show_killed(pos);

    done = 0;

    page_cnt = 0;

    do {
        pages[page_cnt++] = ftell(fp);

        for(i = 0; i < 15; i++) {
            if(!rgetline(dummy, fp)) {
                done = 1;
                break;
            }
            if(strncmp(dummy, HEADERMARK, len) == 0) {
                done = 1;
                break;
            }
        }

    } while(!done);

    pg = 0;

    showpage(fp, pg);

    do {
        gmove(21, 71);
        ch = ggetch();

        opg = pg;

        switch(ch) {

        case 'j' :
        case 'n' :
        case GKEY_DOWN:
            pg++;
            break;

        case 'k' :
        case 'p' :
        case GKEY_UP:
            pg--;
            break;

        case 'd' :
            killed[pos] = killed[pos] ? 0 : 1;
            show_killed(pos);
            break;

        case ' ' :
        case 'q' :
            ch = '\033';
            break;

        }

        if(pg < 0)         pg = 0;
        if(pg >= page_cnt) pg = page_cnt - 1;

        if(pg != opg)
            showpage(fp, pg);

    } while(ch != '\033');

    readerscr(mode);
}


void show_killed(int pos)
{
    gmove(21, 40);

    if(killed[pos])
        gputs("[Deleted]");
    else
        gputs("         ");
}


/*
    indexer() reads the given file, scanning for headings.  headings
    are flagged with a space-backspace combination (" \b") which is
    not shown on most printouts but is easily recognized.

    the array r_index[] contains the seek positions of each heading.
    it is limited by index_cnt.
*/

void indexer(FILE *fp)
{
    char inbuf[80], len;
    long pos;

    index_cnt = 0;

    len = strlen(HEADERMARK);

    rewind(fp);

    for(pos = ftell(fp); rgetline(inbuf, fp); pos = ftell(fp))
        if(strncmp(inbuf, HEADERMARK, len) == 0) {
            killed[index_cnt] = 0;
            r_index[index_cnt++] = pos + len;
        }
}


/*
    rgetline() reads in a maximum of 77 characters, but always reads to
    end-of-line.  '\r' and '\n' characters are not included in the
    string read.

    returns 1 if a line was read, or 0 if at EOF.
*/

int rgetline(char *s, FILE *fp)
{
    int ch, i;

    i = 0;

    while(i < 77 && (ch = getc(fp)) != EOF && ch != '\n')
        if(ch != '\r')
            s[i++] = ch;

    s[i] = '\0';

    if(feof(fp) && i == 0)
        return 0;

    while(!feof(fp) && ch != '\n')
        ch = getc(fp);

    return 1;
}


/*
    show_screen() shows headers from the given file, centering at the
    header number given.
*/

void show_screen(int pos, FILE *fp)
{
    int i;
    char inbuf[80];

    pos -= 8;

    for(i = 0; i < 18; i++) {
        gmove(i, 2);

        if(pos + i < index_cnt && pos + i >= 0) {
            if(killed[pos+i])
                gputch('*');
            else
                gputch(' ');
            fseek(fp, r_index[pos+i], 0);
            rgetline(inbuf, fp);
            gputs(inbuf);
        } else if(pos + i == -1)
            gputs("*** Top of List ***");
        else if(pos + i == index_cnt)
            gputs("*** End of List ***");

        gclrline();
    }
}


/*
    showpage() shows a page of text from the current file, starting at
    the file position in pages[pg], for 19 lines.
*/

void showpage(FILE *fp, int pg)
{
    int i, len;
    char inbuf[128];

    fseek(fp, pages[pg], 0);

    len = strlen(HEADERMARK);

    for(i = 0; i < 19; i++) {
        if(!rgetline(inbuf, fp))
            break;

        if(strncmp(inbuf, HEADERMARK, len) == 0)
            break;

        gmove(i, 2);
        gputs(inbuf);
        gclrline();
    }

    for(; i < 19; i++) {
        gmove(i, 2);
        gclrline();
    }

    gmove(21, 55);
    gprintf("Page %d of %d", pg + 1, page_cnt);
    gclrline();

    gmove(21, 70);
    gputs("< >");
}


/*
    delete_msgs() deletes messages from the named file, using the
    information in the r_index[] and killed[] arrays.
*/

void delete_msgs(char *fn)
{
    int i, done, len;
    char tmp[16], inbuf[128];
    FILE *in, *out;

    len = strlen(HEADERMARK);

    strcpy(tmp, "tmp");
    strcat(tmp, fn);

    in = fopen(fn, "r");
    out = fopen(tmp, "w");

    if(in == NULL || out == NULL) {
        if(in != NULL)  fclose(in);
        if(out != NULL) fclose(out);
        saystat("Message Deletion Failed (System Error)");
        return;
    }

    for(i = 0; i < index_cnt; i++)
        if(!killed[i]) {
            fputs(HEADERMARK, out);

            fseek(in, r_index[i], 0);

            done = 0;

            while(rgetline(inbuf, in) && !done)
                if(strncmp(inbuf, HEADERMARK, len) == 0)
                    done = 1;
                else {
                    fputs(inbuf, out);
                    putc('\n', out);
                }
        }

    fclose(in);
    fclose(out);

    unlink(fn);
    rename(tmp, fn);
}


void readerscr(int mode)
{
    gerase(0);

    gmove(20, 2);

    if(mode)
        gputs("Mail:  (v)iew current  (d)elete  ");
    else
        gputs("News:  (v)iew current  ");

    gputs("(j)down  (k)up  (n)ext  (p)revious");

    gmove(21, 2);
    gputs("Press SPACE to Exit.");
}


void viewerscr(int mode)
{
    gerase(0);

    gmove(20, 2);

    if(mode)
        gputs("Mail:  (d)elete  ");
    else
        gputs("News:  ");

    gputs("(j)down  (k)up");

    gmove(21, 2);
    gputs("Press SPACE to Exit.");

    gmove(21, 70);
    gputs("< >");
}


/* end of file. */
