import argparse

import numpy as np
import cv2
import matplotlib.pyplot as plt

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
#     clear_output(args.output)
#     clear_output("./report")
#     if not os.path.exists("./result"):
#         os.makedirs("./result")
#     if not os.path.exists("./report"):
#         os.makedirs("./report")

# ---------------------------------------------------------------------------- #
#                                   Utilties                                   #
# ---------------------------------------------------------------------------- #

# def display_image(img):
#     cv2.imshow('Image', img)
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

def write_image(id:str, img, type:int = 0, question:str = ""):
    if type == 0:
        cv2.imwrite(args.output+"result"+id+".png", img)
    elif type == 1:
        cv2.imwrite("./report/"+question+"/result"+id+".png", img)

# If mask is k*k, add a (k-1)/2 padding to the border cell
def pad_image(img, mask_size:int):
    assert mask_size%2==1, "Mask size should be odd"
    return np.lib.pad(img, (mask_size-1)//2, 'reflect')

def unpad_image(img: np.ndarray, mask_size: int):
    assert mask_size%2==1, "Mask size should be odd"
    h, w = img.shape
    border_size = (mask_size-1)//2
    return img[border_size: h-border_size, border_size: w-border_size]

def rgb2gray(img):
    return np.dot(img[...,:3], [0.1140, 0.5870, 0.2989])

def global_histogram_equalization(img):
    hist, bin = np.histogram(img.flatten(), bins=256, range=[0, 256])
    pdf = hist/img.size
    cdf = np.cumsum(pdf)
    tranlate_table = np.round(cdf*255).astype(np.uint8)
    return tranlate_table[img]

def local_histogram_equalization(img, mask_size=15):
    assert mask_size%2==1, "Mask size should be odd"
    h, w = img.shape
    block_size = (mask_size-1)//2
    padded_img = np.lib.pad(img, block_size, 'reflect')
    res = np.zeros(img.shape, dtype=np.uint8)

    # todo: O(n^2) -> O(n)?
    for i in range(h):
        for j in range(w):
            trans_x = i + block_size
            trans_y = j + block_size
            block = padded_img[trans_x-block_size:trans_x+block_size+1,trans_y-block_size:trans_y+block_size+1]
            ranking = np.sort(block.flatten())
            rank = np.where(ranking == img[i,j])[0][0]
            res[i, j] = int(255 * (rank / (2*block_size+1)**2))
    return res

def calc_psnr(ground_truth, img):
    return 10 * np.log10(255**2 / np.mean(np.square(img-ground_truth)))

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
#                                   Problem 0                                  #
# ---------------------------------------------------------------------------- #

def p0_a():
    img = read_image("sample1.png", 0)
    flipped_img = np.flipud(img)
    write_image("1", flipped_img)

def p0_b():
    img = read_image("result1.png", 1)
    write_image("2", rgb2gray(img))

# ---------------------------------------------------------------------------- #
#                                   Problem 1                                  #
# ---------------------------------------------------------------------------- #

def p1_a():
    img = read_image("sample2.png", 0, options=cv2.IMREAD_GRAYSCALE)
    img = np.clip(img.astype(np.uint16) // 3, 0, 255).astype(np.uint8)
    write_image("3", img)

def p1_b():
    img = read_image("result3.png", 1, options=cv2.IMREAD_GRAYSCALE)
    img = np.clip(img.astype(np.uint16) * 3, 0, 255).astype(np.uint8)
    write_image("4", img)

def p1_c():
    return

def p1_d():
    targets = [
        ("sample2.png", 0),
        ("result3.png", 1),
        ("result4.png", 1)
    ]
    for idx, target in enumerate(targets):
        img = read_image(target[0], target[1], options=cv2.IMREAD_GRAYSCALE)
        img = global_histogram_equalization(img)
        write_image(f'{5+idx}', img)
    
def p1_e():
    sample2 = read_image("sample2.png", 0, options=cv2.IMREAD_GRAYSCALE)
    result5 = read_image("result5.png", 1, options=cv2.IMREAD_GRAYSCALE)
    result8 = local_histogram_equalization(sample2, mask_size=71)
    write_image("8", result8)

def p1_f():
    sample3 = read_image("sample3.png", 0, options=cv2.IMREAD_GRAYSCALE)
    result9 = global_histogram_equalization(sample3)
    write_image("9", result9)

# ---------------------------------------------------------------------------- #
#                                   Problem 2                                  #
# ---------------------------------------------------------------------------- #

def p2_a():

    sample4 = read_image("sample4.png", 0, options=cv2.IMREAD_GRAYSCALE)
    sample5 = read_image("sample5.png", 0, options=cv2.IMREAD_GRAYSCALE)
    sample6 = read_image("sample6.png", 0, options=cv2.IMREAD_GRAYSCALE)

    lowpass_filter_5x5 = generate_5x5_lowpass_filter()
    result10 = spacial_filtering(sample5, lowpass_filter_5x5)
    write_image("10", result10)

    result11 = median_filtering(sample6, kernel_size=3)
    write_image("11", result11)

def p2_b():
    sample4 = read_image("sample4.png", 0, options=cv2.IMREAD_GRAYSCALE)
    sample5 = read_image("sample5.png", 0, options=cv2.IMREAD_GRAYSCALE)
    sample6 = read_image("sample6.png", 0, options=cv2.IMREAD_GRAYSCALE)
    result10 = read_image("result10.png", 1, options=cv2.IMREAD_GRAYSCALE)
    result11 = read_image("result11.png", 1, options=cv2.IMREAD_GRAYSCALE)

    targets = [sample5, result10, sample6, result11]

    for idx, target in enumerate(targets):
        target_name = [name for name, value in locals().items() if value is target][0]
        print(f'{target_name} PSNR: {calc_psnr(sample4, target)}')

def p2_c():
    sample7 = read_image("sample7.png", 0, options=cv2.IMREAD_GRAYSCALE)
    lowpass_filter_5x5 = generate_5x5_lowpass_filter()

    result12 = np.copy(sample7)
    iteration = 3
    for iter in range(1, iteration+1):
        result12 = median_filtering(result12, kernel_size=3)
        result12 = spacial_filtering(result12, lowpass_filter_5x5)
    
    write_image("12", result12)



# ---------------------------------------------------------------------------- #
#                                 Main Function                                #
# ---------------------------------------------------------------------------- #

if __name__ == "__main__":

    p0_a()
    p0_b()

    p1_a()
    p1_b()
    p1_c()
    p1_d()
    p1_e()
    p1_f()

    p2_a()
    p2_b()
    p2_c()