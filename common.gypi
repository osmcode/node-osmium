{
  'target_defaults': {
      'default_configuration': 'Release',
      'msbuild_toolset':'v140',
      'configurations': {
          'Debug': {
              'cflags_cc!': ['-O3', '-Os', '-DNDEBUG'],
              'xcode_settings': {
                'OTHER_CPLUSPLUSFLAGS!':['-O3', '-Os', '-DNDEBUG'],
                'GCC_OPTIMIZATION_LEVEL': '0',
                'GCC_GENERATE_DEBUGGING_SYMBOLS': 'YES'
              }
          },
          'Release': {
              'ldflags': [
                    '-Wl,-s'
              ],
              'xcode_settings': {
                'GCC_OPTIMIZATION_LEVEL': '3',
                'GCC_GENERATE_DEBUGGING_SYMBOLS': 'NO',
                'DEAD_CODE_STRIPPING': 'YES',
                'GCC_INLINES_ARE_PRIVATE_EXTERN': 'YES'
              },
              'msvs_settings': {
                'VCCLCompilerTool': {
                  'ExceptionHandling': 1, # /EHsc
                  'RuntimeTypeInfo': 'true', # /GR
                  'RuntimeLibrary': '2', # /MD
                  "AdditionalOptions": [
                    "/MP", # compile across multiple CPUs
                    "/wd4068", # suppress unrecognized pragma warnings
                  ],
                }
              }
          }
      }
  }
}
