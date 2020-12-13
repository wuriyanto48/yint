{
    "targets": [
        {
            "target_name": "yint",
            "cflags!": [ "-fno-exceptions" ],
            "cflags_cc!": [ "-fno-exceptions" ],
            "sources": [
                "plugin/Yint.cc", 
                "plugin/YintSocket.cc"
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")"
            ],
            'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
        }
    ]
}
