#ifndef SHARED_MATRICES
#define SHARED_MATRICES

cbuffer shared_matrices : register(b0)
{
	matrix <float, 4, 4> view;
	matrix <float, 4, 4> view_inverse;
	matrix <float, 4, 4> view_projection;
	matrix <float, 4, 4> view_projection_inv_trans;
	matrix <float, 4, 4> orientation_projection;
};

cbuffer personal_matrices : register(b1)
{
	matrix <float, 4, 4> model;
	matrix <float, 4, 4> model_inv_trans;
};

float3 get_camera_location() {
	return float3(view_inverse[0][3], view_inverse[1][3], view_inverse[2][3]);
}

#endif // SHARED_MATRICES