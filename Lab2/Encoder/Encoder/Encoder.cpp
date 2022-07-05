#include "stdafx.h"
using namespace std;

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


rgb** read_from_file()
{
	ifstream file("D:\\ubb_anul_III\\Procesarea datelor audio-video\\Lab2\\Encoder\\Encoder\\nt-P3.ppm");
	string data;

	getline(file, data);
	getline(file, data);
	getline(file, data, ' ');
	m = stoi(data);
	getline(file, data);
	n = stoi(data);
	file >> data;

	rgb** mat = new rgb * [n];
	for (int i = 0; i < n; i++)
	{
		mat[i] = new rgb[m];
	}

	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++) {
			file >> mat[i][j].r;
			file >> mat[i][j].g;
			file >> mat[i][j].b;
		}
	return mat;
}

void alloc_matrix() {
	y = new double* [n];
	u = new double* [n];
	v = new double* [n];
	for (int i = 0; i < n; i++)
	{
		y[i] = new double[m];
		u[i] = new double[m];
		v[i] = new double[m];
	}
}

void rgb_to_yuv(rgb** mat)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			y[i][j] = 0.299 * mat[i][j].r + 0.587 * mat[i][j].g + 0.114 * mat[i][j].b;
			u[i][j] = -0.169 * mat[i][j].r - 0.331 * mat[i][j].g + 0.499 * mat[i][j].b + 128;
			v[i][j] = 0.499 * mat[i][j].r - 0.418 * mat[i][j].g - 0.0813 * mat[i][j].b + 128;
		}
	}
}

vector<block8x8> blocky, blocku, blockv;
void blocks8x8()
{
	for (int i = 0; i < n; i = i + 8)
	{
		for (int j = 0; j < m; j = j + 8)
		{
			block8x8 aux_blocky, aux_blocku, aux_blockv;
			int ii = 0, jj;
			for (int k = i; k < i + 8; k++)
			{
				jj = 0;
				for (int l = j; l < j + 8; l++)
				{
					aux_blocky.block[ii][jj] = y[k][l];
					aux_blocku.block[ii][jj] = u[k][l];
					aux_blockv.block[ii][jj] = v[k][l];

					jj++;
				}
				ii++;
			}
			aux_blocky.type = 'Y'; aux_blocky.poz1 = i; aux_blocky.poz2 = j;
			aux_blocku.poz1 = i; aux_blocku.poz2 = j;
			aux_blockv.poz1 = i; aux_blockv.poz2 = j;
			blocky.push_back(aux_blocky);
			blocku.push_back(aux_blocku);
			blockv.push_back(aux_blockv);
		}
	}
}

vector<block8x8> Gy, Gu, Gv;
void forward_discrete_cosine_transform() {
	//for Y blocks
	for (int k = 0; k < blocky.size(); k++)
	{
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				blocky[k].block[x][y] -= 128;
		
		block8x8 aux_gy;
		aux_gy.type = 'Y';
		aux_gy.poz1 = blocky[k].poz1; aux_gy.poz2 = blocky[k].poz2;

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				double alphai, alphaj;
				if (i == 0)
					alphai = 1 / sqrt(2);
				else
					alphai = 1.0;

				if (j == 0)
					alphaj = 1 / sqrt(2);
				else
					alphaj = 1.0;

				double sum = 0;
				for (int x = 0; x < 8; x++) 
				{
					for (int y = 0; y < 8; y++)
					{
						double cosx, cosy;
						cosx = cos(((2 * (double)x + 1) * i * M_PI) / 16);
						cosy = cos(((2 * (double)y + 1) * j * M_PI) / 16);
						sum += blocky[k].block[x][y] * cosx * cosy;
					}
				}

				aux_gy.block[i][j] = (1.0 / 4) * alphai * alphaj * sum;
			}
		}
		Gy.push_back(aux_gy);
	}

	//for U blocks
	for (int k = 0; k < blocku.size(); k++)
	{
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				blocku[k].block[x][y] -= 128;

		block8x8 aux_gu;
		aux_gu.type = 'U';
		aux_gu.poz1 = blocku[k].poz1; aux_gu.poz2 = blocku[k].poz2;

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				double alphai, alphaj;
				if (i == 0)
					alphai = 1 / sqrt(2);
				else
					alphai = 1.0;

				if (j == 0)
					alphaj = 1 / sqrt(2);
				else
					alphaj = 1.0;

				double sum = 0;
				for (int x = 0; x < 8; x++)
				{
					for (int y = 0; y < 8; y++)
					{
						double cosx, cosy;
						cosx = cos(((2 * (double)x + 1) * i * M_PI) / 16);
						cosy = cos(((2 * (double)y + 1) * j * M_PI) / 16);
						sum += blocku[k].block[x][y] * cosx * cosy;
					}
				}

				aux_gu.block[i][j] = (1.0 / 4) * alphai * alphaj * sum;
			}
		}
		Gu.push_back(aux_gu);
	}

	//for V blocks
	for (int k = 0; k < blockv.size(); k++)
	{
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				blockv[k].block[x][y] -= 128;

		block8x8 aux_gv;
		aux_gv.type = 'V';
		aux_gv.poz1 = blockv[k].poz1; aux_gv.poz2 = blockv[k].poz2;

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				double alphai, alphaj;
				if (i == 0)
					alphai = 1 / sqrt(2);
				else
					alphai = 1.0;

				if (j == 0)
					alphaj = 1 / sqrt(2);
				else
					alphaj = 1.0;

				double sum = 0;
				for (int x = 0; x < 8; x++)
				{
					for (int y = 0; y < 8; y++)
					{
						double cosx, cosy;
						cosx = cos(((2 * (double) x + 1) * i * M_PI) / 16);
						cosy = cos(((2 * (double) y + 1) * j * M_PI) / 16);
						sum += blockv[k].block[x][y] * cosx * cosy;
					}
				}

				aux_gv.block[i][j] = (1.0 / 4) * alphai * alphaj * sum;
			}
		}
		Gv.push_back(aux_gv);
	}
}


void quantization() 
{
	ifstream qfile("D:\\ubb_anul_III\\Procesarea datelor audio-video\\Lab2\\Encoder\\Encoder\\quantization_matrix.txt");
	int Q[8][8];
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			qfile >> Q[i][j];

	//quantization of Y blocks 
	for (int k = 0; k < Gy.size(); k++) {
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				Gy[k].block[i][j] = 1.0*((int)(Gy[k].block[i][j] / Q[i][j]));
			}	
	}
	//quantization of U blocks 
	for (int k = 0; k < Gu.size(); k++) {
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				Gu[k].block[i][j] = 1.0*((int)(Gu[k].block[i][j] / Q[i][j]));
			}
	}
	//quantization of V blocks 
	for (int k = 0; k < Gv.size(); k++) {
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				Gv[k].block[i][j] = 1.0*((int)(Gv[k].block[i][j] / Q[i][j]));
			}
	}
}

void afisare()
{
	ofstream fout("D:\\ubb_anul_III\\Procesarea datelor audio-video\\Lab2\\Encoder\\Encoder\\afisare.txt");
	fout << m << " " << n << "\n";
	fout << Gy.size()<<"\n";
	for (int k = 0; k < Gy.size(); k++)
	{
		fout << Gy[k].poz1 << " " << Gy[k].poz2 << "\n";
		fout << Gy[k].type << "\n";
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				fout << Gy[k].block[i][j] << " ";
			}
			fout << "\n";
		}
	}
	for (int k = 0; k < Gu.size(); k++)
	{
		fout << Gu[k].poz1 << " " << Gu[k].poz2 << "\n";
		fout << Gu[k].type << "\n";
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				fout << Gu[k].block[i][j] << " ";
			}
			fout << "\n";
		}
	}
	for (int k = 0; k < Gv.size(); k++)
	{
		fout << Gv[k].poz1 << " " << Gv[k].poz2 << "\n";
		fout << Gv[k].type << "\n";
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				fout << Gv[k].block[i][j] << " ";
			}
			fout << "\n";
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
	}
	delete(y);
	delete(u);
	delete(v);
}

int main() {
	rgb** mat = read_from_file();
	alloc_matrix();
	rgb_to_yuv(mat);
	blocks8x8();
	forward_discrete_cosine_transform();
	quantization();
	afisare();
	deallocate_memory();
	return 1;
}