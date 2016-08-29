#include "ModelSlice.h"

ModelSlice::ModelSlice()
	: length(0), start(0) {
}


ModelSlice::ModelSlice(unsigned int num_vertices, unsigned int first_vertex)
	: length(num_vertices), start(first_vertex) {

}