//
// rediscretisation.cpp
//

#include <vector>

#include <math.h>
#include "Neuron2D.hpp"

#include "rediscretisation.h"


// assumes an electrotonic Branch2D
// returned surface areas  aren't particularly meaningful except as ratios
double CalculateSurfaceArea(Branch2D &bran)
{
	double surfacearea = 0.0;
	double lastx = 0.0;
	PIT it = bran.m_samples.begin();
	++it;

	assert(it != bran.m_samples.end());

	for(; it != bran.m_samples.end(); ++it)
	{
		surfacearea += 3.1415926 * pow( (double)(*it).d, 1.5) * ((double)(*it).x - lastx);
		lastx = (*it).x;
	}

	//printf("sa: %f\n", surfacearea);

	return surfacearea;
}


struct BranchRediscretiser
{
	double m_h;
	double m_minnodes;

	BranchRediscretiser(double h, double minnodes) : m_h(h), m_minnodes(minnodes)
	{
	}

	void operator () (Branch2D &bran)
	{
		assert(bran.m_samples.size() >= 2);

		// work out the surface area of the branch( assumed to be a chain of cylinders)
		double surfacearea_orig = CalculateSurfaceArea(bran);

		// how many new nodes ?
		double length = bran.m_samples[bran.m_samples.size() - 1].x;
		int nodes = ceil(length/m_h) + 1;
		if(nodes < m_minnodes)
		{
			assert(0);
			nodes = m_minnodes;
		}

		// need to work out the diameters of all the new nodes
		double requiredlength = nodes * m_h;
		std::vector<AxialPoint> newpoints;
		newpoints.reserve(nodes);

		// give new node the diameter of the cylinder on which it lies.
		length = 0.0;
		PIT p = bran.m_samples.begin();
		for(int i=0;i<nodes;++i)
		{
			//printf("np(%f,%f) ",length, (*p).d);
			newpoints.push_back( AxialPoint(length, (*p).d) );
			length += m_h;
			while( (length > (*p).x) && (p+1 != bran.m_samples.end()) ) ++p;
		}

		bran.m_samples = newpoints;

		// now re-jig all the diameters to ensure surface area is the same as for
		// the original branch
		double surfacearea_ratio = CalculateSurfaceArea(bran)/surfacearea_orig;


		//printf("orig sa %f, new sa %f\n", surfacearea_orig, CalculateSurfaceArea(bran));
		printf("Surface area ratio %f\n", surfacearea_ratio);
	}
};


void Rediscretise(Neuron2D &nrn, double h, int minnodes)
{
	BranchRediscretiser redis(h, minnodes);

	for(Neuron2D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		Neuron2D::DendriteTree::recurse((*d).root(), redis);
	}
}


int NodifyBranch(dendrite_axial::iterator it, int total_nodes, int junction_node)
{
	int nodes = total_nodes;

	(*it).m_nodejunc = junction_node;
	(*it).m_nodefirst = total_nodes;

	nodes += (*it).m_samples.size()-1;

	if(it.child())
	{
		nodes = NodifyBranch(it.child(), nodes, nodes-1);
	}

	if(it.peer())
	{
		nodes = NodifyBranch(it.peer(), nodes, junction_node);
	}

	return nodes;
}


void NodifyNeuron(Neuron2D &nrn)
{
	int nodes = 1;

	for(DAIT d = nrn.m_dendrites.begin(); d != nrn.m_dendrites.end(); ++d)
	{
		nodes = NodifyBranch( (*d).root(), nodes, 0);
	}
}



#if 0
//
// old stuff...uses spline data to rediscretise tree.
//


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "neuron.h"
#include "cspline.h"

#define PI 3.1415927

 /*******************************************************************
  *
  *  Some operations for manipulation and information extraction
  *   of neuron data structures
  *
  *******************************************************************/

/* Finds subtree length from branch b by initialising with start=1 */
double Find_Dendrite_Length(branch *b, int start)
{
    static double sumlength;

    if ( start ) sumlength = 0.0;
    sumlength += b->length;
    if ( b->child!=NULL ) Find_Dendrite_Length(b->child, 0);
    if ( !start && b->peer!=NULL ) Find_Dendrite_Length(b->peer, 0);
    return sumlength;
}


/* Finds total neuron length */
double Find_Total_Neuron_Length(neuron *n)
{
    int i;
    double length;

    for ( length=0.0,i=0 ; i<n->ndend ; i++ ){
        length += Find_Dendrite_Length(n->dendlist[i].root, 1);
    }
    return length;
}


/////////////////////////////////////////////////////////////////////////////
// Various neuron discretisation routines (physical space only at the moment)
//
//  Determines number of nodes associated with each branch.
//  Notes actual node numbers associated with each branch.
//  Notes branch location of each node.
//
//  Nodes numbered from 0.
//
/////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////
//  For finite-difference methods...constant internodal length for each
//  branch (may differ between branches).
////////////////////////////////////////////////////////////////////////


//
//
//           b  --- root of a dendritic tree.  requires b->parent == NULL.
//
//    inter_len --- set number of nodes to achieve as near to this
//                  length without going higher
//
//    min_nodes --- minimum number of nodes allowed per branch
//
//           jn --- number of junction node to which branch b is connected
//
//    firstnode --- number of first node of dendritic tree connected to soma
//                  ....only used for root branch (b->parent == NULL)
//

int discTreeRoughInter(branch *b, double inter_len, int min_nodes, int jn,
               int firstnode)
{
  int i;
  int nodes = min_nodes;
  static int total_nodes;

// initialise total tree nodes at root of tree
  if(b->parent == NULL) total_nodes = firstnode; // count soma node already

// get biggest possible interval less than or equal to inter_len
// and determine the number of nodes this requires
  nodes = ceil(b->length / inter_len) + 1;
  if(nodes < min_nodes) nodes = min_nodes;

  b->njunct = jn;
  b->nfirst = total_nodes;

  b->h = b->length/ (double)(nodes-1);
  b->nodes = nodes;
  total_nodes += nodes-1; // add all but junction node which has already been
              // counted

// create and fill in array of node positions along branch
  b->xn = (double *)malloc(nodes*sizeof(double));
  b->xn[0] = 0.0;
  for(i=1;i<nodes-1;i++){
    b->xn[i] = (double)i * b->h;
  }
  b->xn[nodes-1] = b->length;

// create and fill in array of node diameters along branch
  b->dn = (double *)malloc(nodes*sizeof(double));
  b->dn[0] = b->d[0];
  for(i=1;i<nodes-1;i++){
    b->dn[i] = eval_cubic_spline(b->nseg, b->xn[i], b->c_pl);
  }
  b->dn[nodes-1] = b->d[b->nseg-1];

// create and fill in array of generalised capacitances for each node on branch
// note,
  b->cn = (double *)malloc(nodes*sizeof(double));
  b->cn[0] = 1.0;
  for(i=1;i<nodes-1;i++){
    b->cn[i] = 1.0;
  }
  b->cn[nodes-1] = 1.0;


  if(b->child)
    discTreeRoughInter(b->child, inter_len, min_nodes, total_nodes-1, 0);
  if(b->peer)
    discTreeRoughInter(b->peer, inter_len, min_nodes, jn, 0);

  return total_nodes;
}





//
//
//           b  --- root of a dendritic tree.  requires b->parent == NULL.
//
//    total_len --- total length of all branches of the tree
//
// rough_total_nodes --- desired number of nodes used to represent tree
//
//    min_nodes --- minimum number of nodes allowed per branch
//
//           jn --- number of junction node to which branch b is connected
//
//    firstnode --- number of first node of dendritic tree connected to soma
//                  ....only used for root branch (b->parent == NULL)
//
int discTreeRoughTotal(branch *b, double total_len, int rough_total_nodes,
               int min_nodes, int jn, int firstnode)
{
  int nodes = min_nodes;
  static int total_nodes;
  int i;

// initialise total tree nodes at root of tree
  if(b->parent == NULL) total_nodes = firstnode; // count soma node already

// determine number of nodes to assign to this branch
// but make sure at least "min_nodes" assigned
  nodes = (int)( ((double)(rough_total_nodes) * b->length)/total_len );
  if(nodes < min_nodes) nodes = min_nodes;

  b->njunct = jn;
  b->nfirst = total_nodes;

  b->h = b->length/ (double)(nodes-1);
  b->nodes = nodes;
  total_nodes += nodes-1; // add all but junction node which has already been
              // counted

// create and fill in array of node positions along branch
  b->xn = (double *)malloc(nodes*sizeof(double));
  b->xn[0] = 0.0;
  for(i=1;i<nodes;i++){
    b->xn[i] = (double)i * b->h;
  }
// create and fill in array of node diameters along branch
  b->dn = (double *)malloc(nodes*sizeof(double));
  b->dn[0] = b->d[0];
  for(i=1;i<nodes-1;i++){
    b->dn[i] = eval_cubic_spline(b->nseg, b->xn[i], b->c_pl);
  }
  b->dn[nodes-1] = b->d[b->nseg-1];


  if(b->child != NULL)
    discTreeRoughTotal(b->child, total_len, rough_total_nodes,
               min_nodes, total_nodes-1, 0);
  if(b->peer != NULL)
    discTreeRoughTotal(b->peer, total_len, rough_total_nodes,
               min_nodes, jn, 0);

  return total_nodes;
}


//
//           b  --- root of a dendritic tree.  requires b->parent == NULL.
//
// nodes_per_bran --- fixed number of nodes per branch
//
//           jn --- number of junction node to which branch b is connected
//
//    firstnode --- number of first node of dendritic tree connected to soma
//                  ....only used for root branch (b->parent == NULL)
//
int discTreeFixedNodesPerBran(branch *b, int nodes_per_bran, int jn,
                  int firstnode)
{
  int i;
  int nodes = nodes_per_bran;
  static int total_nodes;

// initialise total tree nodes at root of tree
  if(b->parent == NULL) total_nodes = firstnode; // count soma node already

  b->njunct = jn;
  b->nfirst = total_nodes;

  b->h = b->length/(double)(nodes_per_bran-1);
  b->nodes = nodes_per_bran;
  total_nodes += nodes_per_bran-1; // add all but junction node
                   // counted which has already been

// create and fill in array of node positions along branch
  b->xn = (double *)malloc(nodes_per_bran*sizeof(double));
  b->xn[0] = 0.0;
  for(i=1;i<nodes_per_bran;i++){
    b->xn[i] = (double)i * b->h;
  }

// create and fill in array of node diameters along branch
  b->dn = (double *)malloc(nodes*sizeof(double));
  b->dn[0] = b->d[0];
  for(i=1;i<nodes-1;i++){
    b->dn[i] = eval_cubic_spline(b->nseg, b->xn[i], b->c_pl);
  }
  b->dn[nodes-1] = b->d[b->nseg-1];

  if(b->child != NULL)
    discTreeFixedNodesPerBran(b->child, nodes_per_bran, total_nodes-1, 0);
  if(b->peer != NULL)
    discTreeFixedNodesPerBran(b->peer, nodes_per_bran, jn, 0);

  return total_nodes;
}


///////////////////////////////////////////////////////////////
// For spectral methods, using Gauss-Lobatto discretisation nodes.
// Same number for each branch.
///////////////////////////////////////////////////////////////



//
//           b  --- root of a dendritic tree.  requires b->parent == NULL.
//
// nodes_per_bran --- fixed number of nodes for spectral rep. of each branch
//
//           jn --- number of junction node to which branch b is connected
//
//    firstnode --- number of first node of dendritic tree connected to soma
//                  ....only used for root branch (b->parent == NULL)
//
int discTreeSpectralN(branch *b, int nodes_per_bran, int jn, int firstnode)
{
  int i;
  int nodes = nodes_per_bran;
  static int total_nodes;

// initialise total tree nodes at root of tree
  if(b->parent == NULL) total_nodes = firstnode; // count soma node already

  b->njunct = jn;
  b->nfirst = total_nodes;

  b->nodes = nodes_per_bran;
  total_nodes += nodes_per_bran-1; /* add all but junction node */
                   /* counted which has already been*/

// create and fill array of spectral nodes
  b->xn = (double *)malloc(nodes_per_bran*sizeof(double));
  b->xn[0] = 0.0;
  for(i=1;i<nodes_per_bran;i++){
    b->xn[i] = (b->length/2.0) *
          (1+cos( PI *(nodes_per_bran-1-i)/(double)(nodes_per_bran-1) ) );
  }
// create and fill in array of node diameters along branch
  b->dn = (double *)malloc(nodes*sizeof(double));
  b->dn[0] = b->d[0];
  for(i=1;i<nodes-1;i++){
    b->dn[i] = eval_cubic_spline(b->nseg, b->xn[i], b->c_pl);
  }
  b->dn[nodes-1] = b->d[b->nseg-1];

  if(b->child != NULL)
    discTreeFixedNodesPerBran(b->child, nodes_per_bran, total_nodes-1, 0);
  if(b->peer != NULL)
    discTreeFixedNodesPerBran(b->peer, nodes_per_bran, jn, 0);

  return total_nodes;
}


//
// ELECTROTONIC SPACE DISCRETISATION
//

////////////////////////////////////////////////////////////////////////
//  For finite-difference methods...constant internodal length for each
//  branch (may differ between branches).
////////////////////////////////////////////////////////////////////////

//
//
//           b  --- root of a dendritic tree.  requires b->parent == NULL.
//
//    inter_len --- set number of nodes to achieve as near to this
//                  length without going higher
//
//    min_nodes --- minimum number of nodes allowed per branch
//
//           jn --- number of junction node to which branch b is connected
//
//    firstnode --- number of first node of dendritic tree connected to soma
//                  ....only used for root branch (b->parent == NULL)
//

int discTreeRoughInterElectrotonic(branch *b, double inter_len, int min_nodes, int jn,
               int firstnode)
{
  int i;
  int nodes = min_nodes;
  static int total_nodes;

// initialise total tree nodes at root of tree
  if(b->parent == NULL) total_nodes = firstnode; // count soma node already

// get biggest possible interval less than or equal to inter_len
// and determine the number of nodes this requires
  nodes = ceil(b->e_length / inter_len) + 1;
  if(nodes < min_nodes) nodes = min_nodes;

  b->njunct = jn;
  b->nfirst = total_nodes;

  b->h = b->e_length/ (double)(nodes-1);
  b->nodes = nodes;
  total_nodes += nodes-1; // add all but junction node which has already been
              // counted

// create and fill in array of node positions along branch
  b->xn = (double *)malloc(nodes*sizeof(double));
  b->xn[0] = 0.0;
  for(i=1;i<nodes-1;i++){
    b->xn[i] = (double)i * b->h;
  }
  b->xn[nodes-1] = b->e_length;

// create and fill in array of node diameters along branch
  b->dn = (double *)malloc(nodes*sizeof(double));
  b->dn[0] = b->d[0];
  for(i=1;i<nodes-1;i++){
    b->dn[i] = eval_cubic_spline(b->nseg, b->xn[i], b->c_el);
  }
  b->dn[nodes-1] = b->d[b->nseg-1];

// create and fill in array of generalised capacitances for each node on branch
// note,
  b->cn = (double *)malloc(nodes*sizeof(double));
  b->cn[0] = 0.5 * PI * b->cm * pow(b->dn[0], 1.5) * sqrt(b->ga/b->gm);

  for(i=1;i<nodes-1;i++){
    b->cn[i] = 0.5 * PI * b->cm * pow(b->dn[i], 1.5) * sqrt(b->ga/b->gm);
  }
  b->cn[nodes-1] = 0.5 * PI * b->cm * pow(b->dn[nodes-1], 1.5) * sqrt(b->ga/b->gm);

// input function setup

  b->funcn = (CurrentFunction *)malloc(nodes*sizeof(CurrentFunction));
  b->tau = (double *)malloc(nodes*sizeof(double));
  b->type = (int *)malloc(nodes*sizeof(int));
                                          
  b->funcn[0] = NULL;
  b->tau[0] = 1.0;
  b->type[0] = 0;
  for(i=1;i<nodes-1;i++){
    b->funcn[i] = NULL;
  	 b->tau[i] = 1.0;
  	 b->type[i] = 0;
  }
  b->funcn[nodes-1] = NULL;
  b->tau[nodes-1] = 1.0;
  b->type[nodes-1] = 0;


  if(b->child)
    discTreeRoughInter(b->child, inter_len, min_nodes, total_nodes-1, 0);
  if(b->peer)
    discTreeRoughInter(b->peer, inter_len, min_nodes, jn, 0);

  return total_nodes;
}






//
// Function to generate discretisation info for neuron "n",
// using discretisation algorithm "type"
//
// if type == DISC_ROUGHINTERNODE, uses "inter_len" and "min_nodes"
// if type == DISC_ROUGHTOTALNODES, uses "rough_total_nodes" and "min_nodes"
// if type == DISC_FIXEDNODESPERBRAN, uses just "min_nodes"
// if type == DISC_SPECTRALORDERN, uses just "min_nodes"
//
int nodifyNeuron(neuron *n, disctype type,
         double rough_inter_len, int rough_total_nodes, int min_nodes )
{
  int i;
  int totalnodes = 1; // initialised to 1 to account to soma node
              // prior to nodifying all the dendritic trees
  double total_len;


  switch(type){

  case DISC_ROUGHINTERNODE:
    for(i=0;i<n->ndend;i++){
      totalnodes += discTreeRoughInter(n->dendlist[i].root,
                       rough_inter_len, min_nodes, 0, totalnodes);
    }
    break;
  case DISC_ROUGHINTERNODE_ELECTROTONIC:
    for(i=0;i<n->ndend;i++){
      totalnodes += discTreeRoughInterElectrotonic(n->dendlist[i].root,
                       rough_inter_len, min_nodes, 0, totalnodes);
    }
    break;

  case DISC_ROUGHTOTALNODES:
    total_len = Find_Total_Neuron_Length(n);
    for(i=0;i<n->ndend;i++){
      totalnodes += discTreeRoughTotal(n->dendlist[i].root,
                       total_len, rough_total_nodes,
                       min_nodes, 0, totalnodes);
    }
    break;

  case DISC_FIXEDNODESPERBRAN:
    for(i=0;i<n->ndend;i++){
      totalnodes += discTreeFixedNodesPerBran(n->dendlist[i].root,
                          min_nodes, 0, totalnodes);
    }
    break;

  case DISC_SPECTRAL:
    for(i=0;i<n->ndend;i++){
      totalnodes += discTreeSpectralN(n->dendlist[i].root,
                          min_nodes, 0, totalnodes);
    }
    break;

  default:
    printf("Error...unknown discretisation scheme\n");
    break;

  }

  return totalnodes;
}

#endif