#include "stdafx.h"

using namespace std;

ifstream fin("D:\\ubb_anul_III\\Procesarea datelor audio-video\\Lab2\\Encoder\\Encoder\\afisare.txt");
ofstream fout("D:\\ubb_anul_III\\Procesarea datelor audio-video\\Lab2\\Decoder\\Decoder\\decoder_result.ppm");

int m, n;
double** y;
double** u;
double** v;

struct block8x8 {
	double block[8][8];
	char type;
	int poz1, poz2;
};

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

vector<block8x8> Gy, Gu, Gv;
void read()
{
	int nr_blocks,p1,p2;
	char tip;
	fin >> nr_blocks;
	block8x8 auxby, auxbu, auxbv;
	for (int t = 0; t < 3; t++)
	{
		for (int k = 0; k < nr_blocks; k++)
		{
			fin >> p1 >> p2;
			fin >> tip;
			if (tip == 'Y')
			{
				auxby.poz1 = p1;
				auxby.poz2 = p2;
				auxby.type = tip;
				for (int i = 0; i < 8; i++)
					for (int j = 0; j < 8; j++)
						fin >> auxby.block[i][j];
				Gy.push_back(auxby);
			}
			if (tip == 'U')
			{
				auxbu.poz1 = p1;
				auxbu.poz2 = p2;
				auxbu.type = tip;
				for (int i = 0; i < 8; i++)
					for (int j = 0; j < 8; j++)
						fin >> auxbu.block[i][j];
				Gu.push_back(auxbu);
			}
			if (tip == 'V')
			{
				auxbv.poz1 = p1;
				auxbv.poz2 = p2;
				auxbv.type = tip;
				for (int i = 0; i < 8; i++)
					for (int j = 0; j < 8; j++)
						fin >> auxbv.block[i][j];
				Gv.push_back(auxbv);
			}
			
		}
	}
}


void DeQuantization()
{
	ifstream qfile("D:\\ubb_anul_III\\Procesarea datelor audio-video\\Lab2\\Encoder\\Encoder\\quantization_matrix.txt");
	int Q[8][8];
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			qfile >> Q[i][j];

	for (int k = 0; k < Gy.size(); k++) {
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				Gy[k].block[i][j] = Gy[k].block[i][j] * Q[i][j];
			}
	}
	
	for (int k = 0; k < Gu.size(); k++) {
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				Gu[k].block[i][j] = Gu[k].block[i][j] * Q[i][j];
			}
	}
	
	for (int k = 0; k < Gv.size(); k++) {
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				Gv[k].block[i][j] = Gv[k].block[i][j] * Q[i][j];
			}
	}
}

vector<block8x8> idcty, idctu, idctv;
void inverse_discrete_cosine_transform()
{
	block8x8 auxy, auxu, auxv;
	double alphai, alphaj;
	double cosx, cosy;
	for (int k = 0; k < Gy.size(); k++)
	{
		auxy.poz1 = Gy[k].poz1; auxy.poz2 = Gy[k].poz2;
		auxy.type = Gy[k].type;
		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++) 
			{
				double sum = 0;
				for (int i = 0; i < 8; i++)
					for (int j = 0; j < 8; j++)
					{
						if (i == 0)
							alphai = 1 / sqrt(2);
						else
							alphai = 1.0;

						if (j == 0)
							alphaj = 1 / sqrt(2);
						else
							alphaj = 1.0;

						cosx = cos(((2 * (double)x + 1) * i * M_PI) / 16);
						cosy = cos(((2 * (double)y + 1) * j * M_PI) / 16);

						sum += alphai * alphaj * Gy[k].block[i][j] * cosx * cosy;
					}
				auxy.block[x][y] = sum / 4.0;
			}
		}
		idcty.push_back(auxy);
	}
	Gy.clear();

	for (int k = 0; k < Gu.size(); k++)
	{
		auxu.poz1 = Gu[k].poz1; auxu.poz2 = Gu[k].poz2;
		auxu.type = Gu[k].type;
		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				double sum = 0;
				for (int i = 0; i < 8; i++)
					for (int j = 0; j < 8; j++)
					{
						if (i == 0)
							alphai = 1 / sqrt(2);
						else
							alphai = 1.0;

						if (j == 0)
							alphaj = 1 / sqrt(2);
						else
							alphaj = 1.0;

						cosx = cos(((2 * (double)x + 1) * i * M_PI) / 16);
						cosy = cos(((2 * (double)y + 1) * j * M_PI) / 16);

						sum += alphai * alphaj * Gu[k].block[i][j] * cosx * cosy;
					}
				auxu.block[x][y] = sum / 4.0;
			}
		}
		idctu.push_back(auxu);
	}
	Gu.clear();

	for (int k = 0; k < Gv.size(); k++)
	{
		auxv.poz1 = Gv[k].poz1; auxv.poz2 = Gv[k].poz2;
		auxv.type = Gv[k].type;
		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				double sum = 0;
				for (int i = 0; i < 8; i++)
					for (int j = 0; j < 8; j++)
					{
						if (i == 0)
							alphai = 1 / sqrt(2);
						else
							alphai = 1.0;

						if (j == 0)
							alphaj = 1 / sqrt(2);
						else
							alphaj = 1.0;

						cosx = cos(((2 * (double)x + 1) * i * M_PI) / 16);
						cosy = cos(((2 * (double)y + 1) * j * M_PI) / 16);

						sum += alphai * alphaj * Gv[k].block[i][j] * cosx * cosy;
					}
				auxv.block[x][y] = sum / 4.0;
			}
		}
		idctv.push_back(auxv);
	}
	Gv.clear();
}

void create_yuv_matrixes() 
{
	int si, sj;
	int auxi, auxj;
	for (int k = 0; k < idcty.size(); k++) 
	{
		auxi = 0;
		auxj = 0;
		si = idcty[k].poz1;
		sj = idcty[k].poz2;
		for (int i = si; i < si + 8; i++) 
		{
			for (int j = sj; j < sj + 8; j++)
			{
				y[i][j] = idcty[k].block[auxi][auxj]+128;
				auxj++;
			}
			auxi++;
		}
	}
	idcty.clear();

	for (int k = 0; k < idctu.size(); k++)
	{
		auxi = 0;
		auxj = 0;
		si = idctu[k].poz1;
		sj = idctu[k].poz2;
		for (int i = si; i < si + 8; i++)
		{
			for (int j = sj; j < sj + 8; j++)
			{
				u[i][j] = idctu[k].block[auxi][auxj]+128;
				auxj++;
			}
			auxi++;
		}
	}
	idctu.clear();

	for (int k = 0; k < idctv.size(); k++)
	{
		auxi = 0;
		auxj = 0;
		si = idctv[k].poz1;
		sj = idctv[k].poz2;
		for (int i = si; i < si + 8; i++)
		{
			for (int j = sj; j < sj + 8; j++)
			{
				v[i][j] = idctv[k].block[auxi][auxj]+128;
				auxj++;
			}
			auxi++;
		}
	}
	idctv.clear();
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
	fout << "255" << "\n";

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
	DeQuantization();
	inverse_discrete_cosine_transform();
	create_yuv_matrixes();
	yuvTOrgb();
	create_ppd_file();
	deallocate_memory();
	return 1;
}