import os,sys
import numpy as np
from PIL import Image
import argparse

source_img = "/home/victorchen/workspace/Venus/celebA/images/000001.jpg"
component_dir = "/home/victorchen/workspace/Venus/animeface-character-dataset/thumb/000_hatsune_miku/"

class ConcatImager:
    """ Synthesis Image with subimage.
    """
    def __init__(self, component_dir, max_component_num = 500, lowersize = 16):
        """
        :params
		    componet_dir (str): directory that contains sub-images used in synthesis.
		    max_component_num (int): maximum sub-images in use. (Large max_component_num would cost more time in reading images)
		    lowersize (int): size of downsampled sub-images.
        """
        chan_means = []
        parts = []
        for i, component in enumerate(os.listdir(component_dir)):
    
            if component.split(".")[-1] not in ["png","jpg","jpeg"]:
                continue
        
            c = Image.open(component_dir + component)
            c = c.resize((lowersize, lowersize))
    
            arr = np.array(c)
            mean = arr.reshape(-1, 3).mean(axis = 0)
            chan_means.append(mean)
            parts.append(arr.reshape(1, *arr.shape))
    
            if len(parts) == max_component_num:
                break
    
        self.means = np.vstack(chan_means)
        self.parts = np.concatenate(parts, axis = 0) 
        self.lowersize = lowersize


    def synthesis(self, source_img, save_name = "./synthesis.png", downsample_ratio = 0.5):
        """ Synthesis source_img with sub-images

        :params:
            source_img (str): path of one image for synthesis.
            save_name (str): path for saving image
            downsample_ratio (float): used in downsample.
        """

        # downsample original image
        img = Image.open(source_img).convert("RGB")
        m, n = img.size
        m, n = int(m * downsample_ratio), int(n * downsample_ratio)
        img = img.resize((m,n))
        arrimg = np.array(img)
        m, n = n, m

        # designate each pixel with one part
        indices = []
        for d in arrimg.reshape(-1, 3):
            idx = np.argmin(((self.means - d)**2).sum(axis = 1))
            indices.append(idx)
        indices = np.array(indices, dtype=int).reshape(m,n)

        # fill in 
        image = np.zeros((m *self.lowersize, n*self.lowersize, 3), dtype = np.uint8)
        for i in range(1, m + 1):
            for j in range(1, n + 1):
                image[(i-1)*self.lowersize: i* self.lowersize, (j-1)*self.lowersize: j*self.lowersize, :] = self.parts[indices[i-1,j-1],:]

        # save
        image = image.astype(np.uint8) 
        isave = Image.fromarray(image)
        isave.save(save_name)


parser = argparse.ArgumentParser()
parser.add_argument("-c","--component-dir", required=True, type=str, help="用于合成大图的小图所在的文件夹")
parser.add_argument("-mn","--max-component-num", type=int, default=300, help="用于合成的小图的最大数量")
parser.add_argument("-ls","--lower-size", type=int, default=16, help="用于合成的小图的大小")
parser.add_argument("-s","--source", required=True, type=str, help="合成目的的大图的路径")
parser.add_argument("-sn","--save-name",type=str, default="./synthesis.png", help="合成图保存路径")
parser.add_argument("-dr","--downsample-ratio", type=float, default=0.5, help="大图缩小的比率")
opt = parser.parse_args()

imager = ConcatImager(opt.component_dir, opt.max_component_num, opt.lower_size)
imager.synthesis(opt.source, opt.save_name, opt.downsample_ratio)
print("Synthesized Image Stored in {}".format(os.path.abspath(opt.save_name)))
