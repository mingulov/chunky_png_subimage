require "bundler/gem_tasks"

require 'rake/extensiontask'
Rake::ExtensionTask.new('chunky_png_subimage') do |ext|
  ext.lib_dir = File.join('lib', 'chunky_png_subimage')
  #ext.config_options = '--with-cflags="-std=c99"'
end

require 'rspec/core/rake_task'

RSpec::Core::RakeTask.new(:spec) do |task|
  task.pattern = "./spec/spec.rb"
  task.rspec_opts = ['--color']
end

Rake::Task['spec'].prerequisites << :compile

task :default => [:spec]
