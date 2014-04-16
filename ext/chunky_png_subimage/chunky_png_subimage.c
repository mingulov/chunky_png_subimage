#include "chunky_png_subimage.h"

VALUE search_subimage(VALUE self, VALUE args) {
	return Qnil;
}

void Init_chunky_png_subimage()
{
	VALUE mainModule = rb_define_module("ChunkyPNGSubimage");

	rb_define_singleton_method(mainModule, "_search_subimage", search_subimage, -2);
}
