//First attempt at parallelizing WAVE


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "definitions.h"

//OpenMP
#include <omp.h>

typedef struct {
	int nx;   	        // número de pontos em X
	int ny;   	        // número de pontos em Y
	int nz;   	        // número de pontos em Z
	int n_time_steps;   // número de passos de tempo
	float *prev;      // Armazena solucao corrente
	float *next;      // Armazena proxima solucao temporal
	float *vel;       // Armazena a velocidade em cada ponto da malha
}Parameters;


void run_wave_propagation(float *ptr_next, float *ptr_prev, float *ptr_vel, float *coeff, Parameters *p);
void iso_3dfd_it(float *ptr_next, float *ptr_prev, float *ptr_vel, float *coeff, const int n1, const int n2, const int n3);
void write_plane_XY(float *pnext, Parameters *p, int t_step, const char* rootname);

inline float* Allocate3DVector(int nx, int ny, int nz){

	float* array = (float *)malloc(nx*ny*nz*sizeof(float));
	return array;

}

inline void Deallocate3DVector(float *array){
	free(array);
}

int linearIndexFromCoordinates(int x, int y, int z, int max_x, int max_y){

	int a = 1;
	int b = max_x + 1;
	int c = (max_x + 1) * (max_y + 1);
	int d = 0;

	return a*x + b*y + c*z + d;

}

inline int xFromLinearIndex(int linIndex, int max_x) {
	return linIndex % (max_x + 1);
}

inline int yFromLinearIndex(int linIndex, int max_x, int max_y){
	linIndex /= max_x + 1;
	return linIndex % (max_y + 1);
}

inline int zFromLinearIndex(int linIndex, int max_x, int max_y){
	return linIndex /= ( (max_x + 1) * (max_y + 1) );
}

//inicializa as "matrizes" com os valores iniciais
void initialize(float* prev, float* netx, float* vel, Parameters* p){

#pragma omp parallel for
	for (int i = 0; i < ((p->nx) * (p->ny) * (p->nz)); i++){
		prev[i] = 0.0f;
		netx[i] = 0.0f;
		vel[i]  = 2250000.0f*DT*DT;

	}

	// Pulso inicial
	float val = 1.f;
	for (int s = 5; s >= 0; s--) {

#pragma	omp parallel for collapse(1) shared(val)
		for (int k = p->nz / 2 - s; k<p->nz / 2 + s; k++) {
			for (int j = p->ny / 2 - s; j<p->ny / 2 + s; j++) {
				for (int i = p->nx / 2 - s; i<p->nx / 2 + s; i++) {
					prev[linearIndexFromCoordinates(i, j, k, p->nx - 1, p->ny - 1)] = val;
				}
			}
		}
		val *= 10;
	}	

}


int main(int argc, char** argv)
{
	
	omp_set_num_threads(4);

	// Defaults
	// Malha com 256 x 256 x 256 pontos.
	Parameters p;
	p.nx = 256;
	p.ny = 256;
	p.nz = 256;
	p.n_time_steps = 200;

	// Make sure nreps is rouded up to next even number (to support swap)
	p.n_time_steps = ((p.n_time_steps + 1) / 2) * 2;

#if (HALF_LENGTH == 4)
	float coeff[HALF_LENGTH + 1] = {
		-2.847222222,
		+1.6,
		-0.2,
		+2.53968e-2,
		-1.785714e-3 };
#elif (HALF_LENGTH == 8)
	float coeff[HALF_LENGTH + 1] = {
		-3.0548446,
		+1.7777778,
		-3.1111111e-1,
		+7.572087e-2,
		-1.76767677e-2,
		+3.480962e-3,
		-5.180005e-4,
		+5.074287e-5,
		-2.42812e-6 };
#else
#  error "HALF_LENGTH not implemented"
#endif

	//Apply the DX DY and DZ to coefficients
	coeff[0] = (3.0f*coeff[0]) / (DXYZ*DXYZ);
	for (int i = 1; i <= HALF_LENGTH; i++) {
		coeff[i] = coeff[i] / (DXYZ*DXYZ);
	}

	// Arrays de Dados
	p.prev = NULL, p.next = NULL, p.vel = NULL;

	// Alocando dados
	size_t nsize = p.nx * p.ny * p.nz;
	size_t nbytes = nsize*sizeof(float);

	printf("Alocando prev, next e vel: total %g Mbytes\n", (3.0*(nbytes)) / (1024 * 1024)); fflush(NULL);

	p.prev = Allocate3DVector(p.nx, p.ny, p.nz);

	p.next = Allocate3DVector(p.nx, p.ny, p.nz);

	p.vel  = Allocate3DVector(p.nx, p.ny, p.nz);


	if (p.prev == NULL || p.next == NULL || p.vel == NULL) {
		printf("Não foi possivel alocar memoria: prev=%p next=%p vel=%p\n", p.prev, p.next, p.vel);
		printf("TESTE FALHOU!\n"); fflush(NULL);
		exit(-1);
	}

  printf("Inicializando dados\n");
	initialize(p.prev, p.next, p.vel, &p);

  // Escreve o pulso inicial
  write_plane_XY(p.prev, &p, 0, "wave");

  printf("Propagando a onda...\n");
  // Calcula a propagacão em um determinado intervalo de tempo
  run_wave_propagation(p.next, p.prev, p.vel, coeff, &p);

  printf("Finalizando...\n");
  // desaloca a memoria
	Deallocate3DVector(p.prev);
	Deallocate3DVector(p.next);
	Deallocate3DVector(p.vel);
	return 0;

}



void run_wave_propagation(float *ptr_next, float *ptr_prev, float *ptr_vel, float *coeff, Parameters *p)
{



	for (int it = 0; it<p->n_time_steps; it += 2) {

		iso_3dfd_it(ptr_next, ptr_prev, ptr_vel, coeff, p->nx, p->ny, p->nz);

		// Swap previous & next between iterations
		iso_3dfd_it(ptr_prev, ptr_next, ptr_vel, coeff,p->nx, p->ny, p->nz);


		if( it != 0 && it % 10 == 0)
			 write_plane_XY(ptr_prev, p, it, "wave");

	} // time loop

}


void iso_3dfd_it(float *ptr_next, float *ptr_prev, float *ptr_vel, float *coeff, const int n1, const int n2, const int n3)
{

#pragma omp parallel for collapse(1) schedule(dynamic)
	for (int k = HALF_LENGTH; k < (n3 - HALF_LENGTH); k++) {
	   for (int j = HALF_LENGTH; j < (n2 - HALF_LENGTH); j++) {
			for (int i = HALF_LENGTH; i < (n1 - HALF_LENGTH); i++)
			{

				float value = 0.0;
				value += ptr_prev[linearIndexFromCoordinates(i, j, k, n1-1, n2-1)] * coeff[0];
				
				for (int ir = 1; ir <= HALF_LENGTH; ir++) {

					value += coeff[ir] * (ptr_prev[linearIndexFromCoordinates(i+ir, j, k, n1-1, n2-1)] + ptr_prev[linearIndexFromCoordinates(i-ir, j, k, n1-1, n2-1)]);        // horizontal
					
					value += coeff[ir] * (ptr_prev[linearIndexFromCoordinates(i, j+ir, k, n1-1, n2-1)] + ptr_prev[linearIndexFromCoordinates(i, j-ir, k, n1-1, n2-1)]);        // vertical
					
					value += coeff[ir] * (ptr_prev[linearIndexFromCoordinates(i, j, k+ir, n1-1, n2-1)] + ptr_prev[linearIndexFromCoordinates(i, j, k-ir, n1-1, n2-1)]); // in front / behind
					
				}
				ptr_next[linearIndexFromCoordinates(i, j, k, n1-1, n2-1)] = 2.0f* ptr_prev[linearIndexFromCoordinates(i, j, k, n1-1, n2-1)] - ptr_next[linearIndexFromCoordinates(i, j, k, n1-1, n2-1)] + value*ptr_vel[linearIndexFromCoordinates(i, j, k, n1-1, n2-1)];
			
			}
		}
	}
}


void write_plane_XY(float *r, Parameters *p, int t_step, const char* rootname)
{
	 char fname[80];
	 sprintf(fname,"%s_%03d.dat", rootname, t_step);

	 int iz = p->nz/2;

	 FILE* fout = fopen(fname,"w");

	 for (int i = 0; i< p->nx; i++) {
 	   for (int j = 0; j<p->ny; j++) {
			 fprintf(fout, "%8.8f %8.8f %8.8f\n", i*DXYZ, j*DXYZ, r[linearIndexFromCoordinates(i, j, iz, (p->nx) - 1, (p->ny) - 1)]);
		 }
	 }

	 fclose(fout);


	 sprintf(fname,"%s_%03d.plot",rootname, t_step);
	 fout = fopen(fname,"w");
	 fprintf(fout, "set terminal png\n");
	 fprintf(fout, "set output \'%s_%03d.png\'\n", rootname, t_step);
	 fprintf(fout, "set xlabel \'x\'\n");
	 fprintf(fout, "set xlabel \'y\'\n");
	 fprintf(fout, "set pm3d map\n");
	 fprintf(fout, "set palette gray\n");
	 fprintf(fout, "set dgrid3d 100,100\n");
	 fprintf(fout, "splot \'%s_%03d.dat\' u 1:2:3 t\"\"\n", rootname, t_step);
	 fclose(fout);



}
