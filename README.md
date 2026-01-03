# KiCAS2-Numeric

The goal of this repo is to provide utility functions for interacting with numbers used by the KiCAS2 system. Functionality includes:

* Parse rational numbers from the string format `['0'-'9']* ('.' ['0'-'9']*)? ('e' ('+'|'-')? ['0'-'9']*)?`
  * E.g. typicals numbers like `1.25`, `2.998e8`, `1e-8` are parsed to a rational representation.
* Append numbers to the end of a string
* Miscellaneous word-sized mathematical operations, which are useful if they outperform Flint in benchmarks
* Debug allocation tracking to ensure all GMP/Flint allocated memory is freed

A secondary goal is to work through an example of how to dynamically link against the LGPL-licensed libraries GMP and Flint while using CI against various targets. Dynamic linking is important for digital rights compliance given the terms of the LGPL.

[![C++ Tests](https://github.com/JohnDTill/KiCAS-Number/actions/workflows/cpp_integration_tests.yml/badge.svg)](https://github.com/JohnDTill/KiCAS-Number/actions/workflows/cpp_integration_tests.yml)

## License

This example repo is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```[ascii-art]
              ____        J. Till
             /    \
            /   _  \
           |   / \  \
           \   \_/  |
           /       /
          /   /\__/
         /  _\
        /  /   %%%%   ×××    ∂∂∂∂
       /   \  %%     ×   ×  ∂∂
      /  /\/  %      ×××××  ∂∂∂∂∂
     /   \    %%     ×   ×     ∂∂
     \___/     %%%%  ×   ×  ∂∂∂∂
```
