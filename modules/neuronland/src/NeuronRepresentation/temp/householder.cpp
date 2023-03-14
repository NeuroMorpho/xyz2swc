//
//
//


#include <math.h>
#include <stdio.h>


 /***************************************************************
    Performs Householder transformations on a symmetric matrix.
  ***************************************************************/
void house( int n, double *a)
{
    int i, j, k, km1, jmk, imk, dim;
    double alfa, amp, pd, tmp, *v, *z, *d, *e;

/* Allocate d[ ] and e[ ] */

    d = new double[n];
    e = new double[n];

/* Initialise q[ ][ ] at the identity
    for ( k=0 ; k<n ; k++ ) {
        for ( j=0 ; j<n ; j++ ) q[k][j] = 0.0;
        q[k][k] = 1.0;
    } 
	*/

/* A total of (n-2) householder steps are required */
    for ( k=1 ; k<n-1 ; k++ ) 
	{
        dim = n-k;
        km1 = k-1;
        d[km1] = a[km1*n + km1];
        v = new double[dim];
        z = new double[dim];

/* Compute v[ ] */
        for ( amp=0.0,j=0 ; j<dim ; j++ ) {
            v[j] = a[km1*n + k+j];
            amp += v[j]*v[j];
        }
        alfa = v[0] >= 0.0 ? 1.0 : -1.0;
        amp = sqrt(amp);
        tmp = 1.0/sqrt(2.0*amp*(amp+alfa*v[0]));
        v[0] += alfa*amp;
        for ( j=0 ; j<dim ; j++ ) v[j] *= tmp;

/* Assign k-th row of a[ ][ ] */
        e[k] = -alfa*amp;

/* Update a[ ][ ] from the (k,k)th entry downwards - build z[ ] */
        for ( pd=0.0,j=k ; j<n ; j++ ) {
            for ( tmp=0.0,i=k ; i<n ; i++ ) tmp += a[j*n + i]*v[i-k];
            z[j-k] = tmp;
            pd += v[j-k]*z[j-k];
        }
        pd = pd+pd;

/* Update a[ ][ ] itself */
        for ( j=k ; j<n ; j++ ) {
            jmk = j-k;
            for ( i=k ; i<n ; i++ ) {
                imk = i-k;
                a[j*n + i] += 2.0*(pd*v[jmk]*v[imk]-z[imk]*v[jmk]-z[jmk]*v[imk]);
            }
        }

        delete [] v;
        delete [] z;

		printf("Completed householder step %d\n", k);
    }
    d[n-2] = a[(n-2)*n + n-2];
    e[n-1] = a[(n-2)*n + n-1];
    d[n-1] = a[(n-1)*n + n-1];

/* Re-write a[ ][ ] */
    for ( i=0 ; i<n ; i++ ) {
        for ( j=0 ; j<n ; j++ ) a[i*n + j] = 0.0;
    }
    a[0*n + 0] = d[0];
    a[0*n + 1] = e[1];
    for ( i=1 ; i<n-1 ; i++ ) {
        a[i*n + i-1] = e[i];
        a[i*n + i] = d[i];
        a[i*n + i+1] = e[i+1];
    }
    a[(n-1)*n + n-2] = e[n-1];
    a[(n-1)*n + n-1] = d[n-1];

    delete [] d;
    delete [] e;

    return;
}


