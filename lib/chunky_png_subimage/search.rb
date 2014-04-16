require 'chunky_png'
require 'chunky_png_subimage/chunky_png_subimage'

module ChunkyPNGSubimage
  #
  def self.search_subimage img, subimages, single = false, x = 0, y = 0, width = 0, height = 0
    many_subimages = subimages.is_a? Array
    subimages = [subimages] unless many_subimages
    params = [img.pixels, img.width, img.height, x, y, width, height, single, subimages.length]
    subimages.each do |s|
      params << s.pixels
      params << s.width
      params << s.height
    end
    res = self._search_subiamge *params
    res = res[0] unless many_subimages
    res
  end
end
