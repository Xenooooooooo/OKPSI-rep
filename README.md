# OKMPSI

This is the implementation code for **OKMPSI**: Efficient Scalable Multi-Party Private Set Intersection Using OKVS and Secret Sharing. The two party PSI part is from [VOLE-PSI: Fast OPRF and Circuit-PSI from Vector-OLE](https://eprint.iacr.org/2021/266) and [Blazing Fast PSI from Improved OKVS and Subfield VOLE](https://eprint.iacr.org/2022/320.pdf). We modify out construction using EC code from [Expand-Convolute Codes for Pseudorandom Correlation Generators from LPN](https://eprint.iacr.org/2023/882) and RB-OKVS from [Near-Optimal Oblivious Key-Value Stores for Efficient PSI, PSU and Volume-Hiding Multi-Maps](https://eprint.iacr.org/2023/903), and using BLAKE3 as hash function for better performance. This source is implemented based on [volePSI](https://github.com/Visa-Research/volepsi). For more details, please refer to this flie.

## Build

For reviewing's anonymous purpose, the source code for this implementation can be downloaded through the link: https://anonymous.4open.science/r/OKPSI-rep-6984.

The library can be built with networking support as

```shell
cd OKMPSI
python3 build.py -DVOLE_PSI_ENABLE_BOOST=ON
```
After the compiling process, the executable `frontend` can be seen in `out/build/linux/frontend`.

### Compile Options
This repository includes most of the source codes from the dependencies, and there's no need for downloading fundamental dependencies except for `boost`. We use [libsodium](https://github.com/osu-crypto/libsodium) to realize our eclipse curve functions, not [relic](https://github.com/relic-toolkit/relic).

Options can be set as `-D NAME=VALUE`. For example, `-D VOLE_PSI_NO_SYSTEM_PATH=true`. See the output of the build for default/current value. Options include :

 * `VOLE_PSI_NO_SYSTEM_PATH`, values: `true,false`.  When looking for dependencies, do not look in the system install. Instead use `CMAKE_PREFIX_PATH` and the internal dependency management.  
* `CMAKE_BUILD_TYPE`, values: `Debug,Release,RelWithDebInfo`. The build type. 
* `FETCH_AUTO`, values: `true,false`. If true, dependencies will first be searched for and if not found then automatically downloaded.
* `VOLE_PSI_ENABLE_SSE`, values: `true,false`. If true, the library will be built with SSE intrinsics support. 
* `VOLE_PSI_ENABLE_PIC`, values: `true,false`. If true, the library will be built `-fPIC` for shared library support. 
* `VOLE_PSI_ENABLE_ASAN`, values: `true,false`. If true, the library will be built ASAN enabled. 
* `VOLE_PSI_ENABLE_BOOST`, values: `true,false`. If true, the library will be built with boost networking support. This support is managed by libOTe. 
* `VOLE_PSI_ENABLE_OPENSSL`, values: `true,false`. If true,the library will be built with OpenSSL networking support. This support is managed by libOTe. If enabled, it is the responsibility of the user to install openssl to the system or to a location contained in `CMAKE_PREFIX_PATH`.
* `VOLE_PSI_ENABLE_RELIC`, values: `true,false`. If true, the library will be built relic for doing elliptic curve operations. This or sodium must be enabled. This support is managed by libOTe. 

### Installing

The library and any fetched dependencies can be installed. 
```shell
python3 build.py --install
```
or 
```shell
python3 build.py --install=install/prefix/path
```

### Running and Testing

Using the instruction in the terminal to run and test the code:

````shell
# in out/build/linux/frontend
# to check all the available parameters
./frontend
# to check correctness of each part of the code
./frontend -u
# to run OKMPSI locally, and test the performance (4 participants, 2^20 set size)
./frontend -perf -mpsi -nu 4 -id 0 -nn 20 & ./frontend -perf -mpsi -nu 4 -id 1 -nn 20 & ./frontend -perf -mpsi -nu 4 -id 2 -nn 20 & ./frontend -perf -mpsi -nu 4 -id 3 -nn 20
````

 `-nn` is for set size,  and `-nu` is for the number of the participants.

To set limit on the network environment, we use `tc` command to set our LAN and WAN setting.

1. LAN: 20 Gbit rate，0.02ms latency
   `sudo tc qdisc add dev lo root netem rate 20gbit delay 0.02ms`
2. WAN: 200 Mbit, 96ms latency
   `sudo tc qdisc add dev lo root netem rate 200Mbit delay 96ms`
3. to delete all the limits on the `lo`
   `sudo tc qdisc del dev lo root`
