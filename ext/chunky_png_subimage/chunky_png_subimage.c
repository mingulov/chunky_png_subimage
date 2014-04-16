#include "chunky_png_subimage.h"

static inline int is_color_usable(unsigned int color)
{
	return (color & 0xff) == 0xff;
}

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

	for (y = searchY; y < searchY + searchHeight; y++)
	{
		int x = 0;
		for (x = searchX; x < searchX + searchWidth; x++)
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
		if (x != searchX + searchWidth)
			break;
	}

	return result;
}

/* convert image to int*, xfree must be used later */
static unsigned int *convert_image(const VALUE *imgPixels, const int imgWidth, const int imgHeight)
{
	unsigned int *imgReady = xmalloc(sizeof(unsigned int) * imgWidth * imgHeight);
	int i;
	for(i = 0; i < imgHeight * imgWidth; i++)
	{
		imgReady[i] = FIX2UINT(imgPixels[i]);
	}

	return imgReady;
}

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
	imgWidth = FIX2INT(argv[1]);
	imgHeight = FIX2INT(argv[2]);
	searchX = FIX2INT(argv[3]);
	searchY = FIX2INT(argv[4]);
	searchWidth = FIX2INT(argv[5]);
	searchHeight = FIX2INT(argv[6]);
	singleMode = FIX2INT(argv[7]);
	amountSubimages = FIX2INT(argv[8]);

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
		int subWidth = FIX2INT(argv[cur + 1]);
		int subHeight = FIX2INT(argv[cur + 2]);
		const unsigned int *subReady = convert_image(subPixels, subWidth, subHeight);

		VALUE r = search_single_subimage(imgReady, imgWidth, imgHeight,
				searchX, searchY, searchWidth, searchHeight,
				subReady, subWidth, subHeight, singleMode);

		xfree((void *)subReady);

		rb_ary_push(result, r);

		if ((singleMode & 1) && RARRAY_LEN(r) > 0)
			break;

		cur += 3;
	}

	xfree((void *)imgReady);

	return result;
}

void Init_chunky_png_subimage()
{
	VALUE mainModule = rb_define_module("ChunkyPNGSubimage");

	rb_define_singleton_method(mainModule, "_search_subimage", search_subimages, -1);
}
