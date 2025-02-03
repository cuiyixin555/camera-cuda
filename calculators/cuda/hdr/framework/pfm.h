


#ifndef INCLUDED_PFM_FILE_FORMAT
#define INCLUDED_PFM_FILE_FORMAT

#pragma once

#include "calculators/cuda/hdr/framework/rgb32f.h"
#include "calculators/cuda/hdr/framework/image.h"


namespace PFM
{
	image<float> loadR32F(const char* filename);
	void saveR32F(const char* filename, const image<float>& image);

	image<RGB32F> loadRGB32F(const char* filename);
	void saveRGB32F(const char* filename, const image<RGB32F>& image);
}

#endif  // INCLUDED_PFM_FILE_FORMAT
