# # data
# gdown https://drive.google.com/drive/folders/1CpAKvjyWLgZhxXrTlb9vmHRZCeaet6bI?usp=share_link -O ./DATA --folder --no-cookies
# # MC_MODEL
# gdown https://drive.google.com/drive/folders/1MYxwqKkDrfANlPnzjwyTYEuYV5xow5Rj?usp=share_link -O ./MC_MODEL --folder --no-cookies
# # QA_MODEL1
# gdown https://drive.google.com/drive/folders/1Rr3Qn_zOoYXxQ-b5nIqxnzkFD6ZH6o8f?usp=share_link -O ./QA_MODEL1 --folder --no-cookies
# # QA_MODEL2
# gdown https://drive.google.com/drive/folders/1wdrFex14VkHCx5idBYptzfPITppgsJiM?usp=share_link -O ./QA_MODEL2 --folder --no-cookies
# # QA_MODEL3
# gdown https://drive.google.com/drive/folders/1Tzim9wrQ0N0KpPN4Tu93ZtxVfLRwhlhO?usp=share_link -O ./QA_MODEL3 --folder --no-cookies
# # QA_MODEL4
# gdown https://drive.google.com/drive/folders/1n5RHH9KT08d50k9kL-5mTcjOgJcxTaQk?usp=share_link -O ./QA_MODEL4 --folder --no-cookies

# DATA & MODELS
wget "https://www.dropbox.com/scl/fi/nlg612l9psdbyj9nfyxac/DATA.zip?rlkey=dn0g932cpal9hluo2e6f0epgo&dl=0" -O "./DATA.zip"
wget "https://www.dropbox.com/scl/fi/2rywy5x9qng6ti799jr71/MC_MODEL.zip?rlkey=0yyzesm27tv8ijusp800r144m&dl=0" -O "./MC_MODEL.zip"
wget "https://www.dropbox.com/scl/fi/70zam4r9gbrr97j6sf7qs/QA_MODEL1.zip?rlkey=v9go5kuepdizr1hhjjfmdagl5&dl=0" -O "./QA_MODEL1.zip"
wget "https://www.dropbox.com/scl/fi/k2t22d8serc2mu0ocivfp/QA_MODEL2.zip?rlkey=abqx1zv16hak0jg4j6vodnfaa&dl=0" -O "./QA_MODEL2.zip"
wget "https://www.dropbox.com/scl/fi/e8ummcb5j5kvmicbdvt3h/QA_MODEL3.zip?rlkey=8lsov6aow4bm2vmqbr9vjb0b2&dl=0" -O "./QA_MODEL3.zip"
wget "https://www.dropbox.com/scl/fi/jjts2yk3z8eydet30r2r9/QA_MODEL4.zip?rlkey=07sz3kmul45vpnxbludbragh6&dl=0" -O "./QA_MODEL4.zip"
unzip \*.zip
rm *.zip