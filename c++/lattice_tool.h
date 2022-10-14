#pragma once
#include "itensor/all.h"
#include "itensor/util/print_macro.h"
#include <vector>
#include <map>

using namespace itensor;
using namespace std;

vector<float> mult(float a, vector<float> v)
{
    float x = a * v[0];
    float y = a * v[1];
    vector<float> z = {x,y};
    return z;
}

vector<float> sum (vector<float> a, vector<float> b)
{
    vector<float> c = {a[0] + b[0], a[1] + b[1]};
    return c;
}

float distance(vector<float> x, vector<float> y)
{
    float d = (x[0]-y[0])*(x[0]-y[0]) + (x[1]-y[1])*(x[1]-y[1]);
    return sqrt(d);
}


class lattice
{
    public:
            string type;
            int Lx;
            int Ly;

    lattice (string type_, int lx_, int ly_)
    {
        type = type_;
        Lx = lx_;
        Ly = ly_;
    }

    vector<vector<float>> translation_vecs() 
    {
	vector<vector<float>> latVecs;
        vector<float> a1, a2;
        if (type == "honeycomb")
        {
	    a1 = {3./2.,sqrt(3.)/2.};
	    a2 = {3./2.,-sqrt(3.)/2.};
	}

	else if (type == "triangle" or type == "kagome")
	{
	    a1 = {1.,0.};
	    a2 = {0.5,sqrt(3.)/2.};
	}

	else if (type == "square")
	{
	    a1 = {1.,0.};
	    a2 = {0.,1.};
	}

	else Error(type + " is not implemented yet.");

	latVecs.push_back(a1);
	latVecs.push_back(a2);
	return latVecs;
    }

    vector<vector<float>> unit_cell()
    {
	vector<vector<float>> latVecs = translation_vecs();
	vector<float> a1 = latVecs[0];
	vector<float> a2 = latVecs[1];
        vector<vector<float>> s;
    	vector<float> u1, u2, u3;
	if (type == "honeycomb")
	{
	    u1 = {0.0,0.0};
	    u2 = {1.0,0.0};
	    s.push_back(u1);
	    s.push_back(u2);
	}

	else if (type == "square" or type == "triangle")
	{
	    u1 = {0.,0.};
	    s.push_back(u1);
	}

	else if (type == "kagome")
	{
	    u1 = {0.,0.};
	    u2 = mult(0.5, a1);
	    u3 = mult(0.5, a2);
	    s.push_back(u1);
	    s.push_back(u2);
	    s.push_back(u3);
	}

	return s;
    }

    map<int, vector<float>> lattice_coords()
    {
	map<int, vector<float>> coords;
	vector<vector<float>> latVecs = translation_vecs();
	vector<float> a1 = latVecs[0];
        vector<float> a2 = latVecs[1];
	vector<vector<float>> uc = unit_cell();
	int label = 1;
	for (int n : range(Lx)){
	    for (int m : range(Ly)){
		for (int k : range(uc.size())){
		vector<float> u = uc[k];
		vector<float> site = sum(u, sum(mult(n, a1), mult(m, a2)));
		coords[label] = site;
		label += 1;
		}
	    }
	}

	return coords;
    }


};

class NN_bonds: public lattice
{
    public:
	    string boundary_condn;
    
    NN_bonds(string type_, int lx_, int ly_, string bc)
	    :lattice(type_, lx_, ly_)
    {
	    boundary_condn = bc;
    }

    vector<pair<int,int>> bonds()
    {
	float tol = 0.001;
	float lat_const = 0;
	vector<pair<int,int>> bond;
	pair<int,int>  b;
	if (type == "kagome") lat_const = 0.5;
        else lat_const = 1.0;
	map<int, vector<float>> coords = lattice_coords();
		
	for (int i : range1(coords.size()))
	{
	    for (int j : range(i+1,coords.size()+1))
	    {
	        float d = distance(coords[i], coords[j]);
		if (fabs(d-lat_const) < tol)
		{
		    b = make_pair(i, j);
		    bond.push_back(b);
		}
	    }
	}

	if (boundary_condn == "yperiodic")
	{
            vector<float> a2 = translation_vecs()[1];
	    int uc_atm = unit_cell().size();
	    for (int k : range1(coords.size()))
	    {
	        if (k%(uc_atm*Ly) == 0)
		{
		    auto edge = sum(coords[k],mult(-Ly,a2));
		    for (int l : range(1,k))
		    {
		        float d = distance(edge,coords[l]);
			if (fabs(d-lat_const) < tol)
			{
			    b = make_pair(k,l);
			    bond.push_back(b);
			}
		    }
		}
	    }
	}

	return bond;
    }
};

