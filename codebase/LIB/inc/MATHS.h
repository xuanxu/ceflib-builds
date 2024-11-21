/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: MATHS.h,v $
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

#ifndef	__MATHS_H
#define	__MATHS_H

#ifndef	M_PI
#define M_PI		3.14159265358979323846
#endif

#define	DEG_TO_SR(x)	((x) * M_PI / 180.0)
#define	SR_TO_DEG(x)	((x) * 180.0 / M_PI)

extern	const float	FILL_VALUE;
extern	const float	EPSILON;

typedef	double		V3 [3];

typedef	double		M33 [3][3];


/*	Indique si la valeur est une valeur filaire
 *	-------------------------------------------
 */
int	Fill_value (double value);


/*	Norme d'un vecteur
 *	------------------
 */
double	Norm (V3 vector);


/*	Multiple les valeurs d'un vecteur ou d'une matrice par une constante
 *	--------------------------------------------------------------------
 */
void	Constant_product (double * var, int size, double constant);


/*	Affiche contenu matrice
 *	-----------------------
 */
t_err	Vector_display (int level, char * fonction, char * text, V3 vector);


/*	Affiche contenu matrice
 *	-----------------------
 */
t_err	Matrix_display (int level, char * fonction, char * text, M33 mat);


/*	Produit de 2 matrices : dst = m1 x m2 
 *	-------------------------------------
 */
t_err	Matrix_product (M33 m1, M33 m2, M33 dst);


/*	Matrice inverse : dst = inverse (src)
 *	-------------------------------------
 */
t_err	Matrix_inverse (M33 src, M33 dst);


/*	Matrice transposee : dst = transpose (src)
 *	------------------------------------------
 */
t_err	Matrix_transpose (M33 src, M33 dst);


/*	Matrice de rotation par rapport a X d'un angle donne
 *	----------------------------------------------------
 */
t_err	Matrix_rotation_X_axis (double angle, M33 mat);


/*	Matrice de rotation rapport a Y d'un angle donne
 *	----------------------------------------------------
 */
t_err	Matrix_rotation_Y_axis (double angle, M33 mat);


/*	Matrice de rotation par rapport a Z d'un angle donne 
 *	----------------------------------------------------
 */
t_err	Matrix_rotation_Z_axis (double angle, M33 mat);


/*	Diagonalisation matrice par la methode de Jacobi
 *	------------------------------------------------
 *	Cette fonction calcule les valeurs propres et les vecteurs propres d'un matrice symetrique
 *	- in	matrice d'entree symetrique 
 *	- V	vecteur propre
 *	- out	matrice des valeurs propres
 */
t_err	Diagonalisation_Jacobi (M33 in, V3 V, M33 out);


#endif
