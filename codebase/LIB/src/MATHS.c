/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: MATHS.c,v $
 *
 *	Version	: $Revision: 1.3 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2008/02/26 17:20:51 $
 *
 *	==========================================================================================
 *
 *	Module definissant quelques outils de calcul
 */

#define	MODULE_NAME	"MATHS"

#include "CISLIB.h"

#define	MAX_ITER_JACOBI		50

const	float	FILL_VALUE = -1.0E+31;
const	float	EPSILON    = 1.0E-9;

/***************************************************************************************************
 *
 *	Indique si la valeur est une valeur filaire
 *	-------------------------------------------
 */
int	Fill_value (double value)
{
	return (1.0 - value / FILL_VALUE) < EPSILON;
}


/***************************************************************************************************
 *
 *	Produit d'un vecteur ou d'une matrice par une constante
 *	-------------------------------------------------------
 */
void 	Constant_product (double * var, int size, double constant)
{
	int	i;

	for (i = 0; i < size; i++) var [i] *= constant;
}


/***************************************************************************************************
 *
 *	Calcule la norme d'un vecteur
 *	-----------------------------
 */
double	Norm (V3 v)
{
	return sqrt (v [0] * v [0] + v [1] * v [1] + v [2] * v [2]);
}


/***************************************************************************************************
 *
 *	Affiche contenu matrice
 *	-----------------------
 */
t_err	Vector_display (int level, char * fonction, char * text, V3 vector)
{
	t_err		error = OK;

	Affiche_trace (level, fonction, "%s = % 1.4e % 1.4e % 1.4e",
		text,
		vector [0],
		vector [1],
		vector [2]);

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Affiche contenu matrice
 *	-----------------------
 */
t_err	Matrix_display (int level, char * fonction, char * text, M33 mat)
{
	t_err	error = OK;
	int	l;

	Affiche_trace (level, fonction, "%s", text);

	for (l = 0; l < 3; l++) {

		Affiche_trace (level, fonction, "% 1.4e % 1.4e % 1.4e", 
			mat [l][0],
			mat [l][1],
			mat [l][2]);
	}

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Produit de 2 matrices : dst =  m1 x m2 
 *	--------------------------------------
 */
t_err	Matrix_product (M33 m1, M33 m2, M33 dst)
{
	t_err		error = OK;
	int		l, c, k;

	for (l = 0; l < 3; l++) {

		for (c = 0; c < 3; c++) {

			double	sum = 0.0;

			for (k = 0; k < 3; k++) sum += m1 [l][k] * m2 [k][c];

			dst [l][c] = sum;
		}
	}

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Matrice transposee : dst = transpose (src)
 *	------------------------------------------
 */
t_err	Matrix_transpose (M33 src, M33 dst)
{
	t_err		error = OK;
	int		i, j;

	for (i = 0; i < 3; i++) 
		for (j = 0; j < 3; j++)
			dst [i][j] = src [j][i];

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Matrice inverse : dst = inverse (src)
 *	-------------------------------------
 */
t_err	Matrix_inverse (M33 src, M33 dst)
{
	char *		fonction = FNAME ("Matrix_inverse");
	t_err		error = OK;
	double		deter;

	deter	= src [0][0] * (src [1][1] * src [2][2] - src [1][2] * src [2][1])
		+ src [0][1] * (src [1][2] * src [2][0] - src [1][0] * src [2][2])
		+ src [0][2] * (src [1][0] * src [2][1] - src [1][1] * src [2][0]);

	if (fabs (deter) < EPSILON) {

		Affiche_erreur (fonction, "Determinant nul, inversion matrice impossible");
		error = ERROR;
		dst [0][0] = dst [1][1] = dst [2][2] = 1.0;
		dst [1][0] = dst [2][1] = dst [0][2] = 0.0;
		dst [2][0] = dst [0][1] = dst [1][2] = 0.0;
		goto EXIT;
	}

	dst [0][0] = (src [1][1] * src [2][2] - src [1][2] * src [2][1]) / deter;
	dst [1][0] = (src [1][2] * src [2][0] - src [1][0] * src [2][2]) / deter;
	dst [2][0] = (src [1][0] * src [2][1] - src [1][1] * src [2][0]) / deter;

	dst [0][1] = (src [2][1] * src [0][2] - src [2][2] * src [0][1]) / deter;
	dst [1][1] = (src [2][2] * src [0][0] - src [2][0] * src [0][2]) / deter;
	dst [2][1] = (src [2][0] * src [0][1] - src [2][1] * src [0][0]) / deter;

	dst [0][2] = (src [0][1] * src [1][2] - src [0][2] * src [1][1]) / deter;
	dst [1][2] = (src [0][2] * src [1][0] - src [0][0] * src [1][2]) / deter;
	dst [2][2] = (src [0][0] * src [1][1] - src [0][1] * src [1][0]) / deter;

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Matrice de rotation par rapport a X d'un angle donne
 *	----------------------------------------------------
 */
t_err	Matrix_rotation_X_axis (double angle, M33 mat)
{
	t_err		error = OK;

	mat [0][0] = 1.0;
	mat [1][0] = 0.0;
	mat [2][0] = 0.0;

	mat [0][1] = 0.0;
	mat [1][1] = cos (angle);
	mat [2][1] = - sin (angle);

	mat [0][2] = 0.0;
	mat [1][2] = sin (angle);
	mat [2][2] = cos (angle);

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Matrice de rotation par rapport a Y d'un angle donne
 *	----------------------------------------------------
 */
t_err	Matrix_rotation_Y_axis (double angle, M33 mat)
{
	t_err		error = OK;

	mat [0][0] = cos (angle);
	mat [1][0] = 0.0;
	mat [2][0] = sin (angle);

	mat [0][1] = 0.0;
	mat [1][1] = 1.0;
	mat [2][1] = 0.0;

	mat [0][2] = - sin (angle);
	mat [1][2] = 0.0; 
	mat [2][2] = cos (angle);

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Matrice de rotation par rapport a Z d'un angle donne 
 *	----------------------------------------------------
 */
t_err	Matrix_rotation_Z_axis (double angle, M33 mat)
{
	t_err		error = OK;

	mat [0][0] = cos (angle);
	mat [1][0] = - sin (angle); 
	mat [2][0] = 0.0;

	mat [0][1] = sin (angle); 
	mat [1][1] = cos (angle); 
	mat [2][1] = 0.0;

	mat [0][2] = 0.0; 
	mat [1][2] = 0.0; 
	mat [2][2] = 1.0; 

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Diagonalisation matrice par la methode de Jacobi
 *	------------------------------------------------
 *	Cette fonction calcule les valeurs propres et les vecteurs propres d'un matrice symetrique
 *	- in	matrice d'entree symetrique 
 *	- V	vecteur propre
 *	- out	matrice des valeurs propres
 */
t_err	Diagonalisation_Jacobi (M33 in, V3 V, M33 out)
{
	char *		fonction = FNAME ("Diagonalisation_Jacobi");
	t_err		error = OK;
	int		i, j, k, iter;
	V3		B, Z;
	double		somme, tresh, h, t, g, c, s, tau, theta;

	/* 	Initialisation de out avec la matrice unite, de V et B avec la diagonale de in
	 */
	for (i = 0; i < 3; i++) {

		for (j = 0; j < 3; j++) out [i][j] = (i == j) ? 1.0 : 0.0;
		B [i] = in [i][i];
		V [i] = in [i][i];
		Z [i] = 0.0;
	}

	iter	= 0;

	while (iter++ < MAX_ITER_JACOBI) {

		Affiche_trace (5, fonction, "Iteration %d  V = % 1.6E % 1.6E % 1.6E",
			iter, V [0], V [1], V [2]);

		/* 	Somme des valeurs absolues des termes sub-diagonaux
		 */
		somme = fabs (in [0][1]) + fabs (in [0][2]) + fabs (in [1][2]);

		if (fabs (somme) < EPSILON) break; 	/* La diagonalisation est termine */

		/* 	calcul du seuil
		 */
		tresh = (iter < 4) ? (0.2 * somme / 9.0) : 0.0;

		for (i = 0; i < 3 - 1; i++) {

			for (j = i + 1; j < 3; j++) {

				g = 100 * fabs (in [i][j]);

				/* 	Apres 4 tours de boucle, on saute la rotation 
				 *	si le terme extra-diagonal est trop petit
				 */
				if (iter > 4 && fabs (V [i]) + g == fabs (V [i]) && fabs (V [j]) + g == fabs (V [j])) {

					in [i][j] = 0;
				}
				else if (fabs (in [i][j]) > tresh) {
					h = V [j] - V [i];
					if (fabs (h) + g == fabs (h))
						t = in [i][j] / h;      /* t est egal a 1/(2 xtheta) */
					else {	
						theta = 0.5 * h / in [i][j];
						t = 1 / (fabs(theta) + sqrt(1 + theta * theta));
						if (theta < 0) t = -t;
					}
					c	= 1 / sqrt(1 + t * t);
					s	= t * c;
					tau	= s / (1 + c);
					h	= t * in [i][j];
					Z [i]	= Z [i] - h;
					Z [j]	= Z [j] + h;
					V [i]	= V [i] - h;
					V [j]	= V [j] + h;
					in [i][j]= 0.0;

					/*	Cas des rotations 0 <= k < p
					 */
					for (k = 0; k < i; k++) {

						g = in [k][i];
						h = in [k][j];
						in [k][i]= g - s * (h + g * tau);
						in [k][j]= h + s * (g - h * tau);
					}

					/* 	Cas des rotations p < k < q
					 */
					for (k = i + 1; k < j; k++) {

						g = in [i][k];
						h = in [k][j];
						in [i][k]= g - s * (h + g * tau);
						in [k][j]= h + s * (g - h * tau);
					}

					/*	Cas des rotations q < k < 3
					 */
					for (k = j + 1; k < 3; k++) {

						g = in [i][k];
						h = in [j][k];
						in [i][k]= g - s * (h + g * tau);
						in [j][k]= h + s * (g - h * tau);
					}

					for (k = 0; k < 3; k++) {

						g = out [k][i];
						h = out [k][j];
						out [k][i]= g - s * (h + g * tau);
						out [k][j]= h + s * (g - h * tau);
					}
				}
			}
		}
		/*	Mise a jour de V avec la somme des t * a [i][j] et reinitialisation Z
		 */
		for (i = 0; i < 3; i++) {

			B [i] = B [i] + Z [i];
			V [i] = B [i];
			Z [i] = 0;
		}
	}

	if (iter >= MAX_ITER_JACOBI) error = ERROR;

EXIT:	return error;
}
