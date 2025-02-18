import argparse
import os
import itertools
from tqdm import tqdm
import numpy as np
import cv2
# import matplotlib.pyplot as plt
from typing import Literal

parser = argparse.ArgumentParser()
parser.add_argument('--input', type=str, help="The path for sample image")
parser.add_argument('--output', type=str, help="The path for output image")
args = parser.parse_args()

# ---------------------------------------------------------------------------- #
#                                  Environment                                 #
# ---------------------------------------------------------------------------- #
# TEST = True


# def clear_output(path):
#     files = os.listdir(path)
#     for file_name in files:
#         file_path = os.path.join(path, file_name)
#         try:
#             os.remove(file_path)
#         except:
#             clear_output(file_path)

# if TEST:
#     if not os.path.exists("./result"):
#         os.makedirs("./result")
#     else:
#         clear_output("./result")
#     if not os.path.exists("./report"):
#         os.makedirs("./report")
    # else:
        # clear_output("./report")

# ---------------------------------------------------------------------------- #
#                                   Utilties                                   #
# ---------------------------------------------------------------------------- #

def unique_pixel_value_count(img: np.ndarray):
    unique_numbers, counts = np.unique(img, return_counts=True)
    number_count_dict = dict(zip(unique_numbers, counts))
    #print(number_count_dict)
    return number_count_dict

def draw_red_dots(image, coordinates):
    rgb_image = cv2.cvtColor(image, cv2.COLOR_GRAY2RGB)

    for coord in coordinates:
        x, y = coord
        cv2.circle(rgb_image, (y, x), 5, (0, 0, 255), -1)

    # cv2.imwrite(output_path, rgb_image)
    return rgb_image


def plot_support_axis(img:np.array):
    res = np.copy(img)
    h, w = res.shape
    for i in range(h):
        cv2.line(res, (int(0),int(100*i)), (int(w-1),int(100*i)), (0, 0, 0))
    for j in range(w):
        cv2.line(res, (int(100*j),int(0)), (int(100*j),int(h-1)), (0, 0, 0))
    return res

def generate_parameters_pairs(parameters:list[list]):
    parameters = list(itertools.product(*parameters))
    return parameters, len(parameters)

# def display_image(img: np.ndarray):
#     cv2.imshow('Image', img.astype(np.uint8))
#     cv2.waitKey(0)
#     cv2.destroyAllWindows()

# def display_histogram(question: str, datas: list[dict["title":str, "img":np.ndarray]], quiet=False, title:str = ""):
#     cols = 3 if len(datas) >= 3 else len(datas)
#     rows = len(datas) // 3 + (1 if len(datas)%3!=0 else 0)
#     fig, axs = plt.subplots(ncols=cols, nrows=rows, figsize=(4*cols, 3*rows),
#                         layout="constrained")
#     for idx, data in enumerate(datas):
#         row = idx // 3
#         col = idx % 3
#         if len(datas) == 1:
#             ax = axs
#         elif rows == 1:
#             ax = axs[col]
#         else:
#             ax = axs[row, col]
#         ax.hist(data["img"].ravel(), bins=256, range=[0, 256])
#         ax.set_xlabel('Pixel Value')
#         ax.set_ylabel('Frequency')
#         ax.set_title(data["title"])

#     if not os.path.exists("./report/"+question):
#         os.makedirs("./report/"+question)
#     if title == "":
#         plt.savefig("./report/"+question+"/"+question)
#     else:
#         plt.savefig("./report/"+question+"/"+title)

#     if quiet is not True:
#         plttitle = question
#         plt.show()

'''
Read image from given path 
@input
    path: str
    target: int
        = 0: from args.input
        = 1: from args.output
        = 2: no prefix path
    options:
        = cv2.IMREAD_COLOR      : BGR
        = cv2.IMREAD_GRAYSCALE  : GrayScale (default)

@output
    img
'''
def read_image(path: str, target: int, options=cv2.IMREAD_GRAYSCALE):
    if target == 0:
        path = args.input+path
    elif target == 1:
        path = args.output+path
    elif target == 2:
        path = path
    return cv2.imread(path, options)

def write_image(name:str, img, type:int = 0, question:str = ""):
    if type == 0:
        cv2.imwrite(args.output+name+".png", img)
    # elif type == 1:
    #     if not os.path.exists("./report/"+question):
    #         os.makedirs("./report/"+question)
    #     cv2.imwrite("./report/"+question+"/"+name+".png", img)

# If mask is k*k, add a k//2 padding to the border cell
def pad_image(img, mask_size:int):
    assert mask_size%2==1, "Mask size should be odd"
    return np.lib.pad(img, mask_size//2, 'reflect')

def unpad_image(img: np.ndarray, mask_size: int):
    assert mask_size%2==1, "Mask size should be odd"
    h, w = img.shape
    border_size = mask_size//2
    return img[border_size: h-border_size, border_size: w-border_size]


def convolve_2D_kernel(img: np.ndarray, kernel: np.ndarray):
    assert img.ndim==2 and kernel.ndim==2, "Invalid Input"
    assert kernel.shape[0]==kernel.shape[1], "Invalid Kernel shape"
    res = np.zeros_like(img).astype(np.float64)
    h, w = img.shape
    kernel_size = kernel.shape[0]
    padded_img = pad_image(img, kernel_size)

    # print(kernel)
    for i in range(h):
        for j in range(w):
            img_window = padded_img[i:i+kernel_size, j:j+kernel_size].astype(np.float64)
            res[i, j] = np.sum(img_window * kernel)
    return res

def threshold_image_const(img: np.ndarray, threshold: np.float64):
    return np.where(img >= threshold, 255, 0)

def hsl_to_rgb(h, s, l):
    c = (1 - abs(2 * l - 1)) * s
    x = c * (1 - abs((h / 60) % 2 - 1))
    m = l - c / 2

    if 0 <= h < 60:
        r, g, b = c, x, 0
    elif 60 <= h < 120:
        r, g, b = x, c, 0
    elif 120 <= h < 180:
        r, g, b = 0, c, x
    elif 180 <= h < 240:
        r, g, b = 0, x, c
    elif 240 <= h < 300:
        r, g, b = x, 0, c
    else:
        r, g, b = c, 0, x
    
    return np.array([(r + m) * 255, (g + m) * 255, (b + m) * 255], dtype=np.uint8)

def coloring_components(img:np.ndarray, res:np.ndarray, count:int):
    def generate_colors(count):
        hues = np.linspace(0, 360, count, endpoint=False)
        s = 1.0
        l = 0.8
        rgb_colors = np.empty((count, 3), dtype=np.uint8)
        
        for i, hue in enumerate(hues):
            rgb_colors[i] = hsl_to_rgb(hue, s, l)
        
        return rgb_colors
    
    rgb_colors = generate_colors(count=count)
    h, w = img.shape
    colored_img = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)
    for i in range(h):
        for j in range(w):
            if res[i, j] != 0:
                colored_img[i, j] = rgb_colors[res[i, j]-1]
    return colored_img

# ---------------------------------------------------------------------------- #
#                           Morphological Processing                           #
# ---------------------------------------------------------------------------- #

class StructureElement:
    def __init__(self, kernel:np.ndarray, origin: tuple[int, int]):
        self.kernel = kernel
        self.origin = origin
        self.pos = []
        self.__setPos()

    def __setPos(self):
        h, w = self.kernel.shape
        for i in range(h):
            for j in range(w):
                if self.kernel[i, j] == 1:
                    self.pos.append((i-self.origin[0], j-self.origin[1]))
    
    def reflect(self):
        return StructureElement(kernel=np.flipud(np.fliplr(self.kernel)), origin=self.origin)

    def __str__(self) -> str:
        return f"kernel:\n {self.kernel}\norigin: {self.origin}\npos: {self.pos}"

    
# Minkowski addition definition
def dilation(img: np.ndarray, H: StructureElement):
    assert img.ndim==2 and H.kernel.ndim==2, "Invalid Input"
    assert H.kernel.shape[0]==H.kernel.shape[1], "Invalid Kernel shape"

    h, w = img.shape
    kernel_size = H.kernel.shape[0]
    res = np.zeros_like(img)

    for i in range(h):
        for j in range(w):
            if img[i, j] == 255:
                for pos in H.pos:
                    if 0 <= i+pos[0] < h and 0 <= j+pos[1] < w:
                        res[i+pos[0], j+pos[1]] = 255
    return res

# Dual relation of Minkowski addition definition
def erotion(img: np.ndarray, H: StructureElement):
    assert img.ndim==2 and H.kernel.ndim==2, "Invalid Input"
    assert H.kernel.shape[0]==H.kernel.shape[1], "Invalid Kernel shape"

    h, w = img.shape
    kernel_size = H.kernel.shape[0]
    res = np.zeros_like(img)
    thres = len(H.pos)

    for i in range(h):
        for j in range(w):
            if img[i, j] == 255:
                for pos in H.pos:
                    if 0 <= i+pos[0] < h and 0 <= j+pos[1] < w:
                        res[i+pos[0], j+pos[1]] += 1
    
    for i in range(h):
        for j in range(w):
            if res[i, j] == thres:
                res[i, j] = 255
            else:
                res[i, j] = 0
    return res

def opening(img: np.ndarray, H: StructureElement):
    return dilation(img=erotion(img=img, H=H), H=H.reflect())

def closing(img: np.ndarray, H: StructureElement):
    return erotion(img=dilation(img=img, H=H.reflect()), H=H)

def union(img1: np.ndarray, img2: np.ndarray):
    assert img1.ndim==2 and img2.ndim==2, "Invalid Img Input"
    assert img1.shape==img2.shape, "Img1 Img2 should be same size"
    
    res = np.zeros_like(img1)

    h, w = img1.shape
    for i in range(h):
        for j in range(w):
            if img1[i, j] == 255 or img2[i, j] == 255:
                res[i, j] = 255
            else:
                res[i, j] = 0
    return res

def intersection(img1: np.ndarray, img2: np.ndarray):
    assert img1.ndim==2 and img2.ndim==2, "Invalid Img Input"
    assert img1.shape==img2.shape, "Img1 Img2 should be same size"
    
    res = np.zeros_like(img1)

    h, w = img1.shape
    for i in range(h):
        for j in range(w):
            if img1[i, j] == 255 and img2[i, j] == 255:
                res[i, j] = 255
            else:
                res[i, j] = 0
    return res

def XOR(img1: np.ndarray, img2: np.ndarray):
    assert img1.ndim==2 and img2.ndim==2, "Invalid Img Input"
    assert img1.shape==img2.shape, "Img1 Img2 should be same size"
    
    res = np.zeros_like(img1)

    h, w = img1.shape
    for i in range(h):
        for j in range(w):
            if img1[i, j] == 255 and img2[i, j] == 0 or img1[i, j] == 0 and img2[i, j] == 255:
                res[i, j] = 255
            else:
                res[i, j] = 0
    return res


def boundary_extraction(img: np.ndarray, H: StructureElement):
    assert img.ndim==2 and H.kernel.ndim==2, "Invalid Input"

    return img - erotion(img=img, H=H)

def hole_filling(img: np.ndarray, H:StructureElement, hole_coords:list[tuple]):
    assert img.ndim==2 and H.kernel.ndim==2, "Invalid Input"

    G =  np.zeros_like(img)
    for coords in hole_coords:
        G[coords[0], coords[1]] = 255
    prev_G = G.copy()

    i = 0
    #write_image(f"G_{i}", G, 1, "p1_b")
    while True:
        i += 1
        prev_G = G.copy()
        G = intersection(img1=dilation(img=G, H=H), img2=(255-img))
        #if i % 10 == 0:
            #write_image(f"G_{i}", G, 1, "p1_b")
        if np.sum(np.abs(G - prev_G)) == 0:
            break

    #write_image(f"G_{i}", G, 1, "p1_b")
    return union(img1=G, img2=img), G

def subtractive_operator(img: np.ndarray, kernel: np.ndarray, thres: int):
    assert img.ndim==2, "Invalid Input"
    assert kernel.shape[0]==kernel.shape[1], "Kernel shape should be square"

    res = img.copy()
    h, w = img.shape
    kernel_size = kernel.shape[0]
    padded_img = pad_image(img=img, mask_size=kernel_size)

    for i in range(h):
        for j in range(w):
            if img[i, j] == 0:
                img_window = padded_img[i:i+kernel_size, j:j+kernel_size].astype(np.float64)
                img_window = np.where(img_window, 1, 0)
                cnt = np.sum(img_window*kernel)
                if cnt > thres:
                    res[i, j] = 255
    return res

def connected_component_labeling(img: np.ndarray, threshold: int):
    assert img.ndim==2, "Invalid Input"
    cnt = 0
    
    def bfs(img: np.ndarray, res: np.ndarray, coords:tuple[int, int], cnt: int):
        
        from collections import deque
        h, w = img.shape
        visited = set()
        queue = deque([coords])

        while queue:
            cur = queue.popleft()
            if cur not in visited:
                res[cur[0], cur[1]] = cnt
                visited.add(cur)
                for next in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
                    if 0 <= cur[0]+next[0] < h and 0 <= cur[1]+next[1] < w:
                        if (cur[0]+next[0], cur[1]+next[1]) not in visited and img[cur[0]+next[0], cur[1]+next[1]]==0 and res[cur[0]+next[0], cur[1]+next[1]]==0:
                            queue.append((cur[0]+next[0], cur[1]+next[1]))

    h, w = img.shape
    res = np.zeros_like(img)
    for i in range(h):
        for j in range(w):
            if img[i, j] == 0 and res[i, j] == 0:
                cnt += 1
                bfs(img=img, res=res, coords=(i, j), cnt=cnt)
                if len(np.where(res==cnt)[0]) < threshold:
                    res[res == cnt] = 0
                    cnt -= 1

    # unique_values, value_counts = np.unique(res, return_counts=True)
    # value_counts_dict = dict(zip(unique_values, value_counts))
    # for k, v in value_counts_dict.items():
    #     if v < threshold:
    #         res[res == k] = 0
    return res, cnt


# ---------------------------------------------------------------------------- #
#                                   Dithering                                  #
# ---------------------------------------------------------------------------- #

def generate_dither_matrix(dither_matrix: np.ndarray, target_size: int):

    cur_size = dither_matrix.shape[0]
    res = np.copy(dither_matrix)
    while(cur_size < target_size):
        cur_matrix = np.copy(res)
        res = np.zeros(shape=(2*cur_size, 2*cur_size))
        res[0:cur_size, 0:cur_size] = 4*cur_matrix + dither_matrix[0, 0]
        res[0:cur_size, cur_size:2*cur_size] = 4*cur_matrix + dither_matrix[0, 1]
        res[cur_size:2*cur_size, 0:cur_size] = 4*cur_matrix + dither_matrix[1, 0]
        res[cur_size:2*cur_size, cur_size:2*cur_size] = 4*cur_matrix + dither_matrix[1, 1]
        cur_size *= 2
    return res

def generate_threshold_matrix(dither_matrix: np.ndarray, offset:int=0.5):
    assert dither_matrix.ndim == 2, "Invalid dither matrix"
    assert dither_matrix.shape[0]==dither_matrix.shape[1], "Invalid dither matrix shape"
    
    h, w = dither_matrix.shape
    threshold_matrix = np.zeros_like(dither_matrix, dtype=np.float64)

    for i in range(h):
        for j in range(w):
            threshold_matrix[i, j] = (255 * dither_matrix[i, j] + offset) / (h*w)

    return threshold_matrix

def threshold_image_dithering(img: np.ndarray, threshold_matrix: np.ndarray):
    assert threshold_matrix.ndim == 2, "Invalid threshold matrix"
    assert img.ndim == 2, "Invalid img"

    kernel_size = threshold_matrix.shape[0]
    padded_img = np.copy(img)
    if padded_img.shape[0]%kernel_size != 0:
        padded_img = np.vstack((padded_img, np.zeros((kernel_size-(padded_img.shape[0]%kernel_size), padded_img.shape[1]))))
    if padded_img.shape[1]%kernel_size != 0:
        padded_img = np.hstack((padded_img, np.zeros((padded_img.shape[0], kernel_size-(padded_img.shape[1]%kernel_size)))))

    h, w = padded_img.shape
    threshold = np.tile(threshold_matrix, (h//kernel_size, w//kernel_size))
    
    res = np.zeros_like(padded_img)
    res[padded_img >= threshold] = 255

    return res[0:img.shape[0], 0:img.shape[1]]


# ---------------------------------------------------------------------------- #
#                                Error Diffusion                               #
# ---------------------------------------------------------------------------- #

def error_diffusion(img: np.ndarray, kernel: np.ndarray, threshold: np.float64 = 0.5, serpentine: bool = True):
    assert img.ndim==2 and kernel.ndim==2, "Invalid Input ndim"
    
    h, w = img.shape
    kh, kw = kernel.shape

    normalized_img = pad_image(img=(np.copy(img).astype(np.float64) / 255), mask_size=kw)
    ones = np.ones_like(kernel)
    rev_kernel = np.copy(kernel[:, ::-1])

    dir = 0
    for i in range(h):
        if dir % 2 == 0 or serpentine is False:
            for j in range(w):
                cur_v = normalized_img[i, j+(kw//2)]
                new_v = 0 if cur_v < threshold else 1
                
                window = normalized_img[i:i+kh, j:j+kw]
                error = (cur_v - new_v) * ones * kernel
                window += error
                window[window > 1] = 1.0
                window[window < 0] = 0.0

                normalized_img[i:i+kh, j:j+kw] = window
                normalized_img[i, j+(kw//2)] = new_v
        else:
            for j in range(w-1, -1, -1):
                cur_v = normalized_img[i, j+(kw//2)]
                new_v = 0 if cur_v < threshold else 1
                
                window = normalized_img[i:i+kh, j:j+kw]
                error = (cur_v - new_v) * ones * rev_kernel
                window += error
                window[window > 1] = 1.0
                window[window < 0] = 0.0

                normalized_img[i:i+kh, j:j+kw] = window
                normalized_img[i, j+(kw//2)] = new_v

        dir ^= 1
    
    return unpad_image((normalized_img*255).astype(np.uint8), mask_size=kw)


# ---------------------------------------------------------------------------- #
#                               License Plate OCR                              #
# ---------------------------------------------------------------------------- #


def bounding_rect(img: np.ndarray, label: np.ndarray, cnt: int, type: int):
    assert label.ndim==2, "Invalid Input"

    # Get coord of 4 corners
    h, w = label.shape
    coords = {}
    character_segments = {}
    for k in range(1, cnt+1):
        max_i = -np.Inf
        max_j = -np.Inf
        min_i = np.Inf
        min_j = np.Inf
        indices = np.where(label == k)

        for i in range(len(indices[0])):
            max_i = max(max_i, indices[0][i])
            min_i = min(min_i, indices[0][i])
            max_j = max(max_j, indices[1][i])
            min_j = min(min_j, indices[1][i])
        
        if type==0: coords[k] = (min_i, max_i, min_j-5, max_j+5)
        else: coords[k] = (min_i, max_i, min_j, max_j)
        
    
    for k, v in coords.items():
        # print(v)
        min_i, max_i, min_j, max_j = v
        cropped_char = np.copy(img[min_i:max_i+1, min_j:max_j+1])
        character_segments[k] = cropped_char


    return character_segments

def train(img: np.ndarray):
    # Threshold / Binarize
    thres_img = threshold_image_const(img=img, threshold=50)
    # print(thres_img.shape)
    # unique_pixel_value_count(img=thres_img)


    # Labeling / Coloring
    label_mapping, cnt = connected_component_labeling(img=thres_img, threshold=0)
    colored_img = coloring_components(img=img, res=label_mapping, count=cnt)
    only_digit_img = coloring_components(img=np.zeros_like(img), res=label_mapping, count=cnt)
    # print(f"Train Set character cnt: {cnt}")
    #write_image("Train_Colored", only_digit_img, 1, "p2")

    # Bounding Rect.
    character_segments = bounding_rect(img=thres_img, label=label_mapping, cnt=cnt, type=0)
    #for k, v in character_segments.items():
        #write_image(f"TrainSet_{k}", v, 1, "p2")
    features = get_train_feature(character_segments=character_segments)
    return features

# MSE
def inference(train_feature:dict, inference_feature=list):
    best_k = None
    best_loss = np.Inf
    for k, v in train_feature.items():
        # print(k, v)
        loss = 0
        #for i in range(0, len(v)):
        loss += (v[0] - inference_feature[0])**2 * 1
        loss += (v[1] - inference_feature[1])**2 * 100
        loss += (v[2] - inference_feature[2])**2
        # print(k, loss)
        if loss <= best_loss:
            best_k = k
            best_loss = loss

    return best_k

def bit_quads(img:np.ndarray, kernel: np.ndarray):

    w, h = img.shape
    tmp = np.where(img==255, 1, 0)
    # print(tmp)
    cnt = 0
    for i in range(w-1):
        for j in range(h-1):
            if tmp[i, j] == kernel[0, 0] and tmp[i, j+1] == kernel[0, 1] and tmp[i+1, j] == kernel[1, 0] and tmp[i+1, j+1] == kernel[1, 1]:
                cnt += 1
    
    return cnt

def get_train_feature(character_segments: dict):
    char_set = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '6', '8', '9', '4', '5', '7']
    features = {}
    for c in char_set:
        features[c] = []

    # Bit Quads
    Q0 = [np.array([[0, 0], [0, 0]])]
    Q1 = [np.array([[1, 0], [0, 0]]), np.array([[0, 1], [0, 0]]), np.array([[0, 0], [1, 0]]), np.array([[0, 0], [0, 1]])]
    Q2 = [np.array([[1, 1], [0, 0]]), np.array([[0, 1], [0, 1]]), np.array([[0, 0], [1, 1]]), np.array([[1, 0], [1, 0]])]
    Q3 = [np.array([[1, 1], [0, 1]]), np.array([[0, 1], [1, 1]]), np.array([[1, 0], [1, 1]]), np.array([[1, 1], [1, 0]])]
    Q4 = [np.array([[1, 1], [1, 1]])]
    QD = [np.array([[1, 0], [0, 1]]), np.array([[0, 1], [1, 0]])]

    
    for k, v in character_segments.items():
        nQ0, nQ1, nQ2, nQ3, nQ4, nQD = 0, 0, 0, 0, 0, 0
        for kernel in Q0:
            nQ0 += bit_quads(img=v, kernel=kernel)
        for kernel in Q1:
            nQ1 += bit_quads(img=v, kernel=kernel)
        for kernel in Q2:
            nQ2 += bit_quads(img=v, kernel=kernel)
        for kernel in Q3:
            nQ3 += bit_quads(img=v, kernel=kernel)
        for kernel in Q4:
            nQ4 += bit_quads(img=v, kernel=kernel)
        for kernel in QD:
            nQD += bit_quads(img=v, kernel=kernel)

        # feature[0] : Average Area
        # A = (1/4) * (nQ1 + 2*nQ2 + 3*nQ3 + 4*nQ4 + 2*nQD)
        A = (1/4)*nQ1 + (1/2)*nQ2 + (7/8)*nQ3 + 1*nQ4 + (3/4)*nQD
        features[char_set[k-1]].append(A / (v.shape[0]*v.shape[1]))


        # feature[1] : Euler's number
        E = (1/4) * (nQ1 - nQ3 - 2*nQD)
        features[char_set[k-1]].append(E)

        # feature[2] : Perimeters
        P = nQ2 + (1/np.sqrt(2)) * (nQ1 + nQ3 + 2*nQD)
        features[char_set[k-1]].append(P / (v.shape[0]*v.shape[1]))

    
    return features

def get_inference_feature(img=np.ndarray):

    feature = []

    # Bit Quads
    Q0 = [np.array([[0, 0], [0, 0]])]
    Q1 = [np.array([[1, 0], [0, 0]]), np.array([[0, 1], [0, 0]]), np.array([[0, 0], [1, 0]]), np.array([[0, 0], [0, 1]])]
    Q2 = [np.array([[1, 1], [0, 0]]), np.array([[0, 1], [0, 1]]), np.array([[0, 0], [1, 1]]), np.array([[1, 0], [1, 0]])]
    Q3 = [np.array([[1, 1], [0, 1]]), np.array([[0, 1], [1, 1]]), np.array([[1, 0], [1, 1]]), np.array([[1, 1], [1, 0]])]
    Q4 = [np.array([[1, 1], [1, 1]])]
    QD = [np.array([[1, 0], [0, 1]]), np.array([[0, 1], [1, 0]])]


    nQ0, nQ1, nQ2, nQ3, nQ4, nQD = 0, 0, 0, 0, 0, 0
    for kernel in Q0:
        nQ0 += bit_quads(img=img, kernel=kernel)
    for kernel in Q1:
        nQ1 += bit_quads(img=img, kernel=kernel)
    for kernel in Q2:
        nQ2 += bit_quads(img=img, kernel=kernel)
    for kernel in Q3:
        nQ3 += bit_quads(img=img, kernel=kernel)
    for kernel in Q4:
        nQ4 += bit_quads(img=img, kernel=kernel)
    for kernel in QD:
        nQD += bit_quads(img=img, kernel=kernel)

    # feature[0] : Average Area
    # A = (1/4) * (nQ1 + 2*nQ2 + 3*nQ3 + 4*nQ4 + 2*nQD)
    A = (1/4)*nQ1 + (1/2)*nQ2 + (7/8)*nQ3 + 1*nQ4 + (3/4)*nQD
    feature.append(A / (img.shape[0]*img.shape[1]))


    # feature[1] : Euler's number
    E = (1/4) * (nQ1 - nQ3 - 2*nQD)
    feature.append(E)

    # feature[2] : Perimeters
    P = nQ2 + (1/np.sqrt(2)) * (nQ1 + nQ3 + 2*nQD)
    feature.append(P / (img.shape[0]*img.shape[1]))

    # print(feature)
    return feature

def segment_characters(img: np.ndarray, features: dict):

    # Threshold
    thres_img = threshold_image_const(img=img, threshold=120)

    # Invert to black character white background
    black_cnt = np.count_nonzero(thres_img == 0)
    white_cnt = np.count_nonzero(thres_img == 255)
    # print(black_cnt, white_cnt)
    if(black_cnt > white_cnt): 
        thres_img = 255 - thres_img

    # write_image(f"ConvertBlack_{img[0, 0]}", thres_img, 1, "p2")

    # Fill the background black noise
    h, w = thres_img.shape
    for i in range(h):
        cnt = 0
        while cnt < w and thres_img[i, cnt] == 0:
            thres_img[i, cnt] = 255
            cnt += 1
    for i in range(h):
        cnt = w-1
        while cnt >= 0 and thres_img[i, cnt] == 0:
            thres_img[i, cnt] = 255
            cnt -= 1
    for j in range(w):
        cnt = 0
        while cnt < h and thres_img[cnt, j] == 0:
            thres_img[cnt, j] = 255
            cnt += 1
    for j in range(w):
        cnt = h-1
        while cnt >= 0 and thres_img[cnt, j] == 0:
            thres_img[cnt, j] = 255
            cnt -= 1

    # Morphological Processing
    denoised_img = np.copy(thres_img)
    kernel = np.array([[1, 1, 1, 1, 1], [1, 1, 1, 1, 1], [1, 1, 0, 1, 1], [1, 1, 1, 1, 1], [1, 1, 1, 1, 1]])
    #kernel = np.array([[1, 1, 1], [1, 0, 1], [1, 1, 1]])
    for i in range(40):
        denoised_img = subtractive_operator(img=denoised_img, kernel=kernel, thres=16)

    # write_image(f"Denoised_{img[0, 0]}", denoised_img, 1, "p2")

    # Labeling / Coloring
    label_mapping, cnt = connected_component_labeling(img=denoised_img, threshold=85)
    colored_img = coloring_components(img=img, res=label_mapping, count=cnt)
    only_digit_img = coloring_components(img=np.zeros_like(img), res=label_mapping, count=cnt)
    # print(f"Character cnt: {cnt}")
    
    # Bounding Rect.
    character_segments = bounding_rect(img=denoised_img, label=label_mapping, cnt=cnt, type=1)
    result = ""
    for k, v in character_segments.items():
        
        # write_image(f"{k}", v, 1, "p2")

        # Inference Feature
        inference_feature = get_inference_feature(img=v)
        result += inference(train_feature=features, inference_feature=inference_feature)
        


    print(result)
    return only_digit_img


# ---------------------------------------------------------------------------- #
#                               Frequency Domain                               #
# ---------------------------------------------------------------------------- #
# Reference https://github.com/goolu0623/NTU_2023S_DIP

def discrete_fourier_transform(img: np.ndarray):
    spectrum = np.fft.fft2(img)
    center_spectrum = np.fft.fftshift(spectrum)
    return center_spectrum

def distance(point1, point2):
    return ((point1[0] - point2[0])**2 + (point1[1] - point2[1])**2) **(1/2)

def ideal_lowpass_filter(F, D0):
    rows = len(F)
    cols = len(F[0])
    center = [rows/2, cols/2]
    H = np.zeros((rows, cols))

    for i in range(rows):
        for j in range(cols):
            if(distance([i, j], center) <= D0):
                H[i, j] = 1
    return H

def gaussian_lowpass_filter(F, D0):
    rows = len(F)
    cols = len(F[0])
    center = [rows/2, cols/2]
    H = np.zeros((rows, cols))

    for i in range(rows):
        for j in range(cols):
            H[i, j] = np.exp((-distance([i, j], center)**2 / (2*(D0**2))))
    return H

def butterworth_lowpass_filter(F, D0, n):
    rows = len(F)
    cols = len(F[0])
    center = [rows/2, cols/2]
    H = np.zeros((rows, cols))

    for i in range(rows):
        for j in range(cols):
            H[i, j] = 1/(1 + (distance([i, j], center)/D0)**(2*n))
    return H

def LP(F, D0, n):
    ideal_lowpass = ideal_lowpass_filter(F, D0)*F
    gaussian_lowpass = gaussian_lowpass_filter(F, D0)*F
    butterworth_lowpass = butterworth_lowpass_filter(F, D0, n)*F

    ideal_lowpass_output = np.fft.ifft2(np.fft.ifftshift(ideal_lowpass))
    gaussian_lowpass_output = np.fft.ifft2(np.fft.ifftshift(gaussian_lowpass))
    butterworth_lowpass_output = np.fft.ifft2(np.fft.ifftshift(butterworth_lowpass))

    return np.abs(ideal_lowpass_output), np.abs(gaussian_lowpass_output), np.abs(butterworth_lowpass_output)

# ---------------------------------------------------------------------------- #
#                                   Problem 1                                  #
# ---------------------------------------------------------------------------- #

def p1_a():
    sample1 = read_image("sample1.png", 0)
    dither_matrix = np.array([[1, 2], [3, 0]])
    threshold_matrix = generate_threshold_matrix(dither_matrix=dither_matrix)

    result1 = threshold_image_dithering(img=sample1, threshold_matrix=threshold_matrix)
    write_image("result1", result1, 0)

def p1_b():
    sample1 = read_image("sample1.png", 0)
    dither_matrix = np.array([[1, 2], [3, 0]])
    dither_matrix = generate_dither_matrix(dither_matrix=dither_matrix, target_size=256)
    threshold_matrix = generate_threshold_matrix(dither_matrix=dither_matrix)

    result2 = threshold_image_dithering(img=sample1, threshold_matrix=threshold_matrix)
    write_image("result2", result2, 0)

def p1_c():
    thresholds = [0.5]
    
    sample1 = read_image("sample1.png", 0)
    # Floyd-Steinberg
    kernel = np.array([[0, 0, 7/16],[3/16, 5/16, 1/16]], dtype=np.float64)
    for threshold in thresholds:
        result3 = error_diffusion(img=sample1, kernel=kernel, threshold=threshold, serpentine=True)
        write_image("result3", result3, 0)
        #write_image(f"result3_Floyd-Steinberg_serpentine_{threshold}", result3, 1, "p1_c")
        #result3 = error_diffusion(img=sample1, kernel=kernel, threshold=threshold, serpentine=False)
        #write_image(f"result3_Floyd-Steinberg_{threshold}", result3, 1, "p1_c")

    # Jarvis’
    kernel = (1/48) * np.array([[0, 0, 0, 7, 5],[3, 5, 7, 5, 3],[1, 3, 5, 3, 1]], dtype=np.float64)
    for threshold in thresholds:
        result4 = error_diffusion(img=sample1, kernel=kernel, threshold=threshold, serpentine=True)
        write_image("result4", result4, 0)
        #write_image(f"result3_Jarvis_serpentine_{threshold}", result3, 1, "p1_c")
        #result4 = error_diffusion(img=sample1, kernel=kernel, threshold=threshold, serpentine=False)
        #write_image(f"result3_Jarvis_{threshold}", result3, 1, "p1_c")

    # Atkinson
    # kernel = (1/8) * np.array([[0, 0, 0, 1, 1],[0, 1, 1, 1, 0],[0, 0, 1, 0, 0]], dtype=np.float64)
    # for threshold in thresholds:
    #     result3 = error_diffusion(img=sample1, kernel=kernel, threshold=threshold, serpentine=True)
    #     write_image(f"result3_Atkinson_serpentine_{threshold}", result3, 1, "p1_c")
    #     result3 = error_diffusion(img=sample1, kernel=kernel, threshold=threshold, serpentine=False)
    #     write_image(f"result3_Atkinson_{threshold}", result3, 1, "p1_c")

    # Burkes
    # kernel = (1/32) * np.array([[0, 0, 0, 8, 4],[2, 4, 8, 4, 2]], dtype=np.float64)
    # for threshold in thresholds:
    #     result3 = error_diffusion(img=sample1, kernel=kernel, threshold=threshold, serpentine=True)
    #     write_image(f"result3_Burkes_serpentine_{threshold}", result3, 1, "p1_c")
    #     result3 = error_diffusion(img=sample1, kernel=kernel, threshold=threshold, serpentine=False)
    #     write_image(f"result3_Burkes_{threshold}", result3, 1, "p1_c")

    # Sierra
    # kernel = (1/32) * np.array([[0, 0, 0, 5, 3],[2, 4, 5, 4, 2], [0, 2, 3, 2, 0]], dtype=np.float64)
    # for threshold in thresholds:
    #     result3 = error_diffusion(img=sample1, kernel=kernel, threshold=threshold, serpentine=True)
    #     write_image(f"result3_Sierra_serpentine_{threshold}", result3, 1, "p1_c")
    #     result3 = error_diffusion(img=sample1, kernel=kernel, threshold=threshold, serpentine=False)
    #     write_image(f"result3_Sierra_{threshold}", result3, 1, "p1_c")

# ---------------------------------------------------------------------------- #
#                                   Problem 2                                  #
# ---------------------------------------------------------------------------- #

def p2():
    train_img = read_image("TrainingSet.png", 0)
    features = train(train_img)

    sample2 = read_image("sample2.png", 0)
    result5 = segment_characters(img=sample2, features=features)
    #write_image("result5", result5, 0)
    sample2 = read_image("sample3.png", 0)
    result5 = segment_characters(img=sample2, features=features)
    #write_image("result6", result5, 0)
    sample2 = read_image("sample4.png", 0)
    result5 = segment_characters(img=sample2, features=features)
    #write_image("result7", result5, 0)
    
    

# ---------------------------------------------------------------------------- #
#                                   Problem 3                                  #
# ---------------------------------------------------------------------------- #

def p3():
    sample5 = read_image("sample5.png", target=0)
    sample5_frequency = discrete_fourier_transform(img=sample5)

    # sample5_log_frequency = np.log(1 + np.abs(sample5_frequency))
    # write_image("sample5_log_frequency", sample5_log_frequency * 10, 1, "p3")

    # ideal_lowpass_output_25, gaussian_lowpass_output_25, butterworth_lowpass_output_25 = LP(sample5_frequency, 25, 2)
    # ideal_lowpass_output_40, gaussian_lowpass_output_40, butterworth_lowpass_output_40 = LP(sample5_frequency, 40, 2)
    # ideal_lowpass_output_55, gaussian_lowpass_output_55, butterworth_lowpass_output_55 = LP(sample5_frequency, 55, 2)
    ideal_lowpass_output_70, gaussian_lowpass_output_70, butterworth_lowpass_output_70 = LP(sample5_frequency, 70, 2)

    # write_image("ideal_lowpass_output_25", ideal_lowpass_output_25, 1, "p3")
    # write_image("ideal_lowpass_output_40", ideal_lowpass_output_40, 1, "p3")
    # write_image("ideal_lowpass_output_55", ideal_lowpass_output_55, 1, "p3")
    # write_image("ideal_lowpass_output_70", ideal_lowpass_output_70, 1, "p3")

    # write_image("gaussian_lowpass_output_25", gaussian_lowpass_output_25, 1, "p3")
    # write_image("gaussian_lowpass_output_40", gaussian_lowpass_output_40, 1, "p3")
    # write_image("gaussian_lowpass_output_55", gaussian_lowpass_output_55, 1, "p3")
    # write_image("gaussian_lowpass_output_70", gaussian_lowpass_output_70, 1, "p3")

    # write_image("butterworth_lowpass_output_25", butterworth_lowpass_output_25, 1, "p3")
    # write_image("butterworth_lowpass_output_40", butterworth_lowpass_output_40, 1, "p3")
    # write_image("butterworth_lowpass_output_55", butterworth_lowpass_output_55, 1, "p3")
    # write_image("butterworth_lowpass_output_70", butterworth_lowpass_output_70, 1, "p3")

    write_image("result5", ideal_lowpass_output_70, 0)

    # result5_frequency = discrete_fourier_transform(img=ideal_lowpass_output_70)

    # result5__log_frequency = np.log(1 + np.abs(result5_frequency))
    # write_image("ideal_log_frequency_70", result5__log_frequency * 10, 1, "p3")


# ---------------------------------------------------------------------------- #
#                                 Main Function                                #
# ---------------------------------------------------------------------------- #

if __name__ == "__main__":
    np.set_printoptions(threshold=np.inf)
    np.random.seed(1006)
    p1_a()
    p1_b()
    p1_c()

    p2()
    p3()