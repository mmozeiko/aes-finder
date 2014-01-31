AES Finder
==========

Utility to find AES keys in running process memory. Works for 128, 192 and 256-bit keys.


Usage
-----

Open `aes-finder.sln` solution in Visual Studio 2013 to compile source. Alternatively use MinGW:

    g++ -O3 -march=native -s aes-finder.cpp -o aes-finder.exe

To search for keys in process with id = 123, execute following:

    aes-finder.exe -123 

To search for keys in any process with name chrome.exe, execute following:

    aes-finder.exe chrome.exe

Now you can see what kind of AES keys are used in your favorite application!

### Putty

    C:\>aes-finder.exe putty.exe
    Searching PID 2180 ...
    [0016C904] Found AES-256 encryption key: 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f
    [0016C9F4] Found AES-256 decryption key: 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f
    [00AA4540] Found AES-256 encryption key: e2855dae921dba978ffd713e89540101144de13f468fd5da8623608255387dbc
    [00AA4720] Found AES-256 decryption key: e2855dae921dba978ffd713e89540101144de13f468fd5da8623608255387dbc
    [00AA5458] Found AES-256 encryption key: 892372c26c5601a28e5dd20a64976faa219907c6c9a33a9d1ff3376609bd53f7
    [00AA5638] Found AES-256 decryption key: 892372c26c5601a28e5dd20a64976faa219907c6c9a33a9d1ff3376609bd53f7
    Done!

### Dropbox

    C:\>aes-finder.exe dropbox.exe
    Searching PID 2204 ...
    [00F5DDE8] Found AES-128 encryption key: e73c856f10e56d8ded0510e964e71b33
    ... many identical keys
    [0363A708] Found AES-128 encryption key: e73c856f10e56d8ded0510e964e71b33
    [03710A24] Found AES-128 encryption key: 0e7484bb0230b137eb582bae6ea17e09
    [03710C40] Found AES-128 encryption key: 0e7484bb0230b137eb582bae6ea17e09
    [03716388] Found AES-128 encryption key: e73c856f10e56d8ded0510e964e71b33
    ... many identical keys
    [03747888] Found AES-128 encryption key: e73c856f10e56d8ded0510e964e71b33
    [038721A8] Found AES-256 encryption key: 90e804d380d6c279de1d0373a24010ffcbccb8a177f4fffaaa007fa2b07bbe35
    [03872318] Found AES-256 decryption key: 69d292fb3bc5edb2008e2687b40321f01ce1077c0570819ef666f5079b233064
    [0388480C] Found AES-128 encryption key: 0e7484bb0230b137eb582bae6ea17e09
    [03884A28] Found AES-128 encryption key: 0e7484bb0230b137eb582bae6ea17e09
    [03892330] Found AES-256 encryption key: 8843f5b83b0bc88ffefcdc4a6fb1dbec2a2216001d23ef714e3fcad2b106fa4a
    [038E6F88] Found AES-256 decryption key: 845e5cd8a07200ed53df003b4524872e5a1b8faa931c285602ef4b091c80a8f6
    [038FD6E0] Found AES-128 encryption key: 6108ca5991b2d070ba9109ca92895f5d
    [03918310] Found AES-256 encryption key: 55a832a0756807b89d26bfeb2fd0c20affec0444ccc5f8826f1e4c0097bc4961
    [0391DD68] Found AES-256 encryption key: 0495d0bc9b05b893ada8eb36c5fbbfefab3cccd4566accbf18c1a078faae970d
    [0391DF00] Found AES-256 decryption key: fd4c481f479f9a66677adc42060689d1bb95529b217265fec7664e0fd0e990b2
    [0393AB48] Found AES-256 decryption key: 471c720862d7d1329d0a1320a5eaea4ce7a8693ac75e9c732a0c4392aeb3d21e
    [039480A0] Found AES-128 encryption key: 6108ca5991b2d070ba9109ca92895f5d
    [0394B508] Found AES-128 encryption key: 6108ca5991b2d070ba9109ca92895f5d
    [03950418] Found AES-256 encryption key: bbc3ede28857b90389452846e4b80ca5e262ef57ce918ce17c89b6598993faa4
    [039E1A24] Found AES-128 encryption key: afde7f5a3184111f877f0e8b61f85ad8
    [039E1C40] Found AES-128 encryption key: afde7f5a3184111f877f0e8b61f85ad8
    [039F5668] Found AES-256 encryption key: b48ac86c598e3051e1e00f17cb47256c0e6735a694ab3c1282ee9086d38d2647
    [039F5870] Found AES-256 decryption key: 9e12c27da800ada221a60112171ac32b7b2c4781a7d45407fc6aaff2800b67d9
    [03A0BD18] Found AES-128 encryption key: 6108ca5991b2d070ba9109ca92895f5d
    [03A0EA7C] Found AES-256 encryption key: 28ad3cba4b91556825a3f301358901416eb16253b0bc7e43be0303caa53a001e
    [03A0EB6C] Found AES-256 decryption key: 28ad3cba4b91556825a3f301358901416eb16253b0bc7e43be0303caa53a001e
    [071868B0] Found AES-256 decryption key: 7a1d726b3b7c81a65887e12296d3102ead96a3a71a100fd4c48ccbc421f3d570
    Done!
    
### Chrome

    C:\>aes-finder.exe chrome.exe
    Searching PID 1792 ...
    [08D8302C] Found AES-256 decryption key: c73159441a23df68b292a666a082418048a844813dfe8636126e875204813291
    [08D8326C] Found AES-256 encryption key: a85f43b1515c848bcb99a94f8b3ff815bcab2ff37b67954d21231c9744651f80
    [08D834AC] Found AES-256 decryption key: 22715913a08a86472a85c711fe7674180c07cc19cc0683e95dd9f0c7526387c6
    [08D8446C] Found AES-256 decryption key: 2c7d5043ae8d2800cef6b5fe82776eaaa13a1911f97f15d34172aaf4e0cf1d74
    ... lot of random keys
    [0907326C] Found AES-256 decryption key: 28e941c0801544a85c1832510e935d44a3ae096397bbdeb5eb8608d2b1fcc18f
    [090734AC] Found AES-256 encryption key: 4ffc20368848fdc739bb3420cbd5d8333ab6f478ba601309e2bab7db0427047b
    [0907392C] Found AES-256 encryption key: f857325597da770d3a4589b5a585671d33221c108aab002c30a181b79c3c99ed
    [09073B6C] Found AES-128 decryption key: d5f782a86d7f1e3c403cbe349c2844c1
    [09073DAC] Found AES-128 encryption key: cdfb40a1b13d7cfd0f7fd2687848d3de
    [09073FEC] Found AES-128 decryption key: ce3be55b440620be4aa73e33c325456f
    [0907422C] Found AES-128 encryption key: 1954a522cf12287a245d66786d78bba5
    [0907446C] Found AES-128 decryption key: 7972ca8c29805c44bacc6a70691a606d
    [090746AC] Found AES-128 encryption key: 266ffef00c92cd372d8dce1436a124d7
    [090748EC] Found AES-128 decryption key: 1afa9ef367fe19278cdd7d060b397813
    [09074B2C] Found AES-128 encryption key: b7fcd6a9915be8d562f376f8e30b34a3
    [09074D6C] Found AES-128 decryption key: 4e150e928eb9f4366c93d7d664b53587
    [090F702C] Found AES-128 encryption key: 6f3701e7a085102e9b69ffa1c5a7d52d
    [090F726C] Found AES-128 decryption key: 71a46909719d714ebe29e1fb13ba3bc1
    [090F74AC] Found AES-128 encryption key: 526d886eaa03c0a7e36918eb741ba4a9
    Searching PID 2744 ...
    Searching PID 3592 ...
    Done!

### Python + PyCrypto

    C:\>start python -c "from Crypto.Cipher import AES; a=AES.new('\x42'*24, AES.MODE_ECB); input()"
    
    C:\>aes-finder.exe python.exe
    Searching PID 264 ...
    [00B84074] Found AES-192 encryption key: 424242424242424242424242424242424242424242424242
    [00B84164] Found AES-192 decryption key: 424242424242424242424242424242424242424242424242
    Done!    

Notes
-----

* does not work on whitebox AES keys
* 32-bit binary can search in only 32-bit processes, 64-bit binary can search in 32 and 64-bit ones
* "encryption key" means that this key can be used for encryption or decryption
* "decryption key" means that most likely this key is used only for decryption

License
-------

Public Domain. Do whatever you want!
