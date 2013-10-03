/*
	mapper
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

char map[160][160];
char mapb[160][160];
int maprows, mapcols;

char *terrain = "~.%#^";
int count[5];

int fixcol(int col);


main()
{
	int gencount, xforms;

	maprows = 64;
	mapcols = 64;

    srand48(time(NULL));

	genmap();

	gencount = 1;

	while((xforms = generation()) > (maprows*mapcols/30)
	&& gencount < 11)
		fprintf(stderr,
			"Generation %4d -- %5d Transforms\n", gencount++, xforms);

	fprintf(stderr,
		"Generation %4d -- %5d Transforms\n", gencount++, xforms);

	savemap();
}


savemap()
{
	int i, j, k, l;
	FILE *fp;


	fp = fopen("map.out", "w");

	fprintf(fp, "M %d %d\n", maprows, mapcols);

    if(fp != NULL) {
        for(i = 0; i < maprows; i++) {
            for(j = 0; j < maprows; j++)
                putc(map[i][j], fp);
            putc('\n', fp);
        }
    }

/*
	fprintf(fp, "M %d %d\n", maprows / 16, mapcols / 16);

	if(fp != NULL) {
		for(i = 0; i < maprows / 16; i++)
			for(j = 0; j < mapcols / 16; j++) {
				putc('\n', fp);
				for(k = 0; k < 16; k++) {
					for(l = 0; l < 16; l++)
						putc(map[i*16+k][j*16+l], fp);
					if(k == 0)
						fprintf(fp, "  R%d C%d", i, j);
					putc('\n', fp);
				}
			}
	}
*/

	fclose(fp);

	for(i = 0; i < maprows; i++) {
		for(j = 0; j < mapcols; j++)
			putc(map[i][j] == '~' ? ' ' : map[i][j], stdout);
		putc('\n', stdout);
	}

}


int lo3r(int max)
{
	int res1, res2, res3;

	return lrand48() % max;

	res1 = lrand48() % max;
	res2 = lrand48() % max;
	res3 = lrand48() % max;

	if(res1 < res2 && res1 < res3)
		return res1;

	if(res2 < res3)
		return res2;

	return res3;
}


genmap()
{
	int i, j;

	for(i = 0; i < maprows; i++)
		for(j = 0; j < mapcols; j++)
			map[i][j] = terrain[lo3r(5)];
/*
	for(i = 0; i < 4; i++)
		for(j = 0; j < mapcols; j++) {
			map[i][j] = '~';
			map[maprows-i-1][j] = '~';
		}

	for(j = 0; j < 4; j++)
		for(i = 0; i < maprows; i++) {
			map[i][j] = '~';
			map[i][mapcols-j-1] = '~';
		}
*/
}


int generation()
{
	int i, j, trans;
	char old;

	trans = 0;

	for(i = 0; i < maprows; i++)
		for(j = 0; j < mapcols; j++)
			switch(map[i][j]) {

			case '~' :
/*
				old = map[i][j];
				if(lo3r(15) == 0)
					map[i][j] = terrain[lo3r(5)];
				if(old != map[i][j])
					trans++;
*/
				break;

			default :
				old = map[i][j];
				mapb[i][j] = terrain[docount(i, j, 1)];
				if(old != mapb[i][j])
					trans++;
				break;
			}


	for(i = 0; i < maprows; i++)
		for(j = 0; j < mapcols; j++)
			map[i][j] = mapb[i][j];

	return trans;
}


int docount(int r, int c, int mode)
{
	int i, j, k, pos, diff;

	pos = 0;

	for(i = 0; i < 5; i++) {
		count[i] = 0;
		if(map[r][c] == terrain[i])
			pos = i;
	}

	for(i = -1; i < 2; i++)
		for(j = -1; j < 2; j++)
				for(k = 0; k < 5; k++)
					if(map[fixrow(r+i)][fixcol(c+j)] == terrain[k]) {
						diff = 1;
						if(r == i && c == j)
							diff = 2;
						if(terrain[k] != '~' || mode)
							count[k] += diff;
					}

/*
	if(count[pos] >= pos)
		return pos;
*/
	i = 0;
	j = -1;

	for(k = 1; k < 5; k++) {
		if(count[k] > count[i])
			i = k;
		if(count[k] > 5)
			j = k;
	}
/*
	if(!mode && j == -1)
		return pos;
*/

	return i;
}


int fixcol(int col)
{
	return (col + mapcols) % mapcols;
}


int fixrow(int row)
{
	return (row + maprows) % maprows;
}


/* end of file. */
