{
  "includes": [ "common.gypi" ],
  "variables": {
      "module_name":"osmium",
      "module_path":"./lib/binding"
  },
  "targets": [
    {
      "target_name": "<(module_name)",
      "sources": [
        "src/apply.cpp",
        "src/buffer_wrap.cpp",
        "src/file_wrap.cpp",
        "src/filter.cpp",
        "src/handler.cpp",
        "src/location_handler_wrap.cpp",
        "src/multipolygon_collector_wrap.cpp",
        "src/multipolygon_handler_wrap.cpp",
        "src/node_osmium.cpp",
        "src/osm_area_wrap.cpp",
        "src/osm_changeset_wrap.cpp",
        "src/osm_entity_wrap.cpp",
        "src/osm_node_wrap.cpp",
        "src/osm_object_wrap.cpp",
        "src/osm_relation_wrap.cpp",
        "src/osm_way_wrap.cpp",
        "src/basic_reader_wrap.cpp",
        "src/flex_reader_wrap.cpp",
        "src/utils.cpp"
      ],
      "include_dirs": [
          "../libosmium/include/",
          "./src/",
          "<!(node -e \"require('nan')\")"
      ],
      "defines": [
        "_LARGEFILE_SOURCE",
        "_FILE_OFFSET_BITS=64",
        "OSMIUM_WITH_SPARSEHASH"
      ],
      "conditions" : [
        ["OS=='win'",
          {
            'include_dirs':[
              '<!(echo %LODEPSDIR%)/boost',
              '<!(echo %LODEPSDIR%)/sparsehash/include',
              '<!(echo %LODEPSDIR%)/bzip2/include',
              '<!(echo %LODEPSDIR%)/zlib/include',
              '<!(echo %LODEPSDIR%)/expat/include',
            ],
            "libraries": [
              "Ws2_32.lib",
              "<!(echo %LODEPSDIR%)/expat/lib/libexpat.lib",
              "<!(echo %LODEPSDIR%)/zlib/lib/zlibwapi.lib",
              "<!(echo %LODEPSDIR%)/bzip2/lib/libbz2.lib"
            ],
            "defines": [
              "NOMINMAX"
            ]
          },{
            "libraries": [
              "-lexpat",
              "-lz",
              "-lbz2"
            ]
          }
        ]
      ],
      "xcode_settings": {
        "GCC_ENABLE_CPP_RTTI": "YES",
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
        'CLANG_CXX_LIBRARY': 'libc++',
        "CLANG_CXX_LANGUAGE_STANDARD":"c++11",
        'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
        "MACOSX_DEPLOYMENT_TARGET":"10.9"
      },
      "cflags_cc!": ["-fno-rtti", "-fno-exceptions"],
      "cflags_cc" : ["-std=c++11", "-Wno-return-type"],
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [ "<(module_name)" ],
      "copies": [
        {
          "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
          "destination": "<(module_path)"
        }
      ]
    }
  ]
}
