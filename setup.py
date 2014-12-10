from distutils.core import setup, Extension

module1 = Extension('tx',
                    define_macros = [('MAJOR_VERSION', '0'),
                                     ('MINOR_VERSION', '1')],
                    include_dirs = ['/usr/local/include'],
                    libraries = ['tx'],
                    library_dirs = ['/usr/local/lib'],
                    extra_compile_args = ['-C++'],
                    sources = ['tx.cpp'])

setup(name = 'tx-python',
      version = '0.1',
      description = 'Python interface to tx',
      author = 'MURAWAKI Yugo',
      author_email = 'murawaki@ait.kyushu-u.ac.jp',
      url = 'http://www.cl.ait.kyushu-u.ac.jp/~murawaki/',
      long_description = '''
Python interface to tx.
''',
       ext_modules = [module1])
