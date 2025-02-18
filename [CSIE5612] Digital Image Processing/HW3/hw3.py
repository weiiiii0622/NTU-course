import argparse
# import os
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
# TEST = False


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
#     else:
#         clear_output("./report")

# ---------------------------------------------------------------------------- #
#                                   Utilties                                   #
# ---------------------------------------------------------------------------- #

def unique_pixel_value_count(img: np.ndarray):
    unique_numbers, counts = np.unique(img, return_counts=True)
    number_count_dict = dict(zip(unique_numbers, counts))
    print(number_count_dict)
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

def threshold_image(img: np.ndarray, threshold: np.float64):
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

def k_means_clustering(features:np.ndarray, k:int, max_itertions:int=500):
    assert features.ndim==3, "features shape should be (dimension, i, j)"
    assert k >= 1, "k should be positive"

    dimension, h, w = features.shape

    # data_flattened[idx] = features[:, i, j] (idx = i*w+j)
    data_flattened = features.reshape(dimension, -1).T

    centroids_indices = np.random.choice(len(data_flattened), k, replace=False)
    centroids = data_flattened[centroids_indices]
    for _ in range(max_itertions):
        labels = np.argmin(np.linalg.norm(data_flattened[:, None] - centroids, axis=-1), axis=-1)
        new_centroids = np.array([np.mean(data_flattened[labels == i], axis=0) for i in range(k)], dtype=np.float32)
        
        if np.allclose(new_centroids, centroids):
            break
        centroids = new_centroids.copy()
    
    return labels, centroids


# ---------------------------------------------------------------------------- #
#                          Spatial / Median Filtering                          #
# ---------------------------------------------------------------------------- #

def generate_3x3_lowpass_filter(mid_weight: np.float64):
    return (1/(mid_weight+2)**2) * np.array([[1, mid_weight, 1], [mid_weight, mid_weight**2, mid_weight], [1, mid_weight, 1]], dtype=np.float32)

def generate_3x3_laplacian_filter():
    return (1/8)*np.array([[-1, -1, -1], [-1, 8, -1], [-1, -1, -1]], dtype=np.float64)

def generate_5x5_lowpass_filter():
    arr = np.array([1,4,6,4,1], dtype=np.uint32).reshape(1, -1)
    return (1/256) * np.dot(arr.T, arr)

def median_filtering(img, kernel_size: int, epsilon: np.float64 = 0.5):
    assert kernel_size%2==1, "Kernel size should be odd"
    h, w = img.shape
    result_img = np.zeros((h, w), dtype=np.uint8)
    padded_img = pad_image(img, mask_size=kernel_size)
    laplacian_filter_3x3 = generate_3x3_laplacian_filter()

    for i in range(h):
        for j in range(w):
            if kernel_size == 3:
                k = np.clip(abs(np.sum(padded_img[i:i+kernel_size, j:j+kernel_size] * laplacian_filter_3x3) / epsilon), 0, 1)
                res = np.round((1-k) * img[i, j] + k * np.round(np.median(padded_img[i:i+kernel_size, j:j+kernel_size])))
                result_img[i, j] = res
            else:
                result_img[i, j] = np.round(np.median(padded_img[i:i+kernel_size, j:j+kernel_size]))
    return result_img


def spacial_filtering(img, filter):
    mask_size, _ = filter.shape
    assert mask_size==_, "Filter should be square matrix"
    assert mask_size%2==1, "Filter size should be odd"

    h, w = img.shape
    result_img = np.zeros((h, w), dtype=np.uint8)
    padded_img = pad_image(img, mask_size=mask_size)

    for i in range(h):
        for j in range(w):
            result_img[i, j] = np.round(np.sum(filter * padded_img[i:i+mask_size, j:j+mask_size]))
    return result_img

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
    # write_image(f"G_{i}", G, 1, "p1_b")
    while True:
        i += 1
        prev_G = G.copy()
        G = intersection(img1=dilation(img=G, H=H), img2=(255-img))
        # if i % 10 == 0:
            # write_image(f"G_{i}", G, 1, "p1_b")
        if np.sum(np.abs(G - prev_G)) == 0:
            break

    # write_image(f"G_{i}", G, 1, "p1_b")
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
            if img[i, j] == 255:
                img_window = padded_img[i:i+kernel_size, j:j+kernel_size].astype(np.float64)
                img_window = np.where(img_window, 1, 0)
                cnt = np.sum(img_window*kernel)
                if cnt < thres:
                    res[i, j] = 0
    return res

def connected_component_labeling(img: np.ndarray, H:StructureElement):
    assert img.ndim==2 and H.kernel.ndim==2, "Invalid Input"
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
                        if (cur[0]+next[0], cur[1]+next[1]) not in visited and img[cur[0]+next[0], cur[1]+next[1]]==255 and res[cur[0]+next[0], cur[1]+next[1]]==0:
                            queue.append((cur[0]+next[0], cur[1]+next[1]))

    h, w = img.shape
    res = np.zeros_like(img)
    for i in range(h):
        for j in range(w):
            if img[i, j] == 255 and res[i, j] == 0:
                cnt += 1
                bfs(img=img, res=res, coords=(i, j), cnt=cnt)
    return res, cnt


# ---------------------------------------------------------------------------- #
#                               Texture Analysis                               #
# ---------------------------------------------------------------------------- #

def generate_3x3_micro_structure_kernel(filter: np.ndarray):
    L_3 = np.array([[1,2,1]], dtype=np.float32)
    E_3 = np.array([[-1,0,1]], dtype=np.float32)
    S_3 = np.array([[1,-2,1]], dtype=np.float32)
    kernels = [L_3.T*L_3, L_3.T*E_3, L_3.T*S_3, E_3.T*L_3, E_3.T*E_3, E_3.T*S_3, S_3.T*L_3, S_3.T*E_3, S_3.T*S_3]
    kernels = np.array(kernels)
    return kernels[filter]

def generate_5x5_micro_structure_kernel(filter: np.ndarray):
    pass

def texture_analysis_laws_method(img: np.ndarray, kernel_size: int, energy_mask_size: int, energy_strategy: Literal["squared_sum", "absolute_sum", "mean"], kmeans_k: int):
    assert img.ndim==2, "Invalid Image Input"
    assert kernel_size%2==1 and energy_mask_size%2==1, "Kernel and Mask size should be odd"

    h, w = img.shape

    # Gerneate Micro-structre imoulse response kernels
    filter = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8], dtype=np.int32)
    kernels_num = len(filter)
    if kernel_size == 3: 
        kernels = generate_3x3_micro_structure_kernel(filter=filter)
    elif kernel_size == 5:
        kernels = generate_5x5_micro_structure_kernel(filter=filter)
    else:
        raise ValueError("Kernel size should be 3 or 5 currently")
    
    # Compute M
    M = np.zeros(shape=(kernels_num, h, w))
    for i in range(kernels_num):
        M[i] = convolve_2D_kernel(img=img, kernel=kernels[i])
        M_normalized = np.interp(M[i], (M[i].min(), M[i].max()), (0, 255)).astype(np.uint8)
        # write_image(f"M_{filter[i]+1}(k={kernel_size})", M_normalized, 1, "p2_a")

    # Compute Energy
    T = np.zeros(shape=(kernels_num, h, w))
    for k in range(kernels_num):
        energy_mask = np.ones(shape=(energy_mask_size, energy_mask_size))
        res = np.zeros_like(M[k]).astype(np.float64)
        h, w = M[k].shape
        padded_img = pad_image(M[k], energy_mask_size)
        for i in range(h):
            for j in range(w):
                img_window = padded_img[i:i+energy_mask_size, j:j+energy_mask_size].astype(np.float64)
                if energy_strategy == "absolute_sum":
                    res[i, j] = np.sum(np.abs(img_window * energy_mask))
                elif energy_strategy == "squared_sum":
                    res[i, j] = np.sum((img_window * energy_mask)**2)
                elif energy_strategy == "mean":
                    res[i, j] = np.mean((img_window * energy_mask))
                else:
                    raise ValueError(f"Unimplemented energy computing strategy: {energy_strategy}")
        
        T[k] = res
        T_normalized = np.interp(T[k], (T[k].min(), T[k].max()), (0, 255)).astype(np.uint8)
        # write_image(f"T_{filter[k]+1}(k={kernel_size}, {energy_strategy})", T_normalized, 1, "p2_a")

    # K-means clustering
    labels, centroids = k_means_clustering(features=T, k=kmeans_k)
    return labels, centroids


# ---------------------------------------------------------------------------- #
#                               Texture Synthesis                              #
# ---------------------------------------------------------------------------- #

       
def texture_transfer(img: np.ndarray, cluster: np.ndarray, kmeans_k: int):
    assert kmeans_k >= 1, "k should be positive"

    wood = read_image("wood.jpg", 0, options=cv2.IMREAD_COLOR)
    raindrop = read_image("raindrop.jpg", 0, options=cv2.IMREAD_COLOR)
    rock = read_image("rock.jpg", 0, options=cv2.IMREAD_COLOR)
    wool = read_image("wool.jpg", 0, options=cv2.IMREAD_COLOR)
    rugs = read_image("rugs.jpg", 0, options=cv2.IMREAD_COLOR)

    h, w = img.shape
    res = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)

    textures = [raindrop, wood, wool, rock]
    labels = [cluster[50, 70], cluster[160, 260], cluster[320, 110], cluster[380, 100]]
    for i in range(h):
        for j in range(w):
            k = np.where(labels == cluster[i, j])[0][0]
            res[i, j] = textures[k][i, j]
    return res

def get_patch_random(img: np.ndarray, patch_len: int):
    h, w, _ = img.shape
    # i = np.random.randint(h - patch_len)
    # j = np.random.randint(w - patch_len)
    i = 0
    j = 0
    return img[i:i+patch_len, j:j+patch_len]

def get_overlap_error(patch: np.ndarray, res: np.ndarray, patch_len: int, overlap_len: int, y: int, x: int):
    error = 0
    if x > 0:
        error += np.sum((patch[:, :overlap_len] - res[y:y+patch_len, x:x+overlap_len])**2)
    if y > 0:
        error += np.sum((patch[:overlap_len, :] - res[y:y+overlap_len, x:x+patch_len])**2)
    if x > 0 and y > 0:
        error -= np.sum((patch[:overlap_len, :overlap_len] - res[y:y+overlap_len, x:x+overlap_len])**2)
    return error

def get_patch_best(img: np.ndarray, res: np.ndarray, patch_len: int, overlap_len: int, y:int, x:int):
    h, w, _ = img.shape
    err = np.full((h - patch_len, w - patch_len), np.inf, dtype=np.float64)
    for i in range(0, h - patch_len, patch_len//10):
        for j in range(0, w - patch_len, patch_len//10):
            patch = img[i:i+patch_len, j:j+patch_len]
            e = get_overlap_error(patch=patch, res=res, patch_len=patch_len, overlap_len=overlap_len, y=y, x=x)
            err[i, j] = e
    i, j = np.unravel_index(np.argmin(err), err.shape)
    return img[i:i+patch_len, j:j+patch_len]

def get_min_cut_path(errors):
    from collections import defaultdict
    import heapq
    pq = [(err, [idx]) for idx, err in enumerate(errors[0])]
    heapq.heapify(pq)

    h, w = errors.shape
    visited = defaultdict(lambda: False)

    while pq:
        err, path = heapq.heappop(pq)
        cur_depth = len(path) - 1
        cur_idx = path[-1]

        if cur_depth == h - 1:
            return path

        if visited[(cur_depth, cur_idx)] is False:
            visited[(cur_depth, cur_idx)] = True
            for next_dir in [-1, 0, 1]:
                next_idx = cur_idx + next_dir
                if 0 <= next_idx < w:
                    heapq.heappush(pq, (err+errors[cur_depth+1, next_idx], path+[next_idx]))
    # Should Not be here
    return []
                


def get_min_cut_boundary(patch: np.ndarray, res: np.ndarray, patch_len: int, overlap_len: int, y:int, x:int):
    patch = patch.copy()
    min_cut_boundary = np.zeros_like(patch, dtype=bool)

    if x > 0:
        left_overlap_error = np.sum((patch[:, :overlap_len] - res[y:y+patch_len, x:x+overlap_len])**2, axis=2)
        min_cut_path = get_min_cut_path(left_overlap_error)
        for i, j in enumerate(min_cut_path):
            min_cut_boundary[i, :j] = True
    if y > 0:
        left_overlap_error = np.sum((patch[:overlap_len, :] - res[y:y+overlap_len, x:x+patch_len])**2, axis=2)
        # print(left_overlap_error.shape)
        min_cut_path = get_min_cut_path(left_overlap_error.T)
        for i, j in enumerate(min_cut_path):
            min_cut_boundary[:i, j] = True

    np.copyto(patch, res[y:y+patch_len, x:x+patch_len], where=min_cut_boundary)
    return patch
    

# Reference https://github.com/axu2/image-quilting/tree/master
def quilt_texture_synthesis(img: np.ndarray, patch_len: int, overlap_ratio: np.int32, patch_num: tuple[int,int], strategy:Literal["random", "best", "best_min_cut"], extend:bool=False):
    assert img.ndim==3, "Input image should be in RGB or BGR"
    assert patch_len>0 and overlap_ratio>0, "Patch length and overlap ratio should be positive"
    assert extend is True or extend is False and patch_num[0] > 0 and patch_num[1] > 0, "Patch number should be positive in non-extending mode"

    overlap_len = patch_len // overlap_ratio
    h = (patch_num[0] * patch_len) - (patch_num[0] - 1) * overlap_len
    w = (patch_num[1] * patch_len) - (patch_num[1] - 1) * overlap_len
    offset_h, offset_w = 0, 0

    if extend:
        h = img.shape[0] + h
        w = img.shape[1] + w
        offset_h, offset_w = img.shape[0], img.shape[1]

    res = np.zeros(shape=(h, w, img.shape[2]), dtype=np.uint8)
    
    # copy the original image
    if extend:
        res[:img.shape[0], :img.shape[1]] = img
        

    coordinates = [(i, j) for i in range(patch_num[0]) for j in range(patch_num[1])]
    for idx, coord in tqdm(iterable=enumerate(coordinates), total=len(coordinates), colour="green"):
        i, j = coord
        y, x = offset_h + i * (patch_len - overlap_len), offset_w + j * (patch_len - overlap_len)

        if extend is True and y < img.shape[0] and x < img.shape[1]:
            # original image
            continue

        if y == 0 and x == 0 or strategy == "random":
            patch = get_patch_random(img=img, patch_len=patch_len)
        elif strategy == "best":
            patch = get_patch_best(img=img, res=res, patch_len=patch_len, overlap_len=overlap_len, y=y, x=x)
        elif strategy == "best_min_cut":
            patch = get_patch_best(img=img, res=res, patch_len=patch_len, overlap_len=overlap_len, y=y, x=x)
            patch = get_min_cut_boundary(patch=patch, res=res, patch_len=patch_len, overlap_len=overlap_len, y=y, x=x)
        else:
            raise ValueError("Invalid Quilting Strategy")
        res[y:y+patch_len, x:x+patch_len] = patch
            
    
    return res
    


# ---------------------------------------------------------------------------- #
#                                   Problem 1                                  #
# ---------------------------------------------------------------------------- #

def p1_a():
    sample1 = read_image("sample1.png", 0)

    # kernel = np.array([[1, 1, 1], [1, 1, 1], [1, 1, 1]])
    # H = StructureElement(kernel=kernel, origin=(1, 1))
    # result1 = boundary_extraction(img=sample1, H=H)
    # write_image("result1_no_opening", result1, 1, "p1_a")

    # Do a opening first
    kernel = np.array([[0, 1, 0], [1, 1, 1], [0, 1, 0]])
    H = StructureElement(kernel=kernel, origin=(0, 0))
    opened_sample1 = opening(img=sample1, H=H)

    kernel = np.array([[1, 1, 1], [1, 1, 1], [1, 1, 1]])
    H = StructureElement(kernel=kernel, origin=(1, 1))
    result1 = boundary_extraction(img=opened_sample1, H=H)
    write_image("result1", result1)

def p1_b():

    sample1 = read_image("sample1.png", 0)
    kernel = np.array([[0, 1, 0], [1, 1, 1], [0, 1, 0]])
    H = StructureElement(kernel=kernel, origin=(1, 1))
    hole_coords = [
        (109, 189), (109, 220), (114, 236), (102, 287), (110, 302),
        (113, 338), (119, 355), (110, 404), (98, 441), (111, 477),
        (250, 385), (317, 523), (369, 360), (321, 294), (307, 292), 
        (330, 350), (369, 255), (350, 205), (395, 269), (389, 270), 
        (428, 266), (436, 472), (432, 511), (448, 470), (447, 501),
        (460, 480), (473, 541), (495, 483), (501, 484), (317, 525),
    ]

    # sample1_feature_points = draw_red_dots(image=sample1, coordinates=hole_coords)
    # write_image("sample1_feature_points", sample1_feature_points, 1, "p1_b")

    result2, G = hole_filling(img=sample1, H=H, hole_coords=hole_coords)
    # write_image("result2_no_opening", result2, 1, "p1_b")

    # Remove Noise
    kernel = np.array([[1, 1, 1, 1, 1], [1, 1, 1, 1, 1], [1, 1, 0, 1, 1], [1, 1, 1, 1, 1], [1, 1, 1, 1, 1]])
    for i in range(1):
        result2 = subtractive_operator(img=result2, kernel=kernel, thres=8)
    write_image("result2", result2)

def p1_c():
    sample1 = read_image("sample1.png", 0)

    # HW1 Method: Median Filtering / Spacial Filtering
    # lowpass_filter_5x5 = generate_5x5_lowpass_filter()
    # result3_spacial = spacial_filtering(sample1, lowpass_filter_5x5)
    # result3_median_k_3 = median_filtering(sample1, kernel_size=3)
    # result3_median_k_3x2 = median_filtering(result3_median_k_3, kernel_size=3)
    # result3_median_k_5 = median_filtering(sample1, kernel_size=5)

    # write_image("result3_spacial", result3_spacial, 1, "p1_c")
    # write_image("result3_median_k_3", result3_median_k_3, 1, "p1_c")
    # write_image("result3_median_k_5", result3_median_k_5, 1, "p1_c")
    # write_image("result3_median_k_3x2", result3_median_k_3x2, 1, "p1_c")

    # Opening
    # kernel = np.array([[1, 1, 0], [1, 1, 0], [1, 0, 0]])
    # H = StructureElement(kernel=kernel, origin=(1, 1))
    # result3_opening = opening(img=sample1, H=H)

    # write_image("result3_opening", result3_opening, 1, "p1_c")

    # Subtractive Operator 3x3
    # kernel = np.array([[1, 1, 1], [1, 0, 1], [1, 1, 1]])
    # result3_hole_sub = sample1.copy()
    # result3_hole_sub = subtractive_operator(img=result3_hole_sub, kernel=kernel, thres=3)
    # write_image("result3_hole_sub_1", result3_hole_sub, 1, "p1_c")
    # for i in range(11):
    #     result3_hole_sub = subtractive_operator(img=result3_hole_sub, kernel=kernel, thres=3)
    # write_image("result3_hole_sub", result3_hole_sub, 1, "p1_c")
    
    # Subtractive Operator 5x5
    kernel = np.array([[1, 1, 1, 1, 1], [1, 1, 1, 1, 1], [1, 1, 0, 1, 1], [1, 1, 1, 1, 1], [1, 1, 1, 1, 1]])
    result3_hole_sub = sample1.copy()
    for i in range(1):
        result3_hole_sub = subtractive_operator(img=result3_hole_sub, kernel=kernel, thres=8)
    # write_image("result3_hole_sub", result3_hole_sub, 1, "p1_c")

    write_image("result3", result3_hole_sub)

def p1_d():
    sample1 = read_image("sample1.png", 0)
    result2 = read_image("result2.png", 1)
    result4 = result2.copy()

    # Remove Noise (Optional)
    kernel = np.array([[1, 1, 1, 1, 1], [1, 1, 1, 1, 1], [1, 1, 0, 1, 1], [1, 1, 1, 1, 1], [1, 1, 1, 1, 1]])
    for i in range(1):
        result4 = subtractive_operator(img=result4, kernel=kernel, thres=8)
    # write_image("result2_denoise", result4, 1, "p1_d")

    # Connected Component Labeling
    kernel = np.array([[1, 1, 1], [1, 1, 1], [1, 1, 1]])
    H = StructureElement(kernel=kernel, origin=(1, 1))
    res, cnt = connected_component_labeling(img=result4, H=H)
    result4 = coloring_components(img=result4, res=res, count=cnt)
    # write_image("result2_colored", result4, 1, "p1_d")

    # Restore black lines
    h, w = sample1.shape
    for i in range(h):
        for j in range(w):
            if sample1[i, j] == 0:
                result4[i, j] = [0, 0, 0]
    # write_image("result2_final", result4, 1, "p1_d")

# ---------------------------------------------------------------------------- #
#                                   Problem 2                                  #
# ---------------------------------------------------------------------------- #

def p2():

    # Parameters
    # kernel_size = [3]
    # energy_mask_size = [3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29]
    # energy_strategy = ["squared_sum","absolute_sum", "mean"]
    # kmeans_k = [3, 4, 5]
    kernel_size = [3]
    energy_mask_size = [23]
    energy_strategy = ["mean"]
    kmeans_k = [4]

    parameters, total = generate_parameters_pairs([kernel_size, energy_mask_size, energy_strategy, kmeans_k])

    sample2 = read_image("sample2.png", 0)
    for idx, p in tqdm(iterable=enumerate(parameters), total=total, colour="green"):
        labels, centroids = texture_analysis_laws_method(img=sample2, kernel_size=p[0], energy_mask_size=p[1], energy_strategy=p[2], kmeans_k=p[3])
        labels += 1
        result4 = coloring_components(img=sample2, res=labels.reshape(sample2.shape[0], sample2.shape[1]), count=np.max(kmeans_k))
        # write_image(f"result4_(kernel={p[0]},mask={p[1]},{p[2]},k_means={p[3]})", result4, 1, "p2_b")
        result5 = texture_transfer(img=sample2, cluster=labels.reshape(sample2.shape[0], sample2.shape[1]), kmeans_k=p[3])
    
    write_image("result4", result4)
    write_image("result5", result5)


def p2_d():
    # parameters
    patch_len = [200]
    overlap_ratio = [5]
    patch_num_h = 2
    patch_num_w = 2
    strategy = "best_min_cut"

    parameters, total = generate_parameters_pairs([patch_len, overlap_ratio])


    sample3 = read_image("sample3.png", 0, options=cv2.IMREAD_COLOR)
    for idx, p in tqdm(iterable=enumerate(parameters), total=total, colour="green"):
        result6 = quilt_texture_synthesis(img=sample3, patch_len=p[0], overlap_ratio=p[1], patch_num=(patch_num_h*sample3.shape[0]//(p[0]-(p[0]//p[1])), patch_num_w*sample3.shape[1]//(p[0]-(p[0]//p[1]))), strategy=strategy, extend=False)
        # write_image(f"result6(patch={p[0]}, overlap_ratio={p[1]})", result6, 1, "p2_d")
    
    write_image("result6", result6)

# ---------------------------------------------------------------------------- #
#                                 Main Function                                #
# ---------------------------------------------------------------------------- #

if __name__ == "__main__":
    np.set_printoptions(threshold=np.inf)
    np.random.seed(1006)
    p1_a()
    p1_b()
    p1_c()
    # p1_d()

    p2()
    p2_d()