# BAEC-PSI

## 2PSI

本项目中的两方PSI框架来源于[VOLE-PSI: Fast OPRF and Circuit-PSI from Vector-OLE](https://eprint.iacr.org/2021/266)和[Blazing Fast PSI from Improved OKVS and Subfield VOLE](misc/blazingFastPSI.pdf)两篇论文，结合论文[Expand-Convolute Codes for Pseudorandom Correlation Generators from LPN](https://eprint.iacr.org/2023/882)中的Expand-Convolute码与[Near-Optimal Oblivious Key-Value Stores for Efficient PSI, PSU and Volume-Hiding Multi-Maps](https://eprint.iacr.org/2023/903)中的RB-OKVS进行改进，并使用了BLAKE3作为更高效的哈希函数。本项目在开源代码[volePSI](https://github.com/Visa-Research/volepsi)的基础上进行修改。关于技术细节和代码的修改细节，请查看[技术细节说明文档](misc/技术细节.md)。

### Build

代码编译过程如下，依赖库在`out`文件夹下进行编译和安装，在Ubuntu 22.04，g++ 11.3.0和cmake 3.22.1下已通过测试。
```
git clone https://github.com/lx-1234/BAEC-2psi
cd BAEC-2psi
python3 build.py -DVOLE_PSI_ENABLE_BOOST=ON
```

编译完成之后，可以在`out/build/linux/frontend`下看到可执行文件`frontend`。

##### 编译选项

本仓库已包含大部分依赖库的源代码，基础依赖库除了`boost`外无需再下载。特别地，椭圆曲线库默认使用[libsodium](https://github.com/osu-crypto/libsodium)而不是[relic](https://github.com/relic-toolkit/relic)。

编译选项可以使用命令`-D NAME=VALUE`进行设置。例如，`-D VOLE_PSI_NO_SYSTEM_PATH=true`。通过build可以看到各个选项的默认值或者现在的值。一些其它可选的编译选项包括：
 * `VOLE_PSI_NO_SYSTEM_PATH`, values: `true,false`.  When looking for dependencies, do not look in the system install. Instead use `CMAKE_PREFIX_PATH` and the internal dependency management.  
* `CMAKE_BUILD_TYPE`, values: `Debug,Release,RelWithDebInfo`. The build type. 
* `FETCH_AUTO`, values: `true,false`. If true, dependencies will first be searched for and if not found then automatically downloaded.
* `VOLE_PSI_ENABLE_SSE`, values: `true,false`. If true, the library will be built with SSE intrinsics support. 
* `VOLE_PSI_ENABLE_PIC`, values: `true,false`. If true, the library will be built `-fPIC` for shared library support. 
* `VOLE_PSI_ENABLE_ASAN`, values: `true,false`. If true, the library will be built ASAN enabled. 
* `VOLE_PSI_ENABLE_BOOST`, values: `true,false`. If true, the library will be built with boost networking support. This support is managed by libOTe. 
* `VOLE_PSI_ENABLE_OPENSSL`, values: `true,false`. If true,the library will be built with OpenSSL networking support. This support is managed by libOTe. If enabled, it is the responsibility of the user to install openssl to the system or to a location contained in `CMAKE_PREFIX_PATH`.
* `VOLE_PSI_ENABLE_RELIC`, values: `true,false`. If true, the library will be built relic for doing elliptic curve operations. This or sodium must be enabled. This support is managed by libOTe. 


### 安装

本项目和相关的依赖库可以使用以下命令进行安装。
```bash
python3 build.py --install
# or
python3 build.py --install=install/prefix/path
```

### 链接

本项目可以通过cmake进行链接
```cmake
find_package(volepsi REQUIRED)
target_link_libraries(myProject visa::volepsi)
```
为了确保cmake能够找到volepsi，你可以安装volepsi或者在编译时通过编译选项指定volepsi的路径`-D CMAKE_PREFIX_PATH=path/to/volepsi`，或者在cmake种使用`HINTS`提供volepsi的路径，即`find_package(volepsi HINTS path/to/volepsi)`。

### 运行

要运行项目代码，请使用以下命令
```bash
# in BAEC-2psi/out/build/linux/frontend
# 查看所有可选参数
./frontend
# 对各个模块正确性进行测试
./frontend -u
# 本地运行2PSI，进行效率测试
./frontend -perf -psi -v -nn 20 -nt 4
# 通过csv文件读取输入，进行2PSI
./frontend -in senderInput.txt -r 0 -nt 4 & ./frontend -in recverInput.txt -r 1 -nt 4
```

要使用通信量更低的`RB-OKVS`，只需加上参数`-useRB`，如
```bash
# in BAEC-2psi/out/build/linux/frontend
./frontend -perf -psi -v -nn 20 -nt 4 -useRB
```

测试数据生成
```bash
# in BAEC-2psi
python3 data_generation_in_csv.py
```
通过修改`data_number`和`intersection_card`可以分别修改测试集合大小和设定交集大小。