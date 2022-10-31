# geoCFD

Process geometry for CFD simulation - remove internal faces between adjacent buildings.

It's a cross-platform project (currently tested on `x64-windows10` platform, see [geocfd-Ubuntu](https://github.com/SEUZFY/geocfd-Ubuntu) for the basic setting up on 
`wsl-ubuntu` platform). 

- support for all `LoD` levels in `cityjson`(lod 1.2, lod 1.3, lod 2.2).

- support `multithreading` process.

- support exporting as `.json` file or `.off` file.

- visualisation:

	[ninja](https://ninja.cityjson.org/)

- validate the result file via: 

	- `val3dity`  - [validate](http://geovalidation.bk.tudelft.nl/val3dity/) the geometry
  
  	- `validator` - [validate](https://validator.cityjson.org/) the `cityjson` file

## Usage

Compile and build it, enter into the `out\build\x64-Release` folder (on windows for example) then open the console (e.g. Windows PowerShell)

```console
usage: geocfd --dataset=string --adjacency=string --path_result=string [options] ...
```
If you type `--help`, the usage infomation will be printed:
```console
usage: geocfd --dataset=string --adjacency=string --path_result=string [options] ...
options:
  -d, --dataset               dataset (.json) (string)
  -a, --adjacency             adjacency file (.txt) (string)
  -p, --path_result           where the results will be saved (string)
  -l, --lod                   lod level (double [=2.2])
  -m, --minkowski             minkowski value (double [=0.01])
  -e, --target edge length    target edge length for remeshing (double [=3])
      --remesh                activate remeshing processing (warning: time consuming)
      --multi                 activate multi threading process
      --json                  output as .json file format
      --off                   output as .off file format
      --all                   adjacency file contains all adjacent blocks
      --help                  print this message
```
**Note**

for `all adjacency` mode, multi threading should not be enabled (if enabled from the console, it will be switched off). 

The reason is multi threading didn't work as desired when reading all adjacencies.

**example 1 - read in one adjacency file**:
```console
.\geocfd -d 3dbag_v210908_fd2cee53_5907.json -a adjacency5.txt -p D:\SP\geoCFD\data --multi --off

====== this is: D:\SP\geoCFD\out\build\x64-Release\geoCFD.exe ======
=> source file                   D:\SP\geoCFD\data\3dbag_v210908_fd2cee53_5907.json
=> adjacency                     adjacency5.txt
=> all adjacency tag             false
=> lod level                     2.2
=> minkowksi parameter           0.01
=> enable remeshing              false
=> target edge length            3
=> enable multi threading        true
=> output file folder            D:\SP\geoCFD\data
=> output file format            .off
...
```

**example 2 - read in all adjacencies file**:
please be sure to pass `--all` flag when inputting all adjacencies file
```console
 .\geocfd -d 3dbag_v210908_fd2cee53_5907.json -a adjacency5.txt -p D:\SP\geoCFD\data --all --off

 ====== this is: D:\SP\geoCFD\out\build\x64-Release\geoCFD.exe ======
=> source file                   D:\SP\geoCFD\data\3dbag_v210908_fd2cee53_5907.json
=> adjacency                     adjacencies.txt
=> all adjacency tag             true
=> lod level                     2.2
=> minkowksi parameter           0.01
=> enable remeshing              false
=> target edge length            3
=> enable multi threading        false
=> output file folder            D:\SP\geoCFD\data
=> output file format            .off
```
### Note

* if the program does not exit, you may need to re-open your console again and re-run it. (for example, dataset_2).

    This may be due to the complex geometry of the buildings in `dataset_2`, one of the buildings contain holes.

* the `minkowski param` is set to `0.01` by default.

	The param can be altered, but proceed with caution, too small minkowski param may not fill the holes of the building in `dataset_2`.

## Prerequisite

[CGAL](https://www.cgal.org/) - The version should be above `5.0` since we can use the `header-only`, which means we don't have to manually compile `CGAL`.

install `CGAL` via [vcpkg](https://vcpkg.io/en/index.html):

check this -> 

[install vcpkg](https://www.youtube.com/watch?v=b7SdgK7Y510)

[Download CGAL for Windows](https://www.cgal.org/download/windows.html)

## Compile info

C++ Standard: `C++ 11`

Compiler: `MSVC`

Generator: `Ninja`

Commands:
```console
"..\MICROSOFTVISUALSTUDIO\2019\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\CMake\bin\cmake.exe"  
-G "Ninja"  
-DCMAKE_BUILD_TYPE:STRING="RelWithDebInfo" 
-DCMAKE_INSTALL_PREFIX:PATH="..\geoCFD\out\install\x64-Release" 
-DCMAKE_C_COMPILER:FILEPATH="../MicrosoftVisualStudio/2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe" 
-DCMAKE_CXX_COMPILER:FILEPATH="../MicrosoftVisualStudio/2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe"  
-DCMAKE_MAKE_PROGRAM="..\MICROSOFTVISUALSTUDIO\2019\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\Ninja\ninja.exe" 
-DCMAKE_TOOLCHAIN_FILE="../dev/vcpkg/scripts/buildsystems/vcpkg.cmake" 
```
The commands are the information of compiler and generator(for example, the file path of `cl.exe` and `ninja.exe`), which should
be generated by your IDE automatically.

## Other platforms

If you use other platforms (such as `Linux` or `MacOS`), you can refer to `CMakeLists.txt` file and use it to build a `CMake` project using `src`, `include` and `data` folder.

## Attention
- if the `input adjacency file` contains multiple adjacent blocks, be sure to add `--all` flag, otherwise geoCFD may exit with unkown errors.
- currently `multi threading` doesn't work with the input of multiple adjacent blocks, thus even the flag `--multi` is specified, geoCFD will not enable multi threading process.
- there is one possibility that `minkowski sum` will be in executing status for unkown time, if so restart geoCFD.
- `remeshing` is sort of `beta` version, it should be warned that `remeshing` will be time-consuming, thus it is not recommended to activate.
- it may take time for multiple adjacent blocks.

