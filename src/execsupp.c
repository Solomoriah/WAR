/*
	Solomoriah's WAR!

	execsupp.c -- support functions for execpriv.c and execuser.c

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

static char *args[21];


char **parseline(char *line)
{
	int i;
	char *s;

	i = 0;

	args[0] = NULL;

	while(*line && i < 20) {

		/* remove leading spaces. */

		for(; *line && isspace(*line); line++);

		/* is it quoted? */

		if(*line == '\'') {
			s = ++line;
			for(; *line && *line != '\''; line++);
			if(*line == '\'')
				*line++ = '\0';
		} else {
			s = line;
			for(; *line && !isspace(*line); line++);
			if(isspace(*line))
				*line++ = '\0';
		}

		args[i++] = s;
		args[i] = NULL;
	}

	return args;
}


int countargs(char **argv)
{
	int i;

	for(i = 0; argv[i] != NULL; i++);

	return i;
}


/* end of file. */
