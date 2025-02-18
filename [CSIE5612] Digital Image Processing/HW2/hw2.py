import argparse
import os
import itertools
import numpy as np
import cv2

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
#     else:
#         clear_output("./report")

# ---------------------------------------------------------------------------- #
#                                   Utilties                                   #
# ---------------------------------------------------------------------------- #

# def draw_red_dots(image, coordinates, output_path):
#     # Convert grayscale image to RGB
#     rgb_image = cv2.cvtColor(image, cv2.COLOR_GRAY2RGB)

#     # Draw red dots on the image at specified coordinates
#     for coord in coordinates:
#         x, y = coord
#         cv2.circle(rgb_image, (y, x), 5, (0, 0, 255), -1)  # Red color

#     # Save the image
#     cv2.imwrite(output_path, rgb_image)


# def plot_support_axis(img:np.array):
#     res = np.copy(img)
#     h, w = res.shape
#     for i in range(h):
#         cv2.line(res, (int(0),int(100*i)), (int(w-1),int(100*i)), (0, 0, 0))
#     for j in range(w):
#         cv2.line(res, (int(100*j),int(0)), (int(100*j),int(h-1)), (0, 0, 0))
#     return res

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
        = cv2.IMREAD_COLOR      : BGR (default)
        = cv2.IMREAD_GRAYSCALE  : GrayScale

@output
    img
'''
def read_image(path: str, target: int, options=cv2.IMREAD_COLOR):
    if target == 0:
        path = args.input+path
    elif target == 1:
        path = args.output+path
    else:
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
    return np.lib.pad(img, (mask_size-1)//2, 'reflect')

def unpad_image(img: np.ndarray, mask_size: int):
    assert mask_size%2==1, "Mask size should be odd"
    h, w = img.shape
    border_size = (mask_size-1)//2
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


# ---------------------------------------------------------------------------- #
#                              Sobel Kernel (k*k)                              #
# ---------------------------------------------------------------------------- #

def generate_kxk_sobel_kernel(size:int=3):
    assert size%2==1, "Sobel Kernel size must be odd"
    k = size // 2
    indices = np.arange(-k, k + 1)
    x, y = np.meshgrid(indices, indices)
    dist = x**2 + y**2
    dist[k, k] = 1
    
    Gx = x / dist
    Gy = -y / dist
    return Gx, Gy

# orientation in radius
def get_gradient_magnitude_orientation(img:np.ndarray, Gx: np.ndarray, Gy: np.ndarray):
    gradient_x = convolve_2D_kernel(img, Gx)
    gradient_y = convolve_2D_kernel(img, Gy)
    magnitude = np.sqrt(gradient_x.astype(np.float64)**2, gradient_y.astype(np.float64)**2)
    orientation = np.arctan2(gradient_y, gradient_x)
    return magnitude, orientation 


# ---------------------------------------------------------------------------- #
#                             Gaussian Kernel (k*k)                            #
# ---------------------------------------------------------------------------- #
def generate_kxk_gaussian_kernel(size:int=3, sigma:np.float64=1.0):
    assert size%2==1 and size>0, "Gaussian Kernel size must be odd and positive"
    x, y = np.meshgrid(np.arange(-size // 2 + 1, size // 2 + 1), np.arange(-size // 2 + 1, size // 2 + 1))
    kernel = np.exp(-(x**2 + y**2) / (2 * sigma**2))
    kernel /= np.sum(kernel)
    # print(kernel)
    return kernel

# ---------------------------------------------------------------------------- #
#                             Canny Edge Detection                             #
# ---------------------------------------------------------------------------- #

def non_maximal_surpression(gradient_magnitude:np.ndarray, gradient_orientation:np.ndarray):
    assert np.all((gradient_orientation >= -180.0) & (gradient_orientation <= 180.0)), "Gradient orientation must be in range [-180.0, 180.0]"
    assert gradient_magnitude.ndim==2, "Image should be in grayscale"

    def get_dir(degree: np.float64):
        if degree == -180.0 or  degree == 180.0     : return ( 0, -1), ( 0,  1)
        if degree ==  -90.0                         : return ( 1,  0), (-1,  0)
        if degree ==    0.0                         : return ( 0,  1), ( 0, -1)
        if degree ==   90.0                         : return (-1,  0), ( 1,  0)
        if degree >  -180.0 and degree <  -90.0     : return ( 1, -1), (-1,  1)
        if degree >   -90.0 and degree <    0.0     : return ( 1,  1), (-1, -1)
        if degree >     0.0 and degree <   90.0     : return (-1,  1), ( 1, -1)
        if degree >    90.0 and degree <  180.0     : return (-1, -1), ( 1,  1)

    h, w = gradient_magnitude.shape
    res = np.zeros_like(gradient_magnitude)
    padded_gradient_magnitude = pad_image(img=gradient_magnitude, mask_size=3)
    for i in range(h):
        for j in range(w):
            center_i, center_j = i+1, j+1
            d1, d2 = get_dir(gradient_orientation[i, j])
            if padded_gradient_magnitude[center_i, center_j] > padded_gradient_magnitude[center_i+d1[0], center_j+d1[1]] and padded_gradient_magnitude[center_i, center_j] > padded_gradient_magnitude[center_i+d2[0], center_j+d2[1]]:
                res[i, j] = padded_gradient_magnitude[center_i, center_j]
    return res

# @return list[0|1|2] (0: non-edge / 1: weak-edge / 2: strong-edge)
def hysterestic_thresholding(img:np.ndarray, Tl:np.float64, Th:np.float64) -> list[0|1|2]:
    assert Tl <= Th, "Tl must not be greater than Th"

    res = np.zeros_like(img)
    res_bin = np.zeros_like(img)
    h, w = img.shape
    for i in range(h):
        for j in range(w):
            if img[i, j] >= Th:
                res[i, j] = 2
                res_bin[i, j] = 255
            elif img[i, j] >= Tl:
                res[i, j] = 1
                res_bin[i, j] = 127
    return res, res_bin

# DSU-based, 8 connectivity
# @param edge_map:list[0|1|2] (0: non-edge / 1: weak-edge / 2: strong-edge)
def connected_component_labeling(edge_map: np.ndarray):
    def isValid(i, j, h, w):
        return True if (i>=0 and i<h and j>=0 and j<w) else False
    def find(x:tuple[int,int], p:dict[tuple[int, int]]):
        if p[x] == x:
            return x
        p[x] = find(p[x], p)
        return p[x]
    def union(x:tuple[int,int], y:tuple[int,int], p:dict[tuple[int, int]]):
        s1 = find(x, p)
        s2 = find(y, p)
        if(s1 != s2):
            if s1 == (-1, -1):
                p[s2] = s1
            else:
                p[s1] = s2

    res = np.zeros_like(edge_map)
    h, w = edge_map.shape
    p = { (i, j):(i, j) for j in range(w) for i in range(h)}
    p[(-1, -1)] = (-1, -1)

    strong_indices = np.where(edge_map == 2)
    for i in range(len(strong_indices[0])):
        p[(strong_indices[0][i], strong_indices[1][i])] = (-1, -1)

    for i in range(h):
        for j in range(w):
            if edge_map[i, j] == 0:
                continue
            # Top-Left
            if isValid(i-1, j-1, h, w):
                if edge_map[i-1, j-1] != 0:
                    union((i, j), (i-1, j-1), p)
            # Top
            if isValid(i-1, j, h, w):
                if edge_map[i-1, j] != 0:
                    union((i, j), (i-1, j), p)
            # Top-Right
            if isValid(i-1, j+1, h, w):
                if edge_map[i-1, j+1] != 0:
                    union((i, j), (i-1, j+1), p)
            # Left
            if isValid(i, j-1, w, h):
                if edge_map[i, j-1] != 0:
                    union((i, j), (i, j-1), p)
    
    for i in range(h):
        for j in range(w):
            if find((i, j), p) == (-1, -1):
                res[i, j] = 255
    return res


def canny_edge_detector(
    img: np.ndarray,
    gaussian_kernel_size:int|None, 
    gaussian_kernel_sigma:np.float64, 
    sobel_kernel_size:int, 
    hysteric_Th:np.float64, 
    hysteric_Tl:np.float64
):
    # Noise Reduction
    if gaussian_kernel_size != None:
        gaussian_kernel = generate_kxk_gaussian_kernel(size=gaussian_kernel_size, sigma=gaussian_kernel_sigma)

        sample1_denoised = convolve_2D_kernel(img=img, kernel=gaussian_kernel)
        # write_image("sample1_denoised", img=sample1_denoised, type=1, question="p1_b")
    else:
        sample1_denoised = np.copy(img)

    # Compute gradient (w/ sobel kernel)
    Gx, Gy = generate_kxk_sobel_kernel(size=sobel_kernel_size)
    gradient_magnitude, gradient_orientation_radius = get_gradient_magnitude_orientation(img = sample1_denoised, Gx=Gx, Gy=Gy)
    gradient_orientation_degree = np.degrees(gradient_orientation_radius)
    # write_image("sample1_denoised_gradient", img=gradient_magnitude, type=1, question="p1_b")

    # Non-Maximal Suppression
    sample1_NMS = non_maximal_surpression(gradient_magnitude=gradient_magnitude, gradient_orientation=gradient_orientation_degree)
    # write_image("sample1_NMS", img=sample1_NMS, type=1, question="p1_b")

    # Hysterestic thresholding
    # Plot Histogram to decide threshold
    # datas = []
    # datas.append({"title": f'sample1_NMS.png', "img": sample1_NMS})
    # display_histogram(question="p1_b", datas=datas, quiet=True, title="sample1_NMS_hist")

    edge_map, sample1_HyThres = hysterestic_thresholding(sample1_NMS, hysteric_Tl, hysteric_Th)
    # write_image("sample1_HyThres", img=sample1_HyThres, type=1, question="p1_b")

    res = connected_component_labeling(edge_map=edge_map)
    return res


# ---------------------------------------------------------------------------- #
#                          Laplcian of Gaussian (LoG)                          #
# ---------------------------------------------------------------------------- #

def generate_3x3_laplacian_kernel():
    return (1/8) * np.array([[-2, 1, -2], [1, 4, 1], [-2, 1, -2]], dtype=np.float64)

def laplacian_of_gaussian(img:np.ndarray, gaussian_kernel_size:int, gaussian_kernel_sigma:np.float64):
    gaussian_kernel = generate_kxk_gaussian_kernel(size=gaussian_kernel_size, sigma=gaussian_kernel_sigma)
    laplacian_kernel = generate_3x3_laplacian_kernel()
    
    res = convolve_2D_kernel(img, gaussian_kernel)
    res = convolve_2D_kernel(res, laplacian_kernel)
    
    return res

def zero_corssing_detection(img:np.ndarray, threshold:np.float64):
    assert threshold>=0, "Threshold for zero-crossing should be non-negative"
    edge_map = np.copy(img)
    edge_img = np.zeros_like(img).astype(np.uint8)
    h, w = img.shape
    for i in range(h):
        for j in range(w):
            if np.abs(img[i, j]) <= threshold:
                edge_map[i, j] = 0
    for i in range(1, h-1):
        for j in range(1, w-1):
            if edge_map[i-1, j-1]*edge_map[i+1, j+1] < 0 or edge_map[i-1, j+1]*edge_map[i+1, j-1] < 0 or \
               edge_map[  i, j-1]*edge_map[  i, j+1] < 0 or edge_map[i-1,   j]*edge_map[i+1,   j] < 0:
                edge_img[i, j] = 255
    return edge_img
                

# ---------------------------------------------------------------------------- #
#                                Edge Crispening                               #
# ---------------------------------------------------------------------------- #

def genertate_3x3_high_pass_filter():
    return np.array([[-1, -1, -1], [-1, 9, -1], [-1, -1, -1]], dtype=np.int32)

def unsharp_masking(img:np.ndarray, gaussian_kernel_size:int, gaussian_kernel_sigma:np.float64, c:np.float64):
    # assert 3/5<=c and c<=5/6, "c should be between [3/5, 5/6] in unsharp masking"
    gaussian_kernel = generate_kxk_gaussian_kernel(size=gaussian_kernel_size, sigma=gaussian_kernel_sigma)
    filtered_img = convolve_2D_kernel(img=img, kernel=gaussian_kernel)
    res = (c/(2*c-1)) * img.astype(np.int32) - ((1-c)/(2*c-1)) * filtered_img.astype(np.int32)
    res = np.clip(res, 0, 255).astype(np.uint8)
    return res

# ---------------------------------------------------------------------------- #
#               Hough Transform (theta->[0, 179], rho->[-w, +w])               #
# ---------------------------------------------------------------------------- #

def plot_hough_feature_space(accumulator:np.ndarray, question:str, title:str, quiet:bool=True):
    h, w = accumulator.shape
    w = w // 2
    plt.imshow(accumulator, cmap='inferno', aspect='auto', extent=[-w, w, 0, 180])
    plt.xlabel('Rho')
    plt.ylabel('Theta (degrees)')
    plt.title(f'Hough Transform Feature Space ({title})')
    plt.colorbar(label='Votes')
    
    # if   w >= 1050: idx = 500
    # elif w >= 750:  idx = 250
    # elif w >= 300:  idx = 100
    # elif w >= 150:  idx = 50
    # elif w >= 30:   idx = 10
    # elif w >= 15:   idx = 5
    # else:           idx = 1

    idx = int(np.ceil((w/3)/100)) * 100

    x_labels = [0, w, -w]
    tmp = idx
    while tmp < w:
        x_labels.append(tmp)
        x_labels.append(-tmp)
        tmp += idx
    plt.xticks(x_labels)

    if not os.path.exists("./report/"+question):
        os.makedirs("./report/"+question)
    if title == "":
        plt.savefig("./report/"+question+"/"+question)
    else:
        plt.savefig("./report/"+question+"/"+title)
    if quiet is False:
        plt.show()

# theta->[0, 179], rho->[-w, +w] where w = ceil(sqrt(2) * max(h, w))
def hough_transform(img:np.ndarray, top_k:int):
    assert top_k > 0, "Top_k should be positive in hough transform"

    h, w = img.shape
    result_img = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)
    max_rho = np.ceil(np.hypot(h, w)).astype(np.int32)
    # print(h, w, max_rho)

    accumulator = np.zeros((180, 2*max_rho+1))
    sin_table = [np.sin(np.deg2rad(i)) for i in range(180)]
    cos_table = [np.cos(np.deg2rad(i)) for i in range(180)]
    for i in range(h):
        for j in range(w):
            if img[i, j] == 0:
                continue
            
            for theta in range(180):
                rho = int(j*cos_table[theta] + i*sin_table[theta])
                accumulator[theta, rho + max_rho] += 1

    flattened_accumulator = accumulator.flatten()
    targets = np.argpartition(flattened_accumulator, -top_k)[-top_k:]

    for target in targets:
        theta, rho = np.unravel_index(target, accumulator.shape)
        rho -= max_rho

        sin = sin_table[theta]
        cos = cos_table[theta]

        if sin <= 1e-8:
            y0, y1 = 0, h
            x0, x1 = (rho-y0*sin)/cos, (rho-y1*sin)/cos
        else:
            x0, x1 = 0, w
            y0, y1 = (rho-x0*cos)/sin, (rho-x1*cos)/sin
        
        cv2.line(result_img, (int(x0),int(y0)), (int(x1),int(y1)), (255,255,0))

    return accumulator, result_img

# ---------------------------------------------------------------------------- #
#                           Geometrical Modification                           #
# ---------------------------------------------------------------------------- #

# @return in the form of [[x1, y1, 1], [x2, y2, 1], ... , [xn, yn, 1]] (0 <= x < h, 0 <= y < w)
def generate_coords_pairs(h:int, w:int):
    assert h>0 and w>0, "h and w should be positive"
    return np.vstack((np.stack([x for x in np.ndindex(h,w)]).T, np.ones(h*w))).T.astype(np.float64)

# convert image coordinate to cartesian coordinate
def coords_img_to_cartesian(coords:np.ndarray, h:int, w:int):
    assert coords.ndim == 2 and coords.shape[1] == 3, "coords should be in (x,y,1) form"
    return coords[:, [1, 0, 2]] * [1, -1, 1] + [0.5, h-0.5, 0]

# convert cartesian coordinate to image coordinate
def coords_cartesian_to_img(coords:np.ndarray, h:int, w:int):
    assert coords.ndim == 2 and coords.shape[1] == 3, "coords should be in (x,y,1) form"
    return coords[:, [1, 0, 2]] * [-1, 1, 1] + [h-0.5, -0.5, 0]

# coords[]  := the original coordidates on the src (cartesian coordinates form)
# h         := the height of the result image
# w         := the width of the result image
# fill      := the color for blank pixel ([0, 255])
def warpAffine_bilinear_interpolation(src:np.ndarray, coords:np.ndarray, res_h:int, res_w:int, fill:int=0):
    assert fill >= 0 and fill <= 255, "fill should be an interger between [0,255]"
    src_h, src_w = src.shape
    res = np.ones((res_h, res_w)).astype(np.float64) * fill

    img_coords = coords_cartesian_to_img(coords, res_h, res_w)
    for i in range(res_h):
        for j in range(res_w):
            idx = i*res_w+j
            # bilinear interpolation
            if img_coords[idx,0] < 0 or img_coords[idx,0] > src_h-1 or img_coords[idx,1] < 0 or img_coords[idx,1] > src_w-1:
                continue
            y, x = img_coords[idx,0], img_coords[idx,1]
            left_x, left_y = np.floor(x).astype(np.int32), np.floor(y).astype(np.int32)
            right_x, right_y = np.ceil(x).astype(np.int32), np.ceil(y).astype(np.int32)
            assert left_x >= 0 and left_x < src_w and right_x >= 0 and right_x < src_w, f"invalid boundary for x:{x} l:{left_x} r:{right_x}"            
            assert left_y >= 0 and left_y < src_h and right_y >= 0 and right_y < src_h, f"invalid boundary for y:{y} l:{left_y} r:{right_y}"

            a = (y-left_y)
            b = (x-left_x)
            res[i,j] = (1-a)*(1-b)*src[left_y, left_x] + (1-a)*b*src[left_y, right_x] + a*(1-b)*src[right_y, left_x] + a*b*src[right_y, right_x]
    return res

def geometrical_shift(coords:np.ndarray, tx:np.float64, ty:np.float64):
    assert coords.ndim == 2 and coords.shape[1] == 3, "coords should be in (x,y,1) form"
    shift = np.linalg.inv(np.array([[1, 0, tx], [0, 1, ty], [0, 0, 1]]))
    return np.matmul(shift, coords.T).T

def geometrical_scale(coords:np.ndarray, sx:np.float64, sy:np.float64):
    assert coords.ndim == 2 and coords.shape[1] == 3, "coords should be in (x,y,1) form"
    scale = np.linalg.inv(np.array([[sx, 0, 0], [0, sy, 0], [0, 0, 1]]))
    return np.matmul(scale, coords.T).T

# theta: [-180,180]
def geometrical_rotate(coords:np.ndarray, theta: np.float64):
    assert coords.ndim == 2 and coords.shape[1] == 3, "coords should be in (x,y,1) form"
    radius = np.deg2rad(theta)
    rotate = np.linalg.inv(np.array([[np.cos(radius), -np.sin(radius), 0], [np.sin(radius), np.cos(radius), 0], [0, 0, 1]]))
    return np.matmul(rotate, coords.T).T

#! deprecated
def geometrical_polynomial_warping(coords:np.ndarray, feature_src_coords:np.ndarray, feature_dest_coords:np.ndarray):
    # print(feature_src_coords)
    A = np.column_stack((np.ones(len(feature_src_coords)), feature_src_coords[:, 0], feature_src_coords[:, 1], feature_src_coords[:, 0]**2, feature_src_coords[:, 0]*feature_src_coords[:, 1], feature_src_coords[:, 1]**2))
    B = np.column_stack((feature_dest_coords[:, 0], feature_dest_coords[:, 1]))

    pseudo_A_inv = np.linalg.pinv(A)
    para = np.matmul(pseudo_A_inv, B)
    coords_without_1 = coords[:, :2]
    pseudo_para_inv = np.linalg.pinv(para)
    coords_res = np.matmul(coords_without_1, pseudo_para_inv)
    coords_res = coords_res.T[:,1:3]

    img_coords = generate_coords_pairs(600, 600) - (300,300, 0)
    z = np.column_stack((np.ones(len(img_coords)), img_coords[:, 0], img_coords[:, 1], img_coords[:, 0]**2, img_coords[:, 0]*img_coords[:, 1], img_coords[:, 1]**2))
    print(z.shape)
    print(para.shape)
    res_coord = np.matmul(para.T, z.T).T
    for i in range(600):
        for j in range(600):
            print(f"{(i, j)}, {res_coord[i*600+j]}")

    return np.hstack((coords_res, np.ones((coords_res.shape[0], 1))))


# The Pipeline for performing a series of transformation
# Will do the transformation in the inverse order to caculate the original coordinates
# operarions:
#   shift:  {"type":"shift", "tx":0.0, "ty":0.0}
#   scale:  {"type":"scale", "sx":0.0, "sy":0.0}
#   rotate: {"type":"rotate", "theta":0.0}
def geometrical_transformation(src:np.ndarray, series:list, h:int, w:int):
    series = series[::-1]
    img_coords = generate_coords_pairs(h, w)
    res_coords = coords_img_to_cartesian(coords=img_coords, h=h, w=w)
    for op in series:
        if op["type"] == "shift":
            res_coords = geometrical_shift(res_coords, tx=op["tx"], ty=op["ty"])
        elif op["type"] == "scale":
            res_coords = geometrical_scale(res_coords, sx=op["sx"], sy=op["sy"])
        elif op["type"] == "rotate":
            res_coords = geometrical_rotate(res_coords, theta=op["theta"])
    
    return warpAffine_bilinear_interpolation(src=src, coords=res_coords, res_h=h, res_w=w, fill=255)

# reference : https://blog.csdn.net/yangtrees/article/details/9095731
def warping(image:np.ndarray, center_x:int, center_y:int, radius:np.float64):
    result = np.ones_like(image) * 255
    h, w = image.shape
    R1 = np.sqrt(h**2+w**2)/10
    for i in range(-radius, radius+1):
        for j in range(-radius, radius+1):
            distance = np.sqrt(i**2+j**2)
            if distance <= radius:
                new_x, new_y = int(i * np.power(distance/R1, 1.2) + center_x), int(j * np.power(distance/R1, 1.2) + center_y)
                #print(f"{(i,j)} -> {(new_x-center_x, new_y-center_y)}")
                if new_x < 0 or new_x >= w or new_y < 0 or new_y >= h:
                    continue
                result[j+center_y][i+center_x] = image[new_y][new_x]

    return result

def sin_wave_shifting(img:np.ndarray, dist:np.float64, T:np.float64, offset:np.float64):
    res = np.ones_like(img) * 255
    h, w = img.shape

    for i in range(h):
        for j in range(w):
            t = dist * np.sin(np.deg2rad(360 * ((i+offset) / T) ))
            new_x = int(j + t)
            new_y = i
            if new_x < 0 or new_x >= w or new_y < 0 or new_y >= h:
                continue
            res[new_y, new_x] = img[i, j]
    return res

def sin_wave_shifting_ver2(img:np.ndarray, dist:np.float64, T:np.float64, offset:np.float64):
    res = np.ones_like(img) * 255
    h, w = img.shape

    for i in range(h):
        for j in range(w):
            if ((j+0)//160)%2 == 0:
                opposite = -1
            else:
                opposite = 1
            t = dist * np.sin(np.deg2rad(360 * ((i+offset) / T) ))
            new_x = int(j + opposite * t)
            new_y = i
            if new_x < 0 or new_x >= w or new_y < 0 or new_y >= h:
                continue
            res[new_y, new_x] = img[i, j]
    return res


# ---------------------------------------------------------------------------- #
#                                   Problem 1                                  #
# ---------------------------------------------------------------------------- #

def p1_a():
    
    # Parameters
    kernel_sizes:int = [3]
    thresholds: list[np.float64] = [40]

    for kernel_size in kernel_sizes:
        Gx, Gy = generate_kxk_sobel_kernel(size=kernel_size)
        sample1 = read_image("sample1.png", 0, options=cv2.IMREAD_GRAYSCALE)

        result1, _ = get_gradient_magnitude_orientation(img=sample1, Gx=Gx, Gy=Gy)
        write_image("result1", result1)

        # Plot Histogram to decide threshold
        #datas = []
        #datas.append({"title": f'result1 (k={kernel_size})', "img": result1})
        #display_histogram(question="p1_a", datas=datas, quiet=True, title=f"gradient_k={kernel_size}_hist")

        for threshold in thresholds:
            result2 = threshold_image(img=result1, threshold=threshold)
            write_image("result2", result2)
            #write_image(f"result2_k={kernel_size}_t={threshold}", result2, type=1, question="p1_a")
    

def p1_b():
    # Parameters
    gaussian_kernel_size        = [7]
    gaussian_kernel_sigma       = [2.0]
    sobel_kernel_size           = [7]
    hysteric_Th                 = [60.0]
    hysteric_Tl                 = [40.0]

    parameter_pairs = itertools.product(gaussian_kernel_size, gaussian_kernel_sigma, sobel_kernel_size, hysteric_Th, hysteric_Tl)
    parameter_pairs = list(parameter_pairs)
    
    sample1 = read_image("sample1.png", 0, options=cv2.IMREAD_GRAYSCALE)
    #total = len(parameter_pairs)
    #for idx, p in tqdm(iterable=enumerate(parameter_pairs), total=total, colour="green"):
        #if p[3] < p[4]: continue
        #result3 = canny_edge_detector(sample1, gaussian_kernel_size=p[0], gaussian_kernel_sigma=p[1], sobel_kernel_size=p[2], hysteric_Th=p[3], hysteric_Tl=p[4])
        #write_image(f"result3_GKSize={p[0]}_Sigma={p[1]}_SKS={p[2]}_Th={p[3]}_Tl={p[4]}", result3, 1, "p1_b")
    for p in parameter_pairs:
        result3 = canny_edge_detector(sample1, gaussian_kernel_size=p[0], gaussian_kernel_sigma=p[1], sobel_kernel_size=p[2], hysteric_Th=p[3], hysteric_Tl=p[4])
        write_image(f"result3", result3)

def p1_c():
    # Parameters
    gaussian_kernel_size        = [3]
    gaussian_kernel_sigma       = [1.5]
    zero_crossing_threshold     = [3]

    parameters, total = generate_parameters_pairs([gaussian_kernel_size, gaussian_kernel_sigma, zero_crossing_threshold])

    sample1 = read_image("sample1.png", 0, options=cv2.IMREAD_GRAYSCALE)
    # Plot Histogram to decide threshold
    # datas = []
    # datas.append({"title": f'sample1_LoG.png', "img": np.abs(sample1_LoG)})
    # display_histogram(question="p1_c", datas=datas, quiet=True, title="sample1_LoG_hist")
    
    #for idx, p in tqdm(iterable=enumerate(parameters), total=total, colour="green"):
        #sample1_LoG = laplacian_of_gaussian(sample1, gaussian_kernel_size=p[0], gaussian_kernel_sigma=p[1])
        #result4 = zero_corssing_detection(sample1_LoG, threshold=p[2])
        #write_image(f"result4_GKSize={p[0]}_Sigma={p[1]}_T={p[2]}", result4, 1, "p1_c")
    for p in parameters:
        sample1_LoG = laplacian_of_gaussian(sample1, gaussian_kernel_size=p[0], gaussian_kernel_sigma=p[1])
        result4 = zero_corssing_detection(sample1_LoG, threshold=p[2])
        write_image(f"result4", result4)

def p1_d():

    # Parameters
    gaussian_kernel_size        = [21]
    gaussian_kernel_sigma       = [30]
    # unsharp_masking_c           = [3/5, 13/20, 14/20, 15/20, 5/6]
    unsharp_masking_c           = [0.7]

    high_pass_filter = genertate_3x3_high_pass_filter()
    sample2 = read_image("sample2.png", 0, options=cv2.IMREAD_GRAYSCALE)

    parameters, total = generate_parameters_pairs([gaussian_kernel_size, gaussian_kernel_sigma, unsharp_masking_c])

    for p in parameters:
        result5 = unsharp_masking(img=sample2, gaussian_kernel_size=p[0], gaussian_kernel_sigma=p[1], c=p[2])
        write_image("result5", result5)
        #print(np.sum(np.abs(result5 - sample2)))
        #write_image(f"result4_GKSize={p[0]}_Sigma={p[1]}_c={p[2]}", result5, 1, "p1_d")
        #result5_high_pass = convolve_2D_kernel(sample2, high_pass_filter)
        #write_image(f"result4_high_pass_GKSize={p[0]}_Sigma={p[1]}_c={p[2]}", result5_high_pass, 1, "p1_d")


def p1_e():
    # Parameters
    # gaussian_kernel_size        = [5]
    # gaussian_kernel_sigma       = [1.5]
    # sobel_kernel_size           = [5]
    # hysteric_Th                 = [45.0]
    # hysteric_Tl                 = [15.0]
    gaussian_kernel_size        = [5]
    gaussian_kernel_sigma       = [1.25]
    sobel_kernel_size           = [3]
    hysteric_Th                 = [50.0]
    hysteric_Tl                 = [10.0]
    top_k                       = [20]

    parameters, total = generate_parameters_pairs([gaussian_kernel_size, gaussian_kernel_sigma, sobel_kernel_size, hysteric_Th, hysteric_Tl, top_k])
    result5 = read_image("result5.png", 1, options=cv2.IMREAD_GRAYSCALE)

    # Canny edge detection
    # for idx, p in tqdm(iterable=enumerate(parameters), total=total, colour="green"):
    #     if p[3] < p[4]: continue
    #     result6 = canny_edge_detector(result5, gaussian_kernel_size=p[0], gaussian_kernel_sigma=p[1], sobel_kernel_size=p[2], hysteric_Th=p[3], hysteric_Tl=p[4])
    #     write_image(f"result6_GKSize={p[0]}_Sigma={p[1]}_SKS={p[2]}_Th={p[3]}_Tl={p[4]}", result6, 1, "p1_e")

    #     write_image("result6", result6)
    #     result6 = read_image("result6.png", 1, options=cv2.IMREAD_GRAYSCALE)
    #     # Hough Transform Post-Process
    #     accumulator, result7 = hough_transform(img=result6, top_k=p[5])

    #     plot_hough_feature_space(accumulator=accumulator, question="p1_e", title=f"result6_hough_space")
    #     write_image(f"result7_GKSize={p[0]}_Sigma={p[1]}_SKS={p[2]}_Th={p[3]}_Tl={p[4]}_k={p[5]}", result7)
    for p in parameters:
        result6 = canny_edge_detector(result5, gaussian_kernel_size=p[0], gaussian_kernel_sigma=p[1], sobel_kernel_size=p[2], hysteric_Th=p[3], hysteric_Tl=p[4])
        write_image("result6", result6)
        result6 = read_image("result6.png", 1, options=cv2.IMREAD_GRAYSCALE)
        # Hough Transform Post-Process
        accumulator, result7 = hough_transform(img=result6, top_k=p[5])
        write_image(f"result7", result7)



# ---------------------------------------------------------------------------- #
#                                   Problem 2                                  #
# ---------------------------------------------------------------------------- #

def p2_a():
    sample3 = read_image("sample3.png", 0, options=cv2.IMREAD_GRAYSCALE)
    warped_sample3 = warping(sample3, center_x=270, center_y=270, radius=200)
    h, w = warped_sample3.shape
    series = [
        {"type":"shift", "tx":-270, "ty":-310},
        {"type":"scale", "sx":1.4, "sy":1.6},
        {"type":"rotate", "theta":-50.0},
        {"type":"shift", "tx":350, "ty":380},
    ]
    result8 = geometrical_transformation(src=warped_sample3, series=series, h=h, w=w)
    write_image("result8", result8)

def p2_b():
    sample5 = read_image("sample5.png", 0, options=cv2.IMREAD_GRAYSCALE)
    # sample6 = read_image("sample6.png", 0, options=cv2.IMREAD_GRAYSCALE)
    # sample_5_grid = plot_support_axis(sample5)
    # sample_6_grid = plot_support_axis(sample6)
    result9 = sin_wave_shifting(sample5, dist=25, T=150, offset=60)
    #result10 = sin_wave_shifting_ver2(sample5, dist=15, T=150, offset=60)
    # write_image("sample_5_grid", sample_5_grid)
    # write_image("sample_6_grid", sample_6_grid)
    write_image("result9", result9)
    #write_image("result10", result10)

# ---------------------------------------------------------------------------- #
#                                 Main Function                                #
# ---------------------------------------------------------------------------- #

if __name__ == "__main__":

    p1_a()
    p1_b()
    p1_c()
    p1_d()
    p1_e()

    p2_a()
    p2_b()