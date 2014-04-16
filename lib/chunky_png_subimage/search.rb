require 'chunky_png'
require 'chunky_png_subimage/chunky_png_subimage'

module ChunkyPNGSubimage
  #
  def self.search_subimage img, subimages, single = :single_same_subimage, x = 0, y = 0, width = 0, height = 0
    many_subimages = subimages.is_a? Array
    subimages = [subimages] unless many_subimages
    case single
    when :single_everywhere
      single = 3
    when :single_same_subimage # just 1 result is needed per subimage
      single = 2
    when :single_subimage # single image to find
      single = 1
    else
      single = 0
    end
    params = [img.pixels, img.width, img.height, x, y, width, height, single, subimages.length]
    subimages.each do |s|
      params << s.pixels
      params << s.width
      params << s.height
    end
    res = self._search_subimage *params
    res = res[0] if (!many_subimages && res.is_a?(Array))
    res
  end
end
