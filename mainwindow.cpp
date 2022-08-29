#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    initUi();
    QString output;
    //QBlock NewProject
    connect(ui->newProBtn,&QToolButton::clicked,this,[=](){
        QString projName = QInputDialog::getText(this,"Input your porject name","Porject Name");
        if (projName!=""){
            //未使用该path
            QString projPath = QFileDialog::getExistingDirectory(this,"Select the project path");
            QTreeWidgetItem *proj = new QTreeWidgetItem(QStringList()<<projName);
            ui->fileTree->addTopLevelItem(proj);
            QString tmp = QString("%1.prj").arg(projName);
            //追加工程文件
            QStringList projInclude = (QStringList() <<tmp);
            QTreeWidgetItem *proj_1 =new QTreeWidgetItem(projInclude);
            proj->addChild(proj_1);
        }
    });

    connect(ui->newBtn,&QToolButton::clicked,this,[=]()mutable{
        //初始化
        ui->statusFrame->setVisible(true);
        parameters = new Basic_Para;

        output= QString("Console: Sample Data");
        ui->codeEdit->append(output);

        //样本数据 跨度X:3200 Y:1600 Z:40 无压力
        //输出彩色说明为混合模式
        int size = 200;
        std::vector<PointCloud::Point> pointCloud;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                for (int k = 0; k < size; k++) {
                    pointCloud.push_back(PointCloud::Point((long long)i*16,(long long)j*8,(long long)k*0.2,i));
                }
            }
        }

        output= QString("Console: Sample octree size = %1").arg(size);
        ui->codeEdit->append(output);

        //建立八叉树模型模块 fromResoluton
        parameters->resolution = 10;

        //fromResolution
        ui->changeResolution->setValue(parameters->resolution);
        connect(ui->changeResConfirm,&QToolButton::clicked,this,[=]()mutable{
            parameters->resolution = ui->changeResolution->value();
            if (parameters->resolution < 1){
                parameters->resolution = 1;
                ui->changeResolution->setValue(1);
            }
            output = QString("Console: resolution changed = %1").arg(parameters->resolution);
            ui->codeEdit->append(output);
            fromResolution(pointCloud, parameters->resolution, parameters->radius, output);
        });

        output= QString("Console: Octree resolution = %1").arg(parameters->resolution);
        ui->codeEdit->append(output);

        //旧方法
        //uty->genOctree(oct, pointCloud, parameters->resolution);//只要随机数种子不变，其生成的随机数序列就不会改变。

        std::vector< Octree*>().swap(octMul);
        uty->genMultiOctree(octMul, pointCloud, parameters->resolution);
        //改变radius
        //connect(ui->GLWidget->sc,&SignalsCollect::radiusChanged,this,[=]()mutable{
            //fromRadius(oct, uty, pointCloud, resolution, ui->GLWidget->radius, output);
        //});

        //查找点位置
        for (int i = 0; i < octMul.size(); i++){
            oct = octMul[i];
            Octree::Octree_Struct ost; //实体
            PointCloud::Point point = PointCloud::Point(0,0,0);
            parameters->radius = 1000000;

            parameters->level = uty->getLevel(oct->octree, parameters->radius, parameters->resolution);
            output = QString("Console: Locate octree level = %1").arg(parameters->level);
            ui->codeEdit->append(output);

            //查找点在某一层octree意义：在radius搜寻过程中根据radius快速定位
            //这样cube外元素寻找仅需要先确定相邻center点，查询center点在该level的octree即可
            ost = uty->locatePoints(oct->octree, point, parameters->level);

            //查找点radius范围点和节点
            /*
            std::vector<PointCloud::Point> searchPointCloud;
            std::vector<Octree::Octree_Struct> searchOctree;
            uty->radiusSearch(oct->octree, ost, point, radius, searchPointCloud, searchOctree);
            std::cout << "radiusSearchPoints: " << searchPointCloud.size() << std::endl;
            std::cout << "radiusSearchOctree: " << searchOctree.size() << std::endl;
            */

            //得到块体模式Cube长度 创建函数getLevelCube
            int levelCube = parameters->level - 5;
            if (levelCube < 1)
                levelCube = 1;
            double lengthCube = pow(2, levelCube - 1) * parameters->resolution;

            //fromLevel
            connect(ui->changeLevelConfirm,&QToolButton::clicked,this,[=]()mutable{
                parameters->inputLevel = ui->changeLevel->value();
                output = QString("Console: Level changed = %1").arg(parameters->inputLevel);
                ui->codeEdit->append(output);
                fromLevel(parameters->inputLevel, oct, uty, pointCloud, point, parameters->resolution, parameters->radius, output);
            });

            std::vector<PointCloud::Point> searchCubeCenter;
            uty->radiusSearchCube(oct->octree, ost, point, parameters->radius, searchCubeCenter);
            output= QString("Console: radiusSearchCube = %1").arg(searchCubeCenter.size());
            ui->codeEdit->append(output);

            //可视化模块

            //点云模式转Matrix
            //vis->vectorToMatrix(searchCubeCenter, &sPtcMat);
            //块体模式转Matrix补齐8方向
            vis->vectorToMatrixCube(searchCubeCenter, &sPtcMat, lengthCube);
            output= QString("Console: Matrix level = %1").arg(levelCube);
            ui->codeEdit->append(output);
            output= QString("Console: Matrix size = %1 x %2").arg(sPtcMat.height).arg(sPtcMat.width);
            ui->codeEdit->append(output);
            ui->changeLevel->setValue(levelCube);

            //点云旋转平移模块
            Visualization::Matrix R;
            vis->initMatrix(&R, 4, 4);
            double rad = 3.1415926 / 2; //90°
            vis->genRotateMatrix(R, 'x', rad);
            double tx, ty, tz;
            tx = 10; ty = 0; tz = 0;
            vis->genTranslationMatrix(R, tx, ty, tz);
            //原Matrix转置 R*A'
            vis->initMatrix(&sPtcMatT, sPtcMat.width, sPtcMat.height);
            vis->matrixTranspose(sPtcMat, sPtcMatT);
            //旋转平移 存在问题，暂时禁用
            outMat = vis->ptcRotate(R, sPtcMatT);

            //更新OpenGL 注意要清理matMul
            ui->GLWidget->matMul.push_back(sPtcMat);
            //清理内存
            std::vector<PointCloud::Point>().swap(searchCubeCenter);
        }
        ui->GLWidget->update();
    });

    connect(ui->openBtn,&QToolButton::clicked,this,[=]()mutable{
        //根据大小手动挑选一下
        if (false){
            //初始化
            ui->statusFrame->setVisible(true);
            parameters = new Basic_Para;
            std::vector<PointCloud::Point> pointCloud;

            QString blkPath = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                           "/",
                                                           tr("Block (*.txt *.blk *.qblk)"));

            //得到pointCloud 标准读入XYZL
            readBlkFile(pointCloud, blkPath, "txt", 3, parameters, output);
            if (blkPath == "" || pointCloud.size() == 0){
                output= QString("Console: No file selected or faild to read it!");
                ui->codeEdit->append(output);
                return;
            }

            //fromResolution
            ui->changeResolution->setValue(parameters->resolution);
            connect(ui->changeResConfirm,&QToolButton::clicked,this,[=]()mutable{
                parameters->resolution = ui->changeResolution->value();
                output = QString("Console: resolution changed = %1").arg(parameters->resolution);
                ui->codeEdit->append(output);
                fromResolution(pointCloud, parameters->resolution, parameters->radius, output);
            });

            output= QString("Console: Octree resolution = %1").arg(parameters->resolution);
            ui->codeEdit->append(output);

            uty->genOctree(oct, pointCloud, parameters->resolution);//只要随机数种子不变，其生成的随机数序列就不会改变。

            //改变radius
            //connect(ui->GLWidget->sc,&SignalsCollect::radiusChanged,this,[=]()mutable{
                //fromRadius(oct, uty, pointCloud, resolution, ui->GLWidget->radius, output);
            //});

            //查找点位置
            Octree::Octree_Struct ost; //实体
            PointCloud::Point point = PointCloud::Point(0,0,0);
            parameters->radius = 1000000;

            parameters->level = uty->getLevel(oct->octree, parameters->radius, parameters->resolution);
            output = QString("Console: Locate octree level = %1").arg(parameters->level);
            ui->codeEdit->append(output);

            //查找点在某一层octree意义：在radius搜寻过程中根据radius快速定位
            //这样cube外元素寻找仅需要先确定相邻center点，查询center点在该level的octree即可
            ost = uty->locatePoints(oct->octree, point, parameters->level);

            //查找点radius范围点和节点
            /*
            std::vector<PointCloud::Point> searchPointCloud;
            std::vector<Octree::Octree_Struct> searchOctree;
            uty->radiusSearch(oct->octree, ost, point, radius, searchPointCloud, searchOctree);
            std::cout << "radiusSearchPoints: " << searchPointCloud.size() << std::endl;
            std::cout << "radiusSearchOctree: " << searchOctree.size() << std::endl;
            */

            //得到块体模式Cube长度 创建函数getLevelCube
            int levelCube = parameters->level - 5;
            if (levelCube < 1)
                levelCube = 1;
            double lengthCube = pow(2, levelCube - 1) * parameters->resolution;

            //fromLevel
            connect(ui->changeLevelConfirm,&QToolButton::clicked,this,[=]()mutable{
                parameters->inputLevel = ui->changeLevel->value();
                output = QString("Console: Level changed = %1").arg(parameters->inputLevel);
                ui->codeEdit->append(output);
                fromLevel(parameters->inputLevel, oct, uty, pointCloud, point, parameters->resolution, parameters->radius, output);
            });

            std::vector<PointCloud::Point> searchCubeCenter;
            uty->radiusSearchCube(oct->octree, ost, point, parameters->radius, searchCubeCenter);
            output= QString("Console: radiusSearchCube = %1").arg(searchCubeCenter.size());
            ui->codeEdit->append(output);

            //可视化模块

            //点云模式转Matrix
            //vis->vectorToMatrix(searchCubeCenter, &sPtcMat);
            //块体模式转Matrix补齐8方向
            vis->vectorToMatrixCube(searchCubeCenter, &sPtcMat, lengthCube);
            output= QString("Console: Matrix level = %1").arg(levelCube);
            ui->codeEdit->append(output);
            output= QString("Console: Matrix size = %1 x %2").arg(sPtcMat.height).arg(sPtcMat.width);
            ui->codeEdit->append(output);
            ui->changeLevel->setValue(levelCube);

            //点云旋转平移模块
            Visualization::Matrix R;
            vis->initMatrix(&R, 4, 4);
            double rad = 3.1415926 / 2; //90°
            vis->genRotateMatrix(R, 'x', rad);
            double tx, ty, tz;
            tx = 10; ty = 0; tz = 0;
            vis->genTranslationMatrix(R, tx, ty, tz);
            //原Matrix转置 R*A'
            vis->initMatrix(&sPtcMatT, sPtcMat.width, sPtcMat.height);
            vis->matrixTranspose(sPtcMat, sPtcMatT);
            //旋转平移 存在问题，暂时禁用
            outMat = vis->ptcRotate(R, sPtcMatT);

            //更新OpenGL
            ui->GLWidget->mat=sPtcMat;
            ui->GLWidget->update();

            //清理内存
            std::vector<PointCloud::Point>().swap(searchCubeCenter);
        }
        else{
            //初始化
            ui->statusFrame->setVisible(true);
            parameters = new Basic_Para;
            std::vector<PointCloud::Point> pointCloud;

            QString blkPath = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                           "/",
                                                           tr("Block (*.txt *.blk *.qblk)"));

            //得到pointCloud 标准读入XYZL
            readBlkFile(pointCloud, blkPath, "txt", 3, parameters, output);
            if (blkPath == "" || pointCloud.size() == 0){
                output= QString("Console: No file selected or faild to read it!");
                ui->codeEdit->append(output);
                return;
            }

            //fromResolution
            ui->changeResolution->setValue(parameters->resolution);
            connect(ui->changeResConfirm,&QToolButton::clicked,this,[=]()mutable{
                parameters->resolution = ui->changeResolution->value();
                if (parameters->resolution < 1){
                    parameters->resolution = 1;
                    ui->changeResolution->setValue(1);
                }
                output = QString("Console: resolution changed = %1").arg(parameters->resolution);
                ui->codeEdit->append(output);
                fromResolution(pointCloud, parameters->resolution, parameters->radius, output);
            });

            output= QString("Console: Octree resolution = %1").arg(parameters->resolution);
            ui->codeEdit->append(output);

            //旧方法
            //uty->genOctree(oct, pointCloud, parameters->resolution);//只要随机数种子不变，其生成的随机数序列就不会改变。
            parameters->resolution = 50;
            std::vector< Octree*>().swap(octMul);
            uty->genMultiOctree(octMul, pointCloud, parameters->resolution);
            //改变radius
            //connect(ui->GLWidget->sc,&SignalsCollect::radiusChanged,this,[=]()mutable{
                //fromRadius(oct, uty, pointCloud, resolution, ui->GLWidget->radius, output);
            //});
            std::cout<<octMul.size()<<std::endl;
            //查找点位置
            for (int i = 0; i < octMul.size(); i++){
                oct = octMul[i];
                Octree::Octree_Struct ost; //实体
                PointCloud::Point point = PointCloud::Point(0,0,0);
                parameters->radius = 1000000;

                parameters->level = uty->getLevel(oct->octree, parameters->radius, parameters->resolution);
                output = QString("Console: Locate octree level = %1").arg(parameters->level);
                ui->codeEdit->append(output);

                //查找点在某一层octree意义：在radius搜寻过程中根据radius快速定位
                //这样cube外元素寻找仅需要先确定相邻center点，查询center点在该level的octree即可
                ost = uty->locatePoints(oct->octree, point, parameters->level);

                //查找点radius范围点和节点
                /*
                std::vector<PointCloud::Point> searchPointCloud;
                std::vector<Octree::Octree_Struct> searchOctree;
                uty->radiusSearch(oct->octree, ost, point, radius, searchPointCloud, searchOctree);
                std::cout << "radiusSearchPoints: " << searchPointCloud.size() << std::endl;
                std::cout << "radiusSearchOctree: " << searchOctree.size() << std::endl;
                */

                //得到块体模式Cube长度 创建函数getLevelCube
                int levelCube = parameters->level - 5;
                if (levelCube < 1)
                    levelCube = 1;
                double lengthCube = pow(2, levelCube - 1) * parameters->resolution;

                //fromLevel
                connect(ui->changeLevelConfirm,&QToolButton::clicked,this,[=]()mutable{
                    parameters->inputLevel = ui->changeLevel->value();
                    output = QString("Console: Level changed = %1").arg(parameters->inputLevel);
                    ui->codeEdit->append(output);
                    fromLevel(parameters->inputLevel, oct, uty, pointCloud, point, parameters->resolution, parameters->radius, output);
                });

                std::vector<PointCloud::Point> searchCubeCenter;
                uty->radiusSearchCube(oct->octree, ost, point, parameters->radius, searchCubeCenter);
                output= QString("Console: radiusSearchCube = %1").arg(searchCubeCenter.size());
                ui->codeEdit->append(output);

                //可视化模块

                //点云模式转Matrix
                //vis->vectorToMatrix(searchCubeCenter, &sPtcMat);
                //块体模式转Matrix补齐8方向
                vis->vectorToMatrixCube(searchCubeCenter, &sPtcMat, lengthCube);
                output= QString("Console: Matrix level = %1").arg(levelCube);
                ui->codeEdit->append(output);
                output= QString("Console: Matrix size = %1 x %2").arg(sPtcMat.height).arg(sPtcMat.width);
                ui->codeEdit->append(output);
                ui->changeLevel->setValue(levelCube);

                //点云旋转平移模块
                Visualization::Matrix R;
                vis->initMatrix(&R, 4, 4);
                double rad = 3.1415926 / 2; //90°
                vis->genRotateMatrix(R, 'x', rad);
                double tx, ty, tz;
                tx = 10; ty = 0; tz = 0;
                vis->genTranslationMatrix(R, tx, ty, tz);
                //原Matrix转置 R*A'
                vis->initMatrix(&sPtcMatT, sPtcMat.width, sPtcMat.height);
                vis->matrixTranspose(sPtcMat, sPtcMatT);
                //旋转平移 存在问题，暂时禁用
                outMat = vis->ptcRotate(R, sPtcMatT);

                //更新OpenGL 注意要清理matMul
                ui->GLWidget->matMul.push_back(sPtcMat);
                //清理内存
                std::vector<PointCloud::Point>().swap(searchCubeCenter);
            }
            ui->GLWidget->update();
        }
    });

    connect(ui->helpBtn,&QToolButton::clicked,this,[=](){
        QDesktopServices::openUrl(QUrl(QString("www.baidu.com")));
        /*
        #include <QWebEngineView>
        QString path = QApplication::applicationDirPath() + "/smarttest.html";//文件路径
        htmlView = new QWebEngineView(this);//创造对象
        htmlView->setUrl(QUrl(path));//添加路径
        setCentralWidget(htmlView);
        */
    });

    connect(ui->menuHelpBtn,&QToolButton::clicked,this,[=](){
        QDesktopServices::openUrl(QUrl(QString("www.baidu.com")));
    });
}

void MainWindow::initUi(){
    setWindowIcon(QIcon(":/appicon/resource/icon/qblock_icon2.png"));
    ui->setupUi(this);

    ui->fileTree->setHeaderLabel("Project List");
    ui->codeEdit->setText("Operation history:");

    ui->codeEdit->setFocusPolicy(Qt::NoFocus);
    ui->statusFrame->setVisible(false);

    QFont uiFontStyle; //控件字体
    uiFontStyle.setFamily("Courier");
    uiFontStyle.setPointSize(9);
    ui->codeEdit->setFont(uiFontStyle);
    uiFontStyle.setItalic(true);
    ui->commandEdit->setFont(uiFontStyle);
    uiFontStyle.setItalic(false);
    ui->resLabel->setFont(uiFontStyle);
    ui->levelLabel->setFont(uiFontStyle);
    uiFontStyle.setFamily("Consolas");
    uiFontStyle.setItalic(true);
    ui->searchEdit->setFont(uiFontStyle);

    ui->commandEdit->setPlaceholderText(" Command Line");
    ui->searchEdit->setPlaceholderText("Search Doc");
}

void MainWindow::readBlkFile(std::vector<PointCloud::Point>& pointCloud, QString filepath, QString mode, int code, Para_Struct parameters, QString output){
    output= QString("Console: Open Block file");
    ui->codeEdit->append(output);
    //PointCloudLibrary
#ifdef PCLINCLUDE
    if (code == 1){
        if (mode == "XYZ"){
            pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
            if (pcl::io::loadPLYFile<pcl::PointXYZ>(filepath.toStdString(), *cloud) == -1){
                output= QString("Error: can not open such file!");
                ui->codeEdit->append(output);
                return ;
            }
            //写入pointCloud
            for (int i = 0; i < cloud->points.size(); i++){
                pointCloud.push_back(PointCloud::Point(cloud->points[i].x, cloud->points[i].y, cloud->points[i].z));
            }
        }
        else if(mode == "XYZI"){
            pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
            if (pcl::io::loadPLYFile<pcl::PointXYZI>(filepath.toStdString(), *cloud) == -1){
                output= QString("Error: can not open such file!");
                ui->codeEdit->append(output);
                return ;
            }
            //写入pointCloud
            for (int i = 0; i < cloud->points.size(); i++){
                pointCloud.push_back(PointCloud::Point(cloud->points[i].x, cloud->points[i].y, cloud->points[i].z, cloud->points[i].intensity));
            }
        }

        else if(mode == "XYZL"){
            pcl::PointCloud<pcl::PointXYZL>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
            if (pcl::io::loadPLYFile<pcl::PointXYZL>(filepath.toStdString(), *cloud) == -1){
                output= QString("Error: can not open such file!");
                ui->codeEdit->append(output);
                return ;
            }
            //写入pointCloud
            for (int i = 0; i < cloud->points.size(); i++){
                pointCloud.push_back(PointCloud::Point(cloud->points[i].x, cloud->points[i].y, cloud->points[i].z, cloud->points[i].label));
            }
        }
    }
#else
    //iostream
    if (code == 2){
        QByteArray pathTmp = filepath.toLatin1();
        const char *fpath = pathTmp.data();
        FILE* f = fopen(fpath, "rb");
        if (f == NULL){
            output= QString("Error: can not open such file!");
            ui->codeEdit->append(output);
            return;
        }
        if (mode == "XYZRGB"){
            //https://www.cnblogs.com/justsong/p/12219716.html
        }
        else if (mode == "XYZA"){

        }
        else if (mode == "XYZ"){

        }
        else {
            output= QString("Error: wrong file type!");
            ui->codeEdit->append(output);
            return;
        }
    }
    //ifstream  qblk单独成函数
    if (code == 3){
        if (mode == "txt" || mode == "blk" || mode == "qblk"){
            std::ifstream inFile;
            inFile.open(filepath.toStdString());
            if (!inFile) {
                output= QString("Error: can not open such file or you need use unicode path!");
                ui->codeEdit->append(output);
                return;
            }
            char tmp[50];
            double res = 0;
            double ox = 0;
            double oy = 0;
            double oz = 0;
            for (int i = 0; i < 100; i++) {
                inFile.getline(tmp, 50);
                int idx = 0;
                //getResolution
                if (i == 2) {
                    while (idx < 50 && tmp[idx] != '=')
                    {
                        idx++;
                    }
                    int j = 50 - idx;
                    int k = 0;
                    while (k < j && tmp[idx + k + 1] != NULL) {
                        k++;
                    }
                    if (k < 1) {
                        res = 10;
                    }
                    else if (k >= 1) {
                        int a;
                        for (int m = 0; m < k; m++) {
                            a = tmp[idx + m + 1] - 48; //记住转码
                            res += a * pow(10, k - m - 1);
                        }
                    }
                    parameters->resolution = res;
                }
                //Orign x
                if (i == 4) {
                    while (idx < 50 && tmp[idx] != '=')
                    {
                        idx++;
                    }
                    int j = 50 - idx;
                    int k = 0;
                    while (k < j && tmp[idx + k + 1] != '.') {
                        k++;
                    }
                    if (k < 1) {
                        ox = 0;
                    }
                    else if (k >= 1) {
                        int a;
                        for (int m = 0; m < k; m++) {
                            a = tmp[idx + m + 1] - 48; //记住转码
                            ox += a * pow(10, k - m - 1);
                        }
                    }
                }
                //Orign y
                if (i == 5) {
                    while (idx < 50 && tmp[idx] != '=')
                    {
                        idx++;
                    }
                    int j = 50 - idx;
                    int k = 0;
                    while (k < j && tmp[idx + k + 1] != '.') {
                        k++;
                    }
                    if (k < 1) {
                        oy = 0;
                    }
                    else if (k >= 1) {
                        int a;
                        for (int m = 0; m < k; m++) {
                            a = tmp[idx + m + 1] - 48; //记住转码
                            oy += a * pow(10, k - m - 1);
                        }
                    }
                }
                //Orign z
                if (i == 6) {
                    while (idx < 50 && tmp[idx] != '=')
                    {
                        idx++;
                    }
                    int j = 50 - idx;
                    int k = 0;
                    while (k < j && tmp[idx + k + 1] != '.') {
                        k++;
                    }
                    if (k < 1) {
                        oz = 0;
                    }
                    else if (k >= 1) {
                        int a;
                        for (int m = 0; m < k; m++) {
                            a = tmp[idx + m + 1] - 48; //记住转码
                            oz += a * pow(10, k - m - 1);
                        }
                    }
                }
                //goEnd
                if (tmp[0] == 'b') {
                    break;
                }
            }
            while (inFile.getline(tmp, 50)) {
                double x, y, z, l;
                x = y = z = 0;
                l = NO_ATTRIBUTE;
                //x
                int idx = 0;
                while (idx < strlen(tmp) && tmp[idx] != ',') {
                    idx += 1;
                }
                double a; //. ascii = 46
                int b = 0;
                for (int m = 0; m < idx && tmp[m] != '.' ; m++) {
                    a = tmp[m] - 48;
                    x += a * pow(10, idx - m - 1);
                    b = m;
                }
                b += 1;
                if (tmp[b] == '.'){
                    b += 1;
                    x = 0; //重构x值
                    for (int m = 0; m < idx && tmp[m] != '.' ; m++) {
                        a = tmp[m] - 48;
                        x += a * pow(10, idx - m - 3);
                    }
                    for (int m = b; m < idx; m++) {
                        a = tmp[m] - 48;
                        x += a * pow(10, -idx + m);
                    }
                }
                idx += 1;
                int preIdx = idx;
                //y
                while (idx < strlen(tmp) && tmp[idx] != ',') {
                    idx += 1;
                }
                for (int m = preIdx; m < idx && tmp[m] != '.' ; m++) {
                    a = tmp[m] - 48;
                    y += a * pow(10, idx - m - 1);
                    b = m;
                }
                b += 1;
                if (tmp[b] == '.'){
                    b += 1;
                    y = 0;
                    for (int m = preIdx; m < idx && tmp[m] != '.' ; m++) {
                        a = tmp[m] - 48;
                        y += a * pow(10, idx - m - 3);
                    }
                    for (int m = b; m < idx; m++) {
                        a = tmp[m] - 48;
                        y += a * pow(10, -idx + m);
                    }
                }
                idx += 1;
                preIdx = idx;
                //z
                while (idx < strlen(tmp) && tmp[idx] != ',' ) {
                    idx += 1;
                }
                for (int m = preIdx; m < idx && tmp[m] != '.' ; m++) {
                    a = tmp[m] - 48;
                    z += a * pow(10, idx - m - 1);
                    b = m;
                }
                b += 1;
                if (tmp[b] == '.'){
                    b += 1;
                    z = 0;
                    for (int m = preIdx; m < idx && tmp[m] != '.' ; m++) {
                        a = tmp[m] - 48;
                        z += a * pow(10, idx - m - 3);
                    }
                    for (int m = b; m < idx; m++) {
                        a = tmp[m] - 48;
                        z += a * pow(10, -idx + m);
                    }
                }
                idx += 1;
                //a
                //RGBA
                while (idx < strlen(tmp) && tmp[idx] != ',') {
                    idx += 1;
                }
                idx += 1; //R
                while (idx < strlen(tmp) && tmp[idx] != ',') {
                    idx += 1;
                }
                idx += 1; //G
                while (idx < strlen(tmp) && tmp[idx] != ',') {
                    idx += 1;
                }
                idx += 1; //B
                preIdx = idx;
                while (idx < strlen(tmp) && tmp[idx] != ',') {
                    idx += 1;
                }
                //string
                std::string s;
                char tmp2[5];
                int count = 0;
                for (int m = preIdx; m < idx; m++) {
                    s = s + tmp[m];
                    tmp2[count]=tmp[m];
                    count++;
                }
                if (s=="岩石")
                    l = 35;
                else if (s=="夹石")
                    l = 45;
                else if (s=="矿体")
                    l = 55;
                if (strlen(tmp2) >= 4){
                    if(tmp2[0] == -47 && tmp2[1] == -46 && tmp2[2] == -54 && tmp2[3] == -81)
                        l = 35;
                    else if(tmp2[0] == -68 && tmp2[1] == -48 && tmp2[2] == -54 && tmp2[3] == -81)
                        l = 45;
                    if(tmp2[0] == -65 && tmp2[1] == -13 && tmp2[2] == -52 && tmp2[3] == -27)
                        l = 55;
                }
                pointCloud.push_back(PointCloud::Point(x - ox, y - oy, z - oz, l));
                //std::cout<<x-ox<<" "<<y-oy<<" "<<z-oz<<std::endl;
            }
        }
    }
    return;
#endif
}

void MainWindow::fromResolution(std::vector<PointCloud::Point> pointCloud, double resolution, double radius, QString output){
    if (resolution < 1){
        resolution = 1;
    }
    output= QString("Rebuild octree");
    ui->codeEdit->append(output);
    output = QString("Console: Octree resolution = %1").arg(resolution);
    ui->codeEdit->append(output);
    //清理内存
    delete(sPtcMat.elements);
    delete(sPtcMatT.elements);
    delete(outMat.elements);
    delete(oct->octree);

    uty->genOctree(oct, pointCloud, resolution);//只要随机数种子不变，其生成的随机数序列就不会改变。

    //查找点位置
    int level;
    Octree::Octree_Struct ost; //实体
    PointCloud::Point point = PointCloud::Point(0,0,0);

    level = uty->getLevel(oct->octree, radius, resolution);
    output = QString("Console: Locate octree level = %1").arg(level);
    ui->codeEdit->append(output);

    ost = uty->locatePoints(oct->octree, point, level);

    std::vector<PointCloud::Point> searchCubeCenter;
    uty->radiusSearchCube(oct->octree, ost, point, radius, searchCubeCenter);
    output= QString("Console: radiusSearchCube = %1").arg(searchCubeCenter.size());
    ui->codeEdit->append(output);

    int levelCube = level - 5;
    if (levelCube < 1)
        levelCube = 1;
    double lengthCube = pow(2, levelCube - 1) * resolution;

    vis->vectorToMatrixCube(searchCubeCenter, &sPtcMat, lengthCube);
    output= QString("Console: Matrix level = %1").arg(levelCube);
    ui->codeEdit->append(output);
    output= QString("Console: Matrix size = %1 x %2").arg(sPtcMat.height).arg(sPtcMat.width);
    ui->codeEdit->append(output);
    ui->changeLevel->setValue(levelCube);

    Visualization::Matrix R;
    vis->initMatrix(&R, 4, 4);
    double rad = 3.1415926 / 2; //90°
    vis->genRotateMatrix(R, 'x', rad);
    double tx, ty, tz;
    tx = 10; ty = 0; tz = 0;
    vis->genTranslationMatrix(R, tx, ty, tz);
    //原Matrix转置 R*A'
    vis->initMatrix(&sPtcMatT, sPtcMat.width, sPtcMat.height);
    vis->matrixTranspose(sPtcMat, sPtcMatT);
    //旋转平移 存在问题，暂时禁用
    outMat = vis->ptcRotate(R, sPtcMatT);

    //更新OpenGL
    ui->GLWidget->mat=sPtcMat;
    ui->GLWidget->update();

    //清理内存
    std::vector<PointCloud::Point>().swap(searchCubeCenter);
}

void MainWindow::fromRadius(Octree* oct, Utility* uty, std::vector<PointCloud::Point> pointCloud, PointCloud::Point point, double resolution, double radius, QString output){
    //从半径开始查找

    //清理内存
    delete(sPtcMat.elements);
    delete(sPtcMatT.elements);
    delete(outMat.elements);

    int level;
    Octree::Octree_Struct ost; //实体
    level = uty->getLevel(oct->octree, radius, resolution);
    output = QString("Console: level = %1").arg(level);
    ui->codeEdit->append(output);

    //查找点在某一层octree意义：在radius搜寻过程中根据radius快速定位
    //这样cube外元素寻找仅需要先确定相邻center点，查询center点在该level的octree即可
    ost = uty->locatePoints(oct->octree, point, level);

    std::vector<PointCloud::Point> searchCubeCenter;
    uty->radiusSearchCube(oct->octree, ost, point, radius, searchCubeCenter);
    output= QString("Console: radiusSearchCube = %1").arg(searchCubeCenter.size());
    ui->codeEdit->append(output);

    //得到块体模式Cube长度 创建函数getLevelCube
    int levelCube = level - 5;
    if (levelCube < 1)
        levelCube = 1;
    double lengthCube = pow(2, levelCube - 1) * resolution;

    //可视化模块

    //点云模式转Matrix
    //vis->vectorToMatrix(searchCubeCenter, &sPtcMat);
    //块体模式转Matrix补齐8方向
    vis->vectorToMatrixCube(searchCubeCenter, &sPtcMat, lengthCube);
    output= QString("Console: Matrix level = %1").arg(levelCube);
    ui->codeEdit->append(output);
    output= QString("Console: Matrix size = %1 x %2").arg(sPtcMat.height).arg(sPtcMat.width);
    ui->codeEdit->append(output);

    //点云旋转平移模块
    Visualization::Matrix R;
    vis->initMatrix(&R, 4, 4);
    double rad = 3.1415926 / 2; //90°
    vis->genRotateMatrix(R, 'x', rad);
    double tx, ty, tz;
    tx = 10; ty = 0; tz = 0;
    vis->genTranslationMatrix(R, tx, ty, tz);
    //原Matrix转置 R*A'
    vis->initMatrix(&sPtcMatT, sPtcMat.width, sPtcMat.height);
    vis->matrixTranspose(sPtcMat, sPtcMatT);
    //旋转平移 存在问题，暂时禁用
    outMat = vis->ptcRotate(R, sPtcMatT);

    //更新OpenGL
    ui->GLWidget->mat=sPtcMat;
    ui->GLWidget->update();
}

void MainWindow::fromLevel(int inputLevel, Octree* oct, Utility* uty, std::vector<PointCloud::Point> pointCloud, PointCloud::Point point, double resolution, double radius, QString output){
    int level;
    Octree::Octree_Struct ost; //实体
    level = uty->getLevel(oct->octree, radius, resolution);
    output = QString("Console: level = %1").arg(level);
    ui->codeEdit->append(output);

    ost = uty->locatePoints(oct->octree, point, level);

    std::vector<PointCloud::Point> searchCubeCenter;
    uty->radiusSearchCubeByLevel(oct->octree, ost, point, radius, searchCubeCenter, inputLevel);
    output= QString("Console: radiusSearchCube = %1").arg(searchCubeCenter.size());
    ui->codeEdit->append(output);

    //得到块体模式Cube长度 创建函数getLevelCube
    int levelCube = inputLevel;
    double lengthCube = pow(2, levelCube - 1) * resolution;

    //可视化模块

    //点云模式转Matrix
    //vis->vectorToMatrix(searchCubeCenter, &sPtcMat);
    //块体模式转Matrix补齐8方向
    vis->vectorToMatrixCube(searchCubeCenter, &sPtcMat, lengthCube);
    output= QString("Console: Matrix level = %1").arg(levelCube);
    ui->codeEdit->append(output);
    output= QString("Console: Matrix size = %1 x %2").arg(sPtcMat.height).arg(sPtcMat.width);
    ui->codeEdit->append(output);


    //点云旋转平移模块
    Visualization::Matrix R;
    vis->initMatrix(&R, 4, 4);
    double rad = 3.1415926 / 2; //90°
    vis->genRotateMatrix(R, 'x', rad);
    double tx, ty, tz;
    tx = 10; ty = 0; tz = 0;
    vis->genTranslationMatrix(R, tx, ty, tz);
    //原Matrix转置 R*A'
    vis->initMatrix(&sPtcMatT, sPtcMat.width, sPtcMat.height);
    vis->matrixTranspose(sPtcMat, sPtcMatT);
    //旋转平移 存在问题，暂时禁用
    outMat = vis->ptcRotate(R, sPtcMatT);

    //更新OpenGL
    ui->GLWidget->mat=sPtcMat;
    ui->GLWidget->update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

