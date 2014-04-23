# == ChunkyPNGSubimage
#
# Search subimages inside image - fast as ruby C extension is used.
#
# # Synopsys
#
# require 'chunky_png'
# require 'chunky_png_subimage'
#
#  

require 'chunky_png_subimage/chunky_png_subimage'

module ChunkyPNGSubimage

  # Search subimages on the image
  # 
  # ==== Attributes
  #
  # * +img+ - Image where search for subimages will be done
  # * +subimages+ - Subimage or an array of subimages. Just fully opaque pixels are used from any subimage.
  # * +single+ - 'single mode', when to stop if an subimage is found, possible values:
  #   :single_everywhere - fully stop after 1st subimage is found
  #   :single_same_subimage - search maximum 1 subimage position, for every provided subimage
  #   :single_subimage - stop after 1st subimage is found, but find every possible position for that subimage
  #   anything else - search for every possible subimage multiple times
  # * +x+ - if find subimages inside some region of image - offset x (0 by default)
  # * +y+ - if find subimages inside some region of image - offset y (0 by default)
  # * +width+ - if find subimages inside some region of image - width of that region (full image by default)
  # * +height+ - if find subimages inside some region of image - height of that region (full image by default)
  #
  # ==== Examples
  #
  # 
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
