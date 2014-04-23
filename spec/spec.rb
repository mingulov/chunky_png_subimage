require 'chunky_png_subimage'
require 'chunky_png'

describe ChunkyPNGSubimage do

  before(:each) do
    @white_big = ChunkyPNG::Image.new(10, 10, ChunkyPNG::Color::WHITE)
    @white_medium = ChunkyPNG::Image.new(5, 5, ChunkyPNG::Color::WHITE)
    @black_big = ChunkyPNG::Image.new(10, 10, ChunkyPNG::Color::BLACK)
    @black_medium = ChunkyPNG::Image.new(5, 5, ChunkyPNG::Color::BLACK)
    @black_big_tr = ChunkyPNG::Image.new(5, 5, ChunkyPNG::Color::BLACK)
    @black_big_tr[1, 1] = ChunkyPNG::Color::WHITE
    @black_big_tr[3, 1] = ChunkyPNG::Color::WHITE
    @black_big_tr[1, 3] = ChunkyPNG::Color::WHITE
    @black_big_tr[3, 3] = ChunkyPNG::Color::WHITE
    @black_small_tr = ChunkyPNG::Image.new(3, 3, ChunkyPNG::Color::BLACK)
    @black_small_tr[1, 1] = 0
  end

  describe '#search_subimage' do
    #search_subimage img, subimages, single = :single_same_subimage, x = 0, y = 0, width = 0, height = 0
    
    it "white on black should not find anything" do
      ChunkyPNGSubimage.search_subimage(@black_big, @white_medium).should == []
      ChunkyPNGSubimage.search_subimage(@black_big, @white_medium, :single_same_subimage).should == []
      ChunkyPNGSubimage.search_subimage(@black_big, @white_medium, :single_everywhere).should == []
      ChunkyPNGSubimage.search_subimage(@black_big, @white_medium, :single_subimage).should == []
      ChunkyPNGSubimage.search_subimage(@black_big, @white_medium, :everything).should == []
    end

    it "default mode is :single_same_subimage" do
      ChunkyPNGSubimage.search_subimage(@black_big, @black_medium, :single_same_subimage).should == ChunkyPNGSubimage.search_subimage(@black_big, @black_medium)
      ChunkyPNGSubimage.search_subimage(@white_big, @black_medium, :single_same_subimage).should == ChunkyPNGSubimage.search_subimage(@white_big, @black_medium)
      ChunkyPNGSubimage.search_subimage(@white_big, @white_medium, :single_same_subimage).should == ChunkyPNGSubimage.search_subimage(@white_big, @white_medium)
      ChunkyPNGSubimage.search_subimage(@black_big, [@black_medium, @black_small_tr], :single_same_subimage).should == ChunkyPNGSubimage.search_subimage(@black_big, [@black_medium, @black_small_tr])
    end

  end
end
