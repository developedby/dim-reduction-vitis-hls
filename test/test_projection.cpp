static int test_projection(void)
{
	std::cout << "\n===== y = A.x projection test =====\n" << std::endl;

	const data_t test_projection[] =
	{
			1, -1, 1,
			-2, 2, -2,
			3, -3, 3,
	};

	const data_t test_matrix[] =
	{
			1, 1, 1, 1,
			2, 2, 2, 2,
			3, 3, 3, 3,
	};

	const data_t expected_result[] =
	{
			2,  2,  2,  2,
			-4, -4, -4, -4,
			6,  6,  6,  6,
	};

	data_t result[NUM_ELEMS(expected_result)] = {};

	projection(2, 2, 3, test_projection, test_matrix, result);

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

static int test_projection_streaming(void)
{
	std::cout << "\n===== y = A.x projection (w/ streaming if) test =====\n" << std::endl;

	const data_t test_projection[] =
	{
			1, -1, 1,
			-2, 2, -2,
			3, -3, 3,
	};

	const data_t test_matrix[] =
	{
			1, 2, 3,
			1, 2, 3,
			1, 2, 3,
			1, 2, 3,
	};

	const data_t expected_result[] =
	{
			2, -4, 6,
			2, -4, 6,
			2, -4, 6,
			2, -4, 6,
	};

	hls::stream<data_t> projection_matrix_stream;
	hls::stream<data_t> input_stream;
	hls::stream<data_t> temp_stream;
	hls::stream<data_t> output_stream;

	data_t result[NUM_ELEMS(expected_result)] = {};

	for (int i = 0; i < NUM_ELEMS(test_projection); i++)
	{
		projection_matrix_stream << test_projection[i];
	}

	for (int i = 0; i < NUM_ELEMS(test_matrix); i++)
	{
		input_stream << test_matrix[i];
	}

	projection(2, 2, 3, projection_matrix_stream, input_stream, output_stream);

	for (int i = 0; i < NUM_ELEMS(expected_result); i++)
	{
		output_stream >> result[i];
	}

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
