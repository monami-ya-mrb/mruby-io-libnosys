MRuby::Gem::Specification.new('mruby-io-libnosys') do |spec|
  spec.license = 'MIT and others. See also copyright notice on source code.'
  spec.authors = 'Monami-ya LLC, Japan.'

  spec.cc.include_paths << "#{build.root}/src"
end
