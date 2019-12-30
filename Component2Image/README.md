# Component Synthesis

Fun tools used in synthesis image with small component images，用多张小图合成大图的小工具，funny！

原理就是将原图的像素替换成downsample的小图，替换时选择(R,G,B)均值和当前像素点的(R,G,B)值的小图。

## Usage 用法

```shell
python synthesis.py -h # 查看用法帮助啦
```

```shell
# 示例用法
python synthesis.py -c examples/miku/ -s examples/target1.jpg -dr 1.5
```

得到下图：

![合成的图像](synthesis.png)

以上图片合成大概耗时2s。原图的分辨率越高，downsample的比率越大，小图的数量越多，小图的下采样用于合成的大小越大，耗时越多。所以现在效率还不行，用来玩玩还是可以的。

## Requirement

* python3
* numpy
* PIL

## source 

Example images:

* `miku` from [animeface-character-dataset](http://www.nurs.or.jp/~nagadomi/animeface-character-dataset/)
* `target` from [celebA dataset](http://personal.ie.cuhk.edu.hk/~lz013/projects/CelebA.html)