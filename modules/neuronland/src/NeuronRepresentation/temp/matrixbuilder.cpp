//
//
//
#include <math.h>

#include "Neuron2D.hpp"

void BuildBranchMatrix(double *matrix, dendrite_axial::iterator it, int size)
{

	/* do all internal nodes */

	double cprev = pow( (double)(*it).m_samples[0].d, 1.5);
	double ccurr;
	int prevnode = (*it).m_nodejunc;
	int currnode = (*it).m_nodefirst;

	for(int i=1;i<(*it).m_samples.size()-1;++i)
	{
		ccurr = pow( (double)(*it).m_samples[i].d, 1.5);

		// lower diag
		matrix[prevnode + size*currnode] = cprev/(cprev + ccurr);
		assert( matrix[prevnode + size*currnode] != 0.0 );
		
		// diag
		matrix[currnode + size*currnode] = 1.0;
		assert( matrix[currnode + size*currnode] != 0.0 );

		// upper diag
		matrix[currnode+1 + size*currnode] = ccurr/(cprev + ccurr);
		assert( matrix[currnode+1 + size*currnode] != 0.0 );

		// update loop
		cprev = ccurr;
		prevnode = currnode;
		++currnode;
	}

	double cs = pow( (double)(*it).m_samples[(*it).m_samples.size()-1].d, 1.5);
	dendrite_axial::iterator it2 = it.child();

	// loop 1
	while(it2)
	{
		cs += pow( (double)(*it2).m_samples[0].d, 1.5);
		it2 = it2.peer();
	}

	// loop 2
	matrix[currnode-1 + currnode*size] = ccurr/cs;
	matrix[currnode + currnode*size] = 1.0;

	it2 = it.child();
	while(it2)
	{
		matrix[(*it2).m_nodefirst + currnode*size] = pow( (double)(*it2).m_samples[0].d, 1.5)/ cs;
		it2 = it2.peer();
	}


	if(it.child())
	{
		BuildBranchMatrix( matrix, it.child(), size);
	}

	if(it.peer())
	{
		BuildBranchMatrix( matrix, it.peer(), size);
	}

}


// assumes zeroed matrix
void BuildFullMatrix(double *matrix, Neuron2D &nrn, int size)
{
	// soma first
	

	DAIT d;
	// first do elements due to SOMA node

	double cs = 0.0;
	for(d = nrn.m_dendrites.begin(); d != nrn.m_dendrites.end(); ++d)
	{
		cs += pow( (double)(*((*d).root())).m_samples[0].d, 1.5 );
	}

	// do soma diag element
	matrix[0 + 0*size] = 1.0;

	// connecting elements
	for(d = nrn.m_dendrites.begin(); d != nrn.m_dendrites.end(); ++d)
	{
		matrix[ (*((*d).root())).m_nodefirst + 0*size] = pow( (double)(*((*d).root())).m_samples[0].d, 1.5)/cs ;
	}

	// all the dendrites
	for(d = nrn.m_dendrites.begin(); d != nrn.m_dendrites.end(); ++d)
	{
		BuildBranchMatrix( matrix, (*d).root(), size);
	}
}


// should really be done at the same time we build the matrix...
void SymmetriseMatrix(double *matrix, int size)
{
	int i, j;

	for(i = 0; i<size; ++i)
	{
		for(j=i+1; j<size;++j)
		{
			if(matrix[i*size + j] != 0.0)
			{
				double val = sqrt(matrix[i*size + j] * matrix[j*size + i]);
				matrix[i*size + j] = val;
				matrix[j*size + i] = val;
			}
		}
	}
}



#if 0
#include <stdlib.h>

#include <stdio.h>
#include <assert.h>
#include "matrix.h"

/* Allocates memory to a sparse matrix - returns 0 if successful */
void mat_malloc( sparse_mat *a, int n, int w)
{
    a->a = (double *) malloc( w*sizeof(double) );
    if(!a->a)
    {
    	printf("Not enough mem for a\n");
      exit(1);
    }
    a->col = (int *) malloc( w*sizeof(int) );
    if(!a->col)
    {
    	printf("Not enough mem for col\n");
      exit(1);
    }
    a->start_row = (int *) malloc( (n+1)*sizeof(int) );
    if(!a->start_row)
    {
    	printf("Not enough mem for start_row\n");
      exit(1);
    }
    a->diag_index = (int *) malloc( (n+1)*sizeof(int) );
    if(!a->diag_index)
    {
    	printf("Not enough mem for diag_index\n");
      exit(1);
    }
    a->n = n;
    a->start_row[n] = w;
    if ( a->start_row==NULL ) exit(1);
}



/* De-allocates memory of a sparse matrix */
void mat_free( sparse_mat *a)
{
	assert(a->start_row);
   assert(a->diag_index);
	assert(a->a);
	assert(a->col);

   free(a->start_row);
	free(a->diag_index);
   free(a->a);
   free(a->col);

}


/* Multiplies sparse matrix a[ ][ ] with vector v[ ] */
void mat_vec_mult( sparse_mat *a, double *v , double *b)
{
    int i, j, ntop, n;

    n = a->n;
    for ( j=0 ; j<n ; j++) {
        ntop = a->start_row[j+1];
        for( b[j]=0.0,i=(a->start_row[j]) ; i<ntop ; i++ ) {
            b[j] += (a->a[i])*v[a->col[i]];
        }
    }
    return;
}








//////////////////////////////////////////////////////////////////////
//
//
//  Convert a neuron into various matrix forms
//
/////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "neuron.h"
#include "matrix.h"


#include "neur_ops.h"

#define CS 1.0
#define GS 0.091

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//  Construct matrix for General Compartmental Model
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
int constructBranchMat(branch *b, double *bl, double *br, sparse_mat *al, sparse_mat *ar, double ds, int type, int *elemindex)
{
  static int i;
  static int row;
  static branch *p;
  static int count;
  static int nodes;

  double cb=0;
  double hsum=0;
  double h;
  double c;
  double coeff;
  double temp1;

  row = b->nfirst;
  nodes = b->nodes;

  h = b->h;


/* do all internal nodes */
  for(i=1;i<nodes-1;i++)
  {
  	temp1 =  (0.5 * b->cn[i]) + 0.25*(b->cn[i-1] + 2*b->cn[i] + b->cn[i+1])/(h*h);
   temp1 *= ds;

  // note location of first element in this row
  	al->start_row[row] = *elemindex;
  	ar->start_row[row] = *elemindex;

    // set sub-diagonal element
    al->a[*elemindex] = -0.25 * (b->cn[i-1] + b->cn[i])/ (h * h);
    ar->a[*elemindex] = +0.25 * (b->cn[i-1] + b->cn[i])/ (h * h);

    if(i==1)
    {
    	al->col[*elemindex] = b->njunct;
    	ar->col[*elemindex] = b->njunct;
    }
    else
    {
    	al->col[*elemindex] = row-1;
    	ar->col[*elemindex] = row-1;
    }

    (*elemindex)++;

    // set diagonal element info

    al->diag_index[row] = *elemindex;
    ar->diag_index[row] = *elemindex;

    al->a[*elemindex] = 0.0;  // dont set diagonal element here
    ar->a[*elemindex] = 0.0;  // const diagonal info goes in bl, br for now
    al->col[*elemindex] = row;
    ar->col[*elemindex] = row;
    bl[row] = b->cn[i] + temp1;
    br[row] = b->cn[i] - temp1;
    (*elemindex)++;

    // set super-diagonal element
    al->a[*elemindex] = -0.25 * (b->cn[i+1] + b->cn[i])/ (h * h);
    ar->a[*elemindex] = +0.25 * (b->cn[i+1] + b->cn[i])/ (h * h);
    al->col[*elemindex] = row+1;
    ar->col[*elemindex] = row+1;
    (*elemindex)++;

    // next row
    ++row;
  }

  /* do terminal node...could be branch point, discontinuity, or terminal*/

  /* some initialisation and get some useful info about connecting branches*/


  // first trip round connecting branches

  // parent
  c = b->cn[b->nodes-1];
  cb += h * c;
  hsum += h;
  count=CountChildren(b);

  // children
  if( (p=b->child) != NULL)
  {
    do{
    	hsum += p->h;
      cb += p->h * p->cn[0];
    } while( (p=p->peer) != NULL);
  }
  cb /= hsum;


  // second trip round...

  // parent
  h = b->h;
  c = b->cn[b->nodes-1];
  coeff = ds * (c + b->cn[b->nodes-2])/(2*h);
  temp1 = coeff;

  // note location of first element in this row
  al->start_row[row] = *elemindex;
  ar->start_row[row] = *elemindex;

  // set sub-diagonal element (element linking branch point to parent node
  al->a[*elemindex] = -coeff;
  ar->a[*elemindex] = +coeff;
  al->col[*elemindex] = (b->nfirst)+(b->nodes) - 2; // should equal row-1
  ar->col[*elemindex] = (b->nfirst)+(b->nodes) - 2;
  (*elemindex)++;


   // diagonal
  // set diagonal element info

  al->diag_index[row] = *elemindex;
  ar->diag_index[row] = *elemindex;

  al->a[*elemindex] = 0.0;  // dont set diagonal element here
  ar->a[*elemindex] = 0.0;  // const diagonal info goes in bl, br for now
  al->col[*elemindex] = row;
  ar->col[*elemindex] = row;
  (*elemindex)++;

  // children
  // do child node elements
  p=b->child;

  for(i=1;i<=count;i++)
  {
    h = p->h;
    coeff = ds * (p->cn[0] + p->cn[1])/(2*h);
    temp1 +=coeff;

    al->a[*elemindex] = -coeff;
    ar->a[*elemindex] = +coeff;
    al->col[*elemindex] = p->nfirst;
    ar->col[*elemindex] = p->nfirst;
    (*elemindex)++;
    p=p->peer;
  }

  // fill in diag info..
  bl[row] = cb * (1+ds/2) + temp1;
  br[row] = cb * (1-ds/2) - temp1;

  if(b->child != NULL)
  constructBranchMat(b->child, bl, br, al, ar, ds,
             type, elemindex);
  if(b->peer != NULL)
  constructBranchMat(b->peer, bl, br, al, ar, ds,
             type, elemindex);

  return 1;
}



//
// sets up either first order or second order finite difference schemes
//
int neuronToSparseMatrix(neuron *n, sparse_mat *al, sparse_mat *ar,
							double *bl, double *br, double ds)
{
  int i;
  int elemindex; // contains current unfilled element
  int type;

  double cs=0;
  double hsum=0;
  double temp=0;
  double h;
  double c;
  double coeff;

// first do elements due to SOMA node

  elemindex = 0;

  al->start_row[0] = 0;
  ar->start_row[0] = 0;
  al->diag_index[0] = 0;
  ar->diag_index[0] = 0;
  al->col[0] = 0;
  ar->col[0] = 0;

  elemindex = 1;

  // do soma connecting elements..
  for(i=0;i<n->ndend;i++)
  {
    h = n->dendlist[i].root->h;
    c = n->dendlist[i].root->cn[0];
    cs += h * c;
    hsum += h;

    coeff = ds * (cs + n->dendlist[i].root->cn[1])/(4*h);

    al->a[elemindex] = -coeff;
    ar->a[elemindex] = +coeff;
    al->col[elemindex] = n->dendlist[i].root->nfirst;
    ar->col[elemindex] = n->dendlist[i].root->nfirst;
    ++elemindex;

    temp += coeff;
  }
  cs /= n->ndend;

  // do soma diag element

  // note constant contribution to diagonal elements
  bl[0] = (CS + 0.5 * cs) + (0.5 * ds * (GS + 0.5*cs)) + temp;
  br[0] = (CS + 0.5 * cs) - (0.5 * ds * (GS + 0.5*cs)) - temp;

  // at this point, elemindex == n->ndend+1

// now do remaining BRANCH nodes
  for(i=0;i<n->ndend;i++){
    constructBranchMat(n->dendlist[i].root, bl, br,
              al, ar, ds, type, &elemindex);
  }
      return 1;
}







void cgs(int getmem, sparse_mat *a, double *b, double *x0, double *x, double tol)
{
    int i, n, finish;
    static int start=1;
    double rho_old, rho_new, alpha, beta, sigma, err;
    static double *p, *q, *u, *v, *r, *rb, *y;

/* Step 1 - Check memory status */
    n = a->n;
    if ( start ) {
        r = (double *) malloc( n*sizeof(double) );
        rb = (double *) malloc( n*sizeof(double) );
        p = (double *) malloc( n*sizeof(double) );
        q = (double *) malloc( n*sizeof(double) );
        u = (double *) malloc( n*sizeof(double) );
        v = (double *) malloc( n*sizeof(double) );
        y = (double *) malloc( n*sizeof(double) );
        start = 0;
    }

/* Step 2 - Initialise residual, p[ ] and q[ ] */
    mat_vec_mult( a, x0, r);
    for ( rho_old=0.0,i=0 ; i<n ; i++ ) {
        r[i] = b[i]-r[i];
        rho_old += r[i]*r[i];
        rb[i] = r[i];
        p[i] = 0.0;
        q[i] = 0.0;
    }
    if ( rho_old<tol ) {
        for ( i=0 ; i<n ; i++ ) x[i] = x0[i];
        return;
    }
    rho_old = 1.0;
    finish = 0;

/* The main loop */
    while ( !finish ) {

/* Compute scale parameter for solution update */
        for ( rho_new=0.0,i=0 ; i<n ; i++ ) rho_new += r[i]*rb[i];
        beta = rho_new/rho_old;

/* Update u[ ] and p[ ] */
        for ( i=0 ; i<n ; i++ ) {
            u[i] = r[i]+beta*q[i];
            p[i] = u[i]+beta*(q[i]+beta*p[i]);
        }

/* Update v[ ] and compute sigma */
        mat_vec_mult( a, p, v);
        for ( sigma=0.0,i=0 ; i<n ; i++ ) sigma += rb[i]*v[i];

/* Compute alpha and update q[ ], v[ ] and x[ ] */
        alpha = rho_new/sigma;
        for ( i=0 ; i<n ; i++ ) {
            q[i] = u[i]-alpha*v[i];
            v[i] = alpha*(u[i]+q[i]);
            x[i] += v[i];
        }

 /* Update r[ ] and estimate error */
        mat_vec_mult( a, v, y);
        for ( err=0.0,i=0 ; i<n ; i++ ) {
            r[i] -= y[i];
            err += r[i]*r[i];
        }
        rho_old = rho_new;
        if ( err<tol ) finish = 1;
    }

/* Check memory status */
    if ( getmem<=0 ) start = 1;
    if ( start ) {
        free(r);
        free(rb);
        free(p);
        free(q);
        free(u);
        free(v);
        free(y);
    }
    return;
}




#endif