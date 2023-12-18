static int test_pca(void)
{
	std::cout << "\n===== PCA test =====\n" << std::endl;

	const int test_matrix_order = 3;
	const int test_matrix_bands = 4;

	data_t test_matrix[] =
	{
			7.,  1., 11., 11.,  7., 11.,  3.,  1.,  2.,
			26., 29., 56., 31., 52., 55., 71., 31., 54.,
			6., 15.,  8.,  8.,  6.,  9., 17., 22., 18.,
			60., 52., 20., 47., 33., 22.,  6., 44., 22.,
	};

	const data_t expected_result[] =
	{
			32.2326101,  24.0364794, -17.6831430,  19.0780463, -5.19384504, -15.5634975, -38.3042770,  16.4847549, -15.0871280,
			-5.42732988,  5.60434538, -6.26469900, -5.35748390, -6.48829601, -5.51514692,  4.67550623,  11.86020381, 6.91290033,
			2.62288678,  1.96481477, -2.57377159, -3.77179939, 4.38813899, -2.42790296, 1.93121885, -2.48748571, 0.353900251,
			0.262836354, -0.569344152, -0.580459646,  0.0167427470, 0.126110359,  0.380787913,  0.305996565,  0.436992212, -0.379662353,
	};

	data_t result[NUM_ELEMS(expected_result)];
	data_t workspace[2 * NUM_ELEMS(test_matrix)];
	pca(test_matrix_order, test_matrix_order, test_matrix_bands, test_matrix, result, workspace);

	data_t diff = cumulative_percentile_error(NUM_ELEMS(expected_result), result, expected_result);

	if (diff <= 1.0)
	{
		std::cout << "SUCCESS.\n" << std::endl;
		return 0;
	}
	else
	{
		std::cout << "FAILED: Result did not match expected values.\n"
			   "error was " << diff << "%.\n" << std::endl;
		return 1;
	}
}
