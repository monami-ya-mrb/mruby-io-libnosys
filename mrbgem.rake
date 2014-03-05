MRuby::Gem::Specification.new('mruby-io-libnosys') do |spec|
  spec.licenses = ['MIT', 'FatFs license', 'Other permissive']
  spec.authors = ['Monami-ya LLC, Japan.', 'ChaN', 'Shaun Jackman']

  spec.cc.include_paths << "#{build.root}/src"
end
