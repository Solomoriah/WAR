/*
	Solomoriah's WAR!

	display.h -- data manager module includes

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


struct mv_stack {
    int id;
    int moved;
    int dep;
};

void mainloop(int ntn);
void setfocus(int ntn, int r, int c);
void showmap(int r, int c, int force);
void showfocus(int r, int c, int mode);
void showcity(int r, int c);
void showarmies();
void sortview();
void analyze_stack(struct mv_stack *ms, int mc);
void move_mode(int ntn, int *rp, int *cp);
void mainscreen();
void titlescreen();
void produce(int city);
void info_mode(int *rp, int *cp, int n, int ch);


/* end of file. */
