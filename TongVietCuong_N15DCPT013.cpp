#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<iomanip>
#include<cmath>
#include<cstring>
#include <string>
#define round(a)  int(a+0.5)
using namespace std;

class Matrix
{
	protected:
		int width;
		int height;
		int **pixels;
};
class Image : Matrix
{
	private:
		int graylevel;
	public:
		Image()
		{
			width = 0;
			height = 0;
			graylevel = 0;
		}
		
		
		~Image()
		{
			int i;
			for (i = 0; i < height; ++i)
				delete[] pixels[i];
			delete pixels;
		}
		
		
		void set_Image( char* filename)
		{
			ifstream ifs;
			char buffer[1000];
			
			char* c;
			int i = 0, j = 0;
			ifs.open(filename, ios_base::in);
			if (ifs.fail()==true) cout<<"Failed!"<<endl;
			ifs.getline(buffer, 1000, '\n');
			if(buffer[0] != 'P' || buffer[1] != '2')
			{
				cout << "Invalid Format!";
				ifs.close();
				return;
			}
			
			
			do
			{
				ifs.getline(buffer, 1000, '\n');
			}
			while(buffer[0] == '#');
			
			width = strtol(buffer, &c, 10); 
			height = atoi(c); 
			ifs.getline(buffer, 1000, '\n');
			graylevel = atoi(buffer);
			
			pixels = new int*[height];
			
			for(i = 0; i < height; ++i)	
				pixels[i] = new int[width];
			for(i = 0; i < height; ++i)
			{
				for(j = 0; j < width ; ++j)
				{
				 ifs >> pixels[i][j];
				}
			}
			ifs.close();
		}


		int get_Pixel(int x, int y)	
		{
		return this->pixels[x][y];
		}
		
		
		void out_file(int x)
		{
		ofstream ofs;
		int i, j;		
		if (x==1) ofs.open("result_negative.pgm", ios_base::out);
		if (x==2) ofs.open("result_log_transformation.pgm", ios_base::out);
		if (x==3) ofs.open("result_histogram.pgm", ios_base::out);
		if (x==4) ofs.open("result_smoothing.pgm", ios_base::out);
		if (x==5) ofs.open("result_laplacian.pgm", ios_base::out);
		if (ofs.fail()==true)
			cout<<"Failed!"<<endl;
		ofs << "P2" << endl;
		ofs << "# result.pgm" << endl;
		ofs << width << " " << height << endl;
		ofs << graylevel << endl;
		for(i = 0; i < height; ++i)
		{
			for(j = 0; j < width; ++j)
			{
				ofs << get_Pixel(i,j) << " ";
			}
			ofs << endl;
		}
		ofs.close();
		}

		int get_Width() 
		{
		return width;
		}
		
		
		int get_Height()	
		{
		return height;
		}
		
		
		int get_GrayLevel()	
		{
		return graylevel;
		}
		
		
		int **get_Pixels()	
		{
		return pixels;
		}
};

class Filter : Matrix
{
	public:
		Filter()
		{
			width = 0;
			height = 0;
		}
		
		
		~Filter()
		{
			int i;
			for (i = 0; i < height; ++i)
				delete[] pixels[i];
			delete pixels;
		}
		
		
		void negative(Image&image)
		{
			int i, j, width, height, gray;
			width = image.get_Width();
			height = image.get_Height();
			gray = image.get_GrayLevel();
			int **matrix = image.get_Pixels();
			for(i = 0; i < height; ++i)
			{
				for(j = 0; j < width; ++j)
				{
					matrix[i][j] = gray - matrix[i][j];
					//cout<< matrix[i][j];
				}
			}
		}
		
		
		void logarite(Image&image)
		{
		int i, j, width, height, gray;
		width = image.get_Width();
		height = image.get_Height();
		gray = image.get_GrayLevel();
		int **matrix = image.get_Pixels();
		int C=2;
		for(i = 0; i < height; ++i)
		{
			for(j = 0; j < width; ++j)
			{
				matrix[i][j] = round(C*log( 1 + matrix[i][j]));
				//cout<< matrix[i][j];
			}
		}
		}
		
		
		void Histogram(Image &image)
		{
		int i, j, width, height, gray;
		width = image.get_Width();
		height = image.get_Height();
		gray = image.get_GrayLevel();
		int **matrix = image.get_Pixels();
		int sumpixel;
		float p[256] = {0};
		for(i = 0; i < height; ++i)
			for(j = 0; j < width; ++j)
				++p[matrix[i][j]];
		sumpixel = width * height;
		for(i = 0; i < gray+1; ++i)
		{
			p[i] = p[i]*1.0/sumpixel;
			if(i != 0)
				p[i] += p[i-1];
		}
		for(i = 0; i < height; ++i)
			for(j = 0; j < width; ++j)
				matrix[i][j] = round(p[matrix[i][j]] * gray);
		}
		
		
		void smooth(Image &pic)
		{
			int i, j, w, h, g;
			w = pic.get_Width();
			h = pic.get_Height();
			g = pic.get_GrayLevel();
			width = w+2;
			height = h+2;
			pixels = new int*[height];
			for(i = 0; i < height; ++i)	pixels[i] = new int[width];
			int **matrix = pic.get_Pixels();
			for(i = 0; i < h; ++i)
			{
				j = 0;
				pixels[i+1][j] = matrix[i][j];		//j = 0
				for(; j < w; ++j)
				{
					pixels[i+1][j+1] = matrix[i][j];
					if(i == 0)
						pixels[i][j+1] = matrix[i][j];
					else if(i == h - 1)
						pixels[i+2][j+1] = matrix[i][j];
				}
				pixels[i+1][j+1] = matrix[i][j-1]; //j = w
			}
			pixels[0][0] = matrix[0][0];
			pixels[0][width - 1] = matrix[0][w - 1];		
			pixels[height - 1][0] = matrix[h - 1][0];
			pixels[height - 1][width - 1] = matrix[h - 1][w - 1];
			for(i = 1; i < height - 1 ; ++i)
				for(j = 1; j < width - 1 ; ++j)
					matrix[i-1][j-1] = (pixels[i][j] + pixels[i-1][j] + pixels[i+1][j] + pixels[i][j-1] + pixels[i][j+1] + pixels[i+1][j+1] + pixels[i-1][j+1] + pixels[i-1][j-1] + pixels[i+1][j-1])/9;
		}
		
		void laplacian(Image &pic)
		{
			int i, j, w, h, g;
			int filter[3][3];
			w = pic.get_Width();
			h = pic.get_Height();
			g = pic.get_GrayLevel();
			width = w+2;
			height = h+2;
			pixels = new int*[height];
			for(i = 0; i < height; ++i)	pixels[i] = new int[width];
			int **matrix = pic.get_Pixels();
			for(i = 0; i < h; ++i)
			{
				j = 0;
				pixels[i+1][j] = matrix[i][j];		//j = 0
				for(; j < w; ++j)
				{
					pixels[i+1][j+1] = matrix[i][j];
					if(i == 0)
						pixels[i][j+1] = matrix[i][j];
					else if(i == h - 1)
						pixels[i+2][j+1] = matrix[i][j];
				}
				pixels[i+1][j+1] = matrix[i][j-1]; //j = w
			}
			
			pixels[0][0] = matrix[0][0];
			pixels[0][width - 1] = matrix[0][w - 1];		
			pixels[height - 1][0] = matrix[h - 1][0];
			pixels[height - 1][width - 1] = matrix[h - 1][w - 1];
			
			filter[0][0] =  0;
			filter[0][2] =  0;
			filter[2][0] =  0;
			filter[2][2] =  0;
			filter[0][1] = -1;	
			filter[1][0] = -1;	
			filter[1][2] = -1;	
			filter[2][1] = -1;
			filter[1][1] =  4;
			
			for(i = 1; i < height - 1; ++i)
				for(j = 1; j < width - 1; ++j)
				{
					matrix[i-1][j-1] = pixels[i][j]*filter[1][1] + pixels[i-1][j]*filter[0][1] + pixels[i+1][j]*filter[2][1] + pixels[i][j+1]*filter[1][2] + 
					pixels[i][j-1]*filter[1][0] + pixels[i+1][j+1]*filter[2][2] + pixels[i-1][j+1]*filter[0][2] + pixels[i-1][j-1]*filter[0][0] + pixels[i+1][j-1]*filter[2][0];
					
					if(matrix[i-1][j-1] < 0)
						matrix[i-1][j-1] = pixels[i][j] - matrix[i-1][j-1];
					else
						matrix[i-1][j-1] += pixels[i][j];
				}
		}
};


int main()
{
	Image image;
	Filter filter;
	int key, x;
	char *filename = new char[100];
	next:
	system ("cls");
	cout << "\t  ---------MENU---------- \t " << endl;
	cout << "\t1. Choose Image           \t " << endl;
	cout << "\t2. Image negatives        \t " << endl;
	cout << "\t3. Log transformation     \t " << endl;
	cout << "\t4. Histogram equalization \t " << endl;
	cout << "\t5. Smoothing linear filter\t " << endl;
	cout << "\t6. Laplacian filter       \t " << endl;
	cout << "\t0. End                    \t " << endl;
    cout << "\t Choose: "; 
	cin >> key;
	
            switch(key)
			{
				case 1: 
				{
					system ("cls");
					cout << "\t1. mona_lisa.ascii.pgm                        " << endl;
			        cout << "\t2. balloons.ascii.pgm                         " << endl;
			        cout << "\t3. feep.ascii.pgm                             " << endl;
			        cout << "\t4. glassware_noisy.ascii.pgm                  " << endl;
			        cout << "\t5. apollonian_gasket.ascii.pgm                " << endl;
			        cout << "\t6. back                                       " << endl;
			        cout << "\t Choose: "; 
			        cin >> x;
			        switch(x)
							{
								case 1:
									filename = "mona_lisa.ascii.pgm";
									image.set_Image(filename);
									break;
								case 2:
									filename = "balloons.ascii.pgm";
									image.set_Image(filename);
									break;
								case 3:
									filename = "feep.ascii.pgm";
									image.set_Image(filename);
									break;
								case 4:
									filename = "glassware_noisy.ascii.pgm";
									image.set_Image(filename);
									break;
								case 5:
									filename = "apollonian_gasket.ascii.pgm";
									image.set_Image(filename);
									break;
								case 6:
									break;
								default:
									cout << "\tERROR!!!!" << endl;
				}
					cout << "\t===========================\t"<< endl;
					break;
				}
                case 2: 
				{
					if(image.get_GrayLevel() == 0)
					{
						cout << "\t=======================" << endl;
						cout << "\tChoose image first!!!" << endl;
						system("pause");
						break;
					}
					filter.negative(image);
					image.out_file(1);
					break;
				}
                case 3: 
				{
					if(image.get_GrayLevel() == 0)
					{
						cout << "\t=======================" << endl;
						cout << "\tChoose image first!!!" << endl;
						system("pause");
						break;
					}
					filter.logarite(image);
					image.out_file(2);
					break;
				}
                case 4: 
                {
                	if(image.get_GrayLevel() == 0)
					{
						cout << "\t=======================" << endl;
						cout << "\tChoose image first!!!" << endl;
						system("pause");
						break;
					}
					filter.Histogram(image);
					image.out_file(3);
					break;
				}
                case 5: 
				{
					if(image.get_GrayLevel() == 0)
					{
						cout << "\t=======================" << endl;
						cout << "\tChoose image first!!!" << endl;
						system("pause");
						break;
					}
					filter.smooth(image);
					image.out_file(4);
					break;
				}
				case 6: 
				{
					if(image.get_GrayLevel() == 0)
					{
						cout << "\t=======================" << endl;
						cout << "\tChoose image first!!!" << endl;
						system("pause");
						break;
					}
					filter.laplacian(image);
					image.out_file(5);
					break;
				}
				case 0:
				{
					system("cls");
					exit(0); 
					break;
				}
                default:
                    cout << "\tERROR!!!!" << endl;
            }
    char c;
    cout << "\tcontinue? (y/n)";
    cin >> c;
    if (c=='y') goto next;
	    
	system("pause");
	return 0;
}           
