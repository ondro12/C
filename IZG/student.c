/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: student.c 337 2014-02-25 06:52:49Z spanel $
 */

#include "student.h"
#include "transform.h"

#include <memory.h>
#include <math.h>

/******************************************************************************
 * Globalni promenne a konstanty
 */

/* rozmer textury */
const int       TEXTURE_SIZE    = 512;

/* pocet policek sachovnice */
const int       NUM_OF_TILES    = 16;

/* barva poli */
const S_RGBA    BLACK_TILE      = { 75, 75, 75 };
const S_RGBA    WHITE_TILE      = { 255, 255, 255 };


/*****************************************************************************
 * Funkce vytvori vas renderer a nainicializuje jej
 */

S_Renderer * studrenCreate()
{
	int i;
	int j;
	int hlpi;
	int hlpj;

    S_StudentRenderer * renderer = (S_StudentRenderer *)malloc(sizeof(S_StudentRenderer));
    IZG_CHECK(renderer, "Cannot allocate enough memory");
	renderer->layer = (S_RGBA *)malloc(TEXTURE_SIZE*TEXTURE_SIZE*sizeof(S_RGBA));


	/* inicializace default rendereru */
    renInit(&renderer->base);
	renderer->base.projectTriangleFunc = studrenProjectTriangle;
	renderer->base.releaseFunc = studrenRelease;

	for (j = 0; j < TEXTURE_SIZE; j++){
		hlpj = (j/(TEXTURE_SIZE/NUM_OF_TILES)) %2; 
		for (i = 0; i < TEXTURE_SIZE; i++){
			hlpi = (i/(TEXTURE_SIZE/NUM_OF_TILES))%2;
			if (hlpj == 0){
				if(hlpi == 0){
					renderer->layer[i*TEXTURE_SIZE+j] = WHITE_TILE;
				}
				else{
					renderer->layer[i*TEXTURE_SIZE+j] = BLACK_TILE;     //prechadzam vsetky pixeli obrazku pixel po pixeli
				}
			}
			else{
				if(hlpi == 0){
					renderer->layer[i*TEXTURE_SIZE+j] = BLACK_TILE;
				}
				else{
					renderer->layer[i*TEXTURE_SIZE+j] = WHITE_TILE;
				}
			}
		}
	}

    return (S_Renderer *)renderer;
}

/*****************************************************************************
 * Funkce korektne zrusi renderer a uvolni pamet
 */

void studrenRelease(S_Renderer **ppRenderer)
{
    S_StudentRenderer * renderer;

    if( ppRenderer && *ppRenderer ){
        renderer = (S_StudentRenderer *)(*ppRenderer);
		if (renderer->layer){//uvolnujem pamat 
			free(renderer->layer);
			renderer->layer = NULL;
		}
        renRelease(ppRenderer);
    }
}

/******************************************************************************
 * Nova fce pro rasterizaci trojuhelniku do frame bufferu
 * s podporou texturovani a interpolaci texturovacich souøadnic
 * Pozn.: neni nutné øešit perspektivní korekci textury
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * t1, t2, t3 - ukazatele na texturovaci souradnice vrcholu
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky
 */

void studrenDrawTriangle(S_Renderer *pRenderer,
                         S_Coords *v1, S_Coords *v2, S_Coords *v3,
                         S_Coords *n1, S_Coords *n2, S_Coords *n3,
                         S_Coords *t1, S_Coords *t2, S_Coords *t3,
                         int x1, int y1,
                         int x2, int y2,
                         int x3, int y3
                         )
{
	int         xy;
 	int         nx;
	int         ny;
	int         xx;
    int         d1;
	int         d2;
	int         d3;
	int         e1;
	int         e2;
	int         e3;
	int         f1;
	int         f2;
	int         f3;
    int         g1;
	int         g2;
	int         g3;
    int         x;
	int         y;
	int         h1;
	int         h2;
	int         h3;
    double      alpha;
	double      beta;
	double      i1;
	double      i2;
	double      i3;
	double      s;
	double      t;
	double      u;
	double		intensity;
    S_RGBA      paint1;
	S_RGBA      paint2;
	S_RGBA      paint3;
	S_RGBA      paint;
	S_RGBA		col_bord;

    IZG_ASSERT(pRenderer && v1 && v2 && v3 && n1 && n2 && n3);

    paint3 = pRenderer->calcReflectanceFunc(pRenderer, v3, n3);

	paint2 = pRenderer->calcReflectanceFunc(pRenderer, v2, n2); //spocitam farby vrcholov trojuholnika

	paint1 = pRenderer->calcReflectanceFunc(pRenderer, v1, n1);

    nx = MIN(x1, MIN(x2, x3));
	ny = MIN(y1, MIN(y2, y3));
    xx = MAX(x1, MAX(x2, x3));
    xy = MAX(y1, MAX(y2, y3));
    ny = MAX(ny, 0);
    xy = MIN(xy, pRenderer->frame_h - 1);
    nx = MAX(nx, 0);
    xx = MIN(xx, pRenderer->frame_w - 1);
	d3 = y3 - y1;
	d2 = y2 - y3;
    d1 = y1 - y2;
	e3 = x1 - x3;
	e2 = x3 - x2;
    e1 = x2 - x1;
	f3 = x3 * y1 - x1 * y3;
	f2 = x2 * y3 - x3 * y2;
    f1 = x1 * y2 - x2 * y1;
	g3 = d3 * x2 + e3 * y2 + f3;
	g2 = d2 * x1 + e2 * y1 + f2;
    g1 = d1 * x3 + e1 * y3 + f1;
	if( g3 < 0 ){
        d3 *= -1;
        e3 *= -1;
        f3 *= -1;
    }
	if( g2 < 0 ){
        d2 *= -1;
        e2 *= -1;
        f2 *= -1;
    }
    if( g1 < 0 ){
        d1 *= -1;
        e1 *= -1;
        f1 *= -1;
    }
    alpha = 1.0 / ABS(g2);
    beta = 1.0 / ABS(g3);
    for( y = ny; y <= xy; ++y ){			//vyplnanie
		h3 = d3 * nx + e3 * y + f3;
		h2 = d2 * nx + e2 * y + f2; 
        h1 = d1 * nx + e1 * y + f1;  
        for( x = nx; x <= xx; ++x ){
            if( h1 >= 0 && h2 >= 0 && h3 >= 0 )
            {
                i1 = alpha * h2;
                i2 = beta * h3;
                i3 = 1.0 - i1 - i2;
				u = i1 * t1->y + i2 * t2->y + i3 * t3->y;
				t = i1 * t1->x + i2 * t2->x + i3 * t3->x;
                s = i1 * v1->z + i2 * v2->z + i3 * v3->z;               
                paint.red = ROUND2BYTE(i1 * paint1.red + i2 * paint2.red + i3 * paint3.red);
                paint.green = ROUND2BYTE(i1 * paint1.green + i2 * paint2.green + i3 * paint3.green);    //farba
                paint.blue = ROUND2BYTE(i1 * paint1.blue + i2 * paint2.blue + i3 * paint3.blue);
                paint.alpha = 255;
				col_bord = studrenTextureValue((S_StudentRenderer*)pRenderer, t, u);                   
				intensity = col_bord.red / 255.0;
				paint.red = (unsigned char) (paint.red * intensity);
				paint.blue = (unsigned char) (paint.blue * intensity);
				paint.green = (unsigned char) (paint.green * intensity);   
				if( s < DEPTH(pRenderer, x, y) ){
                    PIXEL(pRenderer, x, y) = paint;
                    DEPTH(pRenderer, x, y) = s;
                }
            }
			h3 += d3;
			h2 += d2;
            h1 += d1;   
        }
    }
}

void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i)
{
    S_Coords  * top1;
	S_Coords  * top2;
	S_Coords  * top3;
	S_Coords    xx;           
	S_Coords    yy;
	S_Coords    zz;
    S_Coords    nxx;         
	S_Coords    nyy;
	S_Coords    nzz;
    S_Coords    nm;  
    int         x1;  
	int         x2;
	int         x3;
	int         y1;
	int         y2;
	int         y3;
    S_Triangle  * trojuh;

    IZG_ASSERT(pRenderer && pModel && i >= 0 && i < trivecSize(pModel->triangles));
    trojuh = trivecGetPtr(pModel->triangles, i);
    trTransformVertex(&xx, cvecGetPtr(pModel->vertices, trojuh->v[0]));
    trTransformVertex(&yy, cvecGetPtr(pModel->vertices, trojuh->v[1]));
    trTransformVertex(&zz, cvecGetPtr(pModel->vertices, trojuh->v[2]));
    trProjectVertex(&x1, &y1, &xx);
    trProjectVertex(&x2, &y2, &yy);
    trProjectVertex(&x3, &y3, &zz);
    trTransformVector(&nxx, cvecGetPtr(pModel->normals, trojuh->v[0]));
    trTransformVector(&nyy, cvecGetPtr(pModel->normals, trojuh->v[1]));
    trTransformVector(&nzz, cvecGetPtr(pModel->normals, trojuh->v[2]));
    coordsNormalize(&nxx);
    coordsNormalize(&nyy);
    coordsNormalize(&nzz);
    trTransformVector(&nm, cvecGetPtr(pModel->trinormals, trojuh->n));
    coordsNormalize(&nm);
    if( !renCalcVisibility(pRenderer, &xx, &nm) ){   //test ci je trojuholnik viditelny ku kamere
        return;
    }
	top1 = cvecGetPtr(pModel->texcoords, trojuh->v[0]);
	top2 = cvecGetPtr(pModel->texcoords, trojuh->v[1]);
	top3 = cvecGetPtr(pModel->texcoords, trojuh->v[2]);
    studrenDrawTriangle(pRenderer,&xx, &yy, &zz, &nxx, &nyy, &nzz,top1, top2, top3, x1, y1, x2, y2, x3, y3);
}

/******************************************************************************
 * Vrací hodnotu v aktuálnì nastavené textuøe na zadaných
 * texturovacích souøadnicích u, v
 * Pro urèení hodnoty používá bilineární interpolaci
 * u, v - texturovací souøadnice v intervalu 0..1, který odpovídá šíøce/výšce textury
 */

S_RGBA studrenTextureValue(S_StudentRenderer * pRenderer, double u, double v)
{
	int s1;
	int s2;
	int t1;
	int t2; 
	double o1;
	double o2;
	double o3;
	double p1;
	double r1;
	p1 = u * 512;
	s1 = (int)(floor(p1));
	s2 = (int)(ceil(p1));
	r1 = v * 512;
	t1 = (int)(floor(r1));
	t2 = (int)(ceil(r1));
	o1= (s2 - p1) * (pRenderer->layer[s1*TEXTURE_SIZE+t2].red) + (p1 - s1) * (pRenderer->layer[s2*TEXTURE_SIZE+t2].red);
	o2= (s2 - p1) * (pRenderer->layer[s1*TEXTURE_SIZE+t1].red) + (p1 - s1) * (pRenderer->layer[s2*TEXTURE_SIZE+t1].red);
	o3= (t1 - r1) * (o1) + (r1 - t2) * (o2);
    return makeColor(o3, o3, o3);
}


/******************************************************************************
 ******************************************************************************
 * Callback funkce volana pri startu aplikace
 * Doplnte automaticke vygenerovani a prirazeni texturovacich souradnic
 * vrcholum modelu s vyuzitim mapovani na kouli
 * Muzete predpokladat, ze model je umisten v pocatku souradneho systemu
 * a posunuti neni treba resit
 */

void onInit(S_Renderer *pRenderer, S_Model *pModel)
{
   	S_Coords hlp;
	int j=0;
	double t;
	double s;
	while(j < cvecSize(pModel->vertices)){
           hlp = cvecGet(pModel->vertices, j);
           coordsNormalize(&hlp);
           t = 0.5 + atan2(hlp.z , hlp.x) / (2*PI);
           s = 0.5 - asin(hlp.y) / PI;
           cvecSet(pModel->texcoords, j, makeCoords(t, s, 0.0));
		   j++;
		}
}

/*****************************************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************
 ********************************KONIEC PROGRAMU******************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************































































































































































































 */
