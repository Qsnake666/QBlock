QT       += core gui
QT       += opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14
QT     += openglwidgets
QT     += core gui opengl

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ioStyle.cpp \
    main.cpp \
    mainwindow.cpp \
    PointCloud.cpp \
    openglwidget.cpp \
    signalscollect.cpp

HEADERS += \
    ioStyle.h \
    mainwindow.h \
    PointCloud.h \
    Octree.cuh \
    Utility.cuh \
    Visualization.cuh \
    openglwidget.h \
    signalscollect.h

FORMS += \
    mainwindow.ui

#openGL
LIBS += -lOpengl32

#openCV 从未使用
#MSVC
#INCLUDEPATH += d:\opencv\build\include\
#INCLUDEPATH += d:\opencv\build\include\opencv2
#LIBS += -Ld:/opencv/build/x64/vc15/lib
#MinGW
#INCLUDEPATH +=D:\opencv\opencv_build\include
#LIBS+=D:\opencv\opencv_build\lib\libopencv_*.a

CONFIG(debug,debug|release) {
    LIBS += -Ld:/opencv/build/x64/vc15/lib \
        -lopencv_world454d
    } else {
    LIBS += -Ld:/opencv/build/x64/vc15/lib \
        -lopencv_world454
    }

#CUDA
CUDA_SOURCES += \
    Octree.cu \
    Utility.cu \
    Visualization.cu

CUDA_DIR = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.4/"
SYSTEM_NAME = x64
SYSTEM_TYPE = 64
CUDA_ARCH = compute_75
CUDA_CODE = sm_75
NVCC_OPTIONS = --use_fast_math

# 头文件路径
INCLUDEPATH += "$$CUDA_DIR/include" \
"C:\ProgramData\NVIDIA Corporation\CUDA Samples\v11.4\common\inc"

# 导入库文件路径
QMAKE_LIBDIR += "$$CUDA_DIR/lib/x64"
CUDA_INC = $$join(INCLUDEPATH,'" -I"','-I"','"')

# 依赖库
CUDA_LIB_NAMES += \
cuda \
cudadevrt \
cudart \
#cudadevrt \
#cublas \
#cublas_device \
#cudart_static \
#cufft \
#cufftw \
#curand \
#cusolver \
#cusparse \
#nppc \
#nppi \
#nppial \
#nppicc \
#nppicom \
#nppidei \
#nppif \
#nppig \
#nppim \
#nppist \
#nppisu \
#nppitc \
#npps \
#nvblas \
#nvcuvid \
#nvgraph \
#nvml \
#nvrtc \
#OpenCL \
#kernel32 \
#user32 \
#gdi32 \
#winspool \
#comdlg32 \
#advapi32 \
#shell32 \
#ole32 \
#oleaut32 \
#uuid \
#odbc32 \
#odbccp32 \
#MSVCRT \

CONFIG(debug, debug|release) {
CUDA_LIB_NAMES += ucrtd \
}
else{
CUDA_LIB_NAMES += ucrt \
}

for(lib, CUDA_LIB_NAMES) {
    CUDA_LIBS += $$lib.lib
}
for(lib, CUDA_LIB_NAMES) {
    NVCC_LIBS += -l$$lib
}
LIBS += $$NVCC_LIBS


MSVCRT_LINK_FLAG_DEBUG   = "/MDd"
MSVCRT_LINK_FLAG_RELEASE = "/MD"

CONFIG(debug, debug|release) {
    # Debug 模式
    OBJECTS_DIR = debug/obj
    CUDA_OBJECTS_DIR = debug/cuda
    cuda_d.input = CUDA_SOURCES
    cuda_d.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o
    cuda_d.commands = $$CUDA_DIR/bin/nvcc.exe -D_DEBUG $$NVCC_OPTIONS $$CUDA_INC $$LIBS \
                      --machine $$SYSTEM_TYPE -arch=$$CUDA_ARCH -code=$$CUDA_CODE \
                      --compile -cudart static -g -DWIN32 -D_MBCS \
                      -Xcompiler "/wd4819,/EHsc,/W3,/nologo,/Od,/Zi,/RTC1" \
                      -Xcompiler $$MSVCRT_LINK_FLAG_DEBUG \
                      -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
    cuda_d.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += cuda_d
}
else {
    # Release 模式
    OBJECTS_DIR = release/obj
    CUDA_OBJECTS_DIR = release/cuda
    cuda.input = CUDA_SOURCES
    cuda.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o
    cuda.commands = $$CUDA_DIR/bin/nvcc.exe $$NVCC_OPTIONS $$CUDA_INC $$LIBS \
                    --machine $$SYSTEM_TYPE -arch=$$CUDA_ARCH -code=$$CUDA_CODE \
                    --compile -cudart static -D_MBCS \
                    -Xcompiler "/wd4819,/EHsc,/W3,/nologo,/O3,/Zi" \
                    -Xcompiler $$MSVCRT_LINK_FLAG_RELEASE \
                    -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
    cuda.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += cuda
}

#glut Not include g++ .a file
unix|win32: LIBS += -L$$PWD/resource/OpenGL/glut/lib/ -lglut

INCLUDEPATH += $$PWD/resource/OpenGL/glut/include
DEPENDPATH += $$PWD/resource/OpenGL/glut/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/resource/OpenGL/glut/lib/glut.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/resource/OpenGL/glut/lib/libglut.a

unix|win32: LIBS += -L$$PWD/resource/OpenGL/glut/lib/ -lglut32

INCLUDEPATH += $$PWD/resource/OpenGL/glut/include
DEPENDPATH += $$PWD/resource/OpenGL/glut/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/resource/OpenGL/glut/lib/glut32.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/resource/OpenGL/glut/lib/libglut32.a

#resource
RESOURCES += \
    resource.qrc

#PointCloudLibrary
#预处理器：解决重载外部C链接函数报错，大佬牛逼
#https://blog.csdn.net/qq_41017802/article/details/103482535
DEFINES += NOMINMAX
DEFINES += BOOST_USE_WINDOWS_H

CONFIG(debug,debug|release){
    QMAKE_CXXFLAGS += /utf-8
    QMAKE_CFLAGS += /utf-8

    PCL_Dir =       $$quote(C:\Program Files\PCL 1.10.1)
    OpenNI_Dir =    $$quote(C:\Program Files\PCL 1.10.1\3rdParty\OpenNI2)
    Qhull_Dir =     $$quote(C:\Program Files\PCL 1.10.1\3rdParty\Qhull)
    FLANN_Dir =     $$quote(C:\Program Files\PCL 1.10.1\3rdParty\FLANN)
    Boost_Dir =     $$quote(C:\Program Files\PCL 1.10.1\3rdParty\Boost)
    Eigen_Dir =     $$quote(C:\Program Files\PCL 1.10.1\3rdParty\Eigen)
    VTK_Dir =       $$quote(C:\Program Files\PCL 1.10.1\3rdParty\VTK)

    INCLUDEPATH += \
        $$PCL_Dir/include/pcl-1.10 \
        $$OpenNI_Dir/Include \
        $$Qhull_Dir/include \
        $$FLANN_Dir/include \
        $$Boost_Dir/include/boost-1_72 \
        $$Eigen_Dir/eigen3 \
        $$VTK_Dir/include/vtk-8.2

    LIBS+= \
        -L$$PCL_Dir/lib \
        -L$$OpenNI_Dir/Lib \
        -L$$Qhull_Dir/lib \
        -L$$FLANN_Dir/lib \
        -L$$Boost_Dir/lib \
        -L$$VTK_Dir/lib

    CONFIG(debug, debug|release){
        LIBS += -L$$OpenNI_Dir/Lib \
        -lOpenNI2 \

        LIBS += -L$$FLANN_Dir/lib \
        -lflann-gd\
        -lflann_cpp-gd\
        -lflann_cpp_s-gd\
        -lflann_s-gd

        LIBS += -L$$Qhull_Dir/lib \
        -lqhull_d\
        -lqhullcpp_d\
        -lqhullstatic_d\
        -lqhullstatic_r_d\
        -lqhull_p_d\
        -lqhull_r_d

        LIBS += -L$$VTK_Dir/lib \
       -lvtkChartsCore-8.2-gd \

       -lvtkCommonColor-8.2-gd \

       -lvtkCommonComputationalGeometry-8.2-gd \

       -lvtkCommonCore-8.2-gd \

       -lvtkCommonDataModel-8.2-gd \

       -lvtkCommonExecutionModel-8.2-gd \

       -lvtkCommonMath-8.2-gd \

       -lvtkCommonMisc-8.2-gd \

       -lvtkCommonSystem-8.2-gd \

       -lvtkCommonTransforms-8.2-gd \

       -lvtkDICOMParser-8.2-gd \

       -lvtkDomainsChemistry-8.2-gd \

       -lvtkDomainsChemistryOpenGL2-8.2-gd \

       -lvtkdoubleconversion-8.2-gd \

       -lvtkexodusII-8.2-gd \

       -lvtkexpat-8.2-gd \

       -lvtkFiltersAMR-8.2-gd \

       -lvtkFiltersCore-8.2-gd \

       -lvtkFiltersExtraction-8.2-gd \

       -lvtkFiltersFlowPaths-8.2-gd \

       -lvtkFiltersGeneral-8.2-gd \

       -lvtkFiltersGeneric-8.2-gd \

       -lvtkFiltersGeometry-8.2-gd \

       -lvtkFiltersHybrid-8.2-gd \

       -lvtkFiltersHyperTree-8.2-gd \

       -lvtkFiltersImaging-8.2-gd \

       -lvtkFiltersModeling-8.2-gd \

       -lvtkFiltersParallel-8.2-gd \

       -lvtkFiltersParallelImaging-8.2-gd \

       -lvtkFiltersPoints-8.2-gd \

       -lvtkFiltersProgrammable-8.2-gd \

       -lvtkFiltersSelection-8.2-gd \

       -lvtkFiltersSMP-8.2-gd \

       -lvtkFiltersSources-8.2-gd \

       -lvtkFiltersStatistics-8.2-gd \

       -lvtkFiltersTexture-8.2-gd \

       -lvtkFiltersTopology-8.2-gd \

       -lvtkFiltersVerdict-8.2-gd \

       -lvtkfreetype-8.2-gd \

       -lvtkGeovisCore-8.2-gd \

       -lvtkgl2ps-8.2-gd \

       -lvtkglew-8.2-gd \

       -lvtkGUISupportMFC-8.2-gd \

       -lvtkhdf5-8.2-gd \

       -lvtkhdf5_hl-8.2-gd \

       -lvtkImagingColor-8.2-gd \

       -lvtkImagingColor-8.2 \

       -lvtkImagingCore-8.2-gd \

       -lvtkImagingCore-8.2 \

       -lvtkImagingFourier-8.2-gd \

       -lvtkImagingFourier-8.2 \

       -lvtkImagingGeneral-8.2-gd \

       -lvtkImagingGeneral-8.2 \

       -lvtkImagingHybrid-8.2-gd \

       -lvtkImagingHybrid-8.2 \

       -lvtkImagingMath-8.2-gd \

       -lvtkImagingMath-8.2 \

       -lvtkImagingMorphological-8.2-gd \

       -lvtkImagingMorphological-8.2 \

       -lvtkImagingSources-8.2-gd \

       -lvtkImagingSources-8.2 \

       -lvtkImagingStatistics-8.2-gd \

       -lvtkImagingStatistics-8.2 \

       -lvtkImagingStencil-8.2-gd \

       -lvtkImagingStencil-8.2 \

       -lvtkInfovisCore-8.2-gd \

       -lvtkInfovisLayout-8.2-gd \

       -lvtkInteractionImage-8.2-gd \

       -lvtkInteractionStyle-8.2-gd \

       -lvtkInteractionWidgets-8.2-gd \

       -lvtkIOAMR-8.2-gd \

       -lvtkIOAsynchronous-8.2-gd \

       -lvtkIOCityGML-8.2-gd \

       -lvtkIOCore-8.2-gd \

       -lvtkIOEnSight-8.2-gd \

       -lvtkIOExodus-8.2-gd \

       -lvtkIOExport-8.2-gd \

       -lvtkIOExportOpenGL2-8.2-gd \

       -lvtkIOExportPDF-8.2-gd \

       -lvtkIOGeometry-8.2-gd \

       -lvtkIOImage-8.2-gd \

       -lvtkIOImport-8.2-gd \

       -lvtkIOInfovis-8.2-gd \

       -lvtkIOLegacy-8.2-gd \

       -lvtkIOLSDyna-8.2-gd \

       -lvtkIOMINC-8.2-gd \

       -lvtkIOMovie-8.2-gd \

       -lvtkIONetCDF-8.2-gd \

       -lvtkIOParallel-8.2-gd \

       -lvtkIOParallelXML-8.2-gd \

       -lvtkIOPLY-8.2-gd \

       -lvtkIOSegY-8.2-gd \

       -lvtkIOSQL-8.2-gd \

       -lvtkIOTecplotTable-8.2-gd \

       -lvtkIOVeraOut-8.2-gd \

       -lvtkIOVideo-8.2-gd \

       -lvtkIOXML-8.2-gd \

       -lvtkIOXMLParser-8.2-gd \

       -lvtkjpeg-8.2-gd \

       -lvtkjsoncpp-8.2-gd \

       -lvtklibharu-8.2-gd \

       -lvtklibxml2-8.2-gd \

       -lvtklz4-8.2-gd \

       -lvtklzma-8.2-gd \

       -lvtkmetaio-8.2-gd \

       -lvtkNetCDF-8.2-gd \

       -lvtkogg-8.2-gd \

       -lvtkParallelCore-8.2-gd \

       -lvtkpng-8.2-gd \

       -lvtkproj-8.2-gd \

       -lvtkpugixml-8.2-gd \

       -lvtkRenderingAnnotation-8.2-gd \

       -lvtkRenderingContext2D-8.2-gd \

       -lvtkRenderingContextOpenGL2-8.2-gd \

       -lvtkRenderingCore-8.2-gd \

       -lvtkRenderingExternal-8.2-gd \

       -lvtkRenderingFreeType-8.2-gd \

       -lvtkRenderingGL2PSOpenGL2-8.2-gd \

       -lvtkRenderingImage-8.2-gd \

       -lvtkRenderingLabel-8.2-gd \

       -lvtkRenderingLOD-8.2-gd \

       -lvtkRenderingOpenGL2-8.2-gd \

       -lvtkRenderingVolume-8.2-gd \

       -lvtkRenderingVolumeOpenGL2-8.2-gd \

       -lvtksqlite-8.2-gd \

       -lvtksys-8.2-gd \

       -lvtktheora-8.2-gd \

       -lvtktiff-8.2-gd \

       -lvtkverdict-8.2-gd \

       -lvtkViewsContext2D-8.2-gd \

       -lvtkViewsCore-8.2-gd \

       -lvtkViewsInfovis-8.2-gd \

       -lvtkzlib-8.2-gd \

        LIBS += -L$$PCL_Dir/lib \
       -lpcl_commond \

       -lpcl_featuresd \

       -lpcl_filtersd \

       -lpcl_iod \

       -lpcl_io_plyd \

       -lpcl_kdtreed \

       -lpcl_keypointsd \

       -lpcl_mld \

       -lpcl_octreed \

       -lpcl_outofcored \

       -lpcl_peopled \

       -lpcl_recognitiond \

       -lpcl_registrationd \

       -lpcl_sample_consensusd \

       -lpcl_searchd \

       -lpcl_segmentationd \

       -lpcl_stereod \

       -lpcl_surfaced \

       -lpcl_trackingd \

       -lpcl_visualizationd \

    } else {
        LIBS += -L$$OpenNI_Dir/Lib \
        -lOpenNI2 \

        LIBS += -L$$FLANN_Dir/lib \
                -lflann\
                -lflann_cpp\
                -lflann_cpp_s\
                -lflann_s

        LIBS += -L$$Qhull_Dir/lib \
                -lqhull\
                -lqhullcpp\
                -lqhullstatic\
                -lqhullstatic_r\
                -lqhull_p\
                -lqhull_r

        LIBS += -L$$VTK_Dir/lib \
        -lvtkChartsCore-8.2 \

        -lvtkCommonColor-8.2 \

        -lvtkCommonComputationalGeometry-8.2 \

        -lvtkCommonCore-8.2 \

        -lvtkCommonDataModel-8.2 \

        -lvtkCommonExecutionModel-8.2 \

        -lvtkCommonMath-8.2 \

        -lvtkCommonMisc-8.2 \

        -lvtkCommonSystem-8.2 \

        -lvtkCommonTransforms-8.2 \

        -lvtkDICOMParser-8.2 \

        -lvtkDomainsChemistry-8.2 \

        -lvtkDomainsChemistryOpenGL2-8.2 \

        -lvtkdoubleconversion-8.2 \

        -lvtkexodusII-8.2 \

        -lvtkexpat-8.2 \

        -lvtkFiltersAMR-8.2 \

        -lvtkFiltersCore-8.2 \

        -lvtkFiltersExtraction-8.2 \

        -lvtkFiltersFlowPaths-8.2 \

        -lvtkFiltersGeneral-8.2 \

        -lvtkFiltersGeneric-8.2 \

        -lvtkFiltersGeometry-8.2 \

        -lvtkFiltersHybrid-8.2 \

        -lvtkFiltersHyperTree-8.2 \

        -lvtkFiltersImaging-8.2 \

        -lvtkFiltersModeling-8.2 \

        -lvtkFiltersParallel-8.2 \

        -lvtkFiltersParallelImaging-8.2 \

        -lvtkFiltersPoints-8.2 \

        -lvtkFiltersProgrammable-8.2 \

        -lvtkFiltersSelection-8.2 \

        -lvtkFiltersSMP-8.2 \

        -lvtkFiltersSources-8.2 \

        -lvtkFiltersStatistics-8.2 \

        -lvtkFiltersTexture-8.2 \

        -lvtkFiltersTopology-8.2 \

        -lvtkFiltersVerdict-8.2 \

        -lvtkfreetype-8.2 \

        -lvtkGeovisCore-8.2 \

        -lvtkgl2ps-8.2 \

        -lvtkglew-8.2 \

        -lvtkGUISupportMFC-8.2 \

        -lvtkhdf5-8.2 \

        -lvtkhdf5_hl-8.2 \

        -lvtkImagingColor-8.2 \

        -lvtkImagingCore-8.2 \

        -lvtkImagingFourier-8.2 \

        -lvtkImagingGeneral-8.2 \

        -lvtkImagingHybrid-8.2 \

        -lvtkImagingMath-8.2 \

        -lvtkImagingMorphological-8.2 \

        -lvtkImagingSources-8.2 \

        -lvtkImagingStatistics-8.2 \

        -lvtkImagingStencil-8.2 \

        -lvtkInfovisCore-8.2 \

        -lvtkInfovisLayout-8.2 \

        -lvtkInteractionImage-8.2 \

        -lvtkInteractionStyle-8.2 \

        -lvtkInteractionWidgets-8.2 \

        -lvtkIOAMR-8.2 \

        -lvtkIOAsynchronous-8.2 \

        -lvtkIOCityGML-8.2 \

        -lvtkIOCore-8.2 \

        -lvtkIOEnSight-8.2 \

        -lvtkIOExodus-8.2 \

        -lvtkIOExport-8.2 \

        -lvtkIOExportOpenGL2-8.2 \

        -lvtkIOExportPDF-8.2 \

        -lvtkIOGeometry-8.2 \

        -lvtkIOImage-8.2 \

        -lvtkIOImport-8.2 \

        -lvtkIOInfovis-8.2 \

        -lvtkIOLegacy-8.2 \

        -lvtkIOLSDyna-8.2 \

        -lvtkIOMINC-8.2 \

        -lvtkIOMovie-8.2 \

        -lvtkIONetCDF-8.2 \

        -lvtkIOParallel-8.2 \

        -lvtkIOParallelXML-8.2 \

        -lvtkIOPLY-8.2 \

        -lvtkIOSegY-8.2 \

        -lvtkIOSQL-8.2 \

        -lvtkIOTecplotTable-8.2 \

        -lvtkIOVeraOut-8.2 \

        -lvtkIOVideo-8.2 \

        -lvtkIOXML-8.2 \

        -lvtkIOXMLParser-8.2 \

        -lvtkjpeg-8.2 \

        -lvtkjsoncpp-8.2 \

        -lvtklibharu-8.2 \

        -lvtklibxml2-8.2 \

        -lvtklz4-8.2 \

        -lvtklzma-8.2 \

        -lvtkmetaio-8.2 \

        -lvtkNetCDF-8.2 \

        -lvtkogg-8.2 \

        -lvtkParallelCore-8.2 \

        -lvtkpng-8.2 \

        -lvtkproj-8.2 \

        -lvtkpugixml-8.2 \

        -lvtkRenderingAnnotation-8.2 \

        -lvtkRenderingContext2D-8.2 \

        -lvtkRenderingContextOpenGL2-8.2 \

        -lvtkRenderingCore-8.2 \

        -lvtkRenderingExternal-8.2 \

        -lvtkRenderingFreeType-8.2 \

        -lvtkRenderingGL2PSOpenGL2-8.2 \

        -lvtkRenderingImage-8.2 \

        -lvtkRenderingLabel-8.2 \

        -lvtkRenderingLOD-8.2 \

        -lvtkRenderingOpenGL2-8.2 \

        -lvtkRenderingVolume-8.2 \

        -lvtkRenderingVolumeOpenGL2-8.2 \

        -lvtksqlite-8.2 \

        -lvtksys-8.2 \

        -lvtktheora-8.2 \

        -lvtktiff-8.2 \

        -lvtkverdict-8.2 \

        -lvtkViewsContext2D-8.2 \

        -lvtkViewsCore-8.2 \

        -lvtkViewsInfovis-8.2 \

        -lvtkzlib-8.2 \

        LIBS += -L$$PCL_Dir/lib \
        -pcl_common \

        -pcl_features \

        -pcl_filters \

        -pcl_io \

        -pcl_io_ply \

        -pcl_kdtree \

        -pcl_keypoints \

        -pcl_ml \

        -pcl_octree \

        -pcl_outofcore \

        -pcl_people \

        -pcl_recognition \

        -pcl_registration \

        -pcl_sample_consensus \

        -pcl_search \

        -pcl_segmentation \

        -pcl_stereo \

        -pcl_surface \

        -pcl_tracking \

        -pcl_visualization \

    }
}

#plyread dll
