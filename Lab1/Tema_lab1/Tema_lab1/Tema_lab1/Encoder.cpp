#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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

struct block4x4 {
	double block[4][4];
	char type;
	int poz1, poz2;
};

struct rgb {
	int r, g, b;
};


rgb** read_from_file()
{
	ifstream file("D:\\ubb_anul_III\\Procesarea datelor audio-video\\Lab1\\nt-P3.ppm");
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
	for (int i = 0; i < n; i=i+8)
	{
		for (int j = 0; j < m; j =j+8)
		{
			block8x8 aux_blocky, aux_blocku, aux_blockv;
			int ii=0, jj = 0;
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

vector<block4x4> bu, bv;
void blocks4x4() 
{
	for (int k = 0; k < blocku.size(); k++) {
		block4x4 aux_bu, aux_bv;
		for (int i = 0; i < 8; i += 2) {
			for (int j = 0; j < 8; j += 2) {
				double avg = 0;
				avg+=blocku.at(k).block[i][j] + blocku.at(k).block[i + 1][j] + blocku.at(k).block[i][j + 1] + blocku.at(k).block[i+1][j+1];
				avg /= 4;
				aux_bu.block[i / 2][j / 2] = avg;

				avg = 0;
				avg += blockv.at(k).block[i][j] + blockv.at(k).block[i + 1][j] + blockv.at(k).block[i][j + 1] + blockv.at(k).block[i + 1][j + 1];
				avg /= 4;
				aux_bv.block[i / 2][j / 2] = avg;
			}
		}

		aux_bu.type = 'U'; aux_bu.poz1 = blocku.at(k).poz1; aux_bu.poz2 = blocku.at(k).poz2;
		aux_bv.type = 'V'; aux_bv.poz1 = blockv.at(k).poz1; aux_bv.poz2 = blockv.at(k).poz2;

		bu.push_back(aux_bu);
		bv.push_back(aux_bv);
	}
}

void afisare()
{
	ofstream fout("D:\\ubb_anul_III\\Procesarea datelor audio-video\\Lab1\\afisare.txt");
	int i, j, k;
	fout << m << " " << n << "\n";
	fout << blocky.size()<<"\n";
	for(k=0;k<blocky.size();k++)
	{
		fout << blocky.at(k).poz1 << " " << blocky.at(k).poz2<<"\n";
		fout << blocky.at(k).type << "\n";
		for (i = 0; i < 8; i++)
		{
			for(j=0;j<8;j++){
				fout << blocky.at(k).block[i][j] << " ";
			}
			fout << "\n";
		}
	}

	for (k = 0; k < bu.size(); k++)
	{
		fout << bu.at(k).poz1 << " " << bu.at(k).poz2 << "\n";
		fout << bu.at(k).type << "\n";
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++) {
				fout << bu.at(k).block[i][j] << " ";
			}
			fout << "\n";
		}
	}

	for (k = 0; k < bv.size(); k++)
	{
		fout << bv.at(k).poz1 << " " << bv.at(k).poz2 << "\n";
		fout << bv.at(k).type << "\n";
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++) {
				fout << bv.at(k).block[i][j] << " ";
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
	blocks4x4();

	afisare();
	deallocate_memory();
	return 1;
}