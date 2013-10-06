/*
	Solomoriah's WAR!

	genio.h -- generic screen i/o

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

struct genio_driver {
	char *term;
	int (*init)(),   (*end)(),     (*move)(),   
        (*erase)(),  (*clrline)(), (*addch)(), 
        (*hilite)(), (*refresh)(), (*getch)(),
        (*puts)(),   (*mapspot)(int r, int c, char terr, char mark, int focus);
};

int ginit(void);
int gend(void); 
int gputs(char *s);
int gputch(int ch);
int gclrline(void);
int ggetch(void);
int gmove(int r, int c);
int gerase(int mode);
int gprintf(char *fmt, ...);
int gmapspot(int r, int c, char terr, char mark, int focus);

#define GKEY_LEFT     (256+1)
#define GKEY_RIGHT    (256+2)
#define GKEY_UP       (256+3)
#define GKEY_DOWN     (256+4)
#define GKEY_PAGEUP   (256+5)
#define GKEY_PAGEDOWN (256+6)
#define GKEY_HOME     (256+7)
#define GKEY_END      (256+8)

/* end of file. */
