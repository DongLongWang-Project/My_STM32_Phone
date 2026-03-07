import os

SD_CARD_Path="../论文资料"

print(SD_CARD_Path)

file=[]

for f in os.listdir(SD_CARD_Path):
    if f.endswith(".png"):
        file.append(f)
        
print("找到了",file)