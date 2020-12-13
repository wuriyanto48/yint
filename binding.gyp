{
    "targets": [
        {
            "target_name": "yintcc",
            "cflags!": [ "-fno-exceptions" ],
            "cflags_cc!": [ "-fno-exceptions" ],
            "sources": [
                "plugin/Yintcc.cc", 
                "plugin/YintSocket.cc"
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")"
            ],
            'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
        }
    ]
}
