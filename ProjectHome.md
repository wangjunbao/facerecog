<Jun. 13th 2014> A **bug reported by Kakhkhor Abdijalilov** (thank you):
In FaceRecog\_v2\_0.rar, there is a potential bug in the implementation of "CFld::Pca" method in Fld.cpp. The dimension of the feature vector after PCA was set to be sampleNum. But this is not correct when sampleNum is larger than the PCA feature dimension before PCA. One fix could be changing the first few lines of the method to:
```
        int pcaDim = oriDim < sampleNum ? oriDim : sampleNum;
	CvMat	*eigenVecs = cvCreateMat(pcaDim, oriDim, CV_COEF_FC1),
		*eigenVals = cvCreateMat(pcaDim, 1, CV_COEF_FC1);
```
But I haven't tested it yet.

<Feb. 20th 2012> Now the v2.0 of facerecog demo is released. I aimed at the following improvements this time:
  1. more concise and unified algorithm APIs
  1. more powerful normalization, feature extraction and classification algorithms
  1. add some functions on training and recognizing dialogs
  1. translate Chinese into English
  1. bug fixing

I finished destination 1,2 and 5, 4 is partially finished(which means you can still see some Chinese in the demo, but the algorithm codes are all in English), 3 is also partially finished. The demo is already runnable, except the "Train from file list" button in the train dialog. The usage the demo is the same as the last version. More importantly, the performance is much improved. There is an intro to the algorithm APIs in Downloads. I will write a intro to the Demo later.

Overall, this program is written in VC++, built with MS Visual Studio 2008 SP. Only OpenCV 2.2 is used in this version of the demo. The demo can detect faces from webcam or pictures, then identify it according to the enrolled database. It allows you to train your own classfier using your own pictures, as well as save or load your classifiers or face databases.

In order to reduce the size of the downloading pack, the default classifier (the .frmat file) is not very strong yet. We encourage you to train your own classifier using more pictures. You can edit the default classifier, default algorithms and other configs of the demo in "config.ini". I will keep on improving the codes, you can email me to report bugs.


---

previous introduction to version 1.4

这是一个人脸识别演示/算法测试程序，基于Windows操作系统，在VS2008 SP环境下编写，算法部分用到了OpenCV 2.0，其中ASM人脸对齐部分还用到了OpenCV 1.0；演示程序界面部分用到了MFC。
演示程序的功能是，对摄像头拍摄的或用户指定的图像，检测其中人脸，然后在已存储的人脸库（通过摄像头或图片注册）中找到最匹配的人脸并显示。还有人脸库和分类器的导入/导出功能。
<img src='http://facerecog.googlecode.com/files/ui_recog.jpg' alt='demo' />

整个工程可以分为3个部分：算法、功能和应用。
  1. 算法部分：算法部分目前分为4个模块：人脸对齐、光照归一化、特征提取和选择、子空间降维，每个模块是一个项目，每个项目生成一个dll供功能部分调用。
  1. 功能部分：功能部分只有一个项目FaceMngr，该部分依赖于算法部分，实现人脸注册、训练、识别、导入/导出等具体功能。该项目生成一个dll供应用部分调用。
  1. 应用部分：应用部分依赖于功能部分，实现了与人脸识别有关的应用，目前有两个项目：人脸识别演示程序frtest，和利用人脸库进行算法测试的algotest。
目前实现的算法有(2010.9.1):
  * 人脸对齐：
    * 利用OpenCV自带的Haar Cascade函数；
    * 利用Yao Wei的asmlibrary；
    * 利用给定的人眼坐标进行对齐
  * 光照归一化：
    * 同态滤波+直方图规定化
  * 特征提取和选择：
    * 灰度（像素）特征；
    * Gabor特征，并进行均匀采样
  * 子空间计算
    * Fisherface
附有程序文档，规定了各模块的接口，可以很方便地向项目中添加新的算法。欢迎对项目提出意见或进行改进！