#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

ifstream fin("D:\\ubb_anul_III\\Procesarea datelor audio-video\\Lab1\\afisare.txt");
ofstream fout("D:\\ubb_anul_III\\Procesarea datelor audio-video\\Lab1\\decoder_result.ppm");

int m, n;
double** y;
double** u;
double** v;


struct rgb {
	int r, g, b;
};

rgb** mat;
void init_matrix()
{
	y = new double* [n];
	u = new double* [n];
	v = new double* [n];
	mat = new rgb * [n];
	for (int i = 0; i < n; i++)
	{
		y[i] = new double[m];
		u[i] = new double[m];
		v[i] = new double[m];
		mat[i] = new rgb[m];
	}

}


void read()
{
	int nr_blocks; 
	fin >> nr_blocks;
	
	for (int i = 0; i < nr_blocks; i++)
	{
		int starti, startj;
		char type;
		

		fin >> starti >> startj;
		fin >> type;
		for (int j = starti; j < starti + 8; j++)
		{
			for (int k = startj; k < startj + 8; k++) 
			{
				fin >> y[j][k];
			}
		}	
	}

	for (int o = 0; o < 2; o++)
	{
		for (int i = 0; i < nr_blocks; i++)
		{
			int starti, startj;
			char type;
			double aux;

			fin >> starti >> startj;
			fin >> type;
			for (int j = starti; j < starti + 8; j += 2)
			{
				for (int k = startj; k < startj + 8; k += 2)
				{
					fin >> aux;
					if (type == 'U')
					{
						u[j][k] = aux;
						u[j + 1][k] = aux;
						u[j + 1][k + 1] = aux;
						u[j][k + 1] = aux;
					}
					else
					{
						v[j][k] = aux;
						v[j + 1][k] = aux;
						v[j + 1][k + 1] = aux;
						v[j][k + 1] = aux;
					}

				}
			}
		}
	}
}

void yuvTOrgb()
{
	double aux;
	for (int i = 0; i < n; i++) 
	{
		for (int j = 0; j < m; j++) 
		{
			aux = y[i][j] + 1.402 * (v[i][j] - 128);
			if (aux < 0)
				aux = 0;
			if (aux > 255)
				aux = 255;
			mat[i][j].r = (int)aux;
			
			aux = y[i][j] - 0.334 * (u[i][j] - 128) - 0.714 * (v[i][j] - 128);
			if (aux < 0)
				aux = 0;
			if (aux > 255)
				aux = 255;
			mat[i][j].g = (int)aux;

			aux = y[i][j] + 1.772 * (u[i][j] - 128);
			if (aux < 0)
				aux = 0;
			if (aux > 255)
				aux = 255;
			mat[i][j].b = (int)aux;
		}
	}
}

void create_ppd_file() 
{
	fout << "P3\n";
	fout << "# Created by Denis :)\n";
	fout << m << " " << n << "\n";
	fout << "255" << "\n"; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			fout << mat[i][j].r << "\n";
			fout << mat[i][j].g << "\n";
			fout << mat[i][j].b << "\n";
		}
	}
	
}

void deallocate_memory()
{
	for (int i = 0; i < n; i++)
	{
		delete(y[i]);
		delete(u[i]);
		delete(v[i]);
		delete(mat[i]);
	}
	delete(y);
	delete(u);
	delete(v);
	delete(mat);
}

int main()
{
	fin >> m >> n;
	init_matrix();
	read();
	yuvTOrgb();
	create_ppd_file();
	deallocate_memory();
	return 1;
}