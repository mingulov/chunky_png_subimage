require 'chunky_png_subimage'
#require 'chunky_png'

img = ChunkyPNG::Image.new(100, 100, ChunkyPNG::Color::WHITE)
subimg = ChunkyPNG::Image.new(10, 10, ChunkyPNG::Color::WHITE)

puts ChunkyPNGSubimage::search_subimage(img, [subimg, subimg]).inspect
