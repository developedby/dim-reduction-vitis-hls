static int test_svd(void)
{
	std::cout << "\n===== svd test =====\n" << std::endl;

	const int order = 4;

	data_t input[] = {
			4.0,    -30.0,    60.0,   -35.0,
		   -30.0,   300.0,  -675.0,   420.0,
			60.0,  -675.0,  1620.0, -1050.0,
		   -35.0,   420.0, -1050.0,   700.0 };

	const data_t expected_eigenvalues[] = {
			2585.254,
			37.101,
			1.478,
			0.167,
	};

	const data_t expected_eigenvectors[] = {
			0.02919332, -0.32871206,  0.79141115, -0.51455275,
			0.17918629, -0.74191779,  0.10022814,  0.63828253,
			-0.5820757 , 0.37050219, 0.50957863, 0.51404827,
			0.79260829,  0.45192312,  0.3224164 ,  0.25216117,
	};

	data_t output_eigenvalues[NUM_ELEMS(expected_eigenvalues)];
	data_t output_eigenvectors[NUM_ELEMS(expected_eigenvectors)];
	svd_eigenvalue(order, input, output_eigenvectors, output_eigenvalues);

	const data_t vector_diff = cumulative_percentile_error(order*order, output_eigenvectors, expected_eigenvectors);
	const data_t values_diff = cumulative_percentile_error(order, output_eigenvalues, expected_eigenvalues);

#if EXTENDED_LOGGING
	std::cout << "\neigenvalues:" << std::endl;

	for (int i = 0; i < order; i++)
	{
		std::cout << output_eigenvalues[i] << ",\n";
	}

	std::cout << "\neigenvectors:" << std::endl;

	for (int i = 0; i < order * order; i++)
	{
		std::cout << output_eigenvectors[i] << ",\n";
	}
#endif

	if (vector_diff <= 1.0 && values_diff <= 1.0)
	{
		std::cout << "SUCCESS.\n" << std::endl;
		return 0;
	}
	else
	{
		std::cout << "FAILED: Result did not match expected eigenvalues and eigenvectors.\n"
			   "Eigenvector error was " << vector_diff << "%.\n"
			   "Eigenvalue error was " << values_diff << "%.\n" << std::endl;
		return 1;
	}
}

static int test_svd_streaming(void)
{
	std::cout << "\n===== svd (w/ streaming if) test  =====\n" << std::endl;

	const int order = 4;

	data_t input[] = {
			4.0,    -30.0,    60.0,   -35.0,
		   -30.0,   300.0,  -675.0,   420.0,
			60.0,  -675.0,  1620.0, -1050.0,
		   -35.0,   420.0, -1050.0,   700.0 };

	const data_t expected_eigenvalues[] = {
			2585.254,
			37.101,
			1.478,
			0.167,
	};

	const data_t expected_eigenvectors[] = {
			0.02919332, -0.32871206,  0.79141115, -0.51455275,
			0.17918629, -0.74191779,  0.10022814,  0.63828253,
			-0.5820757 , 0.37050219, 0.50957863, 0.51404827,
			0.79260829,  0.45192312,  0.3224164 ,  0.25216117,
	};

	data_t output_eigenvalues[NUM_ELEMS(expected_eigenvalues)];
	data_t output_eigenvectors[NUM_ELEMS(expected_eigenvectors)];

	hls::stream<data_t> input_stream;
	hls::stream<data_t> vector_stream;
	hls::stream<data_t> value_stream;

	for (int i = 0; i < NUM_ELEMS(input); i++)
	{
		input_stream << input[i];
	}

	svd_eigenvalue(order, input_stream, vector_stream, value_stream);

	for (int i = 0; i < NUM_ELEMS(expected_eigenvectors); i++)
	{
		vector_stream >> output_eigenvectors[i];
	}

	for (int i = 0; i < NUM_ELEMS(expected_eigenvalues); i++)
	{
		value_stream >> output_eigenvalues[i];
	}

	const data_t vector_diff = cumulative_percentile_error(order*order, output_eigenvectors, expected_eigenvectors);
	const data_t values_diff = cumulative_percentile_error(order, output_eigenvalues, expected_eigenvalues);

#if EXTENDED_LOGGING
	std::cout << "\neigenvalues:" << std::endl;

	for (int i = 0; i < order; i++)
	{
		std::cout << output_eigenvalues[i] << ",\n";
	}

	std::cout << "\neigenvectors:" << std::endl;

	for (int i = 0; i < order * order; i++)
	{
		std::cout << output_eigenvectors[i] << ",\n";
	}
#endif

	if (vector_diff <= 1.0 && values_diff <= 1.0)
	{
		std::cout << "SUCCESS.\n" << std::endl;
		return 0;
	}
	else
	{
		std::cout << "FAILED: Result did not match expected eigenvalues and eigenvectors.\n"
			   "Eigenvector error was " << vector_diff << "%.\n"
			   "Eigenvalue error was " << values_diff << "%.\n" << std::endl;
		return 1;
	}
}
