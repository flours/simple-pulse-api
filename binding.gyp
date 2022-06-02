{
  "targets": [
    {
      "target_name": "simple_pulse",
      "cflags!": [ "-fno-exceptions"],
      "cflags": [
        '-Wall',
        '-Wno-deprecated-declarations',
        '<!@(pkg-config --cflags libpulse)'
      ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ "src/pa_simple_read.cc" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'xcode_settings': {
        'OTHER_CFLAGS': [
          '-Wall',
          '-Wno-deprecated-declarations',
          '<!@(pkg-config --cflags libpulse)'
        ]
      },
      'libraries': [
        '-lpulse',
        '-lpulse-simple'
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}
