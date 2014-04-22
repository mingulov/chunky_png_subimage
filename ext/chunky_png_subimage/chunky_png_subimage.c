/** Ruby C extension 'chunky_png_subimage', source
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Denis Mingulov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "chunky_png_subimage.h"

/** Check that the color from subimage is usable to search
 * @param color Color to be checked
 * @return 0 whether color is not usable, non-0 otherwise
 */
static inline int is_color_usable(unsigned int color)
{
	/* currently - fully opaque points are used only */
	return (color & 0xff) == 0xff;
}

/** Search a single subimage inside image
 * @param img Pointer to uint colors for image (must be imgWidth * imgHeight)
 * @param imgWidth Image width
 * @param imgHeight Image height
 * @param searchX Offset X of 1st possible point for subimage
 * @param searchY Offset Y of 1st possible point for subimage
 * @param searchWidth Width for the search region
 * @param searchHeight Height for the search region
 * @param sub Pointer to uint colors for subimage (must be subWidth * subHeight)
 * @param subWidth Subimage width
 * @param subHeight Subimage height
 * @param singleMode 0 whether search every image, non-0 - to stop after 1st found
 * @return Ruby array with [x, y] entries - found positions of subimage
 */
static VALUE search_single_subimage(const unsigned int *img, const int imgWidth, const int imgHeight,
		const int searchX, const int searchY, const int searchWidth, const int searchHeight,
		const unsigned int *sub, const int subWidth, const int subHeight, int singleMode)
{
	VALUE result = rb_ary_new();

	int s1y = 0;
	int s1x = 0;
	unsigned int c;
	int y;

	/* search 1st point usable to do the main search */
	for (s1y = 0; s1y < subHeight; s1y++)
	{
		for (s1x = 0; s1x < subWidth; s1x++)
		{
			if (is_color_usable(sub[s1y * subWidth + s1x]))
				break;
		}
		if (s1x != subWidth)
			break;
	}

	if (s1y == subHeight)
		rb_raise(rb_eStandardError, "Empty subimage is provided, no usable pixels");

	c = sub[s1y * subWidth + s1x];

	for (y = searchY; y < searchY + searchHeight - subHeight; y++)
	{
		int x = 0;
		for (x = searchX; x < searchX + searchWidth - subWidth; x++)
		{
			if (img[(y + s1y) * imgWidth + x + s1x] == c)
			{
				/* found 1st possible point, do the full check */
				int sy = 0;
				for (sy = s1y; sy < subHeight; sy++)
				{
					int sx = 0;
					for (sx = (sy == s1y) ? s1x : 0; sx < subWidth; sx++)
					{
						const unsigned int sc = sub[sy * subWidth + sx];
						if (is_color_usable(sc) && sc != img[(y + sy) * imgWidth + x + sx])
							break;
					}
					if (sx != subWidth)
						break;
				}

				if (sy == subHeight)
				{
					/* subimage is found */
					VALUE coord = rb_ary_new();
					rb_ary_push(coord, INT2FIX(x));
					rb_ary_push(coord, INT2FIX(y));
					rb_ary_push(result, coord);

					if (singleMode & 2)
						break;
				}
			}
		}
		/* if 'break' has been used */
		if (x != searchX + searchWidth - subWidth)
			break;
	}

	return result;
}

/** convert image with Ruby values to unsigned int*, xfree must be used later
 * @param imgPixels Pointer to 1st element of Ruby array with data points (should be imgWidth * imgHeight)
 * @param imgWidth Width of the image
 * @param imgHeight Height of the image
 * @return Pointer to unsigned int values, xfree must be used later
 */
static unsigned int *convert_image(const VALUE *imgPixels, const int imgWidth, const int imgHeight)
{
	unsigned int *imgReady = xmalloc(sizeof(unsigned int) * imgWidth * imgHeight);
	int i;
	for(i = 0; i < imgHeight * imgWidth; i++)
	{
		imgReady[i] = NUM2UINT(imgPixels[i]);
	}

	return imgReady;
}

/** Exportable function to ruby, to search subimages in the image
 * @param argc Amount of elements in argv
 * @param argv Parameters:
 *             img array of pixels, img width, img height,
 *             search offset x, search offset y, search width, search height,
 *             singleMode (bit 1 - for subimage search, bit 0 - to stop after 1st found subimage),
 *             amount of subimages,
 *             for every subimage:
 *                 subimage array of pixels, subimage width, subimage height
 * @param self Object
 * @return Array of found subimages' positions
 */
VALUE search_subimages(int argc, VALUE *argv, VALUE self)
{
	/*
	params = [img.pixels, img.width, img.height, x, y, width, height, single, subimages.length]
	subimages.each do |s|
		params << s.pixels
		params << s.width
		params << s.height
	end
	 */

	VALUE result = rb_ary_new();
	const VALUE *imgPixels;
	int imgWidth;
	int imgHeight;
	int searchX;
	int searchY;
	int searchWidth;
	int searchHeight;
	int singleMode;
	int amountSubimages;
	const unsigned int *imgReady;
	int cur = 9;
	int i;

	if (argc < 12 || (argc - 9) % 3 != 0)
		rb_raise(rb_eArgError, "wrong number of arguments");

	imgPixels = RARRAY_PTR(argv[0]);
	imgWidth = NUM2INT(argv[1]);
	imgHeight = NUM2INT(argv[2]);
	searchX = NUM2INT(argv[3]);
	searchY = NUM2INT(argv[4]);
	searchWidth = NUM2INT(argv[5]);
	searchHeight = NUM2INT(argv[6]);
	singleMode = NUM2INT(argv[7]);
	amountSubimages = NUM2INT(argv[8]);

	/* some parameters checks */
	if (searchX < 0)
		searchX = 0;
	if (searchY < 0)
		searchY = 0;
	if (searchX >= imgWidth)
		searchX = imgWidth - 1;
	if (searchY >= imgHeight)
		searchY = imgHeight - 1;
	if (searchWidth <= 0)
		searchWidth = imgWidth;
	if (searchHeight <= 0)
		searchHeight = imgHeight;
	if (searchX + searchWidth >= imgWidth)
		searchWidth = imgWidth - searchX;
	if (searchY + searchHeight >= imgHeight)
		searchHeight = imgHeight - searchY;

	/* prepare image */
	imgReady = convert_image(imgPixels, imgWidth, imgHeight);

	for (i = 0; i < amountSubimages; i++)
	{
		/* cur++ inside RARRAY_PTR might not be used as it is used in the macro multiple times */
		const VALUE *subPixels = RARRAY_PTR(argv[cur]);
		int subWidth = NUM2INT(argv[cur + 1]);
		int subHeight = NUM2INT(argv[cur + 2]);
		const unsigned int *subReady = convert_image(subPixels, subWidth, subHeight);

		VALUE r = search_single_subimage(imgReady, imgWidth, imgHeight,
				searchX, searchY, searchWidth, searchHeight,
				subReady, subWidth, subHeight, singleMode);

		xfree((void *)subReady);

		rb_ary_push(result, r);

		/* stop if the proper single mode and result is found */
		if ((singleMode & 1) && RARRAY_LEN(r) > 0)
			break;

		cur += 3;
	}

	xfree((void *)imgReady);

	return result;
}

/** Entry point for an initialization */
void Init_chunky_png_subimage()
{
	VALUE mainModule = rb_define_module("ChunkyPNGSubimage");

	rb_define_singleton_method(mainModule, "_search_subimage", search_subimages, -1);
}
