# Midterm Project - Implement ls(1)

## Thong tin sinh vien
- Ho ten: Tran Van Tri
- MSSV: 24IT283
- GitHub repo: https://github.com/tranvantri556/TranVanTri_24IT283_midterm

## Mo ta
Day la ban cai dat don gian cua lenh ls(1) tren NetBSD 10.1, viet bang C, dua tren manual page duoc cung cap.

## Cac option da ho tro
- -A: liet ke tat ca entry tru . va ..
- -a: bao gom ca entry bat dau bang dau cham
- -c: dung thoi gian thay doi status (ctime)
- -d: xem thu muc nhu file thuong
- -F: them ky hieu phan loai (/ * @ = | %)
- -f: khong sap xep
- -h: hien thi kich thuoc dang human-readable
- -i: in inode
- -k: hien thi kich thuoc theo KB
- -l: long format
- -n: giong -l nhung in UID/GID dang so
- -q: thay ky tu khong in duoc bang dau ?
- -R: de quy
- -r: dao nguoc thu tu sap xep
- -S: sap xep theo kich thuoc, lon nhat truoc
- -s: in so block
- -t: sap xep theo thoi gian sua doi, moi nhat truoc
- -u: dung thoi gian truy cap (atime)
- -w: in raw ky tu khong in duoc

## Cau truc project
- main.c          : diem vao chuong trinh
- options.c/.h    : phan tich tham so dong lenh
- list.c/.h       : duyet thu muc, sap xep, de quy
- format.c/.h     : dinh dang long format
- util.c/.h       : ham tien ich
- Makefile        : build project
- .gitignore      : loai bo file build

## Huong dan build
Chay lenh:
    make

Sau khi build thanh cong, file thuc thi myls se xuat hien.

## Huong dan su dung

Cu phap:
    ./myls [option] [file/thu_muc ...]

Vi du co ban:
    ./myls                  -> liet ke thu muc hien tai
    ./myls -l               -> long format
    ./myls -la              -> liet ke ca file an
    ./myls -il              -> hien inode
    ./myls -lh /etc         -> kich thuoc human-readable
    ./myls -R /etc          -> de quy
    ./myls -F               -> them ky hieu phan loai
    ./myls -Sl              -> sap xep theo kich thuoc giam dan
    ./myls -tl              -> sap xep theo thoi gian moi nhat
    ./myls -rl              -> dao nguoc thu tu
    ./myls -d /etc          -> xem thu muc nhu file
    ./myls -laSh            -> ket hop nhieu option

Xu ly loi:
    ./myls /nonexistent
    -> ls: /nonexistent: No such file or directory

## Don build
Chay lenh:
    make clean

Lenh nay xoa cac file .o va file myls.

## Moi truong phat trien
- He dieu hanh: NetBSD 10.1 (GENERIC) tren Oracle VirtualBox
- Trinh bien dich: gcc 10.5.0
- Cong cu build: make
- Trinh soan thao: nano

## Ghi chu
- Project viet tu dau, khong dung thu vien ngoai.
- Chi dung cac ham chuan POSIX/NetBSD: opendir, readdir, lstat, stat, getpwuid, getgrgid, localtime, readlink.
- Code chia module ro rang, co comment.
- Chuong trinh khong bi segmentation fault khi gap loi.
