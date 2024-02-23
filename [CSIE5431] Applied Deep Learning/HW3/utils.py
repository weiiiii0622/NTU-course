from transformers import BitsAndBytesConfig
import torch

arr1 = [
    "翻譯成現代文",
    "翻譯成白話文",
    "文言文翻譯",
]

arr2 = [
    "翻譯成文言文",
    "翻譯成古文",
    "中國古代怎麼說",
]

ex1 = [
    "USER: 辛未，命吳堅為左丞相兼樞密使，常楙參知政事。\n把這句話翻譯成現代文。 ASSISTANT: 初五，命令吳堅為左承相兼樞密使，常增為參知政事。",
    "USER: 翻譯成白話文：\n壬申，以保忠為定難軍節度使。\n答案： ASSISTANT: 十六日，任命趙保忠為定難軍節度使。",
    "USER: 文言文翻譯：\n賈逵、張衡、蔡邕、王蕃、陸績皆以北極紐星之樞，是不動處。 ASSISTANT: 答案：賈逵、張衡、蔡邕、王蕃、陸續都認為北極紐星的樞紐，是不移動的地方。",
    "USER: 將下麵句子翻譯成現代文：\n公正嗟服。還，具言之於武帝，帝大欽重之。 ASSISTANT: 尹公正非常佩服，迴國後把這些事情都告訴瞭周武帝，周武帝十分欽敬看重熊安生。",
]

ex2 = [
    "USER: 翻譯成文言文：\n有鄰跟隨差役去見閻王說： 有人告你的狀說，不待殺死，就活生生的取齣它的腎。 ASSISTANT: 有鄰隨吏見王，王雲： 有訴君雲，不待殺瞭，生取其腎。",
    "USER: 她不僅手巧，擅長女紅，而且體態輕盈，相貌皎潔。\n幫我把這句話翻譯成文言文 ASSISTANT: 善工巧，體貌輕潔。",
    "USER: 唐朝元和年間，博陵人崔玨，從汝鄭來，僑居在長安延福裏。\n翻譯成古文： ASSISTANT: 元和中，博陵崔玨者，自汝鄭來，僑居長安延福裏。",
    "USER: 於是對二公說： 祥瑞應該依德而至，災異也因政而生。\n這句話在中國古代怎麼說： ASSISTANT: 乃言於二公曰： 夫瑞應依德而至，災異緣政而生。",
]

def get_prompt(instruction: str) -> str:
    '''Format the instruction as a prompt for LLM.'''
    prompt = "你是人工智慧助理，以下是用戶和人工智能助理之間的對話。你要對用戶的問題提供有用、安全、詳細和禮貌的回答。請使用以下文本作為少量示例，指導模型進行文言文到現代文或現代文到文言文的翻譯。"
    #prompt = "你是人工智慧助理，以下是用戶和人工智能助理之間的對話。你要對用戶的問題提供有用、安全、詳細和禮貌的回答。"

    # flag = 0
    # n = 1
    # for seg in arr1:
    #     if seg in instruction:
    #         flag = 1
    #         break
    # if flag == 1:
    #     for seg in ex1:
    #         prompt += seg
    #         if n==1:
    #           break

    # for seg in arr2:
    #     if seg in instruction:
    #         flag = 2
    #         break
    # if flag == 2:
    #     for seg in ex2:
    #         prompt += seg
    #         if n==1:
    #           break

    # if flag == 0:
    #     for seg in ex1:
    #         prompt += seg
    #         if n==1:
    #           break
    #     for seg in ex2:
    #         prompt += seg
    #         if n==1:
    #           break

    prompt += f"USER: {instruction} ASSISTANT:"
    # prompt = f"你是人工智慧助理，以下是用戶和人工智能助理之間的對話。你要對用戶的問題提供有用、安全、詳細和禮貌的回答。USER: {instruction} ASSISTANT:"
    return prompt


def get_bnb_config() -> BitsAndBytesConfig:
    '''Get the BitsAndBytesConfig.'''
    config = BitsAndBytesConfig(
        load_in_4bit=True,
        bnb_4bit_quant_type="nf4",
        bnb_4bit_compute_dtype=torch.bfloat16,
        bnb_4bit_use_double_quant=True,
    )
    return config