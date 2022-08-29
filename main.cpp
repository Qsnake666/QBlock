#include "mainwindow.h"
#include <iostream>
#include <QApplication>
#include "Octree.cuh"
#include "Utility.cuh"
#include <math.h>
#include "Visualization.cuh"

/*
日志：
版本：v006d
Brief：该项目为八叉树All in one prj。
八叉树亿级点云创建30-60s
创建时间：2022/4/27
作者：王谨鸿 Andy Wong
2022/4/27：完善了八叉树功能
2022/4/28：完成了radiusSearch功能，修复center初始化及赋值时错误
问题修复后Octree类创建+12s，创建时间50s左右
待修复bug：当radius>最大立方体length时，出现空指针报错
2022/4/29：完善radiusSearch功能
修复bug：radiusSearch搜索重复问题，未引入原始octree
完善了根据radius选择level功能
正在修复空指针bug
解决方案：禁用删除空结点函数。当最外层cube无法刚好包裹点云时，会出现空节点，此时该节点指针octree为
Nullptr，在搜集时会报错
状态：已解决
计划将double更换为float，减小内存占用，增加运算速度，现在稍慢于PCL
2022/4/30：引入OpenGL和Visualization类
完成了vector<PointCloud::Point>到Matrix的转换
在PointCloud::Point结构中添加了属性数据：a
解决心得：在关联类文件中的结构体由于#pragma once在后续编译中会直接调用cache导致修改失效，
需要删除projName->x64->Debug文件夹中的所有文件
2022/5/1：转换Utility库为CUDA库
2022/5/2：更新radiusSearch逻辑，减少计算量，加快了计算效率！
比对计算效率：
均匀点云size200：
v003a：27.043000 s
v004a：23.900000 s
均匀超大点云size500 with radius = 233：
v003a：35.469000 s	Level = 7 radiusSearchPoints: 15013710 radiusSearchOctree: 15013710 sPtcMat: 15013710 程序运行时间：4.826000 s
v004a：116.417000 s Level = 7 radiusSearchPoints: 15013710 radiusSearchOctree: 15013710 sPtcMat: 15013710 程序运行时间：14.737000 s
两者计算结果相同，反应相同点云密度情况下，level越高效率提升更明显。可推理点云密度越高效率越明显！
PointCloudLibrary：Neighbors within radius search at (50 50 50) with radius = 233
Points: 139864453
程序运行时间：250.311000 s
2022/5/3：谁不知道并行计算呢？我真是搞笑，熟不知竟然CPU也能并发？
2022/5/4：优化radiusSearch块体模式，块体搜索效率极高
增加点云旋转
2022/5/6：实现块体模式中心点云8方向延申转Matrix，实现点云旋转平移
2022/5/7：实现点云投射绘图
总结：
开始没头绪，干着急；中期有头绪，进度快；快做完了就摆烂
点云投影图先搁置：https://zhuanlan.zhihu.com/p/73034007
测试CUDA VBO显示
2022/5/9：生成静态库，准备Qt中配置OpenGL
生成sOctree_v004d动态链接，转入QtCreator进行可视化界面工作
2022/5/10：整个工程移植到Qt，完美适配CUDA，输出无法输出中文，暂时输出英文程序
2022/5/11：完成OpenGL块体绘制，ui测试已经可用。
完成ui界面优化：将输出值打印到ui->codeEdit控件
2022/5/12：完成3D块体渲染和ui界面优化
signal运行bug，解决错误，添加QObject
若出现找不到pdb情况重新qmake再运行
该搜索模式最多得到4096*8个块，如何展示细部？
定位->根据缩放倍数确定搜索中心和半径，提高搜索精度，局部搜索，放入缓存存储。
2022/5/13：完成自由更改显示level功能，开始代码优化与整理
更新Octree库Utility类，增加radiusSearchByLevel功能
库中代码已经整理，正在整理mainwindow代码
修复工程文件移动导致无法打开glut.lib问题
已经将该程序代码整理完成
正在引入PCL库，读取PCD数据
2022/5/14：引入PCL库完成
更新Utility库，修复了空块显示bug
更新App图标
创建点云读取函数PCL和iostream，fstream版
2022/5/15：完成blk，txt数据读取显示工作。
完成帮助模块html初体
2022/5/16：修复改变resolution后再改变level使用旧oct指针bug
原因：函数都是重新声明实体指针，导致调用level后使用旧指针
解决方案：在class中声明统一指针
发现问题：OpenGL渲染内存释放问题，内存累计无法得到释放！
2022/5/17：版本更新
更改Octree生成数量逻辑，根据resolution与点云长或宽比值决定
更新Octree库，新增genMutiOctree函数，使用vector来保存oct
拟定新增radiusSearch和radiusSearchCube重载muti方法（待定）
拟定新增openglwidget库鼠标滚轮事件算法（待定）
拟定更新radiusSearch和radiusSearchCube显示属性
属性显示暂定方案为：单一属性（a）或者混合属性（mixed）
拟定更新openglwidget库融合相同属性块功能
完成样例数据优化
2022/5/18：
修复Octree库中一些bug
实现大范围数据显示，测试效果较好
新增Utility库搜索属性功能
2022/5/19：
测试按钮已经装载genMutiOctree，运行效果达到预期
得到一般属性范围，使OpenGL渲染时候受众广，或者调用最大最小值，更改函数
但未更新更改resolution和level函数
openglwidget库更新显示属性
库中函数已经完善的差不多，App中Bug较多 优先级
1.OpenGl中的mat需要清理，在打开新工程时要消除
2.内存占用未得到释放
3.更改Resolution和Level时0值参数或者高值参数问题
2022/5/22：Block Library运算库更新
将PointCloud::Point(x,y,z)中a默认为NO_ATTRIBUTE
更新块体读取函数ioStyle，针对blk数据完成正确属性提取
2022/5/24：更新修复运算库genMutiOctree逻辑bug
OpenOctree模块调用genMutiOctree模块
*/


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
