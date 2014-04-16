# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'chunky_png_subimage/version'

Gem::Specification.new do |spec|
  spec.name          = "chunky_png_subimage"
  spec.version       = ChunkyPNGSubimage::VERSION
  spec.authors       = ["Denis Mingulov"]
  spec.email         = ["denis@mingulov.com"]
  spec.description   = %q{Support to search subimages for ChunkyPNG - fast with ruby extension}
  spec.summary       = %q{Support to search subimages for ChunkyPNG}
  spec.homepage      = "http://github.com/mingulov/chunky_png_subimage"
  spec.license       = "MIT"

  spec.required_ruby_version = '>= 1.9'

  spec.files         = `git ls-files`.split($/)
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.extensions    = spec.files.grep(%r{/extconf.rb$})
  spec.require_paths = ["lib"]

  spec.add_runtime_dependency('chunky_png', '~> 1.3')

  spec.add_development_dependency "bundler", "~> 1.3"
  spec.add_development_dependency "rake"
  spec.add_development_dependency "rspec", "~> 2.6"
  spec.add_development_dependency "rake-compiler"
end
